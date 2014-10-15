if [ ! "${2}" == "" ]
then
   export cluster="cluster='${2}'"
fi
walltime=${1};
oarsub -O ~/Output/all1abr59C80.%jobid%.o -E ~/Output/all1abr59C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr59C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 59 59 abr C 1 1 80'
oarsub -O ~/Output/all1abr62C80.%jobid%.o -E ~/Output/all1abr62C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr62C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 62 62 abr C 1 1 80'
oarsub -O ~/Output/all1abr66C80.%jobid%.o -E ~/Output/all1abr66C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr66C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 66 66 abr C 1 1 80'
oarsub -O ~/Output/all1abr91C80.%jobid%.o -E ~/Output/all1abr91C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr91C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 91 91 abr C 1 1 80'
oarsub -O ~/Output/all1abr92C80.%jobid%.o -E ~/Output/all1abr92C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr92C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 92 92 abr C 1 1 80'
oarsub -O ~/Output/all1abr94C80.%jobid%.o -E ~/Output/all1abr94C80.%jobid%.e -p "${cluster}" -l /nodes=1/cpu=1/core=1,walltime=${walltime} -n all1abr94C80 '/home/orsay/frochajimenezvieira/basicLocal.sh WirelessNetworkSimulator 94 94 abr C 1 1 80'
