#!/bin/bash
DIR=${1}
# init
# look for empty dir

find ${DIR} -type d | while read line
do 
   if [ "$(ls -A ${line})" ]
   then
      sleep 0;
   else
      echo "ls ${line} "
   fi
done

