// main.cpp
// Author: Travis Vanderstad

#include <stdio.h>
#include <string.h>
#include <chrono>
#include <vector>

#include <zed/Mat.hpp>
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

#include "PointCloud.hpp"
#include "CloudViewer.hpp"
#include "HeightMap.hpp"

using namespace sl::zed;
using namespace std;

float controlVal(std::vector<double> *inverseRadiiPtr, std::vector<double> *controlValsPtr, double inverseRadius) {
	std::vector<double> inverseRadii = *inverseRadiiPtr;
	std::vector<double> controlVals = *controlValsPtr;
	// linear search the list of controlval-radius pairs for correct span
	for(int i = 0; i < inverseRadii.size()-1; i++) {
		if(inverseRadius >= inverseRadii[i] && inverseRadius <= inverseRadii[i+1]) {
			double factor = (inverseRadius - inverseRadii[i])/(inverseRadii[i+1] - inverseRadii[i]);
			return factor * inverseRadii[i+1] + (1 - factor) * inverseRadii[i];
		}
	}
}

int main() {
	Camera* camera = new Camera(HD720, 15.0);
	ERRCODE code = camera->init(MODE::PERFORMANCE, 0);

	if (code != SUCCESS) {
		cout << errcode2str(code) << endl;
		delete camera;
		return 1;
	}

	// measured control value - inverse radii pairs
	std::vector<double> *controlVals = new std::vector<double>;
	std::vector<double> *inverseRadii = new std::vector<double>;
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

	int width = camera->getImageSize().width;
	int height = camera->getImageSize().height;
	PointCloud *cloud = new PointCloud(width, height);
	HeightMap *heightMap = new HeightMap(320, 640);
	CloudViewer *viewer = new CloudViewer();
	int key = ' ';
	Mat depth, imLeft;

	// application quits when user stikes 'q'
	while (key != 'q') {
		camera->setConfidenceThreshold(98); // parameter is reliability index ~[1,100] with 100 as no filtering
		camera->grab(SENSING_MODE::RAW);
		depth = camera->retrieveMeasure(MEASURE::DEPTH);
		imLeft = camera->retrieveImage(SIDE::LEFT);
		cloud->fill(imLeft.data, (float*) depth.data, camera->getParameters());
		cloud->fillHeightMap(heightMap);

		// heightMap->calcSobel(5);
		viewer->AddData(heightMap);

		// Update the value of key so that we can quit when the user strikes 'q'
		key = viewer->getKey();
		// key = 'q';
	}

	printf("quitting\n");
	delete camera;
	delete cloud;
	delete viewer;
	return 0;
}
