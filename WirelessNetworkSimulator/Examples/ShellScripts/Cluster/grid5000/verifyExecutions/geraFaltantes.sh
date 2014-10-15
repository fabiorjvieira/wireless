networkSize=$2
order="$3"
buffer=$4
#cluster=$5

echo 'if [ ! "${2}" == "" ]'
echo "then"
#   export cluster="cluster='${2}'"
echo "   export cluster=\"cluster='\${2}'\""
echo "fi"
echo "walltime=\${1};"

cat ${1} | while read line
do
   bash allABR.sh ${order} ${buffer} ${line} ${line} ${networkSize} WirelessNetworkSimulator #"13:40:00" ${cluster}
done

