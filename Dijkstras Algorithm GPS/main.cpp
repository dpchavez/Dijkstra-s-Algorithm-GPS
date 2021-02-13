/*main.cpp*/

//
// Daniela Chavez
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07 part 2: open street maps, graphs, and Dijkstra's alg
// 
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:  
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <utility>
#include <limits>
#include<bits/stdc++.h> 

#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"

using namespace std;
using namespace tinyxml2;

/* Project 7
 * TAKEN FROM PDF:
 * The assignment is to write a console-based C++ program to input a campus
 * map (e.g. UIC’s East campus) and navigate between buildings via footways. 
 * The program should be general enough to work with any college map, though 
 * we don’t plan to extensively test this. Given the time constraints, we’re 
 * going to provide helper functions to read the map for you, which are available
 * in XML format. Your job is to build the underlying graph, input two buildings
 * from the user, and then use Dijkstra’s algorithm to find the shortest weighted 
 * path. This is repeated until the user enters # for the start building.
 * */
/*************************************************************************************************************/
//TAKEN FROM HOMEWORK 17
class Prioritize {
    public:
        bool operator()(const pair < long long, double > & p1,
            const pair < long long, double > & p2) const {
            // if you want smaller values @ the end, return true if 
            // p1's value < p2's value.  if you want larger values
            // @ the end, return true if p1's value > p2's value.
            if (p1.second == p2.second)
                return p1.first > p2.first;

            else
                return p1.second > p2.second;
        }
};

/*****************************************************************************************************************/
/*TAKEN FROM HW 17
Dijkstra:
Performs Dijkstra's shortest weighted path algorithm from the given start vertex.  Returns a vector of vertices in
the order they were visited, along with a map of (string,int) pairs where the string is a vertex V and the int is the 
distance from the start vertex to V
*/
vector<long long>Dijkstra(graph<long long,double>& G, long long startV, 
	map<long long,long long>&predecessors,map<long long, double>&distances)
{
	double INF = numeric_limits<double>::max();
    vector <long long> Visited; //This vector will store the visited nodes 
    priority_queue < pair <long long, double >, vector<pair <long long, double>> , Prioritize >unvisitedQ; //Unvisited nodes
    vector <long long> Vertexs = G.getVertices(); //Thos vector will store the vertices of the G graph
	
	//Travesring the Vertexs vector and storing the information inside of predecessors, distances and unvisitedQ
    for (long long currentV: Vertexs) {
		predecessors[currentV] = -1;
        distances[currentV] = INF;
        unvisitedQ.push(make_pair(currentV, INF));
    }
    distances[startV] = 0;
    unvisitedQ.push(make_pair(startV, 0));
	
	//This while will traverse the queue
    while (!unvisitedQ.empty()) {
        bool mark = false; // This will be used to identify  if the elemnt is equal to Current
        long long Current = unvisitedQ.top().first; //Storing the first element of the queue
        unvisitedQ.pop(); //deleting first element

        for (long long S: Visited) { //Looping through the Visited vector
            if (S == Current) // If S is  equal to the current 
                mark = true; // true if S is equal to current
        }
		
        if (distances[Current] == INF) // If distances[Current] has reached its Max value, then break 
            break;
		
        else if (mark == true) //Coninue
            continue;
		
        else
            Visited.push_back(Current);

        set <long long> N = G.neighbors(Current);
		
        for (long long adjV: N) {
            double Weight = 0;
            G.getWeight(Current, adjV, Weight);
			
            double alt = Weight + distances[Current];
            if (alt < distances[adjV]) {
				predecessors[adjV] = Current;
                distances[adjV] = alt;
                unvisitedQ.push(make_pair(adjV, alt));
            }
        }
    }
    return Visited; //Returns path
}

/****************************************************************************************************************************/
/*This is my minDistance function. This functions purpose is to search the footways and find the nearest nodes to the start
 * and destination buildings; these become the “start” and “dest” nodes, assuming the start and destination buildings were found,
 * you have the start and destination coordinates.*/
