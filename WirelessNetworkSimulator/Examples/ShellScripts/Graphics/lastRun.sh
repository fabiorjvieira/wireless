for i in $(seq 1 1 100)
do
   for file in ${1}/${i}/*.perf
   do
      j=`eval tail -n1 ${file} | cut -d "," -f 1`
      echo "${file}" ${j}
   done
done
