/*
 * main.hpp
 *
 *  Created on: Oct 30, 2013
 *		Author: fabiorjvieira
 */

#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <sstream>
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <cfloat>

const char NO = 0;
const char LEFT = 1;
const char RIGHT = 2;
const bool ASCENDING = true;
const char DEPTH_FIRST = 0;
const char BREADTH_FIRST = 1;
const char INTERLEVED = 2;
const unsigned short int NEIGHBOR = 1;

struct Enumeration
{
	bool sortOrder;
	char search;
};

struct FractionalColoring
{
	unsigned long int cycleSize, numberOfColors;
};

struct Link
{
	unsigned long int senderNode, receiverNode;
};

struct Path
{
	std::vector < unsigned long int > nodes;
};

struct Routes
{
	std::vector < Path > paths;
};

struct NodeNeighborhood
{
	std::vector < std::vector < bool > > neighbors;
};

struct ConflictMatrix
{
	std::vector < std::vector < char > > links;
};

void printSinks(ConflictMatrix & conflictMatrix)
{
   bool isSink;

   for (unsigned long int linkA = 0; linkA < conflictMatrix.links.size(); linkA++)
   {
      isSink = true;
      for (unsigned long int linkB = 0; linkB < linkA; linkB++)
         if (conflictMatrix.links.at(linkA).at(linkB) == RIGHT)
            isSink = false;
      for (unsigned long int linkB = linkA +1; linkB < conflictMatrix.links.size(); linkB++)
         if (conflictMatrix.links.at(linkA).at(linkB) == RIGHT)
            isSink = false;
      if (isSink) std::cout << linkA << " ";
   }
   std::cout << std::endl;
}

