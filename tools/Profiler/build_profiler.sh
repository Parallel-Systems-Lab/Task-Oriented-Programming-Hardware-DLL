#!/bin/bash
rm -f a.out
make
mv a.out profiler.elf
read -p "Press [Enter] key to continue..."