long long minDistance(vector<BuildingInfo>&Buildings, map<long long, Coordinates>Nodes,
					  vector<FootwayInfo>&Footways, long long Destination){
	//Following the instructions of the project PDF
	double CurrDist = numeric_limits<double>::max();
	double temp =  numeric_limits<double>::max();
	long long NodeId = 0;
	  
	   for (unsigned x = 0; x < Footways.size(); x++){ 
		  for (unsigned y = 0; y < Footways[x].Nodes.size(); y++){
			  //Obtaining distance from Starting building and nodes in Footways
			  CurrDist = distBetween2Points(Nodes[Footways[x].Nodes[y] ].Lat,
		      Nodes[Footways[x].Nodes[y]].Lon, Buildings[Destination].Coords.Lat,Buildings[Destination].Coords.Lon);
			  
			  if (CurrDist < temp){
				  temp = CurrDist;
				  NodeId = Nodes[Footways[x].Nodes[y]].ID;
			  }
		   }
	   }
	return NodeId; //This is the current node ID we need
}
/**************************************************************************************************************************/
/*Add edges based on footways.
A footway is a vector of nodes, defining points along the footway. Let’s
suppose the footway is {N1, N2, N3, N4}. Then you add edges in both directions between N1-N2, 
N2-N3, and N3-N4.A footway contains at least 2 nodes. The purpose of this function is to retrieve 
information from the vector Footways and Nodes map and insert the correct data to get the edges 
of each node into the graph G*/
void addEdges(map<long long,Coordinates> Nodes, vector<FootwayInfo>Footways, graph<long long,double>&G){
	for (unsigned x = 0; x < Footways.size(); x++){ //Looping through every vector
	  for (unsigned y = 0; y < Footways[x].Nodes.size() -1; y++){
		  //unsing distBetween2Points
		  double distance = distBetween2Points(Nodes[Footways[x].Nodes[y] ].Lat,
		  Nodes[Footways[x].Nodes[y]].Lon, Nodes[Footways[x].Nodes[y + 1]].Lat,
		  Nodes[Footways[x].Nodes[y + 1]].Lon);
		  //Adding edges one info is recollected	  
		  G.addEdge(Footways[x].Nodes[y], Footways[x].Nodes[y + 1],distance);
		  G.addEdge(Footways[x].Nodes[y+1], Footways[x].Nodes[y],distance);	
		 }
	  }	
}
/****************************************************************************************************************************/
/*This function will be used after confirming that the distances[DestinationNodeId] != INF , then we output the
distance to the destination point and the path */
void DijkstraImplementation(long long DestinationNodeId, map<long long, double>distances, map<long long, long long>predecessors){
	cout << "Distance to dest: "<< distances[DestinationNodeId] << " miles"<< endl;
	cout << "Path: ";
	//This is where we are going to store the paths 	
	deque<long long>D;
	long long index = DestinationNodeId;
	//Pushing in paths into deque	 
	while (index != -1){ 
		D.push_back(index);
		index = predecessors[index];
	}	
	//This  will output the paths from starting point to destination
	while (!D.empty()){  
		cout << D.back();
		D.pop_back();
		if (!D.empty()){
		cout << "->";
		}
	}
}

/****************************************************************************************************************************/
/*This function will be used to output the coordinates of the found ID in the Nodes map assuming
 *that the correct nodes ID's have been */
void buildingCoordinates(long long NodeID1, long long NodeID2, map<long long, Coordinates>Nodes,
	vector<FootwayInfo>Footways,vector<BuildingInfo>&Buildings, long long &StartNodeId,long long &DestinationNodeId){
	
	StartNodeId = minDistance(Buildings,Nodes,Footways,NodeID1);
	DestinationNodeId = minDistance(Buildings,Nodes,Footways,NodeID2);
	  
	cout << endl;
	cout << "Nearest start node:"<< endl;
	cout << " " << StartNodeId << endl;
	cout << " (" << Nodes[StartNodeId].Lat << ", " << Nodes[StartNodeId].Lon << ")"<< endl;

	cout << "Nearest destination node:"<< endl;
	cout << " " << DestinationNodeId << endl;
	cout << " (" << Nodes[DestinationNodeId].Lat << ", " << Nodes[DestinationNodeId].Lon << ")"<< endl;
	
	
}

