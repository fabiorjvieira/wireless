cat ~/sites/grids.in | while read line
do
   eval cluster=`echo ${line} | cut -d " " -f 2`
   eval site=`echo ${line} | cut -d " " -f 1`
   echo "echo ${site} ${cluster}"
   echo "ssh frontend.${site}.grid5000.fr \"oarsub -O ~/${cluster}FreeTest.out -E ~/${cluster}FreeTest.out -p \\\"cluster='$cluster'\\\" -l /nodes=1/cpu=1/core=1,walltime=\${1} -n ${cluster}FreeTest 'date; echo \$HOSTNAME'\""
done

