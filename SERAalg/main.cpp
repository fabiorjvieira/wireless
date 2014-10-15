/*
 * main.cpp
 *
 *  Created on: Oct 30, 2013
 *		Author: fabiorjvieira
 */

#include "main.hpp"
#include "routes.hpp"
#include "conflicts.hpp"

//g++ main.cpp -O3 -o ser.bin
//ser.bin networks/5.0.routes networks/5.0.neighborhood 2

int main(int argn, char ** argc)
{
   std::string argument;

   if (argn > 2)
   {
      argument = argc[1];
      for (int arg = 1; arg +1 < argn; arg++)
         argc[arg] = argc[arg+1];
      argn--;
      if (argument.compare("routes") == 0)
         return routes(argn, argc);
      else if (argument.compare("conflicts") == 0)
         return conflicts(argn, argc);
   }
   else std::cout << "routes or conflicts?" << std::endl;
}
