# Unreal Platformer

A third-person action platformer for **Unreal Engine 5.8**, written entirely in C++ with **zero binary assets** — no `.umap` levels, no `.uasset` meshes, materials, input actions, or UI. Everything you see and play is constructed in code at startup, using only the basic shapes that ship inside the engine.

## The game

Run, jump, and double-jump your way across a floating obstacle course:

- A hand-laid course of platforms rising into the sky
- Moving platforms (horizontal ferry and a vertical elevator) that you ride
- 18 spinning coins to collect, with a live counter
- Red hazard cubes and a bottomless drop — you have 3 lives
- Checkpoints that turn green and save your progress
- A spinning gold pillar at the end. Touch it to win.

## Controls

| Action | Keyboard / Mouse | Gamepad |
|---|---|---|
| Move | `W` `A` `S` `D` | Left stick |
| Camera | Mouse | Right stick |
| Jump / double jump | `Space` (press again mid-air) | Bottom face button (A / Cross) |
| Restart level | `R` | Menu / Start button |

If the vertical camera feels inverted to you, flip `bInvertLookY` in `PlatformerCharacter.h` (or on the character instance in the editor).

## Requirements

- **Unreal Engine 5.8** (install via the Epic Games Launcher)
- **Windows:** Visual Studio 2022 with the **Game development with C++** and **.NET desktop development** workloads (includes the Windows 10/11 SDK)
- **macOS:** Xcode

## Building and playing

### The quick way

1. Double-click `UnrealPlatformer.uproject`.
2. When prompted that modules are missing and need to be rebuilt, click **Yes** and wait for the compile.
3. When the editor opens, press **Play** (the toolbar button, or `Alt+P`).

> If you are on a different UE version, first right-click `UnrealPlatformer.uproject` → **Switch Unreal Engine version...** and pick your engine, then proceed as above.

### The IDE way (Windows)

1. Right-click `UnrealPlatformer.uproject` → **Generate Visual Studio project files**.
2. Open the generated `UnrealPlatformer.sln`.
3. Select the `Development Editor` configuration and `Win64` platform, then build the `UnrealPlatformer` project.
4. Double-click `UnrealPlatformer.uproject` and press **Play**.

### The command-line way

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" ^
    UnrealPlatformerEditor Win64 Development ^
    -project="C:\path\to\unreal-platformer\UnrealPlatformer.uproject" -waitmutex
```

(Adjust both paths. On macOS use `Engine/Build/BatchFiles/Mac/Build.sh` with the same arguments.)

## How it works (code-only architecture)

Because this repository contains no binary assets, the project bootstraps itself in an unusual but instructive way:

| Piece | Where a normal project uses assets | What this project does instead |
|---|---|---|
| Level | A `.umap` file | `Config/DefaultEngine.ini` boots the engine's built-in empty `Entry` map; `APlatformerGameMode::InitGame` spawns an `ACourseBuilder` that lays out every platform, coin, hazard, light, and the `PlayerStart` in `BuildCourse()` |
| Meshes & materials | Imported `.uasset` content | Engine built-ins (`/Engine/BasicShapes/Cube`, `Cylinder`) with `BasicShapeMaterial`, tinted per-actor via dynamic material instances |
| Input | `InputAction`/`InputMappingContext` data assets | `APlatformerCharacter::InitializeInputObjects()` builds the actions, mapping context, and key modifiers with `NewObject` at possession time |
| HUD | UMG widget blueprints | `APlatformerHUD::DrawHUD()` draws text directly to the canvas |
| Character visual | Skeletal mesh + animation blueprint | A scaled, tinted cube on the capsule (it reads better than you'd think) |

### Class overview

- `PlatformerGameMode` — score, lives, checkpoints, win/lose state; spawns the course
- `PlatformerCharacter` — spring-arm camera, tuned movement, double jump, runtime Enhanced Input
- `CourseBuilder` — the "level file": all layout data lives in `BuildCourse()`
- `PlatformBlock` / `MovingPlatform` — static and gliding colored geometry
- `CollectibleCoin`, `Checkpoint`, `Hazard`, `LevelGoal` — overlap-driven gameplay actors
- `PlatformerHUD` — canvas HUD

## Troubleshooting

- **The first Play takes a while** — the engine compiles shaders on first run; later runs are fast.
- **"YOU WIN!"/"GAME OVER" but nothing happens** — press `R` to reload the level.
- **Black screen or no world visible** — make sure you pressed Play; the editor viewport shows the mostly-empty `Entry` map, and the course only spawns when the game actually starts.
- **Editing the course** — tweak the coordinates in `CourseBuilder.cpp` (`BuildCourse()`); each `SpawnBlock(center, scale, color)` cube is 100 units at scale 1. Recompile and press Play.
