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

// temp
#include <unistd.h>
// end temp

using namespace sl::zed;
using namespace std;

float steerToward(float *target) {
	return 0;
}

int main() {
	// temp
	Serial::steer(-.4);
	usleep(2000000);
	Serial::steer(.4);
	return 0;
	// end temp

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
	HeightMap *heightMap = new HeightMap(240, 480);
	CloudViewer *viewer = new CloudViewer();
	PathPlanner *planner = new PathPlanner(heightMap);
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

		heightMap->calcSobel(1.5);
		planner->calcEdges();
		steerToward(planner->getTarget());

		viewer->AddData(heightMap);
		viewer->AddPlanner(planner);

		// Update the value of key so that we can quit when the user strikes 'q'
		key = viewer->getKey();
	}

	printf("quitting\n");
	delete camera;
	delete cloud;
	delete viewer;
	return 0;
}
