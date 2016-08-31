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
echo "running code to generate log files"
sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./scheduler.elf table.bin 3 2&> ./log.txt
cat ./log.txt
echo moving log file
cp  ./log.txt ./../../../tools/Profiler/text.txt
echo moving task.map 
cp ./../src/task.map ./../../../tools/Profiler/task.map
cd ./../../../tools/Profiler
echo running profiler
# hide hardware dll 
#./profiler.elf 0
./profiler.elf 0
#read -p "Press [Enter] key to continue..."