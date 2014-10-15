set terminal pdf font "times,12"
set output "results2.pdf"
set xlabel "Square side"
set ylabel "Performance"
set xrange [0:13103]
set yrange [0.2:1]
set key top center

set xtics ("131" 131, "1000" 1000, "3000" 3000, "5000" 5000, "7000" 7000, "9000" 9000, "11000" 11000, "13100" 13103)
plot "result.statistic" using 1:6 title 'ApproxLogN' with linespoints ps 0.5 pt 1 lt rgb "red", \
"" using 1:7 title "GreedyPhysical" with linespoints ps 0.5 pt 2 lt rgb "green", \
"" using 1:3 title 'MaxCRank' with linespoints ps 0.5 pt 7 lt rgb "blue"

