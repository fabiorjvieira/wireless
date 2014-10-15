origin=${1}
destiny=${2}
oldSuffix=${3}
newSuffix=${4}

for network in {1..100}
do
   if [ ! -e ${destiny}/${network} ]
   then
      echo "bash replicateNetwork.sh ${network} ${origin} ${destiny} ${oldSuffix} ${newSuffix}"
   fi
done
