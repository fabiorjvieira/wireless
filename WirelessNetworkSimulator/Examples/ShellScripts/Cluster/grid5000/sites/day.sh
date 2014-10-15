#for a in {${1}..${2}}
for a in {3..7}
do
   cat ~/sites/transfer/80.$a | sed "s/13:40/$3/" > ~/sites/transfer/80.$a.day
done
