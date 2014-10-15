exec=${1}
networkSize=${2}

for colX in 2 8 7 9 100
do
   colY=${colX}
   if [ ${colX} == 100 ]
   then
      colX=9
      colY=7
   fi
   for type in A B C D
   do
      echo "${exec} ${networkSize} ./NormalRule/${type} .dijkstra.all.routes .nr.perf ${colX} ${colY} 1 5"
      echo "${exec} ${networkSize} ./NormalRule/${type} .each.neighbor.routes .nr.perf ${colX} ${colY} 1 5"
      echo "${exec} ${networkSize} ./NormalRule/${type} .each.n.neighbor.routes .nr.perf ${colX} ${colY} 1 5"
      echo "${exec} ${networkSize} ./NormalRule/${type} .neighbor.n.routes .nr.perf ${colX} ${colY} 1 5"
   done
done

for Buffer in {1..5}
do
   for colX in 2 8 7 9 100
   do
      colY=${colX}
      if [ ${colX} == 100 ]
      then
         colX=9
         colY=7
      fi
      for type in A B C D
      do
         echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .dijkstra.all.routes .abr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .each.neighbor.routes .abr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .each.n.neighbor.routes .abr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .neighbor.n.routes .abr.perf ${colX} ${colY} 1 5"
      done
   done
done

for Buffer in {1..5}
do
   for colX in 2 8 7 9 100
   do
      colY=${colX}
      if [ ${colX} == 100 ]
      then
         colX=9
         colY=7
      fi
      for type in A B C D
      do
         echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .dijkstra.all.routes .hr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .each.neighbor.routes .hr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .each.n.neighbor.routes .hr.perf ${colX} ${colY} 1 5"
         echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .neighbor.n.routes .hr.perf ${colX} ${colY} 1 5"
      done
   done
done
