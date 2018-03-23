#include <omp.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <tuple>
#include <map>
#include <cstdlib>
#include <list>
#include <time.h>
#include <algorithm>

using namespace std;;
void print_2d_vec(vector<vector<int>> arr, int size);
void print_vec(vector<int> arr, int size);

int STOPPING_CONDITION = 10;

map <int, vector<int>> Food_Chain;

map <int, int> Uncoarsen(Graph graph, map <int, int> partial_label)
{
	map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
	while(iter != graph.adjacency_list.end())
	{
		Node& n = get<0>(iter->second);
		if (partial_label.find(n.id) == partial_label.end()) // Partial label does not exist
		{
			partial_label[n.id] = partial_label[graph.findMasterPredator(n.id)];
		}
		iter++;
	}
	return partial_label;
}
int EdgeCut(map<int, int> labels,Graph graph)
{
	map<int, int> :: iterator itr = labels.begin();
	int sum = 0;
	while(itr != labels.end())
	{
		// printf("%d\n",itr->second );			
		vector <Edge> edges = get<1>(graph.adjacency_list[itr->first]);
		for(Edge e : edges)
		{
			if(labels[e.n2] != itr->second)
				sum += e.weight; 
		}
		itr++;
	
	}
	// printf("ones is %d\n",ones );
	return sum / 2;
}

vector<Graph> divideGraph(Graph graph, map<int, int> labels)
{
	map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
	vector<Graph> out;
	out.push_back(*(new Graph())); // first graph 
	out.push_back(*(new Graph())); // second graph
	// printf("asdsad\n");
	while(iter != graph.adjacency_list.end())
	{
		Node node = get<0>(iter->second);
		Graph& g = out[labels[node.id]];
		if (not g.NodeExists(node.id))
			{
				vector<Edge> ve;
				g.createAdjacencyList(node.id, ve);
		}
		for (Edge e : get<1>(iter->second))
		{
			// e.printEdge();
			if((int)(labels[e.n2] != labels[node.id]))
			{
				continue;
			}
			g.getEdges(node.id).push_back(e);

		}
		// printf("\n");
		iter++;
	}
	return out;
}

