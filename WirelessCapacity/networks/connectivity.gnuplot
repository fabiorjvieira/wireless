set datafile separator " "
set datafile comment "#"
set datafile missing "?"
set terminal pdf font "times,12"
set output "connectivity.pdf"
set key bottom right

set ylabel "Frequency (%)"
set xlabel "Square side"

plot 'graph.connectivity.statistic' using 1:($3+1) title "" lt rgb "black"

