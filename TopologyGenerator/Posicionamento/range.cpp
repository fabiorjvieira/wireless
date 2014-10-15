#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <iomanip.h>

int main(int numberOfArguments, char ** arguments)
{
   float oldRadioRange, newRadioRange, oldNodeQuantity, newNodeQuantity;
   oldRadioRange = atof(arguments[1]);
   oldNodeQuantity = atoi(arguments[2]);
   newNodeQuantity = oldNodeQuantity + atoi(arguments[3]);
   newRadioRange = oldRadioRange*(sqrt((oldNodeQuantity/newNodeQuantity)));

   cout.setf(ios::fixed, ios::floatfield);
   cout.setf(ios::showpoint);
   cout << setprecision(15);

   std::cout << newRadioRange << std::endl;
   return 0;
}
