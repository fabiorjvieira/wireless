origin=$1
destiny=$2

rm /tmp/ssh.$USER.out

cat ~/sites/sites.in | while read line
do
   echo "echo $line" >> /tmp/ssh.$USER.out
   echo "scp -r frontend.${line}.grid5000.fr:${origin}/Result/* ~/sites/${destiny}/Result/${line}/" >> /tmp/ssh.$USER.out
done

bash /tmp/ssh.$USER.out

#read -p "Press any key to continue..."

#bash ~/sites/ssh.sh "rm -r ~/Result/*"
