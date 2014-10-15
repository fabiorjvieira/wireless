set term post eps "Times-Roman" 20

set encoding iso_8859_1

set xlabel  "número de nós"
set ylabel  "grau"

set title "área: 2000mx2000m - alcance: 200m"

set pointsize 2

set key left 

plot "pos1/results.dat" u 1:4 t "Random: mean" w linesp 1, "pos1/results.dat" u 1:2 t "Random: max" w linesp 2, "pos1/results.dat" u 1:6 t "Random: min" w linesp 3, "pos1/results.dat" u 1:4 t "MaxDegree (15): mean" w linesp 4, "pos1/results.dat" u 1:2 t "MaxDegree (15): max" w linesp 5, "pos1/results.dat" u 1:6 t "MaxDegree (15): min" w linesp 6
