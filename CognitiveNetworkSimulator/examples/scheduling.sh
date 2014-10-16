for i in `seq 0 2880`
do
   j=$(($i%60))
   j=$((i-j))
   j=$((j/60))
   j=$((j%2))
   if [ $j == 0 ]
   then
      echo $i" AT 000 IV 000 000"
      echo $i" AT 000 PC 000 000"
   fi
done

