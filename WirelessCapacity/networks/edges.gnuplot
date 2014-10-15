set datafile separator " "
set datafile comment "#"
set datafile missing "?"
set terminal pdf font "times,12"
set output "edges.pdf"
set key bottom right

set ylabel "Average number of links"
set xlabel "Square side"

	plot 'graph.connectivity.statistic' using 1:($4>30? ($4*100) : ($1<6203?  (50+$4*100) :  ($1<8000? (25+$4*100) : 	($1<8000? (12+$4*100) : ($1<12091? (6 + $4*100) : ($4+1)))))) title "" ps 0.5 lt rgb "black"

