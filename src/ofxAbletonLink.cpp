/* Copyright 2016, Akihiro Komori. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#include "ofxAbletonLink.h"
#include <algorithm>

ofxAbletonLink::ofxAbletonLink()
    : link(nullptr), quantum_(4.0) {}

ofxAbletonLink::~ofxAbletonLink() {
    if (link) { link->enable(false); delete link; }
}

void ofxAbletonLink::setup(double tempo) {
    if (link) { link->enable(false); delete link; }
    link = new ableton::Link(tempo);
    link->setNumPeersCallback([](std::size_t) {});
    link->setTempoCallback([](double) {});
    link->enable(true);
}

// ── Tempo ─────────────────────────────────────────────────────────────────────
void ofxAbletonLink::setTempo(double bpm) {
    if (!link) return;
    auto state = link->captureAppTimeline();
    state.setTempo(bpm, link->clock().micros());
    link->commitAppTimeline(state);
}

double ofxAbletonLink::tempo() {
    if (!link) return 120.0;
    return link->captureAppTimeline().tempo();
}

// ── Quantum ───────────────────────────────────────────────────────────────────
void   ofxAbletonLink::setQuantum(double q) { quantum_ = q; }
double ofxAbletonLink::quantum()            { return quantum_; }

// ── Enable / peers ────────────────────────────────────────────────────────────
bool        ofxAbletonLink::isEnabled() const  { return link && link->isEnabled(); }
void        ofxAbletonLink::enable(bool v)     { if (link) link->enable(v); }
std::size_t ofxAbletonLink::numPeers()         { return link ? link->numPeers() : 0; }

// ── Start / stop ──────────────────────────────────────────────────────────────
void ofxAbletonLink::enableStartStopSync(bool v) {
    startStopSyncEnabled_ = v;
    // Start/stop sync not supported in this version of the Link library
}
bool ofxAbletonLink::isStartStopSyncEnabled() const { return startStopSyncEnabled_; }

void ofxAbletonLink::setIsPlaying(bool v) {
    isPlaying_ = v;
    // isPlaying not supported in this version of the Link library; tracked locally only
}
bool ofxAbletonLink::isPlaying() const { return isPlaying_; }

// ── App-thread polling ────────────────────────────────────────────────────────
ofxAbletonLink::Status ofxAbletonLink::update() {
    Status s;
    if (!link) return s;
    const auto now   = link->clock().micros();
    const auto state = link->captureAppTimeline();
    s.beat      = state.beatAtTime(now, quantum_);
    s.phase     = state.phaseAtTime(now, quantum_);
    s.tempo     = state.tempo();
    s.isPlaying = isPlaying_;
    return s;
}

// ── Audio-thread session state ────────────────────────────────────────────────
ofxAbletonLink::SessionState ofxAbletonLink::captureAudioSessionState() {
    return SessionState(link->captureAudioTimeline(), quantum_);
}
void ofxAbletonLink::commitAudioSessionState(SessionState& s) {
    link->commitAudioTimeline(s.raw());
}

// ── App-thread session state ──────────────────────────────────────────────────
ofxAbletonLink::SessionState ofxAbletonLink::captureAppSessionState() {
    return SessionState(link->captureAppTimeline(), quantum_);
}
void ofxAbletonLink::commitAppSessionState(SessionState& s) {
    link->commitAppTimeline(s.raw());
}
