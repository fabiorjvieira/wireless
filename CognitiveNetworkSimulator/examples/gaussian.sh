let k=9*3600
for x in `seq 0 1 691200`
do 
   y=`echo "100 * ((1/(e((($x-28800)*($x-28800))/(2*10000*10000)))) + (1/(e((($x-57600)*($x-57600))/(2*10000*10000)))))" | bc -l`
   y=`echo $y | cut -d "." -f 1`
   let y-=1

   if [ 0 ==   $(($x % $k)) ]
   then
      x=$(printf "%07d" $x)
      echo "REQS${x}CA000CC0010900100"
   else
      x=$(printf "%07d" $x)
   fi
         
   y=$(printf "%03d" $y)
   echo "SENS${x}CA000CA0000000100"
   echo "SENS${x}CA000AT0000000100"
   echo "SENS${x}CA000CA0000001000"
   echo "SENS${x}CA000AT0000001${y}"
   echo "SENS${x}CA000CA0000002100"
   echo "SENS${x}CA000AT0000002100"
done
