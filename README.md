# Noclip Toggle — Geode Mod

A toggleable noclip mod for Geometry Dash 2.2 built with Geode SDK.

## Features

| Feature | Details |
|---|---|
| Pause menu toggle | Button in the pause screen to flip noclip on/off |
| Cheat indicator | Red dot + "NOCLIP" label at screen bottom when active |
| Anti-cheat death | Forced death at 97–99% — you cannot complete a level with noclip on |
| Level complete block | `levelComplete` is intercepted and converted to a death if noclip is on |

## Building

### Prerequisites

- [Geode CLI](https://docs.geode-sdk.org/getting-started/geode-cli/) (`cargo install geode-cli`)
- CMake 3.21+
- C++20 toolchain (MSVC on Windows, Clang on macOS)
- Geode Loader installed in your GD copy

### Quick build (recommended)

```powershell
# Windows PowerShell — first-time setup
git clone <this-repo>
cd noclip-toggle
$env:GEODE_SDK = "$PWD/.geode-sdk"
geode sdk install $env:GEODE_SDK
geode sdk install-binaries --platform windows -v 4.0.0
geode build --config Release
```

```bash
# macOS / Linux
export GEODE_SDK="$PWD/.geode-sdk"
geode sdk install $GEODE_SDK
geode sdk install-binaries --platform MacOS -v 4.0.0
geode build --config Release
```

The compiled `.geode` file will appear in `dist/`.

### Install

Drag the `.geode` file onto the Geometry Dash window while it's open, or copy it into:

- **Windows:** `%LOCALAPPDATA%\GeometryDash\geode\mods\`
- **macOS:** `~/Library/Application Support/GeometryDash/geode/mods/`
- **Android:** `/sdcard/Android/data/com.robtopx.geometryjump/files/geode/mods/`

## File structure

```
noclip-toggle/
├── src/
│   └── main.cpp        ← all hooks live here
├── mod.json            ← mod metadata
├── CMakeLists.txt      ← build config
├── about.md            ← in-game description page
└── README.md           ← this file
```

## How it works

### Noclip core (`PlayLayer::destroyPlayer` hook)
When noclip is on, `destroyPlayer` returns early without calling the original, so the player ignores all hazards.

### Anti-cheat death (`PlayLayer::update` hook)
Every frame we call `getCurrentPercent()`. If noclip is active and the percentage is in the 97–99% range, noclip is turned off and `destroyPlayer` is called directly, ending the run.

### Level complete block (`PlayLayer::levelComplete` hook)
If noclip is somehow still on when `levelComplete` fires (e.g. via trigger), we intercept it, disable noclip, and kill the player instead.

### Pause button (`PauseLayer::init` hook)
We hook `PauseLayer::init`, find the left-side button menu via Geode's node IDs, and append a `CCMenuItemSpriteExtra` labelled **Noclip: ON/OFF**. The label updates live on click.

### Cheat indicator (`PlayLayer::init` + `update`)
A `CheatDot` node (a `CCDrawNode` red circle + `CCLabelBMFont`) is added to the `PlayLayer` with a high Z-order and positioned at the bottom-center. Its visibility tracks `g_noclipEnabled` every frame.

## License
MIT
