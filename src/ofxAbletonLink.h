/* Copyright 2016, Akihiro Komori. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "ofConstants.h"

#if defined(TARGET_OSX)
#define LINK_PLATFORM_MACOSX 1
#elif defined(TARGET_LINUX)
#define LINK_PLATFORM_LINUX 1
#elif defined(TARGET_WIN32)
#define LINK_PLATFORM_WINDOWS 1
#endif

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#include "Link.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

// ============================================================================
// ofxAbletonLink
// ============================================================================
// Wraps ableton::Link and exposes two timing APIs:
//
//   App-thread  (call from ofApp::update / ofApp::draw)
//   ─────────────────────────────────────────────────────
//   auto status = link.update();          // beat / phase / isPlaying
//   link.setTempo(130.0);
//
//   Audio-thread  (call from ofApp::audioOut / pdsp::Engine callback)
//   ─────────────────────────────────────────────────────────────────
//   auto state = link.captureAudioSessionState();
//   auto now   = link.clock().micros();
//
//   for (int i = 0; i < numFrames; i++) {
//       auto t = now + std::chrono::microseconds(
//                    llround(1e6 * i / sampleRate));
//       double beat  = state.beatAtTime(t, quantum);
//       double phase = state.phaseAtTime(t, quantum);
//       // fire drum hit at exact sample
//   }
//   link.commitAudioSessionState(state);   // if you changed tempo/playing
//
// ============================================================================

class ofxAbletonLink {
public:

    // ── App-thread status (from update()) ────────────────────────────────────
    struct Status {
        double beat     { 0.0 };
        double phase    { 0.0 };
        double tempo    { 120.0 };
        bool   isPlaying{ false };
    };

    // ── Thin wrapper around ableton::Link::Timeline ───────────────────────────
    // Safe to call from either the audio thread (captureAudioSessionState) or
    // the app thread (captureAppSessionState).  Commit back when done if you
    // modified tempo or playing state.
    class SessionState {
    public:
        explicit SessionState(ableton::Link::Timeline s, double quantum)
            : m_state(std::move(s)), m_quantum(quantum) {}

        // Beat / phase at a given clock time
        double beatAtTime (std::chrono::microseconds t) const
            { return m_state.beatAtTime(t, m_quantum); }
        double phaseAtTime(std::chrono::microseconds t) const
            { return m_state.phaseAtTime(t, m_quantum); }

        // Beat / phase with explicit quantum override
        double beatAtTime (std::chrono::microseconds t, double q) const
            { return m_state.beatAtTime(t, q); }
        double phaseAtTime(std::chrono::microseconds t, double q) const
            { return m_state.phaseAtTime(t, q); }

        // Convenience: beat/phase at sample `i` within an audio buffer
        // that started at `bufferStartMicros`.
        double beatAtSample (std::chrono::microseconds bufferStart,
                             int sampleIndex, double sampleRate) const {
            return beatAtTime(sampleTimeAt(bufferStart, sampleIndex, sampleRate));
        }
        double phaseAtSample(std::chrono::microseconds bufferStart,
                             int sampleIndex, double sampleRate) const {
            return phaseAtTime(sampleTimeAt(bufferStart, sampleIndex, sampleRate));
        }

        // True if a beat subdivision boundary was crossed between sample i-1
        // and sample i.  subdivision=1 → quarter note, 0.25 → 16th note, etc.
        bool crossedBeatAt(std::chrono::microseconds bufferStart,
                           int sampleIndex, double sampleRate,
                           double subdivision = 1.0) const {
            if (sampleIndex == 0) return false;
            double prev = beatAtSample(bufferStart, sampleIndex - 1, sampleRate);
            double cur  = beatAtSample(bufferStart, sampleIndex,     sampleRate);
            return std::floor(prev / subdivision) != std::floor(cur / subdivision);
        }

        double tempo()    const { return m_state.tempo(); }

        // Modify (then commit via commitAudioSessionState / commitAppSessionState)
        void setTempo   (double bpm, std::chrono::microseconds t)
            { m_state.setTempo(bpm, t); }
        void requestBeatAtTime(double beat, std::chrono::microseconds t)
            { m_state.requestBeatAtTime(beat, t, m_quantum); }

        ableton::Link::Timeline& raw() { return m_state; }

    private:
        static std::chrono::microseconds sampleTimeAt(
            std::chrono::microseconds bufStart, int i, double sr) {
            return bufStart + std::chrono::microseconds(
                static_cast<long long>(std::round(1e6 * i / sr)));
        }
        ableton::Link::Timeline m_state;
        double                      m_quantum;
    };

    // ─────────────────────────────────────────────────────────────────────────

    ofxAbletonLink();
    ~ofxAbletonLink();

    ofxAbletonLink(const ofxAbletonLink&)            = delete;
    ofxAbletonLink& operator=(const ofxAbletonLink&) = delete;
    ofxAbletonLink(ofxAbletonLink&&)                 = delete;
    ofxAbletonLink& operator=(ofxAbletonLink&&)      = delete;

    void setup(double bpm);

    // ── Tempo / quantum ───────────────────────────────────────────────────────
    void   setTempo(double bpm);
    double tempo();

    void   setQuantum(double quantum);
    double quantum();

    // ── Enable / peers ────────────────────────────────────────────────────────
    bool        isEnabled() const;
    void        enable(bool bEnable);
    std::size_t numPeers();

    // ── Start / stop ──────────────────────────────────────────────────────────
    void enableStartStopSync(bool bEnable);
    bool isStartStopSyncEnabled() const;
    void setIsPlaying(bool bPlaying);
    bool isPlaying() const;

    // ── App-thread polling (call from update()) ───────────────────────────────
    Status update();

    // ── Audio-thread session state (call from audioOut / pdsp callback) ───────
    // captureAudioSessionState() is real-time safe (no locks).
    // Call once per audio buffer, iterate over samples using the helpers above,
    // then call commitAudioSessionState() if you changed tempo or playing state.
    SessionState captureAudioSessionState();
    void         commitAudioSessionState(SessionState& state);

    // App-thread equivalents (NOT real-time safe — use from update() / draw())
    SessionState captureAppSessionState();
    void         commitAppSessionState(SessionState& state);

    // ── Clock ─────────────────────────────────────────────────────────────────
    // link.clock().micros() → current time as std::chrono::microseconds.
    // Store this at the START of your audio callback and pass to beatAtSample().
    ableton::Link::Clock clock() const { return link ? link->clock() : ableton::Link::Clock{}; }

private:
    ableton::Link* link            { nullptr };
    double         quantum_        { 4.0 };
    bool           startStopSyncEnabled_ { false };
    bool           isPlaying_      { false };
};
