#!/bin/bash

rm ./bin/*.o
rm ./bin/*.elf

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
#:${LD_LIBRARY_PATH}
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf
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


# Build HOST side application
${CROSS_PREFIX}gcc  ./src/scheduler.c ./src/table.c ./src/timing.c ./src/io_handle.c  ${FLAGS} -o ./bin/scheduler.elf   -I ${EINCS} -L ${ELIBS} -le-hal -lrt -le-loader
echo 'HOST CODE COMPILED' 

#compile device code 
e-gcc -T ${ELDF} -c ./src/e_main_sched.c   ${FLAGS} -o ./bin/e-scheduler.o -le-lib
echo 'DEVICE scheduler COMPILED'
read -p "Press [Enter] key to continue..."