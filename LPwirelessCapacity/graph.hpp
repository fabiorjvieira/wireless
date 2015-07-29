/*
 * dgraph.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <lemon/list_graph.h>

class Vertice
{
private:
	Node * node;

public:
	Vertice(Node * node)
	{
		this->node = node;
	}
};

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

		if (this->upLink->getWeight() > this->downLink->getWeight()) link = this->upLink;
		else if (this->upLink->getWeight() < this->downLink->getWeight()) link = this->downLink;
		else link = this->downLink; //???it is not clear what to do when the two weight are equal!

		return link;
	}
};

class WeightGraph
{
private:
	std::vector < Vertice * > * vertices;
	std::unordered_map < std::string, WeightEdgePair * > weightEdgePairs;
	lemon::ListGraph lemonUndirectedGraph;

public:
	WeightGraph(Network * network)
	{
		std::stringstream key;
		Vertice * vertice;
		WeightEdgePair * weightEdgePair;
		this->vertices = new std::vector < Vertice * >;
		std::vector < Node * > * nodes = network->getNodes();
		std::vector < Link * > * links = network->getLinks();

		for (unsigned int nodeIndex = 0; nodeIndex < nodes->size(); nodeIndex++)
		{
			vertice = new Vertice(nodes->at(nodeIndex));
			this->vertices->push_back(vertice);
		}

		for (unsigned int linkIndex = 0; linkIndex < links->size(); linkIndex++)
		{
			mapKey(links->at(linkIndex)->getOrigin(), links->at(linkIndex)->getDestination(), key);
			if (this->weightEdgePairs.count(key.str()) == 0)
			{
				weightEdgePair = new WeightEdgePair(links->at(linkIndex));
				this->weightEdgePairs[key.str()] = weightEdgePair;
			}
			else this->weightEdgePairs[key.str()]->update(links->at(linkIndex));
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
