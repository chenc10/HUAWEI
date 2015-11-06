#27206 30
#139 300
#2448 3000
#17534 30001
#27726 300182
#1661 3082869
#193 31977632
#60 92940186
s=18
for j in 943 10770
do
	mkdir -p "Log/"$s
	for((i=0;i<1000;i++))
	do
		while true
		do
			a=`ps aux | grep cc | wc -l`
			if ((a>20))
			then
				sleep 5 
			else
				break
			fi		
		done
		echo $j","$i
		nohup ./cc $i $j  &> "Log/"$s"/"$i".log" &
	done
	s=$((s+1))
done
