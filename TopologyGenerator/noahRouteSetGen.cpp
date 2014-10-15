/*
 * main.cpp
 *
 *  Created on: Jul 30, 2011
 *      Author: fabiorjvieira
 */

#include "../frjvLibrary/Support.h"

int main(int numberOfArguments, char ** arguments)
{
	std::vector < std::vector < unsigned int > > * routes = new std::vector < std::vector <unsigned int> > ();
	std::string * nextHops;
    unsigned int origin, destiny, networkSize, * cbrs[2], * routeTableSize;
    std::stringstream MyStream;

	readCSVfile(arguments[1], routes);
	networkSize = atoi(arguments[2]);
	nextHops = new std::string [networkSize];
	routeTableSize = new unsigned int [networkSize];
	cbrs[0] = new unsigned int [routes->size()];
	cbrs[1] = new unsigned int [routes->size()];

    for (unsigned int node = 0; node < networkSize; node++)
    {
    	nextHops[node] = "";
    	routeTableSize[node] = 0;
    }
    for (unsigned int route = 0; route < routes->size(); route++)
    {
    	cbrs[0][route] = -1;
    	cbrs[1][route] = -1;
    }

    for (unsigned int route = 0; route < routes->size(); route++)
    {
    	origin = routes->at(route).at(0);
    	destiny = routes->at(route).at(routes->at(route).size() -1);
    	for (unsigned int hop = 0; hop < routes->at(route).size() -1; hop++)
    	{
            MyStream << " " << routes->at(route).at(hop+1) << " " << routes->at(route).at(hop+1) << " " << route;
            routeTableSize[(int)routes->at(route).at(hop)]++;
            if (destiny != routes->at(route).at(hop+1))
            {
            	 MyStream << " " << destiny << " " << routes->at(route).at(hop+1) << " " << route;
                 routeTableSize[(int)routes->at(route).at(hop)]++;
            }
            nextHops[(int)routes->at(route).at(hop)] += MyStream.str(); MyStream.str("");
    	}
        cbrs[0][route] = origin;
        cbrs[1][route] = destiny;
    }

    std::cout << "#hops" << std::endl;
    for (unsigned int node = 0; node < networkSize; node++)
    	if (nextHops[node].size() != 0)
    	{
    		std::cout << "eval \"[$node_(" << node << ") set ragent_] routing " << routeTableSize[node];
        	std::cout << nextHops[node] << "\"" << std::endl;
    	}
    std::cout << "#cbrs" << std::endl;
    for (unsigned int route = 0; route < routes->size(); route++)
    	if (cbrs[0][route] != -1)
    	{
    		std::cout << "set udp" << route << " [new Agent/UDP]" << std::endl;
    		std::cout << "$udp" << route << " set_route " << route << std::endl;
    		std::cout << "$ns attach-agent $node_(" << cbrs[0][route] << ") $udp" << route << std::endl;
    		std::cout << "set cbr" << route << " [new Application/Traffic/CBR]" << std::endl;
    		std::cout << "$cbr" << route << " set packetSize_ 1000B" << std::endl;
    		std::cout << "$cbr" << route << " set interval_ 0.005s" << std::endl;
    		std::cout << "$cbr" << route << " attach-agent $udp" << route << std::endl;
    		std::cout << "set null" << route << " [new Agent/Null]" << std::endl;
    		std::cout << "$ns attach-agent $node_(" << cbrs[1][route] << ") $null" << route << std::endl;
    		std::cout << "$ns connect $udp" << route << " $null" << route << std::endl;
    		std::cout << "$ns at 5.0 \"$cbr" << route << " start\"" << std::endl;
    		std::cout << "$ns at 10.0 \"$cbr" << route << " stop\"" << std::endl;
    	}

	delete routes;
    delete [] cbrs[0];
    delete [] cbrs[1];
    delete [] nextHops;
    delete [] routeTableSize;

	return 0;
}
