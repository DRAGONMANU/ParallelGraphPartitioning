#include <omp.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <tuple>

using namespace std;;
vector<Graph> Partition(Graph input, int num_edges, int num_threads);
void print_2d_vec(vector<vector<int>> arr, int size);
void print_vec(vector<int> arr, int size);

vector<Graph> Partition(Graph input, int num_edges, int num_threads)
{
	vector<Graph> breaks;
	for (int i = 0; i < num_threads; ++i)
	{
		breaks.push_back(*new Graph());
	}
	omp_set_num_threads(num_threads);
#pragma omp parallel num_threads(num_threads)
	{
		int id = omp_get_thread_num();
		int chunk_size = input.node_list.size()/num_threads;
		for(int i=id*chunk_size;i<(id+1)*chunk_size;i++)
		{
			//TODO change this function
			breaks[id].createAdjacencyList(i,get<1>(input.adjacency_list[i]));
		}
		if(id == num_threads-1)
		{
			for (int i = (id+1)*chunk_size; i < input.adjacency_list.size(); ++i)
			{
				//TODO change this function
				breaks[id].createAdjacencyList(i,get<1>(input.adjacency_list[i]));
			}
		}

		// FindMatching(breaks[id]);
	}


	vector<Graph> parts;
	// Graph coarse_graph = Union(breaks);
	// vector<Graph> parts = Bipartition(coarse_graph);
	// Project(parts,input.adjacency_list.size()); // vomit recursive till rhs is 0
	return parts;

}

void print_vec(vector<int> arr, int size)
{
	printf("size = %d\n", size);
	printf("\n[ ");
	for (int i = 0; i < size; i++)
		printf("%d (%d) , ", arr[i], i);
	printf("%d (%d)]\n", arr[size - 1], (size - 1));
}

void print_2d_vec(vector<vector<int>> arr, int size)
{
	printf("2d size = %d\n", size);
	printf("\n[ ");
	for (int i = 0; i < size; i++)
		print_vec(arr[i], arr[i].size());
	printf("]\n");
}
