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


for f in ./*
do
  echo "ploting file $f ..."
  # take action on each file. $f store current file name
  hd $f 
done
cd ../../../
