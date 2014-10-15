ls -c1 $1 | while read file
do
   prefix=`echo $file | rev | cut -d "." -f 2- | rev`
   numberOfDifferentSquareSizes=`cat $file | grep -v "#"  | sed -e '$d' | cut -d " " -f 3 | sort | uniq | wc -l`
   echo processing $prefix $numberOfDifferentSquareSizes
   cat $file | grep -v "#" | sed -e '$d' > $prefix.data
   ../../../Debug/WirelessCapacity statistics ./$prefix.data ${numberOfDifferentSquareSizes} $2 > $prefix.statistics
done
