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


Graph updateEdges(Graph& graph, int level_coarsening)
{
	Graph new_graph = *(new Graph());
	printf("in updateEdges - level = %d\n", level_coarsening);
	map<int,  tuple<Node,vector <Edge>>> :: iterator iter = graph.adjacency_list.begin();
	while(iter != graph.adjacency_list.end())
	{
		Node& old_n = get<0>(iter->second);
		if(old_n.consumer != 0) // If consumed then don't add the node to the new graph
		{
			iter++;
			continue;
		}
		Node new_n = *(new Node(old_n.id));
		new_n.weight = old_n.weight;
		vector<Edge> new_neighbours;
		new_graph.createAdjacencyList2(new_n, new_neighbours);

		// inserting the edges of the original node
		for (unsigned int j = 0; j < get<1>(iter->second).size(); ++j)
		{
			new_graph.insertEdge(old_n.id, graph.modifyEdgeWithParent(get<1>(iter->second)[j]));
		}

		printf("old_n id = %d\n", old_n.id);
		if (old_n.preyExists(level_coarsening))
		{
			int prey_id = old_n.food_chain[level_coarsening]->id;
			//inserting the edges of the prey
			vector<Edge> prey_edges = get<1>(graph.adjacency_list[prey_id]);
			for (unsigned int j = 0; j < prey_edges.size(); ++j)
			{
				new_graph.insertEdge(old_n.id, graph.modifyEdgeWithParent(prey_edges[j]));
			}
		}
		iter++;
	}
	return new_graph;

}
// Finds the internal matching and the external edges
Graph FindMatching(Graph& graph,int id,int chunk_size, int level_coarsening)
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
						// TODO: Store the prey for each level using map
						// map< int , map
						predator.food_chain.insert(pair <int, Node*> (level_coarsening, &prey));
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
	Graph new_graph = updateEdges(graph, level_coarsening);
	new_graph.printGraph();
	return new_graph;
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
			for (unsigned int i = id*chunk_size; i < input.adjacency_list.size(); ++i)
			{
				//TODO change this function
				breaks[id].createAdjacencyList(i+1,get<1>(input.adjacency_list[i+1]));

			}
		}
		//while(breaks[id].size()>10)
		{
		 	Graph g1 = FindMatching(breaks[id], id, chunk_size, 0);
		 	Graph g2 = FindMatching(g1, id, chunk_size, 1);
		 	Graph g3 = FindMatching(g2, id, chunk_size, 2);

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
