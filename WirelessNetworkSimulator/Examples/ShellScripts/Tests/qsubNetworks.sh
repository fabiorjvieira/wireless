i=1
while [ ${i} -le 100 ]
do
   echo "qsub -N ${i}${1}${2} -S /bin/bash -q all.q -o /home/farofino/Output/ -e /home/farofino/Output/ $2 ${i} $3 $1 $4 $5 $6 $7 $8 $9"
   i=$(( $i + 1 ))
done

