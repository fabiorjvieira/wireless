exe=$1
rm /tmp/ssh.$USER.out

cat ~/sites/sites.in | while read line
do
   echo "echo $line" >> /tmp/ssh.$USER.out
   echo "ssh frontend.${line}.grid5000.fr '${exe}'" >> /tmp/ssh.$USER.out
done

bash /tmp/ssh.$USER.out
