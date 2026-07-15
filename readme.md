# ofxAbletonLink

> Version 4

ofxAbletonLink is an [Ableton Link](https://github.com/Ableton/link) addon for [openFrameworks](http://openframeworks.cc).

Ableton Link is a technology that keeps devices in time over a local network. This addon wraps the Link SDK, exposing beat, phase, tempo, peer count and transport start/stop state to your oF app.

## Bundled Link SDK

This addon bundles [Ableton Link](https://github.com/Ableton/link) as a submodule at `libs/link`.  
Current bundled version: **Link-3.1.5**

[Asio](https://github.com/chriskohlhoff/asio) 1.36.0 (header-only, standalone mode) is vendored at `libs/asio` so no nested submodule checkout is required to build.

## Platform support

| Platform | Status |
|---|---|
| Windows (msys2 / VS) | Supported |
| Linux 64-bit | Supported |
| Linux ARMv6/ARMv7 (Raspberry Pi) | Supported |
| macOS | Supported |

## Installation

Move the `ofxAbletonLink` folder into your `of/addons/` directory.

A C++11 (or later) compiler is required.

After cloning, initialise the Link submodule:

```
git submodule update --init
```

## API

```cpp
ofxAbletonLink link;

// setup with initial BPM
link.setup(120.0);

// in update()
auto status = link.update();
// status.beat      – beat position (continuous, fractional)
// status.phase     – phase within quantum [0, quantum)
// status.isPlaying – transport playing state

// tempo
link.setTempo(128.0);
double bpm = link.tempo();

// quantum (beats per bar / loop length)
link.setQuantum(4.0);

// transport start/stop sync (Link 3.x feature)
link.enableStartStopSync(true);
link.setIsPlaying(true);

// peers
std::size_t n = link.numPeers();

// enable/disable
link.enable(false);
```

## Examples

- `example-basic` — minimal tempo sync demo, interoperates with Ableton Live and other Link apps.
- `example-animation` — sprite animation speed driven by Link beat.

## Demo

This gif shows tempo synchronisation between `example-basic` and Ableton's `QLinkHut`.

<img src="https://i.gyazo.com/95d1fde2180d1f6b1156bfe96196c1c5.gif" alt="ofxAbletonLink demo" width="600">
