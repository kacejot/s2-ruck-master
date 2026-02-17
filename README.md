# Ruck Master — Advanced Inventory Sorting Mod for STALKER 2

![](screenshots/title.jpg)

**Ruck Master** is an advanced, fully configurable inventory sorting system for  
**S.T.A.L.K.E.R. 2: Heart of Chornobyl**, implemented as a **UE4SS C++ mod**.

It replaces the game's internal comparator with a modular, rule-based sorting pipeline  
that is fully customizable in real time via an ImGui UI panel.

This project provides:

- A complete re-implementation of the game’s item comparator in C++
- Modular sorting rules with deterministic ordering
- Drag-and-drop reordering of rules and item type priorities
- Live configuration via ImGui (accessible through UE4SS Debug Tools)
- Optional function call logging and original-comparator fallback
- Config presets (Vanilla, CoP-style, Custom) with runtime switching
- Saving/loading of the Custom preset via `.ini` file

## Build Instructions

1. Install **Visual Studio** with C++ and Windows SDK.
2. Clone the repo:
   ```bash
   git clone --recurse-submodules https://github.com/kacejot/s2-ruck-master
   ```
3. Configure CMake project:
   ```bash
   cd s2-ruck-master
   mkdir build && cd build
   cmake ..
   ```
4. After that locate .sln or .slnx file in your build folder, open it via VS2022 and build in `Release`

## Installation

Copy `winmm.dll` here:

```
S.T.A.L.K.E.R. 2 Heart of Chornobyl\Stalker2\Binaries\Win64
```

After game shutdown, you will be able to modify ruck_master.ini located in the same folder. It will allow you to enable logs and modify sorting config without UI.


## Screenshots
Vanilla does have gaps between items, because item size is ignored.
![](screenshots/vanilla.png)

So I added comparing by size from CoP and preset of the same name.<br>
And gaps are gone. But CoP ignored quest flag of the items.
![](screenshots/cop.png)

If you want to change the order or enable/disable the rules, you still can do this in Custom preset:
![](screenshots/custom.png)

## Motivation

The default inventory sorting in **S.T.A.L.K.E.R. 2** has several issues that make the inventory feel messy and inefficient.

Most importantly:

- After sorting, noticeable **“holes”** appear inside the grid.  
- Large items often end up placed between smaller ones, which in turn prevents optimal packing and creates even more gaps.
- The resulting layout feels inconsistent and wastes inventory space.

![](screenshots/holes.png)

**S.T.A.L.K.E.R.: Call of Pripyat** did not suffer from this problem — its sorting logic was far more structured and predictable.  
This led to the idea of **porting CoP’s comparator logic into STALKER 2**, rebuilding it from scratch, and extending it into a modular, configurable system.

For reference, [CoP’s original comparator can be found here](https://github.com/OpenXRay/xray-16/blob/ccfb30e45ddd9945ee49ce3dd8c0dc3ecd3038d6/src/xrGame/ui/UIInventoryUtilities.cpp#L61-L86).



The mod’s name, **Ruck Master**, is a direct homage to this classic implementation.

## Credits

Reverse engineering, mod architecture, comparator design, UI, and implementation by **kacejot**.<br>
Analysis, naming, refactoring, and architectural guidance assisted by **ChatGPT** and **Claude**.
