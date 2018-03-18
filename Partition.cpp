#include <omp.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <tuple>

using namespace std;;
vector<Graph> Partition(Graph& input, int num_edges, int num_threads);
void print_2d_vec(vector<vector<int>> arr, int size);
void print_vec(vector<int> arr, int size);


void FindMatching(Graph& graph,int id,int chunk_size)
{
	vector<tuple<Node,Node>> matchings;
	for (int i = 0; i < graph.adjacency_list.size(); ++i)
	{
		for (int j = 0; j < get<1>(graph.adjacency_list[i]).size(); ++j)
		{
			if(graph.getNode((get<1>(graph.adjacency_list[i]))[j].n2).id < (id+1)*chunk_size && graph.getNode((get<1>(graph.adjacency_list[i]))[j].n2).id >= (id)*chunk_size)
			{
				if(get<0>(graph.adjacency_list[i]).matched==0)
					if(graph.getNode((get<1>(graph.adjacency_list[i]))[j].n2).matched==0)
					{
						graph.getNode((get<1>(graph.adjacency_list[i]))[j].n2).matched = 1;
						get<0>(graph.adjacency_list[i]).matched = 1;
						matchings.push_back(make_tuple(get<0>(graph.adjacency_list[i]),graph.getNode((get<1>(graph.adjacency_list[i]))[j].n2)));
					}
			}
			else
				get<0>(graph.adjacency_list[i]).external_edges.push_back(*new Edge((get<1>(graph.adjacency_list[i]))[j].n2),1);
			
		}
	}

	//eating

}


vector<Graph> Partition(Graph& input, int num_edges, int num_threads)
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
		int chunk_size = input.adjacency_list.size()/num_threads;
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

		while(breaks[id].size()>10)
		{
			FindMatching(breaks[id],id,chunk_size);
		}
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
