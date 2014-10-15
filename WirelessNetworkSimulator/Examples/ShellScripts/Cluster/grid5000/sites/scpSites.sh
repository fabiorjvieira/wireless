origin=$1
destiny=$2

rm /tmp/ssh.$USER.out

cat ~/sites/sites.in | while read line
do
   echo "echo $line" >> /tmp/ssh.$USER.out
   echo "scp -r ${origin} frontend.${line}.grid5000.fr:${destiny}" >> /tmp/ssh.$USER.out
done

bash /tmp/ssh.$USER.out
