# Celeste Sega Genesis Demake

A private, non-commercial technical study focused on porting the core mechanics of *Celeste* to the Sega Genesis/Mega Drive hardware.

> **Note:** This is a hobbyist learning project. There is no set release schedule, and it is intended for educational purposes only.

## 🖼️ Screenshots
<img width="1205" height="796" alt="Celeste Genesis Gameplay" src="https://github.com/user-attachments/assets/00f60461-bde5-45e8-9514-36035ae6c0db" />

## 🚀 Installation & Execution

You can run this project directly in your browser or on dedicated hardware/emulators.

1. **Play in Browser:** [Click here to launch the web-based emulator](https://darkjoy2k2-crypto.github.io/Celeste_demake/).
2. **Download the ROM:** Get the latest build from the [`out/rom.bin`](https://github.com/darkjoy2k2-crypto/Celeste_demake/blob/main/out/rom.bin) directory.
3. **Using an Emulator:** Open the `.bin` file with a Sega Genesis emulator (e.g., BlastEm, Genesis Plus GX, or Fusion).
4. **Using Original Hardware:** Copy the `rom.bin` file to a flash cartridge (like an EverDrive) and insert it into a Sega Genesis/Mega Drive console.

## 🎮 Controls

| Input | Action |
| :--- | :--- |
| **D-Pad Left/Right** | Move on ground |
| **D-Pad Up/Down** | Move while climbing walls |
| **Button A** | Jump |
| **Button B (Hold)** | Climb walls |
| **Button C** | Dash (while in air) |
| **Mode** | Toggle Fly State (Debug) |

## 🎯 Learning Objectives

The primary goal of this repository is to document the process of retro-game development. Key focus areas include:

* **C Programming:** Gaining proficiency in the C language within an embedded context.
* **Genesis Hardware:** Understanding the VDP (Video Display Processor) and memory constraints.
* **SGDK SDK:** Leveraging the [Sega Genesis Development Kit](https://github.com/Stephane-D/SGDK).
* **Game Physics:** Implementing high-precision platformer physics on 16-bit hardware.

## 🛠️ Project Status

### Achieved Features
* **Player Physics:** Glitch-free collision (16x16 sprite vs. 8x8 tiles).
* **Core Mechanics:** Jump, climb, dash, and a functional stamina system.
* **Quality of Life (QoL):** Implemented Coyote-time, buffered jumping, step-ups, and edge-correction.
* **Advanced Camera:** Dynamic camera following with player deadzones and area-based locking.

### To-Do List
* [ ] Death and Respawn mechanics
* [ ] Collectibles (Strawberries)
* [ ] Screen Effects (Fade-in/out, Camera Shake)
* [ ] In-Game Menu & Title Screen
* [ ] Level Transition system

## ⚖️ Copyright & Credits

### Concept & Resources
* **Original Game:** *Celeste* was created by Maddy Thorson and Noel Berry (Maddy Makes Games). This project is a tribute and a study; I make no claim to the original concept or intellectual property.
* **Graphics:** Currently utilizing edited tilesets from *Cavernas* by Adam Saltsman.

### Source Code
The code in this repository was authored by **Darkjoy2k2**, utilizing AI as a pair-programming tool for logic and optimization. No original *Celeste* source code was used, disassembled, or referenced.

## 🔗 Tools & Links
* **Development:** [Visual Studio](https://visualstudio.microsoft.com/de/)
* **Level Design:** [LDTK](https://ldtk.io/)
* **Engine/SDK:** [SGDK](https://github.com/Stephane-D/SGDK)
* **Original Game:** [Celeste on Steam](https://store.steampowered.com/app/504230/Celeste/)