map<int, int> Bipartition(Graph graph,int num_threads)
{
	map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
	int total_weight = 0;
	while(iter != graph.adjacency_list.end())
	{
		total_weight += get<0>(iter->second).weight;
		iter++;
	}
	// cout<<"total="<<total_weight<<endl;

	map<int,int> mincuts;
	map<int, map<int,int> > parts;

	#pragma omp parallel num_threads(num_threads)
	{
		map<int, int> labels;
		int weight = 0;
		int id = omp_get_thread_num();
		do
		{
			weight = 0;
			tuple<Node,vector <Edge>> startNode;
			iter = graph.adjacency_list.begin();	
			int temp = 0;
			map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
			labels = *(new map<int, int>);
			while(iter != graph.adjacency_list.end())
			{
				labels.insert(pair < int, int> (iter->first,0));
				iter++;
			}
			srand (time(NULL));
			int randomno = (rand()*id )% graph.adjacency_list.size();
			iter = graph.adjacency_list.begin();
			while(iter != graph.adjacency_list.end())
			{
				if(temp < randomno)
					temp++;
				else
				{
					startNode = iter->second;
					break;
				}
				iter++;
			}
			// printf("start = %d\n",get<0>(startNode).id);
			// startNode = get<0>(graph.adjacency_list[id])

			list<int> queue;
			queue.push_back(get<0>(startNode).id);
			labels[get<0>(startNode).id] = 1;
			
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
				// printf("%d\n", weight); 
				if(weight>=0.5*total_weight)
					break;
				queue.pop_front();
				for(unsigned int i = 0; i< get<1>(graph.adjacency_list[s]).size();i++)
				{
					if(labels[get<1>(graph.adjacency_list[s])[i].n2]==0)
						queue.push_back(get<1>(graph.adjacency_list[s])[i].n2);
				}
			}
		}
		while(weight<0.45*weight);


		parts.insert(pair <int,map<int,int> >(id,labels));
		mincuts.insert(pair <int,int>(id,EdgeCut(labels,graph)));
	}
	int min = mincuts[0];
	int minid = 0;
	map<int, int > :: iterator itr = mincuts.begin();
	while(itr != mincuts.end())
	{
		// printf("%d\n",itr->first);
		if(min >= itr->second)
		{	
			min = itr->second;
			minid = itr->first;
		}
		itr++;
	}

	return parts[minid];
}
	// {
	// 	printf("hell %d",ex);
	// 	int mincut = EdgeCut(labels,graph);
	// 	map<int, int> :: iterator itr = labels.begin();
	// 	int big = 0;
	// 	vector<int> gain(labels.size());

	// 	while(itr != labels.end())
	// 	{
	// 		int t=0;
	// 		for (int i = 0; i < get<1>(graph.adjacency_list[itr->first]).size(); ++i)
	// 		{
	// 			if(itr->second == 1)
	// 			{
	// 				if(labels[get<1>(graph.adjacency_list[itr->first])[i].n2]==0)
	// 					t+=get<1>(graph.adjacency_list[itr->first])[i].weight;
	// 				else
	// 					t-=get<1>(graph.adjacency_list[itr->first])[i].weight;
	// 			}
	// 			else
	// 			{
	// 				if(labels[get<1>(graph.adjacency_list[itr->first])[i].n2]==1)
	// 					t+=get<1>(graph.adjacency_list[itr->first])[i].weight;
	// 				else
	// 					t-=get<1>(graph.adjacency_list[itr->first])[i].weight;	
	// 			}
	// 		}
	// 		gain[itr->first] = t;

	// 		if(itr->second == 1)
	// 			big+=get<0>(graph.adjacency_list[itr->first]).weight;
	// 		if(itr->second == 0)
	// 			big-=get<0>(graph.adjacency_list[itr->first]).weight;
	// 		itr++;
	// 	}

	// 	if(big>0)
	// 	{
	// 		int j=-1;
	// 		int maxi = 0;
	// 		for (int i = 0; i < gain.size(); ++i)
	// 		{
	// 			if(gain[i]>=maxi && labels[i]==0)
	// 			{
	// 				maxi = gain[i];
	// 				j=i;
	// 			}
	// 		}
	// 		labels[j] = 0;
	// 		if(mincut<EdgeCut(labels,graph))
	// 			labels[j]=1;
	// 	}
	// 	else if(big<0)
	// 	{
	// 		int j=-1;
	// 		int maxi = 0;
	// 		for (int i = 0; i < gain.size(); ++i)
	// 		{
	// 			if(gain[i]>=maxi && labels[i]==1)
	// 			{
	// 				maxi = gain[i];
	// 				j=i;
	// 			}
	// 		}
	// 		labels[j] = 1;
	// 		if(mincut<EdgeCut(labels,graph))
	// 			labels[j]=0;
	// 	}
	// 	else
	// 	{
	// 		int j1=-1;
	// 		int maxi = 0;
	// 		for (int i = 0; i < gain.size(); ++i)
	// 		{
	// 			if(gain[i]>=maxi && labels[i]==1)
	// 			{
	// 				maxi = gain[i];
	// 				j1=i;
	// 			}
	// 		}
	// 		labels[j1] = 0;
			
	// 		int j2=-1;
	// 		maxi = 0;
	// 		for (int i = 0; i < gain.size(); ++i)
	// 		{
	// 			if(gain[i]>=maxi && labels[i]==0)
	// 			{
	// 				maxi = gain[i];
	// 				j2=i;
	// 			}
	// 		}
	// 		labels[j2] = 1;
			

	// 		if(mincut<EdgeCut(labels,graph))
	// 		{
	// 			labels[j1]=1;
	// 			labels[j2]=0;
	// 		}

	// 	}
	// }


