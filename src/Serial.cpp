// Serial.cpp

#include "Serial.hpp"

#include <unistd.h>

static void Serial::open() {
	file = fopen("/dev/ttyACM11","w");
	if(file == NULL) {
		printf("failed to open serial port\n");
	}
	controlValsPtr = new std::vector<double>;
	inverseRadiiPtr = new std::vector<double>;

	// measured values (error on the side of tighter turns)
	controlVals->push_back(-1);
	controlVals->push_back(-0.746031746031746);
	controlVals->push_back(-0.492063492063492);
	controlVals->push_back(-0.238095238095238);
	controlVals->push_back(-0.111111111111111);
	controlVals->push_back(0.142857142857143);
	controlVals->push_back(0.26984126984127);
	controlVals->push_back(0.523809523809524);
	controlVals->push_back(0.777777777777778);
	controlVals->push_back(1);

	inverseRadii->push_back(-0.47998920166788);
	inverseRadii->push_back(-0.398285245315489);
	inverseRadii->push_back(-0.295786020085001);
	inverseRadii->push_back(-0.161900458881389);
	inverseRadii->push_back(-0.129980905282478);
	inverseRadii->push_back(0.0880996586174463);
	inverseRadii->push_back(0.183673772921511);
	inverseRadii->push_back(0.342233651979485);
	inverseRadii->push_back(0.432027836572337);
	inverseRadii->push_back(0.53359967963806);

	// sleep in hopes that by the time we're done serial connection is good
	usleep(2000000);
}

static void Serial::close() {
	fclose(file);
}

static void Serial::steer(float inverseRadius) {
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
			controlVal = controlVals[controlVals.size()]
		}
	}

	write(0x40 | char((controlVal + 1) * 31.5));
}

static void Serial::gas(float intensity) {
	write(0x80 | char((1 - intensity) * 63));
}

static void Serial::brake(float intensity) {
	write(0xc0 | char((intensity) * 63));
}

static void Serial::kill() {
	write(0x00);
}

static void Serial::write(char controlByte) {
	size_t result = fwrite(&controlByte, 1, 1, file);
	if(result != 1) {
		printf("failed to write a value to serial\n");
	}
}