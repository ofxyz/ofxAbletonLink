# Asio (standalone, header-only)

Vendored copy of [Asio](https://github.com/chriskohlhoff/asio) used by the Ableton Link SDK.

- Version: **1.36.0** (`asio-1-36-0`, commit `231cb29bab30f82712fcd54faaea42424cc6e710`)
- License: Boost Software License 1.0 (`LICENSE_1_0.txt`)

Only the public headers from `asio/include` are bundled here so the addon builds without
initialising Link's nested `modules/asio-standalone` submodule.

When updating the bundled Link SDK, check whether Link expects a newer Asio version and
refresh this directory from `libs/link/modules/asio-standalone/asio/include` if needed.