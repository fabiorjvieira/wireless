network=1
program=${1}
type=${2}
buffer=${3}
lastBuffer=$(( ${3} - 1 ))

while [ ${network} -le 100 ]
do
   echo "qsub -hold_jid D${network}${type}${lastBuffer} -N A${network}${type}${buffer} -S /bin/bash -q all.q -o /home/farofino/Output/ -e /home/farofino/Output/ ${program} ${network} ${type} A ${buffer}"
   echo "qsub -hold_jid A${network}${type}${buffer} -N B${network}${type}${buffer} -S /bin/bash -q all.q -o /home/farofino/Output/ -e /home/farofino/Output/ ${program} ${network} ${type} B ${buffer}"
   echo "qsub -hold_jid B${network}${type}${buffer} -N C${network}${type}${buffer} -S /bin/bash -q all.q -o /home/farofino/Output/ -e /home/farofino/Output/ ${program} ${network} ${type} C ${buffer}"
   echo "qsub -hold_jid C${network}${type}${buffer} -N D${network}${type}${buffer} -S /bin/bash -q all.q -o /home/farofino/Output/ -e /home/farofino/Output/ ${program} ${network} ${type} D ${buffer}"
   network=$(( ${network} + 1 ))
done

