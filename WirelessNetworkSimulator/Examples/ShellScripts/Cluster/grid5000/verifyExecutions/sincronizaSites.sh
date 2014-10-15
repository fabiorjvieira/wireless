files=${1}
netSize=${2}
index=${3}

echo Resultados
bash ~/sites/scpResults.sh "~/"

echo Faltantes
#cat ~/sites/sites.in | while read sites
#do
   find ~/sites/Result/ -maxdepth 3 -mindepth 3 -type d | rev | cut -d "/" -f 1,2 | rev | sort | uniq | while read directories
   do
      order=`eval echo ${directories} | cut -d "/" -f 1`
      buffer=`eval echo ${directories} | cut -d "/" -f 2`

      echo computa ${directories} ${files} ${index}

      bash computeRuns.sh ~/sites/Result/\*/${directories}/ "${files}" ${index} | grep -v " [0-9][0-9][0-9]$" | cut -d "." -f 2 > ~/sites/${netSize}.missing.${order}.${buffer}
      bash geraFaltantes.sh ~/sites/${netSize}.missing.${order}.${buffer} ${netSize} ${order} ${buffer} > ~/sites/transfer/${netSize}.missing.${order}.${buffer}.sh 
   done
#done

#bash computeRuns.sh ~/sites/orsay/Result/ "80.*.abr.perf" 40 | grep -v " [0-9][0-9][0-9]$" | cut -d "." -f 2 > ~/sites/orsay/missing 
#bash geraFaltantes.sh ~/sites/orsay/missing 80 C 6 > ~/sites/transfer/80.6

echo Scripts
rm /tmp/ssh.$USER.out

cat ~/sites/sites.in | while read sites
do
   echo "echo $sites" >> /tmp/ssh.$USER.out
   echo "scp ~/sites/transfer/${netSize}.missing.*.sh frontend.${sites}.grid5000.fr:" >> /tmp/ssh.$USER.out
done

bash /tmp/ssh.$USER.out

