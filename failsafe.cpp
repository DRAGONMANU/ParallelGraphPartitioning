#include <stdio.h>
#include "vector"
#include <string>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <iterator>
#include <algorithm>
#include <ctime>

using namespace std;

int main(int argc, char *argv[]) {
	int num_threads = 1;
	int num_nodes;
	int num_edges;
	if(argc!=4)
	{
		cout<<"\nerror in argument";
		return 0;
	}
	int k = atoi(argv[3]);
	char* inputpath = argv[1];
	char* outputpath = argv[2];
	string line;

	ifstream myfile(inputpath);
	if (myfile.is_open())
	{
		myfile >> num_nodes;
		myfile >> num_edges;
	}
	else
	{
		cout << "\nUnable to open file";
		return 0;
	}
	myfile.close();
	srand ( unsigned (time(0) ) );
	vector<int> myvector; 
	for(int i = 0 ;i<num_nodes;i++)
	{
		myvector.push_back(i%k);
	}
	random_shuffle ( myvector.begin(), myvector.end() );

	int zeros = 0;
	int ones = 0;

	ofstream outfile(outputpath);
	if(outfile.is_open())
	{
		for (std::vector<int>::iterator it=myvector.begin(); it!=myvector.end(); ++it)
	   	{
		    outfile << *it << "\n";
			if(*it==0)
				zeros++;
			else
				ones++;
		}
	}
	cout<<zeros<<endl;
	cout<<ones<<endl;
	return 0;
}