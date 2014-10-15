ls -c1 $1 | while read file
do 
   eval newFile=`echo ${file} | sed "s/$2/$3/"`
   cp -v ${file} ${newFile}
   head -n$4 ${newFile}| sed "s/,/	/g" > ${newFile}.tmp
   mv ${newFile}.tmp ${newFile}
done

