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
#include <tuple>
#include <map>

using namespace std;

class Edge
{
public:
	int n2;
	int weight;
	
	Edge(int a2, int wt) {
		n2 = a2;
		weight = wt;
	}

	void printEdge()
	{
		printf("[ %d (%d) ]", n2, weight);
	}
};

class Node
{
	public:	
	int id;
	int weight;
	int matched;
	map<int, Node*> food_chain;
	int consumer;
	vector<Edge> neighbours;
	vector<Edge> external_edges;
	
	Node(){
		id = -1;
		weight = 1;
		matched = 0;
		consumer = -1;
	}
	Node(int id1) {
		id = id1;
		weight = 1;
		matched = 0;
		consumer = -1;
	}


	int preyExists(int level_coarsening)
	{
		return food_chain.find(level_coarsening) != food_chain.end();
	}

	int predatorExists()
	{
		if (consumer == -1)
		{
			return 0;
		}	
		return 1;
	}
	// void Eat(Node *prey)
	// {	
	// 	weight += food->weight;
	// 	food = prey;
	// 	prey->consumer = this;
	// }
	int getId()
	{
		return id;
	}
};

class Graph
{
public:
	map<int, tuple<Node,vector <Edge> > > adjacency_list;
	
	int NodeExists(int x)
	{
		return (adjacency_list.find(x) != adjacency_list.end());
	}

	Node& getNode(int x)
	{
		return get<0>(adjacency_list[x]);
	}

	int numNodes()
	{
		return adjacency_list.size();
		map<int,  tuple<Node,vector <Edge> >> :: iterator iter = adjacency_list.begin();
		int numNodes = 0;
		while (iter != adjacency_list.end())
		{
			if(get<1>(iter->second).size() > 0)
			{
				numNodes++;
			}
			iter++;
		}
		return numNodes;
	}
	vector<Edge>& getEdges(int node_id)
	{
		return get<1>(adjacency_list[node_id]);
	}
	void createAdjacencyList(int x1, vector<Edge> neighbours)
	{
		Node n1; 
		if (NodeExists(x1) != 0)
		{
		 	n1 = getNode(x1);
		}
		else
		{
			//printf("ERROR Node does not exest x1 = %d\n", x1);
			n1 = *(new Node(x1));
		}
		n1.neighbours = neighbours;
		adjacency_list.insert( pair < int, tuple<Node,vector <Edge> > > (n1.getId(),  make_tuple(n1, neighbours)) );
	}

	void createAdjacencyList2(Node& n1, vector<Edge> neighbours)
	{
		n1.neighbours = neighbours;
		adjacency_list.insert( pair < int, tuple<Node,vector <Edge> > > (n1.getId(),  make_tuple(n1, neighbours)) );
	}

	int findMasterPredator(int n)
	{
		int mod_n = n;
		if (NodeExists(n) != 0)
		{
			Node& node = getNode(n);
			if(node.predatorExists() > 0)
			{
				mod_n = node.consumer;
				mod_n = findMasterPredator(mod_n);
			}
		}
		return mod_n;
	}
	Edge modifyEdgeWithParent(Edge& e)
	{
		int node2_id = e.n2;
		Edge mod_e = *(new Edge(e.n2, e.weight));

		if (NodeExists(node2_id) != 0)
		{
			Node& node2 = getNode(node2_id);
			if(node2.predatorExists() > 0)
			{
				mod_e.n2 = node2.consumer;
				mod_e = modifyEdgeWithParent(mod_e);
			}
		}
		return mod_e;
	}
	void insertEdge(int node_id, Edge new_e)
	{
		if(NodeExists(node_id))
		{
			// Don't insert itsef into the neighbour list
			if (node_id == new_e.n2)
			{
				return;
			}
			vector<Edge>& neighbour_list = getEdges(node_id);
			vector <Edge> :: iterator e_itr = neighbour_list.begin();
			while(e_itr != neighbour_list.end())
			{
				if(e_itr->n2 == new_e.n2)
				{
					e_itr->weight += new_e.weight;
					return;
				}
				e_itr++;
			}
			neighbour_list.push_back(new_e);
		}
	}

	void printGraph() 
	{
		printf("Printing Graph\n");

		map<int,  tuple<Node,vector <Edge> > > :: iterator itr = adjacency_list.begin();
		while(itr != adjacency_list.end())
		{
			printf("Node: %d (%d)", get<0>(itr->second).getId(), get<0>(itr->second).weight);
			// if (get<0>(itr->second).food != nullptr) // Food Exists
			// {
			// 	printf("(F - %d)", get<0>(itr->second).food->id);
			// }

			if (get<0>(itr->second).consumer != -1) // Consumer Exists
			{
				printf(" (C - %d)", get<0>(itr->second).consumer);
			}
			// printf("\t");
			// for (unsigned int j = 0; j < get<1>(itr->second).size(); j++ )
			// {
			// 	( get<1>(itr->second)[j] ).printEdge();
			// }
			printf("\n");
			itr++;
		}
	}
};

