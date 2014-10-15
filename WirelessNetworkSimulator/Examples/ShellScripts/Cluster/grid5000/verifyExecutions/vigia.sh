bash computeRuns.sh ~/Result/C/6/ "80.*.abr.perf" 40 | grep " [0-9][0-9]$" > s.out ; oarstat -u $USER | grep rocha > r.out; cat r.out | wc -l; cat s.out | wc -l; cat r.out s.out; rm r.out s.out

