/*
 * gaussian.cpp
 *
 *  Created on: Jul 2, 2015
 *      Author: fabiorjvieira
 */



#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <cfloat>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <iomanip>

char ZERO = 0;

std::string intToByte(unsigned long long int integer)
{
   std::string binary;
   unsigned char byte;

   do
   {
      byte = integer % 256;
      binary.insert(binary.begin(),byte);
      integer = (integer - byte) / 256;
   }
   while (integer != 0);

   return binary;
}

int main(int narqs, char ** arqs)
{
   double multi=8;
   double y0, limit=3600*24*multi;
   int y1, k=9*3600;

   std::string delta = intToByte(900);
   std::string interval;
   std::string channel0 = intToByte(0);
   std::string channel1 = intToByte(1);
   std::string channel2 = intToByte(2);
   std::string throughtput = intToByte(100);

   for (int z=0; z <= 2; z++)
   {
	   for (int x=0; x < limit; x++)
	   {
          interval = intToByte(x + (limit * z));

		  y0 = 100 * ((1/(exp(((x-(limit/3))*(x-limit/3))/(multi*multi*10000*10000)))) + (1/(exp(((x-(limit * 2 / 3))*(x-(limit * 2 / 3)))/(multi*multi*10000*10000)))));
		  y1=y0;
         if (arqs[1][0] == '0')
		    if ( x % k == 0)
               std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000CC001" << std::setw(4) << std::setfill(ZERO) << std::right << delta << std::setw(3) << std::setfill(ZERO) << std::right << throughtput << std::endl;
	      if (arqs[1][0] == '1')
	      {
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000CA000" << std::setw(4) << std::setfill(ZERO) << std::right << channel0 << "100" << std::endl;
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000AT000" << std::setw(4) << std::setfill(ZERO) << std::right << channel0 << "100" << std::endl;
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000CA000" << std::setw(4) << std::setfill(ZERO) << std::right << channel1 << "000" << std::endl;
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000AT000" << std::setw(4) << std::setfill(ZERO) << std::right << channel1 << std::setw(3) << std::setfill('0') << std::right << y1 << std::endl;
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000CA000" << std::setw(4) << std::setfill(ZERO) << std::right << channel2 << "100" << std::endl;
			  std::cout << std::setw(7) << std::setfill(ZERO) << std::right << interval << "CA000AT000" << std::setw(4) << std::setfill(ZERO) << std::right << channel2 << "100" << std::endl;
	      }
	   }
   }
}
