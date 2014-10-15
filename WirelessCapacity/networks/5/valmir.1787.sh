./capacity.average.minMaxDeviation.bin  valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.average.minMaxDeviation.result
./capacity.average.maxDeviation.bin     valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.average.maxDeviation.result
./capacity.average.stdDeviation.bin     valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.average.stdDeviation.result
./capacity.counting.minMaxDeviation.bin valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.counting.minMaxDeviation.result
./capacity.counting.maxDeviation.bin    valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.counting.maxDeviation.result
./capacity.counting.stdDeviation.bin    valmir 0.030 20000000 4.0 25 100 ../100/nodes/graph.1787.0.dot.node $1 > graph.1787.0.dot.node.counting.stdDeviation.result

cat graph.1787.0.dot.node.average.minMaxDeviation.result  | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.average.minMaxDeviation.data
cat graph.1787.0.dot.node.average.maxDeviation.result     | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.average.maxDeviation.data
cat graph.1787.0.dot.node.average.stdDeviation.result     | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.average.stdDeviation.data
cat graph.1787.0.dot.node.counting.minMaxDeviation.result | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.counting.minMaxDeviation.data
cat graph.1787.0.dot.node.counting.maxDeviation.result    | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.counting.maxDeviation.data
cat graph.1787.0.dot.node.counting.stdDeviation.result    | grep -v "#" | sed -e '$d' > graph.1787.0.dot.node.counting.stdDeviation.data

gnuplot valmir.average.1787.gnuplot
gnuplot valmir.counting.1787.gnuplot
