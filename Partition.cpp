#include <omp.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <tuple>
#include <map>
#include <cstdlib>
#include <list>

using namespace std;;
// vector<Graph> Partition(Graph& input, int num_edges, int num_threads);
void print_2d_vec(vector<vector<int>> arr, int size);
void print_vec(vector<int> arr, int size);

map<int, int> Bipartition(Graph graph, int id)
{
	map<int, int> labels;
	map<int,  tuple<Node,vector <Edge>>> :: iterator iter = graph.adjacency_list.begin();
	int total_weight = 0;
	while(iter != graph.adjacency_list.end())
	{
		labels.insert(pair < int, int> (iter->first,0));
		total_weight += get<0>(iter->second).weight;
		iter++;
	}
	cout<<"total="<<total_weight<<endl;
	tuple<Node,vector <Edge>> startNode;
	iter = graph.adjacency_list.begin();
	int temp = 0;
	id = rand() % graph.adjacency_list.size();
	while(iter != graph.adjacency_list.end())
	{
		if(temp<id)
			temp++;
		else
		{
			startNode = iter->second;
			break;
		}
		iter++;
	}

	list<int> queue;
	queue.push_back(get<0>(startNode).id);
	labels[get<0>(startNode).id] = 1;
	int weight = 0;
	
	while(!queue.empty())
	{
		//printf("%d\n", weight);
		int s = queue.front();
		labels[s] = 1;
		iter = graph.adjacency_list.begin();
		weight = 0;
		while(iter != graph.adjacency_list.end())
		{
			if(labels[get<0>(iter->second).id]==1)
				weight += get<0>(iter->second).weight;
			iter++;
		}
		printf("%d\n", weight); 
		if(weight>=0.5*total_weight)
			break;
		queue.pop_front();
		for(unsigned int i = 0; i< get<1>(graph.adjacency_list[s]).size();i++)
		{
			if(labels[get<1>(graph.adjacency_list[s])[i].n2]==0)
				queue.push_back(get<1>(graph.adjacency_list[s])[i].n2);
		}
	}
	map<int, int> :: iterator itr = labels.begin();
	while(itr != labels.end())
	{
		printf("s%d\n", itr->second);
		itr++;
	}
	return labels;
}


// Finds the internal matching and the external edges
void FindMatching(Graph& graph,int id,int chunk_size)
{
	vector<tuple<Node,Node>> matchings;
	map<int,  tuple<Node,vector <Edge>>> :: iterator iter = graph.adjacency_list.begin();
	
	while(iter != graph.adjacency_list.end())
	{
		for (unsigned int j = 0; j < get<1>(iter->second).size(); ++j)
		{
			// cout<<((get<1>(iter->second))[j]).n2;
			// Check whether node is inside the chunk or not
			if(((get<1>(iter->second))[j]).n2 <= (id+1)*chunk_size && (get<1>(iter->second))[j].n2 > (id)*chunk_size)
			{
				Node& predator = get<0>(iter->second);
				Node& prey = graph.getNode((get<1>(iter->second))[j].n2);
				if(get<0>(iter->second).matched==0)
				{		
					if(prey.matched==0)
					{
						prey.matched = 1;
						predator.matched = 1;
						matchings.push_back(make_tuple(predator, prey));
						
						// Eating and being eaten
						predator.food = &prey;
						prey.consumer = &predator;
						predator.weight += prey.weight;
						
						// Transfer the adjacency list
						// Go through the neighbours of prey and add them to adjacency list of predator
						// If the neighbours are the same, then increment the edge weight
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
	// for (unsigned int i = 0; i < matchings.size(); ++i)
	// {
	// 	get<0>(matchings[i]).food = (&get<1>(matchings[i]));
	// 	get<1>(matchings[i]).consumer = (&get<0>(matchings[i]));
	// }
	graph.printGraph();
}


map<int, int> Partition(Graph& input, int num_edges, int num_threads)
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
			for (unsigned int i = id*chunk_size; i < input.adjacency_list.size(); ++i)
			{
				//TODO change this function
				breaks[id].createAdjacencyList(i+1,get<1>(input.adjacency_list[i+1]));

			}
		}
		//while(breaks[id].size()>10)
		// {
		 	FindMatching(breaks[id],id,chunk_size);
		// }
	}

	// Graph coarse_graph = Union(breaks);
	map<int, int> parts = Bipartition(breaks[0],0);
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
