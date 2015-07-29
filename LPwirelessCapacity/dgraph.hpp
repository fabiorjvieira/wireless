/*
 * dgraph.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef DGRAPH_HPP_
#define DGRAPH_HPP_

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

class Arc
{
private:
	Vertice * tail;
	Vertice * head;
public:
	void update(Link * link)
	{
		if ()
	}
};

class Dgraph
{
private:
	std::vector < Vertice * > * vertices;
	std::vector < Arc * > * arcs;

public:

};

#endif /* DGRAPH_HPP_ */
