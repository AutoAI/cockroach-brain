// Serial.cpp

#include "Serial.hpp"

#include <unistd.h>

#define STEERING_MULTIPLIER 0.65

FILE* Serial::file;
std::vector<double>* Serial::controlValsPtr;
std::vector<double>* Serial::inverseRadiiPtr;

void Serial::open() {
	file = fopen("/dev/ttyACM0","w");
	if(file == NULL) {
		printf("failed to open serial port\n");
	}
	controlValsPtr = new std::vector<double>;
	inverseRadiiPtr = new std::vector<double>;

	// measured values (error on the side of tighter turns)
	controlValsPtr->push_back(-1);
	controlValsPtr->push_back(-0.746031746031746);
	controlValsPtr->push_back(-0.492063492063492);
	controlValsPtr->push_back(-0.238095238095238);
	controlValsPtr->push_back(-0.111111111111111);
	controlValsPtr->push_back(0.142857142857143);
	controlValsPtr->push_back(0.26984126984127);
	controlValsPtr->push_back(0.523809523809524);
	controlValsPtr->push_back(0.777777777777778);
	controlValsPtr->push_back(1);

	inverseRadiiPtr->push_back(-0.47998920166788);
	inverseRadiiPtr->push_back(-0.398285245315489);
	inverseRadiiPtr->push_back(-0.295786020085001);
	inverseRadiiPtr->push_back(-0.161900458881389);
	inverseRadiiPtr->push_back(-0.129980905282478);
	inverseRadiiPtr->push_back(0.0880996586174463);
	inverseRadiiPtr->push_back(0.183673772921511);
	inverseRadiiPtr->push_back(0.342233651979485);
	inverseRadiiPtr->push_back(0.432027836572337);
	inverseRadiiPtr->push_back(0.53359967963806);

	// sleep in hopes that by the time we're done serial connection is good
	printf("waiting... \n");
	usleep(2000000);
}

void Serial::close() {
	fclose(file);
}

void Serial::steer(float inverseRadius) {
	inverseRadius *= STEERING_MULTIPLIER;
	std::vector<double> controlVals = *controlValsPtr;
	std::vector<double> inverseRadii = *inverseRadiiPtr;
	// linear search the list of controlval-radius pairs for correct span
	double controlVal;
	bool found = false;
	for(int i = 0; i < inverseRadii.size()-1; i++) {
		if(inverseRadius >= inverseRadii[i] && inverseRadius <= inverseRadii[i+1]) {
			double factor = (inverseRadius - inverseRadii[i])/(inverseRadii[i+1] - inverseRadii[i]);
			controlVal = factor * controlVals[i+1] + (1 - factor) * controlVals[i];
			found = true;
			continue;
		}
	}

	// if we couldn't interpolate, extrapolate (nearest-neighbor-style)
	if(!found) {
		if(inverseRadius < inverseRadii[0]) {
			controlVal = controlVals[0];
		} else {
			controlVal = controlVals[controlVals.size()-1];
		}
	}

	write(0x40 | char((controlVal + 1) * 31.5));
	write('\n');
}

void Serial::gas(float intensity) {
	write(0x80 | char((1 - intensity) * 63));
	write('\n');
}

void Serial::brake(float intensity) {
	write(0xc0 | char((intensity) * 63));
	write('\n');
}

void Serial::kill() {
	write(0x00);
	write('\n');
}

void Serial::write(char controlByte) {
	size_t result = fwrite(&controlByte, 1, 1, file);
	if(result != 1) {
		printf("failed to write a value to serial\n");
	}
}
