for((j=0;;j++))
do
	mkdir -p "Log/"$j
	for((i=0;i<300;i++))
	do
		while true
		do
			a=`ps aux | grep bgps | wc -l`
			if ((a>20))
			then
				sleep 1m
			else
				break
			fi		
		done
		echo $j","$i
		nohup ./bgps $i $j  &> "Log/"$j"/"$i".log" &
	done
done
