directory=${1}
files=${2}
index=${3}

#find ${1} -name ${2} -exec bash ./lastRun.sh {} ${3} ';' | rev | cut -d "/" -f 1 | rev | sort
#bash listLastRun.sh ${directory} "${files}" ${index} 
rm -f lastLine.out

find ${directory} -name ${files} -exec bash ./lastRun.sh {} ${index} ';' | rev | cut -d "/" -f 1 | rev | sort | cut -d "," -f 1 | sort | uniq -c | grep -v Perfo | sed "s/^ *//g" | sed "s/$ *//g" > results.out 
totalRun=0
lastNetwork=''
eval lastNetwork=`cat results.out | head -n1 | cut -d " " -f 2`

cat results.out | while read line
do
   eval network=`echo ${line} | cut -d " " -f 2`
   eval multiply=`echo ${line} | cut -d " " -f 1`
   eval run=`echo ${line} | cut  -d " " -f 3`
   let run=${run}+1
   if [ "${lastNetwork}" == "${network}" ]
   then
      let totalRun=(${run}*${multiply})+${totalRun};
   else
      echo ${lastNetwork} ${totalRun} >> lastLine.out
      let totalRun=${run}*${multiply}
      lastNetwork=${network}
   fi
   echo ${lastNetwork} ${totalRun} > lastLastLine.out
done

cat lastLastLine.out >> lastLine.out
export netSize=`eval head -n1 lastLine.out | cut -d "." -f 1`
export sufix=`eval head -n1 lastLine.out | cut -d "." -f 3- | cut -d " " -f 1`

for net in {1..100}
do
   grep "^${netSize}\.${net}\." lastLine.out > grep.out
   missing=`eval cat grep.out | wc -l`
   if [ "${missing}" == "0" ]
   then
      echo "${netSize}.${net}.${sufix} 0" >> lastLine.out
   fi
done

cat lastLine.out
rm -f results.out lastLastLine.out grep.out lastLine.out
