set terminal pdf fsize 10 fname 'Times' 
#size 6.5,2.7 dashed
set termoption enhanced
set datafile commentschars '#';
set datafile separator ',';
set xrange [0:60];
set yrange [0:5.5];
set y2range [0:5.5];
set ylabel '{/Times-Italic F_{d}}'
set xlabel "|{/Times-Italic C^{'}_{r}}|"
set pointsize 1
set grid nopolar
set ytics 0.5
set y2tics 0.5 
set xtics 10
set key bot right horizontal width 5 samplen 1

set output 'novo.60nos-grau4.pdf'
plot \
     'new.60.AODV.abr.perf.graphic' using 1:2:($3*10) with errorlines title 'AODV' ps 0.01 lc 4, \
     'new.60.AOMDV.abr.perf.graphic' using 1:2:($3*10) with errorlines title 'AOMDV' ps 0.01 lc 3, \
     'new.60.R-AOMDV.abr.perf.graphic' using 1:2:($3*10) with errorlines title 'R-AOMDV' ps 0.01 lc 2

#     '60.OLSR.abr.perf.graphic' using 1:2:($3*10) with errorlines title 'OLSR' ps 0.01 lc 2, \
#     'new.60.MP-OLSR.abr.perf.graphic' using 1:2:($3*10) with errorlines title 'MP-OLSR' ps 0.01 lc 1