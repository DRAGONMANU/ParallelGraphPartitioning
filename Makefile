all:
	g++ -std=c++11 -fopenmp -Wall main.cpp -o main

Graph: 
	g++ -std=c++11 -fopenmp -Wall Graph.cpp -o Graph

Partition:
	g++ -std=c++11 -fopenmp -Wall Partition.cpp -o Partition
