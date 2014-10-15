set datafile separator " "
set datafile comment "#"
set datafile missing "?"

set terminal pdf enhanced size 20cm,30cm
set output "graph.0.0.average.pdf"

set grid
set multiplot layout 1,9
set lmargin 5
set xrange[0:30]

set origin 0,0.22
set size 1,0.11
plot 'graph.0.0.dot.node.average.minMax.data' using 4:2 title "average.minMax" with lines lc 1
set origin 0,0.11
set size 1,0.11
plot '' using 4:6 title "standard deviation" with lines lc 1
set origin 0,0.00
set size 1,0.11
plot '' using 4:($4/$3) title "throughput" with lines lc 1

set origin 0,0.55
set size 1,0.11
plot 'graph.0.0.dot.node.average.maxDeviation.data' using 4:2 title "average.maxDeviation" with lines lc 2
set origin 0,0.44
set size 1,0.11
plot '' using 4:6 title "standard deviation" with lines lc 2
set origin 0,0.33
set size 1,0.11
plot '' using 4:($4/$3) title "throughput" with lines lc 2

set origin 0,0.88
set size 1,0.11
plot 'graph.0.0.dot.node.average.stdDeviation.data' using 4:2 title "average.stdDeviation" with lines lc 3
set origin 0,0.77
set size 1,0.11
plot '' using 4:6 title "standard deviation" with lines lc 3
set origin 0,0.66
set size 1,0.11
plot '' using 4:($4/$3) title "throughput" with lines lc 3
