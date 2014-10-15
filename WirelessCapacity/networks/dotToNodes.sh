ls -c1 $1 | while read file
do
   echo Processing $file
   cat $file | grep " pos " | cut -f 2 -d "=" | sed "s/\"//g" | sed "s/! ];//" | sed "s/ //g" | sed "s/,/ /" > $file.node
done
