networkSize=${1}

echo "set datafile commentschars '#';"
echo "set datafile separator ',';"
echo "set xrange [0:1];"
echo "#set yrange [0:1.05];"
echo "#set ytic 0,0.05;"
echo "set xtic 0,0.05;"
echo "set ylabel 'quantidade de pares o/d'"
echo "set xlabel 'desempenho'"
echo "set terminal pdf"
echo "set obj rect from 0.33333333333333333333333333, graph 0, graph 0 to graph 1, graph 1 behind fc rgb 'red' fs solid 0.15 noborder"
echo "set pointsize 1" 
echo

for route in .dijkstra.all .each.neighbor .each.n..neighbor .neighbor.n.
do
   for n in {1..5}
   do
      if [ ${route} == .neighbor.n. -a ${n} == 1 ]; then continue; fi
      eval routeN=`echo ${route} | sed 's/\.n\./\.${n}/g'` 
      echo "set output 'NormalRule${routeN}.ABCD.ju.pdf'"
      echo "set title '${routeN}'; plot \\"
      echo "'./NormalRule/A/${networkSize}/${networkSize}${routeN}.routes.nr.perf.ju.graphic' using 1:2 with linespoints pt 1 title 'DESCENDING BREADTH', \\"
      echo "'./NormalRule/B/${networkSize}/${networkSize}${routeN}.routes.nr.perf.ju.graphic' using 1:2 with linespoints pt 2 title 'DESCENDING DEPTH', \\"
      echo "'./NormalRule/C/${networkSize}/${networkSize}${routeN}.routes.nr.perf.ju.graphic' using 1:2 with linespoints pt 6 title 'ASCENDING BREADTH', \\"
      echo "'./NormalRule/D/${networkSize}/${networkSize}${routeN}.routes.nr.perf.ju.graphic' using 1:2 with linespoints pt 8 title 'ASCENDING DEPTH'"
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
         echo "set output 'AntecipationBufferedRule${routeN}.Buffer${Buffer}.ABCD.ju.pdf'"
         echo "set title '${routeN}'; plot \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/A/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints pt 1 title 'DESCENDING BREADTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/B/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints pt 2 title 'DESCENDING DEPTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/C/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints pt 6 title 'ASCENDING BREADTH', \\"
         echo "'./AntecipationBufferedRule/Buffer${Buffer}/D/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints pt 8 title 'ASCENDING DEPTH'"
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
         echo "set output 'HybridRule${routeN}.Buffer${Buffer}.ABCD.ju.pdf'"
         echo "set title '${routeN}'; plot \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/A/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints pt 1 title 'DESCENDING BREADTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/B/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints pt 2 title 'DESCENDING DEPTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/C/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints pt 6 title 'ASCENDING BREADTH', \\"
         echo "'./HybridRule/State1/Buffer${Buffer}/D/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints pt 8 title 'ASCENDING DEPTH'"
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
         echo "set output 'AntecipationBufferedRule${routeN}.${type}.Buffers.ju.pdf'"
         echo "set title '${routeN} ${type}'; plot \\"
         for Buffer in {1..5}
         do
            echo "'./AntecipationBufferedRule/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints title 'Buffer${Buffer}', \\"
         done
         echo "0 notitle;"
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
         echo "set output 'HybridRule${routeN}.${type}.Buffers.ju.pdf'"
         echo "set title '${routeN} ${type}'; plot \\"
         for Buffer in {1..5}
         do
            echo "'./HybridRule/State1/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints title 'Buffer${Buffer}', \\"
         done
         echo "0 notitle;"
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
            echo "set output 'All${routeN}.${type}.Buffer${Buffer}.ju.pdf'"
            echo "set title '${routeN} ${type} Buffer${Buffer}'; plot \\"
            echo "'./NormalRule/${type}/${networkSize}/${networkSize}${routeN}.routes.nr.perf.ju.graphic' using 1:2 with linespoints pt 1 title 'Normal', \\"
            echo "'./AntecipationBufferedRule/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.abr.perf.ju.graphic' using 1:2 with linespoints pt 2 title 'Antecipation', \\"
            echo "'./HybridRule/State1/Buffer${Buffer}/${type}/${networkSize}/${networkSize}${routeN}.routes.hr.perf.ju.graphic' using 1:2 with linespoints pt 6 title 'Hybrid'"
            echo
            if [ ${route} == ${routeN} ]
            then
               break
            fi
         done
      done
   done
done
