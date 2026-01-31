# Tic Tac Toe Game in C++ (SFML)

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Classes & Components](#classes--components)
- [Gameplay Modes](#gameplay-modes)
- [File Management](#file-management)
- [Dependencies](#dependencies)
- [Future Enhancements](#future-enhancements)
- [License](#license)

---

## Overview
This project is a **Tic Tac Toe game developed in C++** using the **SFML graphics library**.  
It provides a modern GUI with multiple gameplay modes including AI, classic, tournament, and speed battles.  
The game also supports **user login and sign-up** functionality with persistent score tracking.

---

## Features
- User authentication (Login & Sign-Up)
- Persistent score saving using files (`player_data.txt`)
- Multiple gameplay modes:
  - Classic Mode (2 players)
  - AI Mode
  - Tournament Mode
  - Speed Mode (timed games)
- Grid size customization (3x3, 4x4, 5x5, up to 8x8)
- Simple AI opponent with basic strategy
- Interactive GUI with clickable buttons
- Real-time game status updates and visual feedback
- Professional UI with background images and custom fonts

---

## Installation

### Prerequisites
- C++ compiler (C++11 or higher)
- [SFML 2.6](https://www.sfml-dev.org/)
- Standard C++ libraries: `iostream`, `fstream`, `vector`, `string`

### Steps
1. Clone the repository:
```bash
git clone https://github.com/tlhasami/TicTacToeGUI.git
cd tic-tac-toe-sfml
