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

	Position & getPosition()
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

		try
		{
			inFile.open(nodeFile.data(), std::ifstream::in);
			if (!inFile.is_open()) throw std::exception();

			Node::loadPosition(inFile, position);
			do{
				node = new Node(position, fileLine++);
				nodes->push_back(node);

				//ignore the other fields until the end of the current line.
				inFile.ignore(std::numeric_limits<std::streamsize>::max(), endOfLine.str().data()[0]);
				Node::loadPosition(inFile, position);
			}while (!inFile.eof());
			inFile.close();

		}
		catch(std::exception &)
		{
			std::cerr << PARAMETER_NodeFileName << SEPARATOR << FILE_NOT_FOUND << nodeFile << std::endl;
			exit(0);
		}

		return nodes;
	}
};

class Link;

//needed for std::sort();
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
	float getSNRsize();
	float getWeight();
	unsigned int getIdentification();
	LinkIdentification getLinkIdentification();
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
		if (linkFile.empty()) linkFile = cliqueNetwork(nodeFile);
		this->links = Link::loadLinks(linkFile, this->nodes);
	}

   static void randomNetworks(unsigned int numberOfNodes, unsigned int minimumSquareSide, unsigned int maximumSquareSide, unsigned int step, char * nodeFile)
   {
      std::ofstream outFile;
      std::stringstream buffer;
      Position position;

      for (unsigned int squareSide = minimumSquareSide; squareSide <= maximumSquareSide; squareSide += step)
      {
         try
         {
            buffer.str("");
            buffer << nodeFile << FILE_SEPARATOR << squareSide;
            outFile.open(buffer.str().data(), std::ifstream::out);
            if (!outFile.is_open()) throw std::exception();

            srandom(getRandomSeed());

            for (unsigned int nodeIndex = 0; nodeIndex < numberOfNodes; nodeIndex++)
            {
               position.x = random() % squareSide;
               position.y = random() % squareSide;
               position.z = 0; //??? bidimentional for this time

               outFile << position.x << SEPARATOR << position.y << SEPARATOR << position.z << std::endl;
            }
            outFile.close();
         }
         catch(std::exception &)
         {
            std::cerr << PARAMETER_NodeFileName << SEPARATOR << FILE_NOT_CREATED << nodeFile << std::endl;
            exit(0);
         }
      }
   }

	std::string cliqueNetwork(std::string nodeFile)
	{
		std::ofstream outFile;
		std::string linkFile = nodeFile + LINK_FILE_TERMINATION;

		try
		{
			outFile.open(linkFile.data(), std::ifstream::out);
			if (!outFile.is_open()) throw std::exception();

			srandom(getRandomSeed());

			for (unsigned int nodeIndexA = 0; nodeIndexA < nodes->size(); nodeIndexA++)
			{
				for (unsigned int nodeIndexB = 0; nodeIndexB < nodes->size(); nodeIndexB++)
				{
               if (euclideanDistance(this->nodes->at(nodeIndexA)->getPosition(), this->nodes->at(nodeIndexB)->getPosition()) <= MinimumDistance)
               {
                  if (random() % 2 > 0)
                  {
                        outFile << this->nodes->at(nodeIndexA)->getIdentification() << SEPARATOR << this->nodes->at(nodeIndexB)->getIdentification() << std::endl;
                        outFile << this->nodes->at(nodeIndexB)->getIdentification() << SEPARATOR << this->nodes->at(nodeIndexA)->getIdentification() << std::endl;
                  }
                  else
                  {
                     if (random() % 2 > 0) outFile << this->nodes->at(nodeIndexA)->getIdentification() << SEPARATOR << this->nodes->at(nodeIndexB)->getIdentification() << std::endl;
                     else outFile << this->nodes->at(nodeIndexB)->getIdentification() << SEPARATOR << this->nodes->at(nodeIndexA)->getIdentification() << std::endl;

                  }
               }
				}
			}
			outFile.close();

		}
		catch(std::exception &)
		{
			std::cerr << PARAMETER_LinkFileName << SEPARATOR << FILE_NOT_CREATED << nodeFile << std::endl;
			exit(0);
		}

		return linkFile;
	}

	std::vector < Node * > * getNodes()
	{
		return this->nodes;
	}

	std::vector < Link * > * getLinks()
	{
		return this->links;
	}

	static float snrMinimumDistance(const Position & originPosition, const Position & destinationPosition)
	{
      float distance;

      distance = euclideanDistance(destinationPosition, originPosition);
	   if (distance < 1) distance = SNR_MINIMUM_DISTANCE;

	   return distance;
	}


	static float snr(LinkIdentification linkIdentification, std::vector < LinkIdentification > * schedulingLinkIdentifications)
	{
		Node * noiseOrigin;
		Node * destination = linkIdentification.getLink()->getDestination();
		float snr = 0;
		float equivalentNoise = (NoiseFloor / TransmissionPower);

		for (unsigned int noiseIndex = 0; noiseIndex < schedulingLinkIdentifications->size(); noiseIndex++)
		{
			//attention! linkIdentification is included in linkIdentifications, so euclideanDistance = 0 when noiseLinkIdentifications.at(noiseIndex) == linkIdentification.
			if (schedulingLinkIdentifications->at(noiseIndex).getIdentification() == linkIdentification.getIdentification()) continue;
			noiseOrigin = schedulingLinkIdentifications->at(noiseIndex).getLink()->getOrigin();
			snr += 1 / pow(snrMinimumDistance(destination->getPosition(), noiseOrigin->getPosition()), PathLossExponent);
		}
		snr = (1/pow(linkIdentification.getLink()->getSNRsize(),PathLossExponent)) / (equivalentNoise + snr);

		return snr;
	}

	static bool scheduleOK(std::vector < LinkIdentification > * schedulingLinkIdentifications)
	{
		bool ok = true;
		for (unsigned int linkIdentificationIndex = 0; linkIdentificationIndex < schedulingLinkIdentifications->size() and ok; linkIdentificationIndex++)
			ok = (Network::snr(schedulingLinkIdentifications->at(linkIdentificationIndex), schedulingLinkIdentifications) > SNRthreshold);

		return ok;
	}
};

#endif /* NETWORK_HPP_ */
