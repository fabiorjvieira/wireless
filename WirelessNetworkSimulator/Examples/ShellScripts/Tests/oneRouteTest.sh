exec=${1}
networkSize=${2}
network=${3}
routes=${4}
buffer=${5}

${exec} ${networkSize} ${network} ${routes} 1 ${buffer} > ${routes}.${buffer}.1.out
${exec} ${networkSize} ${network} ${routes} 2 ${buffer} > ${routes}.${buffer}.2.out
${exec} ${networkSize} ${network} ${routes} 3 ${buffer} > ${routes}.${buffer}.3.out
${exec} ${networkSize} ${network} ${routes} 4 ${buffer} > ${routes}.${buffer}.4.out
${exec} ${networkSize} ${network} ${routes} 5 ${buffer} > ${routes}.${buffer}.5.out
${exec} ${networkSize} ${network} ${routes} 6 ${buffer} > ${routes}.${buffer}.6.out
${exec} ${networkSize} ${network} ${routes} 7 ${buffer} > ${routes}.${buffer}.7.out
${exec} ${networkSize} ${network} ${routes} 8 ${buffer} > ${routes}.${buffer}.8.out
${exec} ${networkSize} ${network} ${routes} 9 ${buffer} > ${routes}.${buffer}.9.out
${exec} ${networkSize} ${network} ${routes} A ${buffer} > ${routes}.${buffer}.A.out
${exec} ${networkSize} ${network} ${routes} B ${buffer} > ${routes}.${buffer}.B.out
${exec} ${networkSize} ${network} ${routes} C ${buffer} > ${routes}.${buffer}.C.out

more ${routes}.${buffer}.*.out | grep "Arrive\|Rule"

