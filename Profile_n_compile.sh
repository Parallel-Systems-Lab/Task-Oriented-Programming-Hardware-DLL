#!/bin/bash
if [ "$#" -eq  "0" ]
   then
 USER_APP=user_app
 else
USER_APP="$1"
 fi
 
./compile.sh ${USER_APP}
./profiler.sh ${USER_APP}

