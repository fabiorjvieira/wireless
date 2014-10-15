/*
 * main.cpp
 *
 *  Created on: Nov 11, 2013
 *		Author: fabiorjvieira
 */

#include "network.hpp"
#include "networkOneSlot.hpp"
#include "networkOlga.hpp"
#include "fabioOlga.hpp"
#include "olgaOlga.hpp"
#include "santiOlga.hpp"
#include "valmir.hpp"
#include "olga.hpp"
#include "santi.hpp"
#include "xu.hpp"
#include "fabio.hpp"
#include "fabioOneSlot.hpp"
#include "optimumOneSlot.hpp"
#include "statistics.hpp"
#include "solver.hpp"

int main(int argn, char ** argc)
{
	std::string argument;
	std::string msg("solver or compare or statistics or network or fabio(Old) or olga or santi or valmir?");

	if (argn > 1)
	{
		argument = argc[1];
		for (int arg = 1; arg +1 < argn; arg++)
			argc[arg] = argc[arg+1];
		argn--;
		if (argument.compare("network") == 0)
			return network(argn, argc);
      if (argument.compare("networkOneSlot") == 0)
         return networkOneSlot(argn, argc);
      if (argument.compare("networkOlga") == 0)
         return networkOlga(argn, argc);
      if (argument.compare("fabioOlga") == 0)
         return fabioOlga(argn, argc);
      if (argument.compare("santiOlga") == 0)
         return santiOlga(argn, argc);
      if (argument.compare("olgaOlga") == 0)
         return olgaOlga(argn, argc);
		else if (argument.compare("valmir") == 0)
         return valmir(argn, argc);
		else if (argument.compare("fabio") == 0)
			return fabio(argn, argc);
      else if (argument.compare("fabioOneSlot") == 0)
         return fabioOneSlot(argn, argc);
      else if (argument.compare("optimumOneSlot") == 0)
         return optimumOneSlot(argn, argc);
		else if (argument.compare("olga") == 0)
			return olga(argn, argc);
		else if (argument.compare("santi") == 0)
			return santi(argn, argc);
		else if (argument.compare("statistics") == 0)
			return performance(argn, argc);
		else if (argument.compare("compare") == 0)
			return compare(argn, argc);
		else if (argument.compare("solver") == 0)
			return solver(argn, argc);
		else if (argument.compare("connectivity") == 0)
			return connectivity(argn, argc);
		else std::cout << msg << std::endl;
	}
	else std::cout << msg << std::endl;

	return 0;
}

