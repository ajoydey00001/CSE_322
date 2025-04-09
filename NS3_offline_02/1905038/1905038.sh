#!/bin/bash

ch=1

FILE_1=./scratch/Offline_02/data_1.txt
rm -f $FILE_1
touch $FILE_1


for i in $(seq 1 50 301)  #vary bottle neck rate
do
    ./ns3 run "scratch/1905038.cc --rate_bottleneck=${i} --choice=${ch}"
   
done
gnuplot -c "./scratch/set.gnuplot" "./scratch/Offline_02/throughput_bottleNeckDataRate_1.png" "throughput vs bottle neck data rate" "bottle neck data rate" "throughput (kbps)" "./scratch/Offline_02/data_1.txt" 1 3 4 "Adaptive Reno"
gnuplot -c "./scratch/results.gnuplot" "./scratch/Offline_02/Fairness Index_bottle neck dataRate_1.png" "Fairness Index vs bottle neck data rate" "bottle neck data rate" "Fairness Index" "./scratch/Offline_02/data_1.txt" 1 5 "NewReno + Adaptive Reno"

FILE_1=./scratch/Offline_02/data_1.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 2 1 6)  #vary packet loss expression 
do
    ./ns3 run "scratch/1905038.cc --expression=${i} --choice=${ch}"
    
done
gnuplot -c "./scratch/set.gnuplot" "./scratch/Offline_02/throughput_packetLossExpression_1.png" "throughput vs packet loss expression" "packet loss expression" "throughput (kbps)" "./scratch/Offline_02/data_1.txt" 2 3 4 "Adaptive Reno"
gnuplot -c "./scratch/results.gnuplot" "./scratch/Offline_02/Fairness Index_packetLossExpression_1.png" "Fairness Index vs packet loss expression" "packet loss expression" "Fairness Index" "./scratch/Offline_02/data_1.txt" 2 5 "NewReno + Adaptive Reno"



FILE_1=./scratch/Offline_02/flow1.txt
rm -f $FILE_1
touch $FILE_1

FILE_1=./scratch/Offline_02/flow2.txt
rm -f $FILE_1
touch $FILE_1

./ns3 run "scratch/1905038.cc --choice=${ch}"
gnuplot -c "./scratch/set_2.gnuplot" "./scratch/Offline_02/Congestion_Window_vs_Time_1.png" "Congestion window vs time" "Time" "Congestion Window" "./scratch/Offline_02/flow2.txt" 1 2 2 "Adaptive Reno"


ch=2

FILE_1=./scratch/Offline_02/data_1.txt
rm -f $FILE_1
touch $FILE_1


for i in $(seq 1 50 301)  #vary bottle neck rate
do
    ./ns3 run "scratch/1905038.cc --rate_bottleneck=${i} --choice=${ch}"
   
done
gnuplot -c "./scratch/set.gnuplot" "./scratch/Offline_02/throughput_bottleNeckDataRate_2.png" "throughput vs bottle neck data rate" "bottle neck data rate" "throughput (kbps)" "./scratch/Offline_02/data_1.txt" 1 3 4 "TcpHighSpeed"
gnuplot -c "./scratch/results.gnuplot" "./scratch/Offline_02/Fairness Index_bottle neck dataRate_2.png" "Fairness Index vs bottle neck data rate" "bottle neck data rate" "Fairness Index" "./scratch/Offline_02/data_1.txt" 1 5 "NewReno + High Speed"



FILE_1=./scratch/Offline_02/data_1.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 2 1 6)  #vary packet loss expression 
do
    ./ns3 run "scratch/1905038.cc --expression=${i} --choice=${ch}"
    
done
gnuplot -c "./scratch/set.gnuplot" "./scratch/Offline_02/throughput_packetLossExpression_2.png" "throughput vs packet loss expression" "packet loss expression" "throughput (kbps)" "./scratch/Offline_02/data_1.txt" 2 3 4 "TcpHighSpeed"
gnuplot -c "./scratch/results.gnuplot" "./scratch/Offline_02/Fairness Index_packetLossExpression_2.png" "Fairness Index vs packet loss expression" "packet loss expression" "Fairness Index" "./scratch/Offline_02/data_1.txt" 2 5 "NewReno + High Speed"



FILE_1=./scratch/Offline_02/flow1.txt
rm -f $FILE_1
touch $FILE_1

FILE_1=./scratch/Offline_02/flow2.txt
rm -f $FILE_1
touch $FILE_1

./ns3 run "scratch/1905038.cc --choice=${ch}"
gnuplot -c "./scratch/set_2.gnuplot" "./scratch/Offline_02/Congestion_Window_vs_Time_2.png" "Congestion window vs time" "Time" "Congestion Window" "./scratch/Offline_02/flow2.txt" 1 2 2 "TcpHighSpeed"


