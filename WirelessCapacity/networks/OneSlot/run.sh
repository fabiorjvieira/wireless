ls -c1 $1 | sort -Vr | while read file
do
   ./wc.bin fabioOneSlot 0.030 20000000 4.0 25 20 $file > $file.out
done

