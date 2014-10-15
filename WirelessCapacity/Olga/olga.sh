rm result.out
ls -c1 "$1" | while read file
do
   echo $file
   java abc "$file" >> result.out
done
