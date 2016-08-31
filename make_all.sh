#!/bin/bash
if [ "$#" -eq  "0" ]
   then
 USER_APP=user_app
 else
USER_APP="$1"
 fi
 
set -e
./make_tools.sh
./Compile_n_run.sh ${USER_APP}




