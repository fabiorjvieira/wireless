bash /home/farofino/indentifySGE.sh
mkdir -p /tmp/fjimenez/Networks
cd /tmp/fjimenez/Networks
cp /home/farofino/${1}.Networks.tgz ./
tar -xzf ${1}.Networks.tgz
chown -R farofino:farofino /tmp/fjimenez

