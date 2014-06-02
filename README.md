hard86
======

an emulator for the Intel 8086 microprocessor
---------------------------------------------

(C) 2014 Stephen Zhang - http://zsteve.phatcode.net

Hard86 is an emulator for the Intel 8086 microprocessor,
built primarily for use on Windows.

It is composed of two main components : the emulator and the frontend, or interface.
The emulator is the heart of hard86. Written in C and C++, it provides the opcode implementations, system
accessor functions, and many other things. The emulator component is mainly platform independent, relying
only on common platform facilities such as mutexes and threads.

The frontend is written in C++ and provides a user interface and platform dependent implementation of certain
parts of the program