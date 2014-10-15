/*
 * main.cpp
 *
 *  Created on: Jul 31, 2011
 *      Author: fabiorjvieira
 */

#include "../frjvLibrary/Support.h"

void randRouteSetGen(const char * inputFileName, const char * baseOutputFileName, unsigned int networkSize, std::vector <unsigned int> & groupsSize)
{
	std::vector <unsigned int> * listOfNodeIdentifiers = NULL;
	std::vector < std::vector <unsigned int> > * routeTable = new std::vector < std::vector <unsigned int> > ();
	std::vector < std::vector <unsigned int> > randRouteTable;
	unsigned int origin, destiny, aux;
    std::stringstream MyStream;
    bool routeFound;

	Randomic::setSeed();
	readCSVfile(inputFileName, routeTable);
    Randomic::listOfIdentifiers(networkSize, listOfNodeIdentifiers);

    while (listOfNodeIdentifiers->size() > 0)
    {
    	origin = Randomic::randomicFromList(listOfNodeIdentifiers);
    	do destiny = Randomic::getRandomic((unsigned long) networkSize); while (destiny == origin);
    	routeFound = false;
    	while (not routeFound)
    	{
        	for (unsigned int routeTableLine = 0; routeTableLine < routeTable->size(); routeTableLine++)
        		if (routeTable->at(routeTableLine).at(0) == origin and routeTable->at(routeTableLine).at(routeTable->at(routeTableLine).size()-1) == destiny)
        		{
        			randRouteTable.push_back(routeTable->at(routeTableLine));
        			routeFound = true;
        		}
        	aux = origin;
        	origin = destiny;
        	destiny = aux;
    	}
    }

    while (groupsSize.size() > 0)
    {
    	groupsSize.back() = networkSize - groupsSize.back();
    	networkSize -= groupsSize.back();
    	while (groupsSize.back() > 0)
    	{
    		origin = randRouteTable.back().at(0);
    		destiny = randRouteTable.back().at(randRouteTable.back().size()-1);
    		randRouteTable.pop_back();
    		if (origin != randRouteTable.back().at(0) or destiny != randRouteTable.back().at(randRouteTable.back().size()-1))
    			groupsSize.back()--;
    	}
    	groupsSize.pop_back();

    	MyStream.str("");
    	MyStream << baseOutputFileName  << Support::FILE_NAME_SEPARATOR << networkSize;
    	writeCSVfile(MyStream.str().data(), & randRouteTable);
    }
}

int main(int numberOfArguments, char ** arguments)
{
	std::vector <unsigned int> groupsSize;

	readCSVline(arguments[4], & groupsSize);
	randRouteSetGen(arguments[1], arguments[2], atoi(arguments[3]), groupsSize);

	return 0;
}