/****************************************************************************************************************************/
/*This is my vector iteration. The purpose of this function is to iterarte through a vector and then return the
index in which the userInput matches the full name of the building or its abreviaton */
unsigned vectorIteration(string userInput, vector<BuildingInfo>&Buildings){
	unsigned index;
	for(index = 0; index != Buildings.size(); index++){
		if( userInput == Buildings[index].Abbrev || string::npos != Buildings[index].Fullname.find(userInput)){
			//If the user input is found among the fullname building names or their abbreviations, then retunr the index
			return index;
		}
	}
	//This part means that the building name or abbreviation have not been found...
	return index;
	
}
/***********************************************************************************************************************/
int main()
{
  map<long long, Coordinates>  Nodes;     // maps a Node ID to it's coordinates (lat, lon)
  vector<FootwayInfo>          Footways;  // info about each footway, in no particular order
  vector<BuildingInfo>         Buildings; // info about each building, in no particular order
  XMLDocument                  xmldoc;
  double INF = numeric_limits<double>::max();
	
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "")
  {
    filename = def_filename;
  }

  // Load XML-based map file 
  if (!LoadOpenStreetMap(filename, xmldoc))
  {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  
  // Read the nodes, which are the various known positions on the map:
  unsigned int nodeCount = ReadMapNodes(xmldoc, Nodes);

  // Read the footways, which are the walking paths:
  unsigned int footwayCount = ReadFootways(xmldoc, Footways);

  // Read the university buildings:
  unsigned int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  // Stats
  assert(nodeCount == Nodes.size());
  assert(footwayCount == Footways.size());
  assert(buildingCount == Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

  /**************************************************************************************************/	
  // TODO: build the graph, output stats:
  graph<long long,double> G;
  //Adding nodes as vertices
  for (pair<long long, Coordinates> P : Nodes){
	  G.addVertex(P.first);
  }
  //add edges
  addEdges(Nodes,Footways,G);
  //Printing edges
  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  /**************************************************************************************************/
  // Navigation from building to building
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#")
  {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);
	  
	 // i and j are the returnd indices of the found startBuilding and destBuilding
	 unsigned i = vectorIteration(startBuilding, Buildings);
	 unsigned j = vectorIteration(destBuilding, Buildings);
	  
    /**********************************STARTING POINT********************************************/ 
	if (i >= Buildings.size()) //startBuilding not found
		cout << "Start building not found"<< endl;

	else if (j < Buildings.size()){ //startBuilding found
		cout << "Starting point:"<< endl;
		cout <<" "<<Buildings[i].Fullname << endl;
		cout <<" (" <<Buildings[i].Coords.Lat << ", "<< Buildings[i].Coords.Lon <<")"<<endl;
	}
 
    /**************************************DESTINATION POINT************************************/	  
		if (i < Buildings.size() && j >= Buildings.size())
			cout << "Destination building not found"<< endl;
	  
	   //Just checking for requirements before continuing
		else if (i < Buildings.size() && j < Buildings.size()){
			cout << "Destination point:"<< endl;
			cout <<" "<<Buildings[j].Fullname << endl;
			cout <<" (" <<Buildings[j].Coords.Lat << ", "<< Buildings[j].Coords.Lon <<")"<<endl;
				
		/******************************* GETTING CLOSEST NODE *************************************/
			long long StartNodeId = 0;
			long long DestinationNodeId = 0;
			buildingCoordinates(i,j,Nodes,Footways,Buildings, StartNodeId,DestinationNodeId);
        /********************************************************************************************************/
		// TODO: lookup buildings, find nearest start and dest nodes,
		// run Dijkstra's alg, output distance and path to destination:
			map<long long, double>distances;
			map<long long, long long>predecessors; 
			vector<long long> X = Dijkstra(G,StartNodeId, predecessors, distances);
	  
			cout << endl;
			cout << "Navigating with Dijkstra..." << endl;
		 
			if (distances[DestinationNodeId] != INF)
				DijkstraImplementation(DestinationNodeId,distances,predecessors);
		
			else 
				cout <<"Sorry, destination unreachable";
		
		  cout<<endl;
		 }

    // another navigation?
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  // done:
  cout << "** Done **" << endl;

  return 0;
}
