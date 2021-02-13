/*graph.h*/

//
// Daniela Chavez
// University of Illinois, Chicago
// Spring 2020 /Quarantine time
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// UPDATE: Now it us a graph class using adjecency list representation. 
//This does not have a size limitation
//
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include <utility>

using namespace std;


template<typename VertexT, typename WeightT>
class graph
{
private:
	bool EdgeExists;
	int numEdges; //will be used to return the number of edges
	int numVertices; //will be used to return the number of 
	map<VertexT, map<VertexT, WeightT>> DataEdges;


public:
  //
  // constructor:
  //
  // Constructs an empty graph .
  // NOTE: the graph is implemented using an adjacency list.
/****************************************************************************/
  graph()
  {
	numEdges = 0;
	numVertices = 0;
	EdgeExists = false;
  }
/****************************************************************************/
  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
    return numVertices;
  }
/****************************************************************************/
  // NumEdges
  // Returns the # of edges currently in the graph.

  int NumEdges() const
  {
	return numEdges;
  }
/****************************************************************************/
  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {
	  
	map<VertexT, WeightT> DummyMap; //empty map...
	//Making sure that the vertex does NOT exist in the map
    if (DataEdges.count(v) == 1)
		return false;
	//Vertex does not exist in the map... so now we insert...
	DataEdges.insert(make_pair(v, DummyMap));
	numVertices++;
    return true;
  }
/****************************************************************************/
  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    int index1 = DataEdges.count(from);
	int index2 = DataEdges.count(to);
	
	if(index1 == 1 && index2 == 1){
		//This is created to overwride the edges
		if(DataEdges[from].count(to) == 1){
			DataEdges[from].erase(to);
			numEdges--;
		}
		// DataEdges.at(from) will point at the value that is related to from
		// and in this case, the "value" is our second map in which we insert to and weight
		DataEdges.at(from).insert(make_pair(to, weight));
		numEdges++;
		return true;
	}
	//If you cant add edges, return false
	return false;
  }
/****************************************************************************/
  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const
  {
	  
	  //Making sure that from and to exist 
	  int index1 = DataEdges.count(from);
	  int index2 = DataEdges.count(to);
	 //If from and to exist... 
	 if(index1 == 1 && index2 == 1){
		 if(DataEdges.at(from).count(to) == 1){
			 weight = DataEdges.at(from).at(to);
			 return true;
		 }		
	}
	//If you cant find the vertices... return false
	return false;
  }
/****************************************************************************/

  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const
  {
    set<VertexT>  S;
	
    int index1 = DataEdges.count(v);
	//trying to see if V exists in DataEdges
    if (index1 == 0)  // not found:
		return S;
	
	//As v exists inside of DataEdges, we are going to store
	// the map inside of DataEdges into the Neighbors pair
	// Finally, we are inserting the first element of that map into the set S
    for(pair<VertexT, WeightT>Neighbors:DataEdges.at(v)){
		S.insert(Neighbors.first);
	}
    return S;
  }
/****************************************************************************/

  // getVertices
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const
  {
    vector<VertexT>dummyVector;
	for(pair<VertexT,map<VertexT,WeightT>> V : DataEdges){
		dummyVector.push_back(V.first);
	}
	return dummyVector;
  }
/****************************************************************************/
  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;

 //This is made to output all existing vertices
    output << endl;
    output << "**Vertices:" << endl;
	for(pair<VertexT,map<VertexT,WeightT>> V : DataEdges){
		output << V.first << endl;
	}
    
	//This is made to output all vertices and existing edges
    output << endl;
    output << "**Edges:" << endl;
	for(pair<VertexT,map<VertexT,WeightT>>i : DataEdges){
		output << i.first<< ": ";
		 for (pair<VertexT, WeightT> j : i.second){
			output << "(" << i.first <<", " << j.first << ", " << j.second << ") ";
		}
		output << endl;
	}
    output << "**************************************************" << endl;
  }

};