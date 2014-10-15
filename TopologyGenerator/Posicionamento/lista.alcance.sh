#!/bin/tcsh -f

# ================= configuracoes do NS =================
set xy=1500
set alcance=200
set grau=4
set distmin=25

# ================= configuracoes gerais ================
set nrofruns=100
set ic=99

# ================== simulacao #1 =======================
    foreach nrnos (80 100 120 140 160 180 200 220 240)
       echo ${nrnos} ${alcance}    
       set alcance=`eval ./range ${alcance} ${nrnos} 20`
    end 	

echo "FIM"
