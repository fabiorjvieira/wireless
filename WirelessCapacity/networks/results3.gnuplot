set terminal pdf font "times,12"
set output "results3.pdf"
set xlabel "Number of nodes"
set ylabel "Gain"
set xrange [0:25600]
set yrange [0.95:1.4]
set key top center

set xtics ("100" 100, "3200" 3200, "6400" 6400, "12800" 12800, "25600" 25600)
plot 1 lt rgb "black" title "", "results3.statistic" using 1:(1+$4) title 'ApproxLogN' with linespoints ps 0.5 pt 1 lt rgb "red", \
"" using 1:(1+$3) title "GreedyPhysical" with linespoints ps 0.5 pt 2 lt rgb "green", \
"" using 1:(1+$2) title 'MaxCRank' with linespoints ps 0.5 pt 7 lt rgb "blue"

