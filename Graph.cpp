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
	Node* eaten;
	vector<Edge> external_edges;
	Node(){
		id = -1;
		matched = 0;
	}
	Node(int id1) {
		id = id1;
		weight = 1;
	}
	void addEaten(Node* food)
	{	
		weight += food->weight;
		eaten = food;
	}
	int getId()
	{
		return id;
	}
};

class Graph
{
public:
	vector <tuple<Node,vector <Edge>>> adjacency_list;
	Graph(){}
	
	int NodeExists(int x)
	{
		for (unsigned int i = 0; i < adjacency_list.size(); i++)
		{
			if (x == get<0>(adjacency_list[i]).getId())
			{
				return 1;
			}
		}
		return 0;
	}

	Node& getNode(int x)
	{
		Node n =(*new Node());
		for (unsigned int i = 0; i < adjacency_list.size(); i++)
		{
			if (x == get<0>(adjacency_list[i]).getId())
			{
				return get<0>(adjacency_list[i]);
			}
		}
		return n;
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
			n1 = *(new Node(x1));
		}
		adjacency_list.push_back( make_tuple(n1, neighbours));
	}

	void printGraph() 
	{
		printf("Printing Graph\n");
		for (unsigned int i = 0; i < adjacency_list.size(); i++)
		{
			printf("Node- %d ", get<0>(adjacency_list[i]).getId());
			for (unsigned int j = 0; j < get<1>(adjacency_list[i]).size(); j++ )
			{
				( get<1>(adjacency_list[i])[j] ).printEdge();
			}
			printf("\n");
		}
	}
};

// void printGraph(Graph g) {
// 	printf("Printing Graph\n");
// 	for (int i = 0; i < g.edge_list.size(); i++)
// 	{
// 		printf("%d %d\n", get<0>(g.edge_list[i]).id, get<1>(g.edge_list[i]).id);
// 	}
// }
