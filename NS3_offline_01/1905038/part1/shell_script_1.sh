#!/bin/bash
number_flow=10
number_node=20
num_packets_per_second=500
cov_area=5


FILE_1=./scratch/1905038/part1/plot/vary_flow.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 10 10 50)  #vary number of flows
do
    ./ns3 run "scratch/1905038/part1/1905038_1.cc --number_node=${number_node} --number_flow=${i} --num_packets_per_second=${num_packets_per_second} --cov_area=${cov_area} --file=${FILE_1}"
    echo "Number of Flow : $i  is done"
done
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/throughput_flow.png" "throughput vs flow" "flow" "throughput (Kbps)" "./scratch/1905038/part1/plot/vary_flow.txt" 2 5
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/delivery_ratio_flow.png" "delivery ratio vs flow" "flow" "delivery ratio" "./scratch/1905038/part1/plot/vary_flow.txt" 2 6

FILE_1=./scratch/1905038/part1/plot/vary_node.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 20 20 100)  #vary number of nodes
do
    j=$(($i / 2))
    ./ns3 run "scratch/1905038/part1/1905038_1.cc --number_node=${i} --number_flow=${j} --num_packets_per_second=${num_packets_per_second} --cov_area=${cov_area} --file=${FILE_1}"
    echo "Number of nodes : $i is done"
done
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/throughput_node.png" "throughput vs node" "node" "throughput (Kbps)" "./scratch/1905038/part1/plot/vary_node.txt" 1 5
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/delivery_ratio_node.png" "delivery ratio vs node" "node" "delivery ratio" "./scratch/1905038/part1/plot/vary_node.txt" 1 6

FILE_1=./scratch/1905038/part1/plot/vary_num_pps.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 100 100 500)  # vary number of packets per second
do
    ./ns3 run "scratch/1905038/part1/1905038_1.cc --number_node=${number_node} --number_flow=${number_flow} --num_packets_per_second=${i} --cov_area=${cov_area} --file=${FILE_1}"
    echo "Number of packets per second : $i is done"
done
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/throughput_vs_packets_per_second.png" "throughput vs packets per second" "packets per second" "throughput (Kbps)" "./scratch/1905038/part1/plot/vary_num_pps.txt" 3 5
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/delivery_ratio_vs_packets_per_second.png" "delivery ratio vs packets per second" "packets per second" "delivery ratio" "./scratch/1905038/part1/plot/vary_num_pps.txt" 3 6

FILE_1=./scratch/1905038/part1/plot/vary_coverage.txt
rm -f $FILE_1
touch $FILE_1

for i in $(seq 1 1 5)  #vary coverage area
do
    ./ns3 run "scratch/1905038/part1/1905038_1.cc --number_node=${number_node} --number_flow=${number_flow} --num_packets_per_second=${num_packets_per_second} --cov_area=${i} --file=${FILE_1}"
    echo "coverage : $i done"
done
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/throughput_vs_coverage_area.png" "throughput vs coverage area" "coverage area" "throughput (Kbps)" "./scratch/1905038/part1/plot/vary_coverage.txt" 4 5
gnuplot -c "./scratch/1905038/results.gnuplot" "./scratch/1905038/part1/plot/delivery_ratio_vs_coverage_area.png" "delivery ratio vs coverage area" "coverage area" "delivery ratio" "./scratch/1905038/part1/plot/vary_coverage.txt" 4 6

