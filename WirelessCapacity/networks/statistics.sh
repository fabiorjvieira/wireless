result=$1
executable=$2
prefix=`echo $result | rev | cut -f 2- -d "." | rev`
algorithm=`echo $result | cut -f 1 -d "."`
statistic=$prefix".statistic"
data=$prefix".data"

cat $result | grep -v "#" > $data

numberOfDifferentSquareSizes=`cat $result | grep -v "#" | cut -d " " -f 2 | sort | uniq | wc -l`
$executable statistics $data ${numberOfDifferentSquareSizes} 100 > $statistic

