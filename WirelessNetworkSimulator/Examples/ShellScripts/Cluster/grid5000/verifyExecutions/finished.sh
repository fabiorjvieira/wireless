find ${1} -name "${2}" -type f | while read line
do
   lastLine=`eval tail -n1 ${line}`
   finish=`eval echo ${lastLine} | cut -d "," -f 1`
   echo ${line} ${lastLine} ${finish}
   if [ "${finish}" = "${3}" ]
   then
      a=0
   else
      echo "rm -v ${line}"
   fi
done

