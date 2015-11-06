for((i=0;i<300;i++))
do
	while true
	do
		a=`ps aux | grep cc | wc -l`
		if ((a>20))
		then
			sleep 1m
		else
			break
		fi		
	done
	nohup ./cc $i &> "Log/"$i".log" &
done
