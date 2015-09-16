// Serial.hpp

#pragma once

#include <stdio.h>
#include <vector>

class Serial {
public:
	static FILE *file;

	static void open();
	static void close();
	static void steer(float inverseRadius);
	static void gas(float intensity);
	static void brake(float intensity);
	static void kill();

private:
	// measured values for interpolation
	static std::vector<double> *controlValsPtr;
	static std::vector<double> *inverseRadiiPtr;

	static void write(char controlVal);
};