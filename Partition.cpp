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
		printf("%d %d\n", itr->first, itr->second);
		itr++;
	}
	return labels;
}

Graph updateEdges(Graph& graph, int level_coarsening, int debug)
{
	Graph new_graph = *(new Graph());
	if (debug)
	{
		printf("in updateEdges - level = %d\n", level_coarsening);
		graph.printGraph();
	}
	map<int,  tuple<Node,vector <Edge>>> :: iterator iter = graph.adjacency_list.begin();
	while(iter != graph.adjacency_list.end())
	{
		Node& old_n = get<0>(iter->second);
		
		if (debug)
		{
			printf("----check old_n id = %d\n", old_n.id);
			graph.printGraph();
			printf("\nNEw Graph\n");
			new_graph.printGraph();

		}
		if(old_n.consumer != nullptr) // If consumed then don't add the node to the new graph
		{
			iter++;
			continue;
		}

		Node new_n = *(new Node(old_n.id));
		new_n.weight = old_n.weight;
		vector<Edge> new_neighbours;
		new_graph.createAdjacencyList2(new_n, new_neighbours);

		// inserting the edges of the original node
		if (debug) printf("PRINTING EDGES\n");
		for (Edge old_edge : get<1>(iter->second))
		{
			Edge modified_edge = graph.modifyEdgeWithParent(old_edge);
			if(debug) modified_edge.printEdge();
			new_graph.insertEdge(old_n.id, modified_edge);
		}	
		if (debug) printf("\n");

		if(debug) 
		{
			// graph.printGraph();
		}
		if (old_n.preyExists(level_coarsening))
		{
			int prey_id = old_n.food_chain[level_coarsening]->id;
			//inserting the edges of the prey
			if (debug) printf("PRINTING EDGES of Prey\n");
			vector<Edge> prey_edges = get<1>(graph.adjacency_list[prey_id]);
			for (unsigned int j = 0; j < prey_edges.size(); ++j)
			{
				Edge modified_edge = graph.modifyEdgeWithParent(prey_edges[j]);
				if(debug) modified_edge.printEdge();
				new_graph.insertEdge(old_n.id, modified_edge);
			}
			if (debug) printf("\n");
		}
		iter++;
	}
	return new_graph;

}

// Finds the internal matching and the external edges
Graph FindMatching(Graph& graph,int id,int chunk_size, int level_coarsening, int debug)
{
	
	if (debug)
	{
		printf("\nFIND Matching  level = %d \n", level_coarsening);
		graph.printGraph();
	}
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
				// printf("ext %d - %d\n", get<0>(iter->second).id, get<1>(iter->second)[j].n2);
			}
		}
		iter++;
	}
	if(debug)
	{
		printf("matches\n");
		for (unsigned int i = 0; i < matchings.size(); ++i)
		{
			printf("%d - %d\n", get<0>(matchings[i]).id,get<1>(matchings[i]).id);
		}
	}
	Graph new_graph = updateEdges(graph, level_coarsening, debug);
	// printf("++++++\n");
	// new_graph.printGraph();
	// printf("@+++++\n");
	return new_graph;
}


map<int, int> Partition(Graph& input, int num_edges, int num_threads)
{
	vector<Graph> breaks;
	map<int, vector<Graph>> coarse_graphs;	//[proc][k_level]

	for (int i = 0; i < num_threads; ++i)
	{
		breaks.push_back(*new Graph());
	}
	omp_set_num_threads(num_threads);
	
	#pragma omp parallel num_threads(num_threads)
	{

		int id = omp_get_thread_num();
		int chunk_size = input.adjacency_list.size()/num_threads;
		int debug = 0;
		if(id == 0) 
		{	
			debug = 0;
		}

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
		vector<Graph> coarse_p;
		coarse_p.push_back(breaks[id]);
		int k_level = 0;
		do 
		{
		 	coarse_p.push_back(FindMatching(coarse_p[k_level], id, chunk_size, k_level, debug));
		 	k_level++;
		 	#pragma omp barrier
		}while(k_level < 2);
		
		#pragma omp critical
		{
			coarse_graphs.insert(pair <int, vector<Graph>> (id, coarse_p));	
		}
		// Graph coarse_graph = Union(breaks);

		// map<int, int> parts = Bipartition(coarse_graphs[id][k_level],0);
	}
	// {paralled ends}
	for (int x = 0; x < num_threads; x++)
	{	for (int y = 0; y < 3; y++)
		{
			printf("\n%d ---- %d \n", x, y);
			coarse_graphs[x][y].printGraph();
		}
	}
	// Find union of pralllel graojsafl

	map<int, int> parts;
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
