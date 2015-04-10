#include <iostream>

static const double BOLTZMANN = 1.3803e-23;
static const double REFERENCE_TEMPERATURE = 290.0; //kelvin
static const double NOISE_FIGURE = 0.031622776602; //15dBm

int main(int n, char * a[])
{
  long double thermalNoise = BOLTZMANN * REFERENCE_TEMPERATURE * 20000000;
        long double noiseFloor = thermalNoise; //* NOISE_FIGURE; // noiseFigure; //perda dentro do receptor não ideal
std::cout << noiseFloor;
  return 0;
}
