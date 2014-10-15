set terminal pdf font "times,12"
set output "results4.pdf"
set xlabel "Square side"
set ylabel "Gain"
set xrange [0:13103]
set yrange [0.95:1.4]
set key top center


set xtics ("131" 131, "1000" 1000, "3000" 3000, "5000" 5000, "7000" 7000, "9000" 9000, "11000" 11000, "13100" 13103)
plot 1 lt rgb "black" title "", "results4.statistic" using 1:4 title 'ApproxLogN' with linespoints ps 0.5 pt 1 lt rgb "red", \
"" using 1:3 title "GreedyPhysical" with linespoints ps 0.5 pt 2 lt rgb "green", \
"" using 1:2 title 'MaxCRank' with linespoints ps 0.5 pt 7 lt rgb "blue"

