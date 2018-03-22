all:
	g++ -std=c++11 -fopenmp -Wall -O2 main.cpp -o main

Graph: 
	g++ -std=c++11 -fopenmp -Wall -O2 Graph.cpp -o Graph

Partition:
	g++ -std=c++11 -fopenmp -Wall -O2 Partition.cpp -o Partition
