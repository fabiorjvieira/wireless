inDirectoryName=$1
outDirectoryName=$2
fileName=$3
index=$4

find ${inDirectoryName} -type f -name "${fileName}" | rev | cut -d '/' -f 1 | sort -u | rev | while read files
do
   echo ${files}
   rm -f ${outDirectoryName}/${files}.tmp;
   find ${inDirectoryName} -name "${files}" | sort | while read file
   do
      echo ${file}
      lastLine=`eval grep -n "^[0-9]*,${index}," ${file} | tail -n1 | cut -d ':' -f 1`;
      if [ "${lastLine}" != "" ]
      then
         head -n ${lastLine} ${file} >> ${outDirectoryName}/${files}.tmp;
      fi
      #rm ${file}
   done

   outFile=`eval find ${outDirectoryName} -maxdepth 1 -name "${files}"`;
   lastLine=0
   if [ "${outFile}" != "" ]
   then
      lastLine=`eval tail -n1 ${outFile} | cut -d ',' -f 1`;
   fi

   echo ${lastLine}

   cat ${outDirectoryName}/${files}.tmp | grep -v Performance | cut -d"," -f 2- | sed "s/^/,/" | while read line
   do
      echo ${lastLine}${line} >> ${outDirectoryName}/${files}
      incr=`eval echo ${line} | cut -d "," -f 2`
      if [ "${incr}" == "${index}" ]
      then
         lastLine=$(( $lastLine + 1 ))
      fi
   done
   rm -f ${outDirectoryName}/${files}.tmp;
done