// mode = 1 , PARALLEL MODE store the old nodes
// mode = 0 , UNION MODE do not store the old nodes
Graph updateEdges(Graph& graph, int level_coarsening, int mode, int debug) 
{
	Graph new_graph = *(new Graph());
	if (debug)
	{
		printf("in updateEdges - level = %d\n", level_coarsening);
		graph.printGraph();
	}
	map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
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
		
		vector<Edge> new_neighbours;

		if(old_n.consumer != -1) // If consumed then don't add the node to the new graph
		{
			if (mode == 0)
			{
				iter++;
				continue;
			}
			// We store the old node so that we can keep track of the consumer of the prey
			Node new_n = old_n;
			new_graph.createAdjacencyList2(new_n, new_neighbours);
			iter++;
			continue;
		}

		Node new_n = *(new Node(old_n.id));
		new_n.weight = old_n.weight;
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

		if (mode == 0)
		{
			iter++;
			continue;
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
Graph FindMatching(Graph graph,int id,int chunk_size, int level_coarsening, int debug)
{
	
	if (debug)
	{
		printf("\nFIND Matching  level = %d \n", level_coarsening);
		graph.printGraph();
	}
	vector<tuple<Node,Node> > matchings;
	map<int,  tuple<Node,vector <Edge> > > :: iterator iter = graph.adjacency_list.begin();
	
	while(iter != graph.adjacency_list.end())
	{
		vector<int> weights;
		vector<int> pos;
		for (unsigned int j = 0; j < get<1>(iter->second).size(); ++j)
		{
			weights.push_back((get<1>(iter->second))[j].weight);
			pos.push_back(distance(weights.begin(), max_element(weights.begin(), weights.end())));
			weights[j]=0;
		}

		for (int j : pos)
		{
			// Check whether node is inside the chunk or not
			// if(((get<1>(iter->second))[j]).n2 <= (id+1)*chunk_size && (get<1>(iter->second))[j].n2 > (id)*chunk_size)
			if(graph.NodeExists(((get<1>(iter->second))[j]).n2))
			{
				if(get<0>(iter->second).matched==0)
				{		
					Node& predator = get<0>(iter->second);
					Node& prey = graph.getNode((get<1>(iter->second))[j].n2);
					

					if(prey.matched==0)
					{
						prey.matched = 1;
						predator.matched = 1;
						matchings.push_back(make_tuple(predator, prey));
						
						// Eating and being eaten
						// TODO: Store the prey for each level using map
						// map< int , map
						Food_Chain[predator.id].push_back(prey.id);
						predator.food_chain.insert(pair <int, Node*> (level_coarsening, &prey));
						prey.consumer = predator.id;
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
	Graph new_graph = updateEdges(graph, level_coarsening, 1, debug);
	// printf("++++++\n");
	// new_graph.printGraph();
	// printf("@+++++\n");
	return new_graph;
}


map<int, int> Partition(Graph& input, int num_threads, int k)
{
	vector<Graph> breaks;
	map<int, vector<Graph> > coarse_graphs;	//[proc][k_level]
	map<int, Graph> union_coarse_graphs;	//[k_level]

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
			map<int,  tuple<Node,vector <Edge> > > :: iterator iter = input.adjacency_list.begin();
			for(int i = 0; i< (id) * chunk_size; i++)
			{
				iter++;
			}
			for(int i=id*chunk_size;i<(id+1)*chunk_size; i++)
			{
				breaks[id].createAdjacencyList(iter->first, get<1>(iter->second));
				iter++;
			}
		}
		else
		{
			map<int,  tuple<Node,vector <Edge> > > :: iterator iter = input.adjacency_list.begin();
			for(int i = 0; i< (id) * chunk_size; i++)
			{
				iter++;
			}
			for (unsigned int i = id*chunk_size; i < input.adjacency_list.size(); ++i)
			{
				breaks[id].createAdjacencyList(iter->first, get<1>(iter->second));
				iter++;			}
		}
		vector<Graph> coarse_p;
		coarse_p.push_back(breaks[id]);
		int k_level = 0; // level of coarsening each processor does
		while(k_level < STOPPING_CONDITION)
		{
		 	coarse_p[0] = (FindMatching(coarse_p[0], id, chunk_size, k_level, debug));
		 	k_level++;
		 	#pragma omp barrier
		 	// printf("Level = %d\n", k_level);
		}
		// TODO: Set a stopping condition which is consistent with all the threads
		
		#pragma omp critical
		{
			coarse_graphs.insert(pair <int, vector<Graph> > (id, coarse_p));	
		}
		

		// map<int, int> parts = Bipartition(coarse_graphs[id][k_level],0);
	}
	// {paralled ends}

	// Find union of pralllel graphs 

	// for (int y = 1; y <= STOPPING_CONDITION; y++)
	// {
	// 	for (int x = 1; x < num_threads; x++)
	// 	{
	// 		// merging the adjacency lists of all the processors into processor 0
	// 		coarse_graphs[0][y].adjacency_list.insert(coarse_graphs[x][y].adjacency_list.begin(), coarse_graphs[x][y].adjacency_list.end());
	// 	}
	// 	// printf("\n%d ----\n", y);
	// 	// coarse_graphs[0][y].printGraph();
	// 	printf("\n%d Updated ----\n", y);
	// 	union_coarse_graphs.insert(pair <int, Graph> (y, updateEdges(coarse_graphs[0][y], y, 0, 0)));
	// 	union_coarse_graphs[y].printGraph();
	// }

	for (int x = 1; x < num_threads; x++)
	{
		// merging the adjacency lists of all the processors into processor 0
		coarse_graphs[0][0].adjacency_list.insert(coarse_graphs[x][0].adjacency_list.begin(), coarse_graphs[x][0].adjacency_list.end());
	}
	// printf("Union find\n");
	// printf("Partition k = %d\n", k);
	// input.printGraph();
	union_coarse_graphs.insert(pair <int, Graph> (0, updateEdges(coarse_graphs[0][0], 0, 0, 0)));
	// printf("Number of nodes = %d\n", union_coarse_graphs[0].numNodes());
	
	// double start_time = omp_get_wtime();
	
	map <int,int> bipart = Bipartition(union_coarse_graphs[0],num_threads);
	// // union_coarse_graphs[0].printGraph();
	// // printf("biparts\n");
	// map<int,int> :: iterator itere = bipart.begin();
	// while(itere!=bipart.end())
	// {
	// 	printf("%d -> %d\n",itere->first, itere->second);
	// 	itere++;
	// }

	map<int, int> parts = Uncoarsen(coarse_graphs[0][0], bipart);
	// double time_taken = omp_get_wtime() - start_time;
	// cout << "\nTime taken (Bipartition) = " << time_taken << endl;
	map<int,int> :: iterator itert = parts.begin();
	// while(itert!=parts.end())
	// {
	// 	printf("%d -> %d\n",itert->first, itert->second);
	// 	itert++;
	// }
	// cout<<"mincut="<<EdgeCut(parts,input) << endl;
	
	if(k<=2)
	{
		// map<int,int> :: iterator iter = parts.begin();
		// printf("K = %d\n", k);
		// while(iter!=parts.end())
		// {
		// 	printf("%d -> %d\n",iter->first, iter->second);
		// 	iter++;
		// }
		return parts;
	}
	vector<Graph> div = divideGraph(input, parts);

	map<int,int> a = Partition(div[0],num_threads,k/2);
	map<int,int> b = Partition(div[1],num_threads,k/2);
	
	// div[0].printGraph();
	// div[1].printGraph();
	
	map<int,int> :: iterator itera = a.begin();
	// printf("a = \n");
	
	while(itera!=a.end())
	{
		// printf("%d ",itera->second);
		itera->second *= 2;
		itera->second += 1;
		itera++;
	}
	itera = b.begin();
	// printf("b = \n");
	while(itera!=b.end())
	{
		// printf("%d ",itera->second);
		itera->second *= 2;
		itera++;
	}

	a.insert(b.begin(),b.end());
	
	// itera = a.begin();
	// // printf("final list K = %d\n", k);
	// while(itera!=a.end())
	// {
	// 	printf("%d -> %d\n",itera->first, itera->second);
	// 	itera++;
	// }
	// printf("\n");
	return a;
	// map<int,  std::vector<int>> :: iterator FCiter = Food_Chain.begin();
	// while(FCiter != Food_Chain.end())
	// {
	// 	printf("%d : ", FCiter->first);
	// 	for (int x : FCiter->second)
	// 	{
	// 		printf("%d ", x);
	// 	}
	// 	printf("\n");
	// 	FCiter++;
	// }
	// Project(parts,input.adjacency_list.size()); // vomit recursive till rhs is 0
	// return parts;
}