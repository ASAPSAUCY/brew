# Memory Quest: Kyle & Claudia

*Four years in the making.*

A cozy exploration game — Animal Crossing meets Pokémon — about Kyle and Claudia's four years together. Built for **Unreal Engine 5.8**, written entirely in C++ with **zero binary assets**: the whole island, every creature, every line of dialogue and the UI are constructed in code at startup.

## The game

You play as Kyle, exploring a little island of your shared history with Buddy trotting at your heels. Your memories as a couple have gotten loose and wander the world as friendly creatures. Catch them, collect keepsakes, and when you've gathered enough of your story — go finish the one escape room you two never finished. (We all know why.)

**Five zones, five eras:**

| Zone | The era |
|---|---|
| **The Apartment** (hub) | Where he finally asked. Home of the fountain, the seafood-pasta table, and The Final Room |
| **Hallow Grove** | The Halloween party — a dark angel, a "sexy" police officer, and two whole years of "just friends" |
| **The Courts** | Mid-COVID basketball and billiards. Same team, every time |
| **Little New York** | The birthday weekend that changed everything (and the bottomless brunch of 2023) |
| **Cozy Park** | Picnics, bee sting pizza, golf, movie nights, the Montreal Corner in fall colors — and Rumor's memorial tree |

**What's in it:**

- **13 catchable memory creatures** (Memorydex) — from Bogey the Golf Gremlin to Mimossy the Brunch Spirit — each carrying a memory card about a real moment
- **Gentle Pokémon-style battles**: tease, sweet-talk, then win them over with a treat. Nothing ever faints; worst case you get flustered and it scampers off
- **8 keepsakes** for the scrapbook (the subway token, the suspicious golf scorecard...)
- **NPC villagers** who tell the story — including Claudia herself, who has *notes*
- **Rumor** 🐾 — a quiet tribute in the park. Sit with him; he joins the Memorydex as its one special entry
- **The Final Room**: unlocks at 8 memories + 6 keepsakes. Press the pedestals in the order your story happened, claim the trophy, and finally finish an escape room together

## Controls

| Action | Keyboard | Gamepad |
|---|---|---|
| Move | `W` `A` `S` `D` | Left stick |
| Jump | `Space` | Bottom face button |
| Interact / talk / advance dialogue | `E` | Left face button |
| Scrapbook | `Tab` | Top face button |
| Battle choices | `1` `2` `3` `4` | D-pad Up / Right / Down / Left |

The camera is a fixed cozy top-down follow camera — no mouse needed.

## Building and playing

Requirements: **Unreal Engine 5.8** (Epic Games Launcher), and Visual Studio 2022 with the *Game development with C++* AND *.NET desktop development* workloads (Windows) or Xcode (macOS).

1. Double-click `MemoryQuest.uproject`.
2. Click **Yes** when asked to rebuild the missing modules; wait for the compile.
3. When the editor opens, press **Play**.

On a different UE version: right-click the `.uproject` → **Switch Unreal Engine version...** first.

Command line alternative:

```bat
"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" ^
    MemoryQuestEditor Win64 Development ^
    -project="C:\path\to\memory-quest\MemoryQuest.uproject" -waitmutex
```

## Editing your story

Everything personal lives in **one place**: `Source/MemoryQuest/StoryData.h` / `StoryData.cpp` — names, colors, every creature, memory card, keepsake, and line of dialogue. Change the text, recompile, press Play. No other code needs touching.

The world layout (buildings, trees, zones) lives in `Source/MemoryQuest/WorldBuilder.cpp` if you want to redecorate.

## Notes

- First Play compiles shaders — give it a minute.
- The editor viewport shows a nearly empty map: the world only spawns when you press Play (the project ships no map assets — the level is built by code).
- Happy anniversary, you two. Finish the room.
