#!/bin/bash
if [ "$#" -eq  "0" ]
   then
 USER_APP=user_app
 else
USER_APP="$1"
 fi
set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib:${LD_LIBRARY_PATH}
EHDF=${EPIPHANY_HDF}
cd ./user_code/

rm -f ./${USER_APP}/dump/*
echo
echo
echo ---------------source FILE --------------------
cat ./${USER_APP}/src/*.c
echo
echo
echo ---------------source FILE --------------------
echo ---------------task map FILE --------------------
cat ./${USER_APP}/src/task.map
echo
echo
echo ---------------task map FILE --------------------
echo ---------------CODE README FILE --------------------
cat ./${USER_APP}/READ_ME.txt
echo
echo
echo ---------------CODE README FILE --------------------
#read -p "Press [Enter] key to continue..."
cd ./${USER_APP}/Debug
cp  ../../../tools/scheduler/bin/scheduler.elf ./
echo execution
sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./scheduler.elf table.bin 3
#read -p "Press [Enter] key to continue..."
cd ../../../
