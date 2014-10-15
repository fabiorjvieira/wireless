i=1
j=${2}
k=${1}

cd /tmp/fjimenez/Networks/
while [ ${i} -le 100 ]
do
   l=1
   while [ ${l} -le ${j} ]
   do
      echo "tar -czf ${k}.${i}.neighbor.${l}.routes.tgz ${k}/${i}/*.neighbor.${l}.routes"
      echo "tar -czf ${k}.${i}.each.${l}.neighbor.routes.tgz ${k}/${i}/*.each.${l}.neighbor.routes"
      echo "tar -czf ${k}.${i}.each.neighbor.routes.tgz ${k}/${i}/*.each.neighbor.routes"
      echo "tar -czf ${k}.${i}.dijkstra.all.routes.tgz ${k}/${i}/*.dijkstra.all.routes"
      echo "tar -czf ${k}.${i}.min.routes.tgz ${k}/${i}/*.min.routes"
#      echo "tar -czf ${k}.${i}.all.routes.tgz ${k}/1/${k}.${i}.*.all.routes"
      l=$(( $l + 1 ))
   done
   i=$(( $i + 1 ))
done

