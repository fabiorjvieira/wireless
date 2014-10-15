set datafile separator " "
set datafile comment "#"
set datafile missing "?"

set terminal pdf
set output "result.pdf"

plot "olga.statistic" using 2:4 title 'olga' with lines, \
"santi.statistic" using 2:4 title 'santi' with lines, \
"fabio.statistic" using 2:4 title 'fabio' with lines, \
"fabio.maximizeC.statistic" using 2:4 title 'fabio.maxC' with lines, \
"olga.multicoloring.statistic" using 2:4 title 'olga.multi' with lines, \
"santi.multicoloring.statistic" using 2:4 title 'santi.multi' with lines, \
"valmir.statistic" using 2:4 title 'valmir' with lines

