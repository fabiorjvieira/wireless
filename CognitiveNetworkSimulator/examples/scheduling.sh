for i in `seq 0 86400`
do
   j=$(($i%3600))
   j=$((i-j))
   j=$((j/3600))
   j=$((j%2))
   #if [ $j == 0 ]
   #then
      echo $i" AT 000 IV 000 0000"
      echo $i" AT 000 PC 000 0000"
   #fi
done

