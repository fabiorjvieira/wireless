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

int main()
{
   double multi=8;
   double y0, limit=3600*24*multi;
   int y1, k=9*3600;

   for (int x=0; x <= limit; x++)
   {
      y0 = 100 * ((1/(exp(((x-(limit/3))*(x-limit/3))/(multi*multi*10000*10000)))) + (1/(exp(((x-(limit * 2 / 3))*(x-(limit * 2 / 3)))/(multi*multi*10000*10000)))));
      y1=y0;
      if ( x % k == 0) std::cout << "REQS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000CC0010900100" << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000CA0000000100" << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000AT0000000100" << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000CA0000001000" << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000AT0000001" << std::setw(3) << std::setfill('0') << std::right << y1 << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000CA0000002100" << std::endl;
      std::cout << "SENS" << std::setw(7) << std::setfill('0') << std::right << x << "CA000AT0000002100" << std::endl;
   }
}
