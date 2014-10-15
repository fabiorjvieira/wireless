exec=${1}
networkSize=${2}

for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
do
   echo "${exec} ${networkSize} ./NormalRule/A ./NormalRule/B ${route}.routes .nr.perf .nr.perf AB 1 5"
   echo "${exec} ${networkSize} ./NormalRule/A ./NormalRule/C ${route}.routes .nr.perf .nr.perf AC 1 5"
   echo "${exec} ${networkSize} ./NormalRule/A ./NormalRule/D ${route}.routes .nr.perf .nr.perf AD 1 5"
   echo "${exec} ${networkSize} ./NormalRule/B ./NormalRule/C ${route}.routes .nr.perf .nr.perf BC 1 5"
   echo "${exec} ${networkSize} ./NormalRule/B ./NormalRule/D ${route}.routes .nr.perf .nr.perf BD 1 5"
   echo "${exec} ${networkSize} ./NormalRule/C ./NormalRule/D ${route}.routes .nr.perf .nr.perf CD 1 5"
done
   
for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/A ./AntecipationBufferedRule/Buffer${Buffer}/B ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.AB 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/A ./AntecipationBufferedRule/Buffer${Buffer}/C ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.AC 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/A ./AntecipationBufferedRule/Buffer${Buffer}/D ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.AD 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/B ./AntecipationBufferedRule/Buffer${Buffer}/C ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.BC 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/B ./AntecipationBufferedRule/Buffer${Buffer}/D ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.BD 1 5"
      echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${Buffer}/C ./AntecipationBufferedRule/Buffer${Buffer}/D ${route}.routes .abr.perf .abr.perf Buffer${Buffer}.CD 1 5"
   done
done
   
for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/A ./HybridRule/State1/Buffer${Buffer}/B ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.AB 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/A ./HybridRule/State1/Buffer${Buffer}/C ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.AC 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/A ./HybridRule/State1/Buffer${Buffer}/D ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.AD 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/B ./HybridRule/State1/Buffer${Buffer}/C ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.BC 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/B ./HybridRule/State1/Buffer${Buffer}/D ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.BD 1 5"
      echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/C ./HybridRule/State1/Buffer${Buffer}/D ${route}.routes .hr.perf .hr.perf Buffer${Buffer}.CD 1 5"
   done
done

for BufferX in {1..5}
do
   for BufferY in {1..5}
   do
      for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
      do
         for type in A B C D
         do
            if [ ${BufferX} -lt ${BufferY} ]
            then
               echo "${exec} ${networkSize} ./AntecipationBufferedRule/Buffer${BufferX}/${type} ./AntecipationBufferedRule/Buffer${BufferY}/${type} ${route}.routes .abr.perf .abr.perf ${type}.Buffer${BufferX}.Buffer${BufferY} 1 5"
            fi
         done
      done
   done
done

for BufferX in {1..5}
do
   for BufferY in {1..5}
   do
      for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
      do
         for type in A B C D
         do
            if [ ${BufferX} -lt ${BufferY} ]
            then
               echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${BufferX}/${type} ./HybridRule/State1/Buffer${BufferY}/${type} ${route}.routes .hr.perf .hr.perf ${type}.Buffer${BufferX}.Buffer${BufferY} 1 5"
            fi
         done
      done
   done
done

for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      for type in A B C D
      do
         echo "${exec} ${networkSize} ./NormalRule/${type} ./AntecipationBufferedRule/Buffer${Buffer}/${type} ${route}.routes .nr.perf .abr.perf ${type}.Buffer${Buffer} 1 5"
      done
   done
done

for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      for type in A B C D
      do
         echo "${exec} ${networkSize} ./NormalRule/${type} ./HybridRule/State1/Buffer${Buffer}/${type} ${route}.routes .nr.perf .hr.perf ${type}.Buffer${Buffer} 1 5"
      done
   done
done

for Buffer in {1..5}
do
   for route in .dijkstra.all .each.neighbor .each.n.neighbor .neighbor.n
   do
      for type in A B C D
      do
         echo "${exec} ${networkSize} ./HybridRule/State1/Buffer${Buffer}/${type} ./AntecipationBufferedRule/Buffer${Buffer}/${type} ${route}.routes .hr.perf .abr.perf ${type}.Buffer${Buffer} 1 5"
      done
   done
done
