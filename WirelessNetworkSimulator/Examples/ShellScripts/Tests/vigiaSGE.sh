watch -n20 'qstat -f | grep "au\|adu" | grep all.q; qstat | grep " r "; echo wait/run; qstat | grep " qw " | wc -l; qstat | grep " r " | wc -l'
