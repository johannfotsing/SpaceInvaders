This project aims to emulate the Intel 8080 processor chip in order to play the famous SpaceInvaders game.
This project is interesting for learning basics of computer architecture.
The project will be written mainly in C.
This work is inspired by the tutorial written by @emulator101 available on [Emulator101](www.emulator101.com) website.

ISSUES & TODO
-------------

    - [ ] How to implement interruptions ?
    - [ ] How to impose a frequency to the CPU ?
    - [x] Run CPU diagnostics
    - [x] Update flags is not well crafted for substraction operations
    - [x] Auxiliary carry is not taken into account in update flags
    - [x] Write specific add and substract functions to properly handle carry and auxilliary carry flags ?
    - [x] Separate public interface from private interface in Cpu8080 and all others
