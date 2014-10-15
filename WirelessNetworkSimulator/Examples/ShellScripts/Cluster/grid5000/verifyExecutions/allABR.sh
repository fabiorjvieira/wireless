#echo "#" ord_enu ${1} buf ${2} ini ${3} fim ${4} size ${5} exe ${6}
i=${3}

while [ $i -le ${4} ]
do
   echo "oarsub -O ~/Output/all${2}abr${i}${1}${5}.%jobid%.o -E ~/Output/all${2}abr${i}${1}${5}.%jobid%.e -p \"\${cluster}\" -l /nodes=1/cpu=1/core=1,walltime=\${walltime} -n all${2}abr${i}${1}${5} '$HOME/basicLocal.sh ${6} ${i} ${i} abr ${1} ${2} ${2} ${5}'"
   i=$(( $i + 1 ))
done

