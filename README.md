# SwampThing
Beginning of a Juno-106 inspired DCO monosynth for UF AES. Built around Raspberry Pi Pico Microcontroller

## Component Diagram:
![Component Diagram](img/component_diagram.png "Component Diagram")

## Installation / Build

### Prereqs
- [VS Code](https://code.visualstudio.com/)
- [Raspberry Pi Pico VS Code Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)

### Build Steps
1. Clone this repo
2. Open the `src` folder in VS Code (not the root folder!)
3. Open a PowerShell terminal in VS Code
4. Run:
```powershell
mkdir build
cd build
cmake -G "Ninja" ..
cmake --build .
```
5. The `.uf2` file will be in `build/SwampThing.uf2`

### Quick Build & Flash
- Press `Ctrl+Shift+B` to build and flash in one step - nifty!

### Manual Flashing
1. Hold BOOTSEL on your Pico and plug it in
2. Drag `SwampThing.uf2` onto the Pico drive

### Notes:
- Clock output: GPIO 13
- CV output: GPIO 14

### More to come!