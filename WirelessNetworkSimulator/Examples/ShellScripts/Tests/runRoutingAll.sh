bash /home/farofino/idSGE.sh

i=1
execType=${1}

while [ ${i} -le 100 ]
do
   echo ${i} ${execType} 
   /home/farofino/WirelessNetworkSimulator/Release/WirelessNetworkSimulator${execType} ${networkSize} /tmp/fjimenez/Networks/${networkSize}/${i}/${networkSize}.${i} 1 5 > /tmp/fjimenez/Networks/Output/${networkSize}.${i}.${execType}.out
   i=$(( $i + 1 ))
done

