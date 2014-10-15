maxSlots=${1}
subFile=${2}
params=${3}

rm ${subFile}.PMC.out

eval submited=`oarstat -u $USER | grep -v "^-\|^Job" | wc -l`
let freeSlots=${maxSlots}-${submited}

cat ${subFile} | while read subLine
do
   echo free slots ${freeSlots}
   while [ $freeSlots == 0 ]
   do
      echo zero free slots. sleeping...
      sleep 3
      eval submited=`oarstat -u $USER | grep -v "^-\|^Job" | wc -l`
      let freeSlots=${maxSlots}-${submited}
   done
   let freeSlots-=1
   echo ${subLine} > ${subFile}.in
   bash ${subFile}.in ${params}
   rm ${subFile}.in
   echo ${subLine} >> ${subFile}.PMC.out
done

