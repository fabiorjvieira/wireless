set parno 0
proc parameterindex { } {
	global argv argc parno
	
	set thisparm [lindex $argv $parno]
	incr parno
	return $thisparm
}

set modo		[eval parameterindex]
set xy		        [eval parameterindex]
set nrnos		[eval parameterindex]
set alcance		[eval parameterindex]
set grau_maximo		[eval parameterindex]
set distancia_minima    [eval parameterindex]

set ns_   [new Simulator]

set rng_ [new RNG]
$rng_ seed 0

set grau_maximo_atual  1000

for {set i 0} {$i<$nrnos} {incr i} {
    set terminal_($i) [$ns_ node]
}    

proc PosicionaTudo { } {

    global xy nrnos alcance distancia_minima grau_maximo_atual rng_ terminal_

    for {set i 0} {$i<$nrnos} {incr i} {
	$terminal_($i) set X_ [$rng_ uniform 0 $xy]
	$terminal_($i) set Y_ [$rng_ uniform 0 $xy]     
	$terminal_($i) set vizinhos_ 0
    }    
    for {set i 0} {$i<$nrnos} {incr i} {
	set X [$terminal_($i) set X_]
	set Y [$terminal_($i) set Y_]
	for {set j 0} {$j<$i} {incr j} {
	    set auxX [$terminal_($j) set X_]
	    set auxY [$terminal_($j) set Y_]
	    
	    ;# calcula distancia euclidiana entre no i e j
	    set distance [expr sqrt(pow(($auxX - $X),2) + pow(($auxY - $Y),2))]
	    
	    ;# se i esta no alcance de j, incrementa o nr de vizinhos
	    if {$distance < $alcance} {
		$terminal_($i) set vizinhos_ [expr [$terminal_($i) set vizinhos_] + 1]
		$terminal_($j) set vizinhos_ [expr [$terminal_($j) set vizinhos_] + 1]
	    }
	}
    }
}

proc Posiciona_e_Testa { } {

    global xy nrnos alcance distancia_minima grau_maximo rng_ terminal_

    set i 0

    while {$i < $nrnos} {

	set ok 0
	set tentativas 0

	while {$tentativas < 1000} {

	    incr tentativas

	    if {$i != 0} {
		$terminal_($i) set X_ [$rng_ uniform 0 $xy]
		$terminal_($i) set Y_ [$rng_ uniform 0 $xy]     
	    } else {
		$terminal_($i) set X_ [expr $xy/2.0]
		$terminal_($i) set Y_ [expr $xy/2.0]
		set ok 1
	    }
	    $terminal_($i) set vizinhos_ 0
		
	    set X [$terminal_($i) set X_]
	    set Y [$terminal_($i) set Y_]

	    for {set j 0} {$j<$i} {incr j} {
		set auxX [$terminal_($j) set X_]
		set auxY [$terminal_($j) set Y_]
	    
		;# calcula distancia euclidiana entre no i e j
		set distancia [expr sqrt(pow(($auxX - $X),2) + pow(($auxY - $Y),2))]
	    
		;# se i esta no alcance de j, incrementa o nr de vizinhos
		if {$distancia <= $alcance} {
		    
		    set viz_i [expr [$terminal_($i) set vizinhos_] + 1]
		    set viz_j [expr [$terminal_($j) set vizinhos_] + 1]
		    if { $viz_i <= $grau_maximo && $viz_j <= $grau_maximo && $distancia >= $distancia_minima} {
			$terminal_($i) set vizinhos_ $viz_i
			set ok 1
		    } else {
			set j $i
			set ok 0
		    }
		}
	    }
	    if {$ok} {

		for {set j 0} {$j<$i} {incr j} {
		    set auxX [$terminal_($j) set X_]
		    set auxY [$terminal_($j) set Y_]
		    
		    ;# calcula distancia euclidiana entre no i e j
		    set distancia [expr sqrt(pow(($auxX - $X),2) + pow(($auxY - $Y),2))]
		    
		    ;# se i esta no alcance de j, incrementa o nr de vizinhos
		    if {$distancia <= $alcance} {
			$terminal_($j) set vizinhos_ [expr [$terminal_($j) set vizinhos_] + 1]
		    }
		}

		set tentativas 1000
		incr i
	    }
	}
	if {!$ok} {
	    ;# puts stderr "tentativas ultrapassadas: recomecando ..."
	    set i 0
	}
    }
}

proc plot_dot {} {

    global  nrnos alcance xy terminal_

    set grafo [open ./grafo.dot w]

    puts $grafo "graph G \{"

    set cenario [open ./cenario.dat w]

    for {set i 0} {$i<$nrnos} {incr i} {

	puts $grafo "n$i \[ pos \= \"[$terminal_($i) set X_],[$terminal_($i) set Y_]\!\" \];"

        puts $cenario "\$node_($i) \set X_ [$terminal_($i) set X_]"
        puts $cenario "\$node_($i) \set Y_ [$terminal_($i) set Y_]"
        puts $cenario "\$node_($i) \set Z_ 0.000000000000"

	for {set j [expr $i + 1]} {$j < $nrnos} {incr j} {

	    set X [$terminal_($i) set X_]
	    set Y [$terminal_($i) set Y_]
	    
	    set auxX [$terminal_($j) set X_]
	    set auxY [$terminal_($j) set Y_]
	    
	    set distance [expr sqrt(pow(($auxX - $X),2) + pow(($auxY - $Y),2))]
	    if {$distance <= $alcance} {
		puts $grafo "n$i -- n$j;"
	    }
	}
    }

    puts $grafo "bl \[ pos \= \"0.0,0.0\!\", height \= 0.0, width \= 0.0, fontsize \= 0.0 \];"
    puts $grafo "br \[ pos \= \"$xy,0.0\!\", height \= 0.0, width \= 0.0, fontsize \= 0.0 \];"
    puts $grafo "tl \[ pos \= \"0.0,$xy\!\", height \= 0.0, width \= 0.0, fontsize \= 0.0 \];"
    puts $grafo "tr \[ pos \= \"$xy,$xy\!\", height \= 0.0, width \= 0.0, fontsize \= 0.0 \];"

    puts $grafo "bl -- br -- tr -- tl -- bl;"
    
    puts $grafo "\}"
    close $grafo
    close $cenario
}

if {$modo} {
    Posiciona_e_Testa
} else {
    PosicionaTudo
}

# calcula o grau maximo e medio
set nrmaxvizinhos 0
set nrminvizinhos 1000
set media 0
for {set i 0} {$i<$nrnos} {incr i} {
    set aux [$terminal_($i) set vizinhos_]
    set media [expr $media + $aux]
    if {$aux > $nrmaxvizinhos} {
	set nrmaxvizinhos $aux
    }
    if {$aux < $nrminvizinhos} {
	set nrminvizinhos $aux
    }
}
set trace [open ./simulacao.log w]
puts $trace "$nrnos\t$nrmaxvizinhos\t[expr $media*1.0/$nrnos]\t$nrminvizinhos"
close $trace

plot_dot

