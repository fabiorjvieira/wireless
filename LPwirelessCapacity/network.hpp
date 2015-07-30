/*
 * network.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <math.h>

#include "parameters.hpp"
#include "constants.hpp"

class Node
{
private:
	Position position;
	unsigned int identification;

public:
	Node(Position position, unsigned int identification)
	{
		this->position.x = position.x;
		this->position.y = position.y;
		this->position.z = position.z;
		this->identification = identification;
	}

	Position getPosition()
	{
		return this->position;
	}

	unsigned int getIdentification()
	{
		return this->identification;
	}

	static void loadPosition(std::ifstream & inFile, Position & position)
	{
		inFile >> position.x;
		inFile >> position.y;
		inFile >> position.z;
	}

	static std::vector < Node * > * loadNodes(std::string nodeFile)
	{
		std::ifstream inFile;
		std::vector < Node * > * nodes = new std::vector < Node * >;
		Node * node;
		std::stringstream endOfLine;
		unsigned int fileLine = 0;
		endOfLine << std::endl;

		Position position;

		try{
			inFile.open(nodeFile.data(), std::ifstream::in);
			if (!inFile.is_open()){
				throw std::exception();
			}

			Node::loadPosition(inFile, position);
			do{
				node = new Node(position, fileLine++);
				nodes->push_back(node);

				//ignore the other fields until the end of the current line.
				inFile.ignore(std::numeric_limits<std::streamsize>::max(), endOfLine.str().data()[0]);
				Node::loadPosition(inFile, position);
			}while (!inFile.eof());
			inFile.close();

		}catch(std::exception &){
			std::cerr << FILE_NOT_FOUND << nodeFile << std::endl;
		}

		return nodes;
	}
};

class Link;

class LinkIdentification
{
private:
	unsigned int identification;
	Link * link;

public:
	LinkIdentification();
	unsigned int getIdentification();
	Link * getLink();
	void set(unsigned int identification, Link * link);
	bool operator < (const LinkIdentification & id) const;
	bool operator > (const LinkIdentification & id) const;
	bool operator >= (const LinkIdentification & id) const;
	bool operator <= (const LinkIdentification & id) const;
	bool operator == (const LinkIdentification & id) const;
	bool operator != (const LinkIdentification & id) const;
	LinkIdentification & operator = (const LinkIdentification & id);
};

class Link
{
private:
	Node * origin;
	Node * destination;
	LinkIdentification identification;
	float size;
	float weight;

public:
	Link(Node * origin, Node * destination, unsigned int identification);
	Node * getOrigin();
	void updateWeight(float weight);
	Node * getDestination();
	float getSize();
	float getWeight();
	LinkIdentification & getIdentification();
	static std::vector < Link * > * loadLinks(std::string linkFile, std::vector < Node * > * nodes);
};

class Network
{
private:
	std::vector < Node * > * nodes;
	std::vector < Link * > * links;

public:
	Network(std::string nodeFile, std::string linkFile)
	{
		this->nodes = Node::loadNodes(nodeFile);
		this->links = Link::loadLinks(linkFile, this->nodes);
	}

	std::vector < Node * > * getNodes()
	{
		return this->nodes;
	}

	std::vector < Link * > * getLinks()
	{
		return this->links;
	}

	static float snr(LinkIdentification linkIdentification, std::vector < LinkIdentification > * linkIdentifications)
	{
		Node * noiseOrigin;
		Node * destination = linkIdentification.getLink()->getDestination();
		float snr = 0;
		float equivalentNoise = (Network::noiseFloor() / TransmissionPower);

		for (unsigned int noiseIndex = 0; noiseIndex < linkIdentifications->size(); noiseIndex++)
		{
			//attention! linkIdentification is included in linkIdentifications, so euclideanDistance = 0 when noiseLinkIdentifications.at(noiseIndex) == linkIdentification.
			if (linkIdentifications->at(noiseIndex).getIdentification() == linkIdentification.getIdentification()) continue;
			noiseOrigin = linkIdentifications->at(noiseIndex).getLink()->getOrigin();
			snr += 1 / pow(euclideanDistance(destination->getPosition(), noiseOrigin->getPosition()), PathLossExponent);
		}
		snr = (1/pow(linkIdentification.getLink()->getSize(),PathLossExponent)) / (equivalentNoise + snr);

		return snr;
	}

	static long double noiseFloor()
	{
		long double thermalNoise = BOLTZMANN * REFERENCE_TEMPERATURE * BandWidth;
		long double noiseFloor = thermalNoise * NOISE_FIGURE; // Noise Figure = perda dentro do receptor não ideal
		return noiseFloor;
	}
};

#endif /* NETWORK_HPP_ */
