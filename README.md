# Strategic "Base Conquest" Game Program in C++
This program is written in C++ and is designed to run on the Ubuntu operating system.<br>
It serves as a representation of a player in a strategic board game where two programs<br>
compete against each other, aiming to destroy the opponent's base.
## Overview:
Players, represented by programs, have a base capable of producing units. The primary objective is to destroy the opponent's base within a maximum of 2000 turns (1000 for each player). If the game exceeds the maximum turns without destroying a base, the player with the most units on the board wins. The game takes place on a grid defined by a text file (mapa.txt).

## Key Mechanics:
## Board:
Defined by mapa.txt (e.g., 32x32 tiles). Each unit, building, or obstacle occupies one tile.
## Mediator:
Controls the game by running player programs with: ./<player_program> mapa.txt status.txt rozkazy.txt [time_limit]
## Player Program Tasks:
Read board state and plan unit actions, saving these to an output file (rozkazy.txt), all within a time limit (default: 5 seconds).
## Game Status:
Stored in status.txt, detailing the gold amount and unit status.
## Units:
Various types, including Knight, Swordsman, Archer, etc., with unique stats and actions. Units can be produced during the game.
## Movement and Combat:
Each unit has movement and attack rules. Units can make several moves in one turn, but only one attack.
## Resources:
Players start with 2000 gold units, which are spent to produce combat units. Gold can be mined by Worker units on specific map tiles.
## File-based Communication:
All interactions between programs are exclusively managed through text files, such as mapa.txt, status.txt, and rozkazy.txt.



