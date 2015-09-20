// main.cpp
// Author: Travis Vanderstad

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <vector>

#include <zed/Mat.hpp>
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

#include "PointCloud.hpp"
#include "CloudViewer.hpp"
#include "HeightMap.hpp"
#include "PathPlanner.hpp"
#include "Serial.hpp"

using namespace sl::zed;
using namespace std;

float steerToward(float *target) {
	float x = target[0];
	float y = target[1];

	Serial::steer(-2*x /(x*x + y*y));
	return 0;
}

int main() {
	Serial::open();

	Camera* camera = new Camera(HD720, 15.0);
	ERRCODE code = camera->init(MODE::QUALITY, 0);

	if (code != SUCCESS) {
		cout << errcode2str(code) << endl;
		delete camera;
		return 1;
	}

	int width = camera->getImageSize().width;
	int height = camera->getImageSize().height;
	PointCloud *cloud = new PointCloud(width, height);
	HeightMap *heightMap = new HeightMap(80, 160);
	CloudViewer *viewer = new CloudViewer();
	PathPlanner *planner = new PathPlanner(heightMap);
	int key = ' ';
	Mat depth, imLeft;

	printf("LET'S GO!\n");
	Serial::gas(0.25);

	// application quits when user stikes 'q'
	while (key != 'q') {
		camera->setConfidenceThreshold(98); // parameter is reliability index ~[1,100] with 100 as no filtering
		camera->grab(SENSING_MODE::RAW);
		depth = camera->retrieveMeasure(MEASURE::DEPTH);
		imLeft = camera->retrieveImage(SIDE::LEFT);
		cloud->fill(imLeft.data, (float*) depth.data, camera->getParameters());
		cloud->fillHeightMap(heightMap);

		heightMap->calcSobel(0.5);
		planner->calcEdges();
		steerToward(planner->getTarget());

		viewer->AddData(heightMap);
		viewer->AddPlanner(planner);
		viewer->AddData(cloud);

		// Update the value of key so that we can quit when the user strikes 'q'
		key = viewer->getKey();
	}

	printf("quitting\n");
	delete camera;
	delete cloud;
	delete viewer;
	return 0;
}