void loadRoutes(std::string fileName, Routes & routes)
{
	std::ifstream file;
	Path path;

	file.open(fileName.data(), std::ios::in);
	if (not file)
	 {
		std::cerr << fileName << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "#Reading file:" << fileName << std::endl;

	do
	{
		path.nodes.push_back(0);
		file >> path.nodes.back();

		if (file.peek() == '\n')
		{
			routes.paths.push_back(path);
			path.nodes.clear();
		}
	}
	while (!file.eof());
	if (path.nodes.size() > 1)
		routes.paths.push_back(path);

	file.close();
}

void loadNeighborhood(std::string fileName, NodeNeighborhood & nodeNeighborhood)
{
	std::ifstream file;
	std::vector < bool > neighbors;
	unsigned short int neighbor;

	file.open(fileName.data(), std::ios::in);
	if (not file)
	 {
		std::cerr << fileName << " not found!" << std::endl;
	 	exit(1);
	}
	std::cout << "#Reading file:" << fileName << std::endl;

	do
	{
		file >> neighbor;
		if (neighbor == NEIGHBOR) neighbors.push_back(true);
		else neighbors.push_back(false);

		if (file.peek() == '\n')
		{
			nodeNeighborhood.neighbors.push_back(neighbors);
			neighbors.clear();
		}
	}
	while (!file.eof());
	if (neighbors.size() > 1)
		nodeNeighborhood.neighbors.push_back(neighbors);

	file.close();
}

void sortRoutes(Routes & routes, bool sortOrder)
{
	//insertion sort
	unsigned long int holePosition;
	Path pathToInsert;

	//links must be order by the sum of the interference caused by the others and the interference provoke on others
	for (unsigned long int path = 1; path < routes.paths.size(); path++)
	{
		pathToInsert = routes.paths.at(path);
		holePosition = path;
		while (holePosition > 0 and ((pathToInsert.nodes.size() < routes.paths.at(path).nodes.size()) == sortOrder))
		{
			routes.paths.at(holePosition) = routes.paths.at(holePosition - 1);
			holePosition--;
		}
		routes.paths.at(holePosition) = pathToInsert;
	}
}

void buildLinks(Routes & routes, std::vector < Link > & links, char search)
{
	long int hop;
	Link link;
	bool noMoreHops, flipFlop;

	links.clear();

	if (search == DEPTH_FIRST)
	{
		hop = 0;
		noMoreHops = false;
		while (not noMoreHops)
		{
			noMoreHops = true;
			for (unsigned long int path = 0; path < routes.paths.size(); path++)
			{
				if (hop +1 < (int) routes.paths.at(path).nodes.size())
				{
					link.senderNode = routes.paths.at(path).nodes.at(hop);
					link.receiverNode = routes.paths.at(path).nodes.at(hop +1);
					links.push_back(link);
					noMoreHops = false;
				}
			}
			hop++;
		}
	}
	else if (search == BREADTH_FIRST)
	{
		for (unsigned long int path = 0; path < routes.paths.size(); path++)
		{
			for (hop = 0; hop +1 < (int) routes.paths.at(path).nodes.size(); hop++)
			{
				link.senderNode = routes.paths.at(path).nodes.at(hop);
				link.receiverNode = routes.paths.at(path).nodes.at(hop +1);
				links.push_back(link);
			}
		}
	}
	else if (search == INTERLEVED)
	{
		flipFlop = true;
		noMoreHops = false;
		hop = 0;
		while (not noMoreHops)
		{
			noMoreHops = true;
			for (unsigned long int path = 0; path < routes.paths.size(); path++)
			{
				flipFlop = not flipFlop;
				if (flipFlop)
				{
					if (hop +1 < (int) routes.paths.at(path).nodes.size())
					{
						link.senderNode = routes.paths.at(path).nodes.at(hop);
						link.receiverNode = routes.paths.at(path).nodes.at(hop +1);
						links.push_back(link);
						noMoreHops = false;
					}
				}
				else
				{
					if ((int)routes.paths.at(path).nodes.size() - hop -2 >= 0)
					{
						link.senderNode = routes.paths.at(path).nodes.at(routes.paths.at(path).nodes.size() - hop -1);
						link.receiverNode = routes.paths.at(path).nodes.at(routes.paths.at(path).nodes.size() - hop -2);
						links.push_back(link);
						noMoreHops = false;
					}
				}
			}
			hop++;
		}
	}
}

void buildConflictMatrix(std::vector < Link > & links, NodeNeighborhood & nodeNeighborhood, ConflictMatrix & conflictMatrix, bool distance2)
{
	unsigned long int senderNodeA, senderNodeB, receiverNodeA, receiverNodeB;

	conflictMatrix.links.resize(links.size());
	for (unsigned long int link = 0; link < links.size(); link++)
		conflictMatrix.links.at(link).resize(links.size(), NO);

	//distance 1
	for (unsigned long int linkA = 0; linkA < links.size(); linkA++)
	{
		senderNodeA = links.at(linkA).senderNode;
		receiverNodeA = links.at(linkA).receiverNode;

		for (unsigned long int linkB = linkA +1; linkB < links.size(); linkB++)
		{
			senderNodeB = links.at(linkB).senderNode;
			receiverNodeB = links.at(linkB).receiverNode;
			if (senderNodeA == senderNodeB or
					senderNodeA == receiverNodeB or
					receiverNodeA == receiverNodeB or
					receiverNodeA == senderNodeB)
			{
				conflictMatrix.links.at(linkA).at(linkB) = LEFT;
				conflictMatrix.links.at(linkB).at(linkA) = RIGHT;
			}
		}
	}

	//distance 2
	if (distance2)
	{
		for (unsigned long int linkA = 0; linkA < links.size(); linkA++)
		{
			senderNodeA = links.at(linkA).senderNode;
			receiverNodeA = links.at(linkA).receiverNode;

			for (unsigned long int linkB = linkA +1; linkB < links.size(); linkB++)
			{
				senderNodeB = links.at(linkB).senderNode;
				receiverNodeB = links.at(linkB).receiverNode;
				if (	nodeNeighborhood.neighbors.at(senderNodeA).at(receiverNodeB) or
//only valid in bidirectional communication networks
//						nodeNeighborhood.neighbors.at(senderNodeA).at(senderNodeB) or
//						nodeNeighborhood.neighbors.at(receiverNodeA).at(receiverNodeB) or
						nodeNeighborhood.neighbors.at(receiverNodeA).at(senderNodeB))
				{
					conflictMatrix.links.at(linkA).at(linkB) = LEFT;
					conflictMatrix.links.at(linkB).at(linkA) = RIGHT;
				}
			}
		}
	}
}

void edgeReversalDynamics(ConflictMatrix & conflictMatrix)
{
	std::vector < unsigned long int > sinks;
	bool isSink;
	unsigned long int sinkLink;

   /*debug*/printSinks(conflictMatrix);

	for (unsigned long int linkA = 0; linkA < conflictMatrix.links.size(); linkA++)
	{
		isSink = true;
		for (unsigned long int linkB = 0; linkB < linkA and isSink; linkB++)
			if (conflictMatrix.links.at(linkA).at(linkB) == RIGHT)
				isSink = false;
		for (unsigned long int linkB = linkA +1; linkB < conflictMatrix.links.size() and isSink; linkB++)
			if (conflictMatrix.links.at(linkA).at(linkB) == RIGHT)
				isSink = false;
		if (isSink) sinks.push_back(linkA);
	}

	for (unsigned long int sinkIndex = 0; sinkIndex < sinks.size(); sinkIndex++)
	{
		sinkLink = sinks.at(sinkIndex);
		for (unsigned long int link = 0; link < sinkLink; link++)
			if (conflictMatrix.links.at(sinkLink).at(link) != NO)
			{
				conflictMatrix.links.at(sinkLink).at(link) = RIGHT;
				conflictMatrix.links.at(link).at(sinkLink) = LEFT;
			}

		for (unsigned long int link = sinkLink +1; link < conflictMatrix.links.size(); link++)
			if (conflictMatrix.links.at(sinkLink).at(link) != NO)
			{
				conflictMatrix.links.at(sinkLink).at(link) = RIGHT;
				conflictMatrix.links.at(link).at(sinkLink) = LEFT;
			}
	}
}

bool compareConflictMatrices(ConflictMatrix & conflictMatrix, ConflictMatrix & conflictMatrixPhotograph)
{
	bool result = true;

	for (unsigned long int line = 0; line < conflictMatrix.links.size() and result; line++)
		for (unsigned long int column = line +1; column < conflictMatrix.links.size() and result; column++)
			if (conflictMatrixPhotograph.links.at(line).at(column) != conflictMatrix.links.at(line).at(column))
				result = false;

	return result;
}

FractionalColoring captureCycle(ConflictMatrix & conflictMatrix)
{
	FractionalColoring fractionalColoring;
	bool isSink;
	ConflictMatrix conflictMatrixPhotograph = conflictMatrix;

	fractionalColoring.cycleSize = 0;
	fractionalColoring.numberOfColors = 0;
	do
	{
		isSink = true;
		fractionalColoring.cycleSize++;
		for (unsigned long int link = 1; link < conflictMatrix.links.size() and isSink; link++)
			if (conflictMatrix.links.at(0).at(link) == RIGHT)
				isSink = false;
		if (isSink) fractionalColoring.numberOfColors++;
		edgeReversalDynamics(conflictMatrix);
	}
	while (not compareConflictMatrices(conflictMatrix, conflictMatrixPhotograph));

	return fractionalColoring;
}

void acyclicOrientation(ConflictMatrix & conflictMatrix)
{
   for (unsigned long int linkA = 0; linkA < conflictMatrix.links.size(); linkA++)
   {
      conflictMatrix.links.at(linkA).at(linkA) = NO;
      for (unsigned long int linkB = linkA +1; linkB < conflictMatrix.links.size(); linkB++)
         if (conflictMatrix.links.at(linkA).at(linkB) != NO)
         {
            conflictMatrix.links.at(linkA).at(linkB) = LEFT;
            conflictMatrix.links.at(linkB).at(linkA) = RIGHT;
         }
   }
}

void loadConflictMatrix(std::string fileName, ConflictMatrix & conflictMatrix)
{
   std::ifstream file;
   Path path;
   std::vector < char > linkEdges;
   unsigned short edge;

   file.open(fileName.data(), std::ios::in);
   if (not file)
    {
      std::cerr << fileName << " not found!" << std::endl;
      exit(1);
   }
   std::cout << "#Reading file:" << fileName << std::endl;

   conflictMatrix.links.clear();

   do
   {
      file >> edge; //0 vai virar o caracter 0 ou o inteiro 0?
      linkEdges.push_back((char)edge);

      if (file.peek() == '\n')
      {
         conflictMatrix.links.push_back(linkEdges);
         linkEdges.clear();
      }
   }
   while (!file.eof());
   if (linkEdges.size() > 1)
      conflictMatrix.links.push_back(linkEdges);

   file.close();
}

#endif /* MAIN_HPP_ */
