#!/bin/bash
 if [ "$#" -eq  "0" ]
   then
 USER_APP=user_app
 else
USER_APP="$1"
 fi

cd ./user_code/
cd ./${USER_APP}
mkdir -p ./input
mkdir -p ./dump
rm -f ./Debug/table.bin
rm -f ./Debug/*
rm -f ./dump/*

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
#:${LD_LIBRARY_PATH}
EINCS=${ESDK}/tools/host/include
#ELDF=${ESDK}/bsps/current/internal.ldf
ELDF=${ESDK}/bsps/current/fast.ldf
#ELDF=${ESDK}/bsps/current/legacy.ldf
EHDF=${EPIPHANY_HDF}
#compile host code
#for debug -g 
# no optimizations -O0
# full optimization -O3
FLAGS="-g -O0"
SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac
#make dec.h table
>./src/dec.h
while read -r line || [[ -n $line ]]
do
set -- $line
VARNAME=${line%(*}
VARNAME=${VARNAME%(*}
VARNAME=${VARNAME##* }
VARNAME="${VARNAME%$'\r'*}"
if [ "$VARNAME" != "" ];then
echo 'extern void '$VARNAME'(void);'>>./src/dec.h
fi
done < ./src/task.map
echo 'dec.h created'


e-gcc -T ${ELDF} -c ./src/usercode.c ${FLAGS} -o ./Debug/e-usercode-temp.o -le-lib
echo 'DEVICE CODE COMPILED' 
e-gcc -T ${ELDF}  ../../tools/scheduler/bin/e-scheduler.o ./Debug/e-usercode-temp.o ${FLAGS} -o ./Debug/e-usercode.elf -le-lib
echo 'DEVICE CODE linked'
rm -f ./Debug/e-usercode-temp.o

#make table of function names and adresses
>./Debug/lookUpTable.txt

while read -r line || [[ -n $line ]]
do
set -- $line
VARNAME=${line%(*}
#echo $VARNAME
VARNAME=${VARNAME%(*}
#echo $VARNAME  
VARNAME=${VARNAME##* }
#echo $VARNAME 
VARNAMES=_${VARNAME}$
#echo $VARNAMES 
TEMP=$(objdump -t ./Debug/e-usercode.elf | grep ${VARNAMES} | awk '{print $1 ;}')
#echo $VARNAME $TEMP
echo $VARNAME $TEMP >>./Debug/lookUpTable.txt
done < ./src/task.map
echo 'user code tasks extracted form symbol table'
# run Topaz to compile task map
cd ../../tools/topaz/bin/

sudo -E LD_LIBRARY_PATH=${ELIBS} EPIPHANY_HDF=${EHDF} ./topaz.elf ..//..//..//user_code//${USER_APP}//src//task.map ..//..//..//user_code//${USER_APP}//Debug//lookUpTable.txt ..//..//..//user_code//${USER_APP}//Debug//table.bin 1
cd ../../../user_code/${USER_APP}
echo task map compiled
# Convert device ELF to SREC file
e-objcopy --srec-forceS3 --output-target srec ./Debug/e-usercode.elf Debug/e-usercode.srec
echo 'user code elf converted to srec'
#make input and output records
echo generating INPUT pointers from input.rec
>./Debug/inputTable.txt

while read -r line || [[ -n $line ]]
do
set -- $line
VARNAME=${line%(*}
VARNAMES=_${VARNAME}
#echo $VARNAMES
TEMP=$(objdump -t ./Debug/e-usercode.elf | grep " ${VARNAMES}" | awk '{print $1,$5 ;}') 
#echo $VARNAME $TEMP
echo $VARNAME $TEMP >>./Debug/inputTable.txt
done < ./src/input.rec
echo 'input rec extracted form symbol table'
cat ./Debug/inputTable.txt
#read -p "Press [Enter] key to continue..."
echo generating OUTPUT pointers from output.rec
>./Debug/dumpTable.txt

while read -r line || [[ -n $line ]]
do
set -- $line
VARNAME=${line%(*}
VARNAMES=_${VARNAME} 
#echo $VARNAMES
TEMP=$(objdump -t ./Debug/e-usercode.elf | grep " ${VARNAMES}" | awk '{print $1,$5 ;}') 
#echo $VARNAME $TEMP
echo $VARNAME $TEMP >>./Debug/dumpTable.txt
done < ./src/output.rec
echo 'output rec extracted form symbol table'
cat ./Debug/dumpTable.txt


echo 'done!'
cd ..
#read -p "Press [Enter] key to continue..."




