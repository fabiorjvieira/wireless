inFile=${1}
outFile=${2}
ini=$3
end=$4

rm -f ${inFile}.out
i=${ini}

sed "s/qsub/qsub -q all.q@nodeID -hold_jid/g" ${inFile} | grep "qsub" > ${inFile}.tmp
cat ${inFile}.tmp |
while read fileLine
do
   echo ${fileLine} | sed "s/nodeID/node${i}/g" >> ${inFile}.out
   if [ ${i} -lt ${end} ]
   then
      let i+=1
   else
      i=${ini}
   fi
done

i=${ini}
j=${ini}
rm -f ${inFile}.tmp

cat ${inFile}.out |
while read fileLine
do
   eval array[${i}]=`echo ${fileLine#* -N } | cut -d" " -f 1`
   if [ ${i} -le ${end} ]
   then
      echo ${fileLine} | sed "s/\-hold_jid//g"  >> ${inFile}.tmp
   else
      echo ${fileLine} | sed "s/hold_jid/hold_jid ${array[${j}]}/g" >> ${inFile}.tmp
      let j+=1
   fi
   let i+=1
done

cat ${inFile}.tmp > ${outFile}
rm -f ${inFile}.out
rm -f ${inFile}.tmp
