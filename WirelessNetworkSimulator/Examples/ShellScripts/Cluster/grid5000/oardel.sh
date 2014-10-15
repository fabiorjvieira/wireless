oarstat -u $USER | grep "^[0-9]" | cut -d " " -f 1 | sed "s/^/oardel /" > /tmp/oardel.out
bash /tmp/oardel.out
rm /tmp/oardel.out

