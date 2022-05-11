# DiskOS
DiskOS is a simple 32bit [maybe 64bit] operating system created for a school project. Its first purpose was to communicate with ATA hard drives in order to send commands to them but in the future I plan on adding more functionalities to it, like paging, file system support and maybe even a GUI.
## Functionalities
|State|Functionality
:------------ | :-------------
:heavy_check_mark: | Starting in 16bit real mode  
:heavy_check_mark: | Loading more sectors from disk to load the 2'nd stage of the bootloader  
:heavy_check_mark: | A20 Line  
:heavy_check_mark: | 32bit Protected Mode  
:heavy_check_mark: | Printing in pMode using assembly language
:x: | Loading kernel
:x: | Higher half kernel
:x: | Paging
:x: | Memory manager
:x: | FAT32 support
:x: | Other file system support
:x: | VESA support
## How to Build and Run
### Requirements
* gcc
* nasm
* qemu
### Building and Running
To build and run DiskOS go into its main folder("DiskOS") and run "make".  
DiskOS will be built and run automatically.
## Screenshots
![Algorithm schema](./screenshots/scr1.png)
