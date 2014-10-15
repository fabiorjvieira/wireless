for file in *.o*
do
   eval network=`cat ${file} | grep all | grep -v perf`
   eval begin=`cat ${file} | grep 2009 | head -n1 | sed  "s/ //g" | sed "s/BRST2009//g" | sed "s/Nov/11/g" | sed "s/Sun\|Mon\|Tue\|Wed\|Tue\|Fri\|Sat//g"`
   eval end=`cat ${file} | grep 2009 | tail -n1 | sed  "s/ //g"`
   eval fim=`cat ${file} | grep fim`
   eval count=`cat ${file} | grep 2009 | wc -l`
   echo ${file} , ${network} , ${begin} , ${end} , ${fim} , ${count}
done

