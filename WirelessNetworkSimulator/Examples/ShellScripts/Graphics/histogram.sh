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
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      echo "${exec} ${networkSize} ./NormalRule/A ${route}.routes .nr.perf ${colX} ${colY} 1 5"
   done
done
