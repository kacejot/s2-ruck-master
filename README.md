# Ruck Master - Inventory Sorting Mod for STALKER 2

![](screenshots/title.jpg)

**Ruck Master** is a configurable inventory sorting system for  
**S.T.A.L.K.E.R. 2: Heart of Chornobyl**

It replaces the game's internal comparator with a rule-based sorting pipeline  
that is customizable in real time via an ImGui UI panel.

## Installation

Copy `winmm.dll` here:
```
S.T.A.L.K.E.R. 2 Heart of Chornobyl\Stalker2\Binaries\Win64
```

After first game shutdown or any change to the rules, ruck_master.ini will appear in the same folder. It will allow you to enable logs and modify sorting config without UI.

## How to use?

By default toggle key is hyphen or minus (`-`) button (the one to the right of 0).  
You can change it in .ini file on any key you want.

In overlay you can select sorting preset between Vanilla or Call of Pripyat. Or you could make your own configuration.  
You can change sorting rules order in the rules list by drag-and-drop them. Enable and disable them by clicking. The higher the rule - the more priority it has.  
You can change item type priorities in the same way (through drag-and-drop & clicking). Type priority list is enabled only if such rule is enabled in the rules list.

Your settings will be preserved between game launches. If something goes wrong, you will error window upon entering the overlay. Screenshot it and post an issue. If error occurs every time, please enable logs and attach log to the issue. Log is called `ruck_master_log.txt` and is located near `Stalker2.exe` (only after you enabled logs in .ini file).

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
