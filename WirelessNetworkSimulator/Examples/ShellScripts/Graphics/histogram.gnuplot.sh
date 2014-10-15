networkSize=${1}

echo "set datafile commentschars '#';"
echo "set datafile separator ',';"
echo "set xtic 0,1;"
echo "set ylabel 'quantidade'"
echo "set xlabel '${columnX}-${columnY}'"
echo "set style data histogram"                                                             
echo "set style histogram cluster gap 1"
echo "set style fill solid border -1"
echo "set boxwidth 0.1"
echo "set terminal fig color size 20 10;"

echo

for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
do
   for n in {1..5}
   do
      if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
      eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
      echo "set output 'histogram${routeN}.fig'"
      echo "set title '${routeN}'; plot \\"
      for colX in 2 8 7 100 9
      do
         colY=${colX}
         if [ ${colX} == 100 ]
         then
            colX=9
            colY=7
         fi
         echo "'./NormalRule/A/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${colX}.${colY}.histogram.graphic' using 2:xtic(1) title '${colX}.${colY}', \\"
      done
      echo "0 notitle;"
      echo
      if [ ${route} == ${routeN} ]
      then
         break
      fi
   done
done
