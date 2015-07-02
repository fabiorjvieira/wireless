set terminal pdf enhanced
set output "risk.pdf"
set yrange [0:1.1]
set xrange [0:86400]
set xlabel "time in seconds"
set ylabel "risk"

plot (1/(exp(((x-28800)*(x-28800))/(2*10000*10000)))) + (1/(exp(((x-57600)*(x-57600))/(2*10000*10000)))) ti ""
