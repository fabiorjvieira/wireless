ls -c1 $1 | while read file
do 
   cat ${file}| sed "s/	/,/g" > ${file}.tmp
   mv ${file}.tmp ${file}
done

