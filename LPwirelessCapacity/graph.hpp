/*
 * dgraph.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <lemon/list_graph.h>
#include <lemon/matching.h>

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
	lemon::ListGraph * lemonUndirectedGraph;
	std::vector < lemon::ListGraph::Node * > lemonNodes;
	std::unordered_map < lemon::ListGraph::Edge *, WeightEdgePair * > lemonEdges; //maps lemon Edge to WeightEdgePair
	std::unordered_map < lemon::ListGraph::Edge *, Link * > lemonEdgeLinkMap; //maps lemon Edge to Link
	lemon::ListGraph::EdgeMap < float > * lemonWeightEdgesMap;
	std::unordered_map < std::string, WeightEdgePair * > weightEdgePairs; //maps a pair of Node::identification (WeightGraph::mapKey) to WeightEdgePair

protected:
	void initLemonNodes(Network * network)
	{
		//init lemon nodes
		std::vector < Node * > * nodes = network->getNodes();
		lemon::ListGraph::Node * lemonNode;
		for (unsigned int nodeIndex = 0; nodeIndex < nodes->size(); nodeIndex++)
		{
			lemonNode = new lemon::ListGraph::Node;
			(*lemonNode) = this->lemonUndirectedGraph->addNode();
			this->lemonNodes.push_back(lemonNode);
		}
	}

	void initWeightEdgePairs(Network * network)
	{
		//init map of weight edge pairs
		std::stringstream key;
		std::vector < Link * > * links = network->getLinks();
		for (unsigned int linkIndex = 0; linkIndex < links->size(); linkIndex++)
		{
			WeightGraph::mapKey(links->at(linkIndex)->getOrigin(), links->at(linkIndex)->getDestination(), key);
			if (this->weightEdgePairs.count(key.str()) == 0)
				this->weightEdgePairs[key.str()] = new WeightEdgePair(links->at(linkIndex));
			else this->weightEdgePairs[key.str()]->update(links->at(linkIndex));
		}
	}

	void initLemonEdges()
	{
		//init map of lemon edges
		unsigned int originIdentification, destinationIdentification;
		lemon::ListGraph::Edge * lemonEdge;
		for (std::unordered_map < std::string, WeightEdgePair * >::iterator mapIndex = this->weightEdgePairs.begin(); mapIndex != this->weightEdgePairs.end(); mapIndex++)
		{
			originIdentification = mapIndex->second->getUpLink()->getOrigin()->getIdentification();
			destinationIdentification = mapIndex->second->getUpLink()->getDestination()->getIdentification();
			lemonEdge = new lemon::ListGraph::Edge;
			(*lemonEdge) = this->lemonUndirectedGraph->addEdge((*this->lemonNodes.at(originIdentification)), (*this->lemonNodes.at(destinationIdentification)));
			this->lemonEdges[lemonEdge] = mapIndex->second;
		}
	}

	void updateLemonWeights()
	{
		//update lemon edge weights
		for (std::unordered_map < lemon::ListGraph::Edge *, WeightEdgePair * >::iterator mapIndex = this->lemonEdges.begin(); mapIndex != this->lemonEdges.end(); mapIndex++)
		{
			this->lemonEdgeLinkMap[mapIndex->first] = mapIndex->second->getWeightedLink();
			this->lemonWeightEdgesMap->set((*(mapIndex->first)), mapIndex->second->getWeightedLink()->getWeight());
		}
	}

	static void mapKey(Node * origin, Node * destination, std::stringstream & key)
	{
		key.str("");
		if (origin->getIdentification() < destination->getIdentification()) key << origin->getIdentification() << SEPARATOR << destination->getIdentification();
		else key << destination->getIdentification() << SEPARATOR << origin->getIdentification();
	}

public:
	WeightGraph(Network * network)
	{
		this->lemonUndirectedGraph = new lemon::ListGraph();
		this->initLemonNodes(network);
		this->initWeightEdgePairs(network);
		this->initLemonEdges();
		this->lemonWeightEdgesMap =  new lemon::ListGraph::EdgeMap < float > ((*this->lemonUndirectedGraph));
		this->updateLemonWeights();
	}

	void MaxWeightedMatching(std::vector < unsigned int > & linkIdentifications)
	{
		linkIdentifications.clear();
		this->updateLemonWeights();
		lemon::MaxWeightedMatching < lemon::ListGraph, lemon::ListGraph::EdgeMap < float > > maxWeightedMatching((*lemonUndirectedGraph), (*lemonWeightEdgesMap));
		maxWeightedMatching.run();
		for (std::unordered_map < lemon::ListGraph::Edge *, WeightEdgePair * >::iterator mapIndex = this->lemonEdges.begin(); mapIndex != this->lemonEdges.end(); mapIndex++)
			if (maxWeightedMatching.matching((*mapIndex->first)))
				linkIdentifications.push_back(this->lemonEdgeLinkMap[mapIndex->first]->getIdentification());
	}
};

class mapping
{
private:

public:

};

#endif /* GRAPH_HPP_ */
