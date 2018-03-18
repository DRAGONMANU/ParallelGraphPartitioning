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

// Finds the internal matching and the external edges
void FindMatching(Graph& graph,int id,int chunk_size)
{
	vector<tuple<Node,Node>> matchings;
	map<int,  tuple<Node,vector <Edge>>> :: iterator iter = graph.adjacency_list.begin();
	
	while(iter != graph.adjacency_list.end())
	{
		for (int j = 0; j < get<1>(iter->second).size(); ++j)
		{
			// cout<<((get<1>(iter->second))[j]).n2;
			// Check whether node is inside the chunk or not
			if(((get<1>(iter->second))[j]).n2 <= (id+1)*chunk_size && (get<1>(iter->second))[j].n2 > (id)*chunk_size)
			{
				if(get<0>(iter->second).matched==0)
				{		
					if(graph.getNode((get<1>(iter->second))[j].n2).matched==0)
					{
						graph.getNode((get<1>(iter->second))[j].n2).matched = 1;
						get<0>(iter->second).matched = 1;
						matchings.push_back(make_tuple(get<0>(iter->second),graph.getNode((get<1>(iter->second))[j].n2)));
					}
				}
			}
			else
			{
				get<0>(iter->second).external_edges.push_back(*new Edge((get<1>(iter->second))[j].n2,1));
				printf("ext %d - %d\n", get<0>(iter->second).id, get<1>(iter->second)[j].n2);
			}
		}
		iter++;
	}
	printf("matches\n");
	for (unsigned int i = 0; i < matchings.size(); ++i)
	{
		printf("%d - %d\n", get<0>(matchings[i]).id,get<1>(matchings[i]).id);
	}
	//eating 
	for (unsigned int i = 0; i < matchings.size(); ++i)
	{
		get<0>(matchings[i]).food = get<1>(matchings[i]);
		printf("%d - %d\n", get<0>(matchings[i]).id,get<1>(matchings[i]).id);
	}
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
		if(id < num_threads-1)
		{ 
			for(int i=id*chunk_size;i<(id+1)*chunk_size;i++)
			{
				//TODO change this function
				breaks[id].createAdjacencyList(i+1,get<1>(input.adjacency_list[i+1]));
			}
		}
		else
		{
			for (int i = id*chunk_size; i < input.adjacency_list.size(); ++i)
			{
				//TODO change this function
				breaks[id].createAdjacencyList(i+1,get<1>(input.adjacency_list[i+1]));

			}
		}
		//while(breaks[id].size()>10)
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
