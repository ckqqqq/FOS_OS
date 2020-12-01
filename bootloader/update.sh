#!/bin/bash

cd ../kernel
make clean
make
cd ../bootloader

sudo mount -o loop ./boot.img  ~/fd/
sudo cp ../kernel/kernel.bin ~/fd/kernel.bin
sync
sudo umount ~/fd/


