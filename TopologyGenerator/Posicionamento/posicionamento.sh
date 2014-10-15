#!/bin/tcsh -f

rm -rf ./pos1 ./cenario.dat ./cenarios/* ./simulacao.log ./grafo.dot

# ================= configuracoes do NS =================
set xy=1500
set alcance=200
set grau=4
set distmin=25

# ================= configuracoes gerais ================
set nrofruns=100
set ic=99

# ================== simulacao #1 =======================
foreach modo (1)

    set dirscen={pos$modo}

    \rm -fr $dirscen
    mkdir $dirscen

    foreach nrnos (20)
     
	set run=1

	while ($run <= $nrofruns)

	    ns posicionamento.tcl $modo $xy $nrnos $alcance $grau $distmin

	    mv simulacao.log {$dirscen}/log.$nrnos.$run 

	    mv grafo.dot {$dirscen}/grafo.$nrnos.$run.dot 

            mv cenario.dat ./cenarios/$nrnos.$run

	    neato -n -Tps -Nshape=circle -Nheight=0.4 -Nwidth=0.4 -Nfixedsize=true $dirscen/grafo.$nrnos.$run.dot -o $dirscen/grafo.$nrnos.$run.ps

	    set run = `expr $run + 1`
		    
	end

	./ic.awk nrvar=3 ic=$ic {$dirscen}/log.$nrnos.* >> {$dirscen}/results.dat

	#set grau = `expr $grau + 1`
        #set alcance=`eval ./range ${alcance} ${nrnos} 20`
    end 	

end

#gnuplot posicionamento.gnu > area{$xy}alcance{$alcance}.eps

echo "FIM"
