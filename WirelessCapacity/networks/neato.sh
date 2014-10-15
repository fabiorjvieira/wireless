ls -c1 $1*.dot | while read file
do
   neato -n -Tps -Nshape=circle -Nheight=0.4 -Nwidth=0.4 -Nfixedsize=true $file -o $file.ps
done
