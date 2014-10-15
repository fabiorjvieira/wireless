inFile=b.out
outFile=a.out
maxLoop=${2}
multiply=3
fileList=${1}

rm -f $outFile

for file in ${fileList}
do
#   echo Processing ${file}
   tail -n8 ${file} | head -n4 | grep -v Total >>  ${outFile}
done

cut -d: -f2 ${outFile} > ${inFile}
let maxLoop=${maxLoop}*${multiply}
rm -f $outFile
i=0
j=0

cat $inFile |
while read inLine
do
   let i+=1
   let j+=1
   if [ ${j} ==  1 ]
   then
      a=${inLine}
   else
      if [ ${i} == 1 ]
      then
         a=${a}";"${inLine}
      else 
         a=${a}","${inLine}
      fi
   fi
   if [ ${i} ==  ${multiply} ]
   then
      i=0
   fi
   if [ ${j} ==  ${maxLoop} ]
   then
      echo ${a} >> ${outFile}
      a=""
      j=0
   fi
done

sed "s/;/	/g" ${outFile} | sed "s/,/-/g" > ${outFile}.tsf

#echo 
#echo Results
#cat ${outFile}
#echo 
#cat ${outFile}.tsf


