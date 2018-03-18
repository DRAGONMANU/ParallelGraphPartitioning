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
	map<int,  tuple<Node,vector <Edge>>> adjacency_list;
	
	
	int NodeExists(int x)
	{
		return (adjacency_list.find(x) != adjacency_list.end());
	}

	Node& getNode(int x)
	{
		return get<0>(adjacency_list[x]);
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
		adjacency_list.insert( pair < int, tuple<Node,vector <Edge>> > (n1.getId(),  make_tuple(n1, neighbours)) );
	}

	void printGraph() 
	{
		printf("Printing Graph\n");

		map<int,  tuple<Node,vector <Edge>>> :: iterator itr = adjacency_list.begin();
		while(itr != adjacency_list.end())
		{
			printf("Node- %d ", get<0>(itr->second).getId());
			for (unsigned int j = 0; j < get<1>(itr->second).size(); j++ )
			{
				( get<1>(itr->second)[j] ).printEdge();
			}
			printf("\n");
			itr++;
		}
	}
};

