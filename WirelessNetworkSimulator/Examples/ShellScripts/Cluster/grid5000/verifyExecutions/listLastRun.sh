find ${1} -name ${2} -exec bash ./lastRun.sh {} ${3} ';' | rev | cut -d "/" -f 1 | rev | sort
