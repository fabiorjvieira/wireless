bash /home/farofino/idSGE.sh

i=${1}
while [ ${i} -le ${2} ]
do
   echo ${i} 
   qsub -S /bin/bash -q all.q@node${i} -o /home/farofino/Output/ -e /home/farofino/Output/ $1 $2 $3 $4 $5 $6 $7 $8 $9
   i=$(( $i + 1 ))
done

