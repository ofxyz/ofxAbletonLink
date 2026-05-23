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

To install ofxAbletonLink, move the ofxAbletonLink folder to your `of/addons/` folder.
A compiler that supports C++ 11 is required.

ofxAbletonLink relies on `link`, `asio-standalone` and `catch` as submodules. After checking out the
main repositories and change the current directory into `ofxabletonlink`, those submodules have to be loaded using
```
git submodule update --init
```

## Examples

- `example-basic`: The simplest demo. This demo acts with other Ableton Link applications.
- `example-animation`: A sprite animation speed control demo. This demo acts with other Ableton Link applications. (Thank you [Irasutoya](http://www.irasutoya.com/))

## Demo

This gif shows the state of tempo synchronization between our `example-basic` and  Ableton's `QLinkHut`.

<img src="https://i.gyazo.com/95d1fde2180d1f6b1156bfe96196c1c5.gif" alt="https://gyazo.com/95d1fde2180d1f6b1156bfe96196c1c5" width="600">

## Note

This addon was tested on macOS Sierra only.
