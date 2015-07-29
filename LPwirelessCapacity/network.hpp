/*
 * network.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef NETWORK_HPP_
#define NETWORK_HPP_

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

	const Position getPosition()
	{
		return this->position;
	}

	static void loadPosition(std::ifstream inFile, Position & position)
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

			loadPosition(inFile, position);
			do{
				node = new Node(position, fileLine++);
				nodes->push_back(node);

				//ignore the other fields until the end of the current line.
				inFile.ignore(std::numeric_limits<std::streamsize>::max(), endOfLine.str().data()[0]);
				loadPosition(inFile, position);
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

public:
	Link(Node * origin, Node * destination, unsigned int identification)
	{
		if (euclideanDistance(origin->position, destination->position) <= MinimumDistance)
		{
			this->origin = origin;
			this->destination = destination;
			this->identification = identification;
			this->size = euclideanDistance(this->origin->getPosition(), this->destination->getPosition());
		}
		else std::cerr << NOT_A_LINK << origin->identification << LINK << destination->identification;
	}

	const Node * getOrigin()
	{
		return this->origin;
	}

	const Node * getDestination()
	{
		return this->destination;
	}

	float getSize()
	{
		return this->size;
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

		Position position;

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

	float snr(unsigned int linkIdentification, std::vector < unsigned int > & noiseLinkIdentifications)
	{
		float snr = Network::noiseFloor() / TransmissionPower;
		Node * noiseOrigin;
		Node * destination = this->links->at(linkIdentification)->getDestination();

		for (unsigned int noiseIndex; noiseIndex < noiseLinkIdentifications.size(); noiseIndex++)
		{
			noiseOrigin = this->links->at(noiseLinkIdentifications.at(noiseIndex))->getOrigin();
			snr += 1/pow(euclideanDistance(destination->getPosition(), noiseOrigin->getPosition()), PathLossExponent);
		}
		snr = (1/this->links->at(linkIdentification)->getSize()) / snr;

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
