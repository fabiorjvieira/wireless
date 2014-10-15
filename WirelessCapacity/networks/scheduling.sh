executable=$1
algorithm=$2
pathLoss=$3
dB_SNR=$4
numberOfNodes=$5
pathToNodes=$6
numberOfSchedules=$7

if [ "$pathLoss" == "" ]
then
   pathLoss="4.0"
fi

if [ "$dB_SNR" == "" ]
then
   dB_SNR="25"
fi

if [ "$pathToNodes" == "" ]
then
   pathToNodes="./nodes/"
fi

if [ "$numberOfSchedules" == "" ]
then
   numberOfSchedules=1
fi

if [ "$numberOfNodes" == "" ]
then
   numberOfNodes="100"
fi

if [ "$algorithm" == "" ]
then
   echo "algorithm?"
elif [ "$executable" == "" ]
then
   echo "executable?"
else
   ls -c1 $pathToNodes | sort -V -r | while read file
   do
      echo $executable $algorithm 0.030 20000000 $pathLoss $dB_SNR $numberOfNodes $pathToNodes$file $numberOfSchedules
   done 
fi
