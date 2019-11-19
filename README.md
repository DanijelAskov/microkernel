# MicroKernel

![Demonstration](demo/demo.gif)

## Description

This project is an implementation of a small, but *fully functional* **multithreaded operating system kernel** with **time sharing**.
More precisely, it contains the source code of kernel's **thread management subsystem**.

User application ([`user.cpp`](src/user.cpp)) and the kernel are compiled and linked into a single console application (`m-kernel.exe`).
Static library `lib/APPLICAT.lib` contains the implementation of system's *scheduler*.
You can see *scheduler*'s interface in [`scheduler.h`](h/schedule.h).

The full project specification can be found [here](https://1drv.ms/b/s!AuZ7wmWsDfythjdUikBibqfRTVH2) (Serbian language only).

## Setup

This project uses a very old C++ compiler, **B**orland **C**++ **C**ompiler (BCC), version 3.1, from 1992.
BCC provides a simple way of tempering with interrupt vector table (IVT) on MS DOS (Microsoft **D**isk **O**perating **S**ystem), which is necessary in order to implement time-sharing functionality.

1. Make sure you have 32-bit Windows (installed or available as a virtual machine)
2. Download BCC 3.1 from [here](https://1drv.ms/u/s!AuZ7wmWsDfythjZTNW7PU0fB466C?e=Xtk0uW)
3. Extract the archive to `C:\bc31\`
4. Move the content of this repository to `C:\project\`
5. Open command prompt, navigate to `C:\project\` and run `build.bat`
6. Run `run.bat`

## Note to the reader of this README

If you have any question, suggestion or collaboration offer, please feel free to [contact me](mailto:danijel.askov@gmail.com).