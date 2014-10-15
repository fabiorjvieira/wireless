/*
 * Analisys.h
 *
 *  Created on: Dec 10, 2008
 *      Author: fjimenez
 */

#ifndef ANALISYS_H_
#define ANALISYS_H_

namespace Performances {

class Analisys {
public:
   Analisys() {};
   virtual ~Analisys() {};

   static unsigned int * edgeReversal(char ** graph, unsigned int size, unsigned int originTransmitionQuantity);
};

}

#endif /* ANALISYS_H_ */
