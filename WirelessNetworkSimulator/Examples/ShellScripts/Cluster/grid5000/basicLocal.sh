export jobId=${pmc_job_id}

if [ "${jobId}" = "" ]
then
   export jobId=${OAR_JOB_ID}
fi
if [ "${jobId}" = "" ]
then
   export jobId=${JOB_ID}
fi

export workspace="$HOME/"

echo path ${workspace}

#oarsub -q x -n test -O ${workspace}/Output/test.%jobid%.o -E ${workspace}/Output/test.%jobid%.e -l walltime=168 /home/rocha/test.sh
#qsub -e ${workspace}/Output -o ${workspace}/Output -S /bin/bash -N allNR1A basic.sh WirelessNetworkSimulatorNR 1 1 NR A 1 1;
exe=$1
i=$2
f=$3
rule=$4
orderEnumeration=$5
bi=$6
bf=$7
size=$8

mkdir -p /tmp/fjimenez/${jobId}/
cp -r ${workspace}/${exe} /tmp/fjimenez/${jobId}/
cd /tmp/fjimenez/${jobId}/${exe}/Release/
make clean; make all
exe=/tmp/fjimenez/${jobId}/${exe}/Release/WirelessNetworkSimulator
cd /tmp/fjimenez

bash ${workspace}/preGRID.sh

while [ $i -le $2 ]
do
 echo $i
 date +"%Y-%m-%d %H:%M:%S"
 mkdir -p /tmp/fjimenez/${jobId}/Network/${8}/$i/
 cp ${workspace}/Network/${8}/$i/${8}.$i /tmp/fjimenez/${jobId}/Network/${8}/$i/
 cp ${workspace}/Route/${8}/$i/${8}.$i.min.routes /tmp/fjimenez/${jobId}/Network/${8}/$i/
 cd /tmp/fjimenez/${jobId}/Network/
 mkdir -p ${workspace}/Result/${orderEnumeration}/${bi}/${jobId}/
 $exe ${8} /tmp/fjimenez/${jobId}/Network/${8}/$i/${8}.$i 1 1 $orderEnumeration $bi $bf > /tmp/fjimenez/${jobId}/Network/${8}.$i.all.minimum.path.routes.$rule.$orderEnumeration.$bi.$bf.perf.out
 date +"%Y-%m-%d %H:%M:%S"
 i=$(( $i + 1 ))
done

bash ${workspace}/postGRID.sh

