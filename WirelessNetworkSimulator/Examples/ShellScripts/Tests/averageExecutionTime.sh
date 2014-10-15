for file in ${1} 
do
   begin=`eval cat ${file} | grep ${2} | head -n1`
   end=`eval cat ${file} | grep ${2} | tail -n1`
   beginS=`eval date -d \"${begin}\" +'%s'`
   endS=`eval date -d \"${end}\" +"%s"`
   let endS-=${beginS}
   let endM=${endS}/60
   let endS-=${endM}*60
   let endH=${endM}/60
   let endM-=${endH}*60
   echo File:${file} Begin:${begin} End:${end} Duration:${endH}:${endM}:${endS}
done
