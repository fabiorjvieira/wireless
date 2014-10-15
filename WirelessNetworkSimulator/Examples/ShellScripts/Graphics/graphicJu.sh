exec=${1}
networkSize=${2}
minimum=${3}
maximum=${4}
increment=${5}

for type in A B C D
do
   echo "${exec} ${networkSize} ./NormalRule/${type} .dijkstra.all.routes .nr.perf ${minimum} ${maximum} ${increment} 1 5"
   echo "${exec} ${networkSize} ./NormalRule/${type} .each.neighbor.routes .nr.perf ${minimum} ${maximum} ${increment} 1 5"
   echo "${exec} ${networkSize} ./NormalRule/${type} .each.n.neighbor.routes .nr.perf ${minimum} ${maximum} ${increment} 1 5"
   echo "${exec} ${networkSize} ./NormalRule/${type} .neighbor.n.routes .nr.perf ${minimum} ${maximum} ${increment} 1 5"
done

for Buffer in {1..5}
do
   for type in A B C D
   do
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .dijkstra.all.routes .abr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .each.neighbor.routes .abr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .each.n.neighbor.routes .abr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/${type} .neighbor.n.routes .abr.perf ${minimum} ${maximum} ${increment} 1 5"
   done
done

for Buffer in {1..5}
do
   for type in A B C D
   do
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .dijkstra.all.routes .hr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .each.neighbor.routes .hr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .each.n.neighbor.routes .hr.perf ${minimum} ${maximum} ${increment} 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} .neighbor.n.routes .hr.perf ${minimum} ${maximum} ${increment} 1 5"
   done
done
