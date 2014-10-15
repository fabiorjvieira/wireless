i=1

while [ $i -le 100 ]
do
   j=`eval ls -lhX ${1} | grep ${2} | grep "80\.$i\."| wc -l`
   echo ${i} , ${j}
   i=$(( $i + 1 ))
done

