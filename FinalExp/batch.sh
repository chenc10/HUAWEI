for((i=0;;i++))
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
	echo $i
	nohup ./bgps $i &> "Log/"$i".log" &
done
