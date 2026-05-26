# Noclip Toggle

A toggleable noclip mod for Geometry Dash with built-in anti-cheat measures.

## Features

- **Pause menu button** — toggle noclip on/off without leaving the level
- **Red dot cheat indicator** — a small red "NOCLIP" dot appears at the bottom of the screen whenever noclip is active, so observers can see you're using a cheat
- **Forced death at 97–99%** — if noclip is on when you reach that range, the game kills you instead of letting you complete the level, preventing illegitimate completions
- **Level completion blocked** — if somehow `levelComplete` fires while noclip is on, it is intercepted and converted into a death

## How to Use

1. Start a level and open the **pause menu**
2. Press the **Noclip: OFF** button to turn noclip on (it turns green/red to indicate state)
3. A red dot labeled **NOCLIP** will appear at the bottom-center of the screen
4. Toggle it back off the same way

## Notes

- Noclip is automatically disabled whenever you exit a level
- Deaths at 97–99% are intentional — this is the anti-cheat in action
- The state does **not** persist between game sessions

## Building

```bash
# Install Geode CLI first: https://docs.geode-sdk.org/getting-started/
geode build --config Release
```

The `.geode` file will be in `dist/`.
