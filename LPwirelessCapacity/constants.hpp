/*
 * constants.hpp
 *
 *  Created on: Jul 28, 2015
 *      Author: fabiorjvieira
 */

#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

static const std::string FILE_NOT_FOUND = "File not found: ";
static const std::string FILE_NOT_CREATED = "File not created: ";
static const std::string NOT_A_LINK = "Not a link: ";
static const std::string LINK_SYMBOL = "->";
static const std::string LINK_FILE_TERMINATION = ".link.in";
static const std::string SEPARATOR = " ";

static const float BOLTZMANN = 1.3803e-23;
static const float REFERENCE_TEMPERATURE = 290.0; //kelvin
static const float NOISE_FIGURE = 0.031622776602; //15dBm

static const std::string LINEAR_PROGRAM_NAME = "FractionalColoring";
static const std::string LINEAR_PROGRAM_VARIABLE_PREFIX = "x";
static const std::string LINEAR_PROGRAM_CONSTRAINT_MOK_PREFIX = "cmok";
static const std::string LINEAR_PROGRAM_CONSTRAINT_MNOK_PREFIX = "cmnok";
static const float LINEAR_PROGRAM_VARIABLE_COEFFICIENT = 1.0;
static const float LINEAR_PROGRAM_LOWER_BOUND = 0.0;
static const float LINEAR_PROGRAM_UPPER_BOUND = 1.0;
static const unsigned int LINEAR_PROGRAM_IGNORE = 0;
static const bool LINEAR_PROGRAM_MATCHING_OK = true;
static const bool LINEAR_PROGRAM_MATCHING_NOK = true;

static const std::string PARAMETER_PREFIX = "-";
static const std::string HELP = PARAMETER_PREFIX + "help";
static const std::string PARAMETER_PathLossExponent = PARAMETER_PREFIX + "PathLossExponent";
static const std::string PARAMETER_SNRthreshold = PARAMETER_PREFIX + "SNRthreshold";
static const std::string PARAMETER_MinimumDistance = PARAMETER_PREFIX + "MinimumDistance";
static const std::string PARAMETER_BandWidth = PARAMETER_PREFIX + "BandWidth";
static const std::string PARAMETER_TransmissionPower = PARAMETER_PREFIX + "TransmissionPower";
static const std::string PARAMETER_ThermalNoise = PARAMETER_PREFIX + "ThermalNoise";
static const std::string PARAMETER_NoiseFloor = PARAMETER_PREFIX + "NoiseFloor";
static const std::string PARAMETER_LearningStep = PARAMETER_PREFIX + "LearningStep";
static const std::string PARAMETER_NodeFileName = PARAMETER_PREFIX + "NodeFileName";
static const std::string PARAMETER_LinkFileName = PARAMETER_PREFIX + "LinkFileName";
static const std::string PARAMETER_NOT_RECOGNIZED = " not recognized!";

#endif /* CONSTANTS_HPP_ */
