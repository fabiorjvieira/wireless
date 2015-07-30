/*
 * dgraph.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <lemon/list_graph.h>

class WeightEdgePair
{
private:
	Link * upLink;
	Link * downLink;

public:
	WeightEdgePair(Link * upLink, Link * downLink)
	{
		this->upLink = upLink;
		this->downLink = downLink;
	}

	WeightEdgePair(Link * upLink)
	{
		this->upLink = upLink;
		this->downLink = NULL;
	}

	void update(Link * downLink)
	{
		this->downLink = downLink;
	}

	Link * getWeightedLink()
	{
		Link * link;

		if (this->downLink != NULL)
		{
			if (this->upLink->getWeight() > this->downLink->getWeight()) link = this->upLink;
			else if (this->upLink->getWeight() < this->downLink->getWeight()) link = this->downLink;
			else link = this->downLink; //???it is not clear what to do when the two weight are equal!
		}
		else link = this->upLink;

		return link;
	}

	Link * getUpLink()
	{
		return this->upLink;
	}
};

class WeightGraph
{
private:
	std::vector < lemon::ListGraph::Node > lemonNodes;
	//std::vector < lemon::ListGraph::Edge > lemonEdges;
	lemon::ListGraph::EdgeMap < float > * lemonWeightEdges;
	lemon::ListGraph lemonUndirectedGraph;
	std::unordered_map < std::string, WeightEdgePair * > weightEdgePairs;
	std::unordered_map < lemon::ListGraph::Edge, unsigned int > lemonEdges;

public:
	WeightGraph(Network * network)
	{
		unsigned int originIdentification, destinationIdentification;
		std::stringstream key;
		lemon::ListGraph::Edge lemonEdge;
		std::vector < Node * > * nodes = network->getNodes();
		std::vector < Link * > * links = network->getLinks();

		for (unsigned int nodeIndex = 0; nodeIndex < nodes->size(); nodeIndex++)
			this->lemonNodes.push_back(this->lemonUndirectedGraph.addNode());

		for (unsigned int linkIndex = 0; linkIndex < links->size(); linkIndex++)
		{
			mapKey(links->at(linkIndex)->getOrigin(), links->at(linkIndex)->getDestination(), key);
			if (this->weightEdgePairs.count(key.str()) == 0)
				this->weightEdgePairs[key.str()] = new WeightEdgePair(links->at(linkIndex));
			else this->weightEdgePairs[key.str()]->update(links->at(linkIndex));
		}

		for (auto mapIndex = this->weightEdgePairs.begin(); mapIndex != this->weightEdgePairs.end(); mapIndex++)
		{
			originIdentification = mapIndex->second->getUpLink()->getOrigin()->getIdentification();
			destinationIdentification = mapIndex->second->getUpLink()->getDestination()->getIdentification();
			lemonEdge = this->lemonUndirectedGraph.addEdge(this->lemonNodes.at(originIdentification), this->lemonNodes.at(destinationIdentification));
			this->lemonEdges[lemonEdge] = mapIndex->second->getUpLink()->getIdentification();
		}

		this->lemonWeightEdges =  new lemon::ListGraph::EdgeMap(this->lemonUndirectedGraph);
		for (auto mapIndex = this->lemonEdges.begin(); mapIndex != this->lemonEdges.end(); mapIndex++)
		{
			lemonEdge = mapIndex->second;
			(*this->lemonWeightEdges)[lemonEdges] = 0;
		}
	}

	void mapKey(Node * origin, Node * destination, std::stringstream & key)
	{
		key.str("");
		if (origin->getIdentification() < destination->getIdentification()) key << origin->getIdentification() << SEPARATOR << destination->getIdentification();
		else key << destination->getIdentification() << SEPARATOR << origin->getIdentification();
	}
};

class mapping
{
private:

public:

};

#endif /* GRAPH_HPP_ */
