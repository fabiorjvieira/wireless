/*
 * executable.hpp
 *
 *  Created on: Mar 30, 2009
 *      Author: fjimenez
 */

#ifndef EXECUTABLE_HPP_
#define EXECUTABLE_HPP_

//---User defines

#define SINGLEDENSE
//#define DOUBLEDENSE
//#define QUADRIDENSE
//#define OCTODENSE

//#define REDUCED_RUNS
//#define ONLY_MAX_RUNS
//#define LOCAL_OUTPUT

//#define GRAPHICS
//#define GRAPHICS_JU
//#define DOMINANCE
//#define HISTOGRAM
#define GRAPHICS_ALL_MINIMUM_PATH
//#define STATISTICS

//#define ROUTING_DIJKSTRA
//#define ROUTING_NEIGHBOR_N_EACH
//#define ROUTING_NEIGHBOR_N

//#define PERFORMANCE_DIJKSTRA
//#define PERFORMANCE_ALL_MINIMUM_PATH
//#define PERFORMANCE_NEIGHBOR_EACH
//#define PERFORMANCE_NEIGHBOR_N_EACH
//#define PERFORMANCE_NEIGHBOR_N

//#define PERFORMANCE_NR
//#define PERFORMANCE_ABR
//#define PERFORMANCE_HR

//#define PERFORMANCE_AR
//#define PERFORMANCE_VIZ

//---Debug defines
//#define NETWORKING
//#define PERFORMANCE_DEBUG
//#define PERFORMANCE_FILE_DEBUG
//#define MAIN_DEBUG
//---

#if defined(ROUTING_DIJKSTRA) || defined(ROUTING_NEIGHBOR_N_EACH) || defined(ROUTING_NEIGHBOR_N)
   #define ROUTING
#endif

#if defined(PERFORMANCE_DIJKSTRA) || defined(PERFORMANCE_NEIGHBOR_EACH) || defined(PERFORMANCE_NEIGHBOR_N_EACH) || defined(PERFORMANCE_NEIGHBOR_N) || defined(PERFORMANCE_ALL_MINIMUM_PATH)
   #define PERFORMANCE
#endif /* PERFORMANCE_DIJKSTRA */

#ifdef ROUTING
   #define NETWORKING
#endif /* ROUTING */

#ifdef PERFORMANCE
   #define NETWORKING
#endif /* PERFORMANCE */

#endif /* EXECUTABLE_HPP_ */
