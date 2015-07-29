/*
 * network.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

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

class Link
{
private:
	Node * origin;
	Node * destination;
	unsigned int identification;
	float size;
	float weight;

public:
	Link(Node * origin, Node * destination, unsigned int identification)
	{
		if (euclideanDistance(origin->getPosition(), destination->getPosition()) <= MinimumDistance)
		{
			this->origin = origin;
			this->destination = destination;
			this->identification = identification;
			this->size = euclideanDistance(this->origin->getPosition(), this->destination->getPosition());
			this->weight = 0;
		}
		else std::cerr << NOT_A_LINK << origin->getIdentification() << LINK << destination->getIdentification();
	}

	Node * getOrigin()
	{
		return this->origin;
	}

	void updateWeight(float weight)
	{
		this->weight = weight;
	}

	Node * getDestination()
	{
		return this->destination;
	}

	float getSize()
	{
		return this->size;
	}

	float getWeight()
	{
		return this->weight;
	}

	unsigned int getIdentification()
	{
		return this->identification;
	}

	static std::vector < Link * > * loadLinks(std::string linkFile, std::vector < Node * > * nodes)
	{
		std::ifstream inFile;
		std::vector < Link * > * links = new std::vector < Link * >;
		Link * link;
		unsigned int originIndetification, destinationIndetification;
		std::stringstream endOfLine;
		unsigned int fileLine = 0;
		endOfLine << std::endl;

		try{
			inFile.open(linkFile.data(), std::ifstream::in);
			if (!inFile.is_open()){
				throw std::exception();
			}

			inFile >> originIndetification;
			inFile >> destinationIndetification;
			do{
				link = new Link(nodes->at(originIndetification), nodes->at(destinationIndetification), fileLine++);
				links->push_back(link);

				//ignore the other fields until the end of the current line.
				inFile.ignore(std::numeric_limits<std::streamsize>::max(), endOfLine.str().data()[0]);
				inFile >> originIndetification;
				inFile >> destinationIndetification;
			}while (!inFile.eof());
			inFile.close();

		}catch(std::exception &){
			std::cerr << FILE_NOT_FOUND << linkFile << std::endl;
		}

		return links;
	}
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

	float snr(unsigned int linkIdentification, std::vector < unsigned int > & noiseLinkIdentifications)
	{
		Node * noiseOrigin;
		Node * destination = this->links->at(linkIdentification)->getDestination();
		float snr = 0;
		float equivalentNoise = (Network::noiseFloor() / TransmissionPower);

		for (unsigned int noiseIndex = 0; noiseIndex < noiseLinkIdentifications.size(); noiseIndex++)
		{
			//attention! linkIdentification is included in noiseLinkIdentifications, so euclideanDistance = 0 when noiseLinkIdentifications.at(noiseIndex) == linkIdentification.
			if (noiseLinkIdentifications.at(noiseIndex) == linkIdentification) continue;
			noiseOrigin = this->links->at(noiseLinkIdentifications.at(noiseIndex))->getOrigin();
			snr += 1 / pow(euclideanDistance(destination->getPosition(), noiseOrigin->getPosition()), PathLossExponent);
		}
		snr = (1/pow(this->links->at(linkIdentification)->getSize(),PathLossExponent)) / (equivalentNoise + snr);

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
