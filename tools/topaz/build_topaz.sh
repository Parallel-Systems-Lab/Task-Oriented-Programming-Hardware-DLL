#!/bin/bash

rm ./bin/topaz.elf

set -e

ESDK=${EPIPHANY_HOME}
ELIBS=${ESDK}/tools/host/lib
EINCS=${ESDK}/tools/host/include
ELDF=${ESDK}/bsps/current/fast.ldf
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
${CROSS_PREFIX}gcc ./src/table.c  ${FLAGS} -o ./bin/topaz.elf -I ${EINCS}  -L ${ELIBS} -le-hal #-le-loader
echo 'Topaz build succefuly located at' 

echo $PWD'/bin/'

read -p "Press [Enter] key to continue..."