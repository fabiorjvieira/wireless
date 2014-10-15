i=1

while [ $i -le 100 ]
do
   j=`eval find ${1} -type f -name "${2}\.$i\.*"| wc -l`
   echo ${i} , ${j}
   i=$(( $i + 1 ))
done

