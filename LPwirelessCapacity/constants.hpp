/*
 * constants.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

static const std::string FILE_NOT_FOUND = "File not found: ";
static const std::string NOT_A_LINK = "Not a link: ";
static const std::string LINK = "->";
static const std::string SEPARATOR = " ";

static const float BOLTZMANN = 1.3803e-23;
static const float REFERENCE_TEMPERATURE = 290.0; //kelvin
static const float NOISE_FIGURE = 0.031622776602; //15dBm

static const std::string LINEAR_PROGRAM_NAME = "FractionalColoring";
static const std::string LINEAR_PROGRAM_VARIABLE_PREFIX = "x";
static const std::string LINEAR_PROGRAM_CONSTRAINT_MOK_PREFIX = "cmok";
static const std::string LINEAR_PROGRAM_CONSTRAINT_MNOK_PREFIX = "cmnok";
static const float LINEAR_PROGRAM_NAME_COEFFICIENT = 1.0;
static const float LINEAR_PROGRAM_LOWER_BOUND = 0.0;
static const float LINEAR_PROGRAM_UPPER_BOUND = 1.0;
static const unsigned int LINEAR_PROGRAM_IGNORE = 0;

#endif /* CONSTANTS_HPP_ */
