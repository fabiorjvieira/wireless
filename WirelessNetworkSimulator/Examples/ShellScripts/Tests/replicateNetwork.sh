missingNetwork=${1}
origin=${2}
destiny=${3}
oldSuffix=${4}
newSuffix=${5}

cp -rv ${origin}/${missingNetwork} ${destiny}
for file in ${destiny}/${missingNetwork}/*${oldSuffix}*perf
do
   eval newName=`echo ${file} | sed  "s/${oldSuffix}/${newSuffix}/g"`
   mv -v ${file} ${newName}
done
