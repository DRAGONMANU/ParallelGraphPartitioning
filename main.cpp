#include "iostream"
#include <stdio.h>
#include "vector"
#include "ctime"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <omp.h>
#include <iterator>
#include "Graph.cpp"
#include "Partition.cpp"
#include <map>
#include <algorithm>

using namespace std;

extern map<int, int> Partition(Graph& input, int num_threads, int k);

int main(int argc, char* argv[]) {
	// int num_threads = omp_get_num_procs();
	int num_threads = 4;
	
	if(argc!=4)
	{
		cout<<"\nerror in argument";
		return 0;
	}
	// int num_threads = 4;
	printf("Num Threads = %d\n", num_threads);
	int num_nodes;
	int num_edges;
	string line;


	ifstream myfile(argv[1]);
	if (myfile.is_open())
	{
		myfile >> num_nodes;
		myfile >> num_edges;
	}

	printf("num edge = %d num nodes = %d\n", num_edges, num_nodes);

	// vector<vector<int> > input(num_nodes);
	// vector<int> output;
	
	Graph input_graph = *(new Graph());
	if (myfile.is_open())
	{
		
		for(int input_ind = 0; input_ind <= num_nodes; input_ind++)
		{
			getline(myfile, line);
			istringstream is(line);
			vector<int> v((istream_iterator<int>(is)), istream_iterator<int>());
			// for (int x : v) std::cout << x << ' ';
			// std::cout << std::endl;
			std::vector<Edge> ve;
			for (unsigned int i = 0; i < v.size(); i++)
			{
				ve.push_back( *(new Edge(v[i], 1)));
			}
			if(input_ind!=0)
				input_graph.createAdjacencyList(input_ind, ve);
		}
		myfile.close();
	}

	else
	{
		cout << "Unable to open file";
		return 0;
	}
	//input_graph.printGraph();
	double start_time = omp_get_wtime();
	int k =atoi(argv[3]);
	// printf("input size = %d\n", input.size());
	map<int, int> parts = Partition(input_graph, num_threads,k, num_nodes);
	double time_taken = omp_get_wtime() - start_time;
	// Printing stats and results
	cout << "\nTime taken = " << time_taken << endl;
	printf("Parts size = %d\n", (int)parts.size());
	cout << "mincut=" << EdgeCut(parts,input_graph) << endl;
	map<int,int> :: iterator itera = parts.begin();
	ofstream outfile(argv[2]);
	
	map<int,int> distri;
	if(outfile.is_open())
	{
		itera = parts.begin();
		while(itera!=parts.end())
		{
			if(distri.find(itera->second)!=distri.end())
				distri[itera->second]++;
			else
				distri.insert(pair<int,int>(itera->second,1));
			outfile << itera->second << " ";
			itera++;
		}
		// cout<<ones;
	}

	itera = distri.begin();
	while(itera!=distri.end())
	{
		printf("%d -> %d\n",itera->first, itera->second);
		itera++;
	}

	return 0;
}