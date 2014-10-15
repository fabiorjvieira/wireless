bash /home/farofino/idSGE.sh
echo 'begin'

network=${1}
type=${2}
routeType=${3}
buffer=${4}
networkSize=${5}

echo ${network} ${type} ${routeType} ${buffer}
cd /tmp/fjimenez
rm -f /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network}.*.${type}.perf
rm -f /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network}.${routeType}.${type}.perf.tgz
/home/farofino/WirelessNetworkSimulator/Release/WirelessNetworkSimulator${type} ${networkSize} /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network} 1 5 ${routeType} ${buffer} ${buffer} > /tmp/fjimenez/Networks/Output/${networkSize}.${network}.${routeType}.${type}.${buffer}.out
tar -czf /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network}.${routeType}.${type}.${buffer}.perf.tgz /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network}.*.${type}.perf
cp -v /tmp/fjimenez/Networks/${networkSize}/${network}/${networkSize}.${network}.${routeType}.${type}.${buffer}.perf.tgz /home/farofino/Result

