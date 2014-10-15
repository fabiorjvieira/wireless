#min=`eval cat ${2} | grep -v "#" | cut -d "," -f 2 | sort | head -n1`
#max=`eval cat ${2} | grep -v "#" | cut -d "," -f 2 | sort | tail -n1`

eval lastRun=`grep "^[0-9]*,${2}," ${1} | tail -n1`
if [ "${lastRun}" != "" ] 
then
   echo ${1} ${lastRun}
fi
