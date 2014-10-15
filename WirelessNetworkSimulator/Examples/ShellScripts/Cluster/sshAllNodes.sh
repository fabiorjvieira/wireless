i=$3
f=$2

while [ $f -le $i ];
do
   echo '--------------------------------------------------------------------------------'
   if [ $i -le 9 ]
   then
      ssh node0${i} "echo \$HOSTNAME;"$1
   else
      ssh node${i} "echo \$HOSTNAME;"$1
   fi
   let i=$i-1;
done
