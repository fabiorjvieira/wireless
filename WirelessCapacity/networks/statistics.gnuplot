set terminal pdf enhanced color
set output "result.beta.pdf"
set xlabel "Square side"
set ylabel "Performance"

plot "result.statistic" using 1:6 title 'olga' with lines, \
"result.statistic" using 1:6 title 'olga-M' lt -1 pi -4 pt 6 ps 0.5, \
"" using 1:7 title "santi" with linespoints ps 0.5 pt 2, \
"" using 1:5 title 'santi-M' with linespoints ps 0.5 pt 4, \
"" using 1:3 title 'max|C|' with linespoints ps 0.5 pt 7, \
"" using 1:4 title 'max|C|-M' with linespoints ps 0.5 pt 8, \
"" using 1:2 title 'maxObj' with linespoints ps 0.5 pt 12

