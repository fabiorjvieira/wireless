networkSize=${1}
columnX=${2}
columnY=${3}

echo "set datafile commentschars '#';"
echo "set datafile separator ',';"
echo "set xrange [0:30];"
echo "set yrange [0:1.05];"
echo "set ytic 0,0.05;"
echo "set xtic 0,1;"
echo "set ylabel 'desempenho'"
echo "set xlabel '${columnX}-${columnY}'"
echo "set terminal pdf"
echo "set obj rect from graph 0, 0.33333333333333333333333333, graph 0 to graph 1, graph 1 behind fc rgb 'red' fs solid 0.15 noborder"
echo

for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
do
   for n in {1..5}
   do
      if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
      eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
      echo "set output 'NormalRule${routeN}.ABCD.averageGroupedBy.pdf'"
      echo "set title '${routeN}'; plot \\"
      echo "'./NormalRule/A/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING BREADTH', \\"
      echo "'./NormalRule/B/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING DEPTH', \\"
      echo "'./NormalRule/C/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING BREADTH', \\"
      echo "'./NormalRule/D/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING DEPTH'"
      echo
      if [ ${route} == ${routeN} ]
      then
         break
      fi
   done
done

echo   
for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
   do
      for n in {1..5}
      do
         if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
         eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
         echo "set output 'AntecipationBufferedRule${routeN}.Buffer${Buffer}.ABCD.averageGroupedBy.pdf'"
         echo "set title '${routeN}'; plot \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/A/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING BREADTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/B/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING DEPTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/C/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING BREADTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/D/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING DEPTH'"
         echo
         if [ ${route} == ${routeN} ]
         then
            break
         fi
      done
   done
done

echo   
for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
   do
      for n in {1..5}
      do
         if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
         eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
         echo "set output 'HybridRule${routeN}.Buffer${Buffer}.ABCD.averageGroupedBy.pdf'"
         echo "set title '${routeN}'; plot \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/A/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING BREADTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/B/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'DESCENDING DEPTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/C/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING BREADTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/D/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'ASCENDING DEPTH'"
         echo
         if [ ${route} == ${routeN} ]
         then
            break
         fi
      done
   done
done

echo
for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
do
   for n in {1..5}
   do
      if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
      eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
      for type in A B C D
      do
         echo "set output 'AntecipationBufferedRule${routeN}.${type}.Buffers.averageGroupedBy.pdf'"
         echo "set title '${routeN} ${type}'; plot \\"
         for Buffer in {1..5}
         do
            echo "'./AntecipationBufferedRule/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'Buffer${Buffer}', \\"
         done
         echo "0 notitle"
         echo
      done
      if [ ${route} == ${routeN} ]
      then
         break
      fi
   done
done

echo
for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
do
   for n in {1..5}
   do
      if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
      eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
      for type in A B C D
      do
         echo "set output 'HybridRule${routeN}.${type}.Buffers.averageGroupedBy.pdf'"
         echo "set title '${routeN} ${type}'; plot \\"
         for Buffer in {1..5}
         do
            echo "'./HybridRule/State1/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'Buffer${Buffer}', \\"
         done
         echo "0 notitle"
         echo
      done
      if [ ${route} == ${routeN} ]
      then
         break
      fi
   done
done

echo
for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
   do
      for type in A B C D
      do
         for n in {1..5}
         do
            if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
            eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
            echo "set output 'All${routeN}.${type}.Buffer${Buffer}.averageGroupedBy.pdf'"
            echo "set title '${routeN} ${type} Buffer${Buffer}'; plot \\"
            echo "'./NormalRule/${type}/${networkSize}/${networkSize}${routeN}.routes.nr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'Normal', \\"
            echo "'./AntecipationBufferedRule/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.abr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'Antecipation', \\"
            echo "'./HybridRule/State1/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.hr.perf.${columnX}.${columnY}.averageGroupedBy.graphic' using 1:2:3 with errorbars notitle, '' using 1:2 with lines title 'Hybrid'"
            echo
            if [ ${route} == ${routeN} ]
            then
               break
            fi
         done
      done
   done
done
