// PathPlanner.cpp

#include "PathPlanner.hpp"

#include <iostream>

#define TEMP 0

PathPlanner::PathPlanner(HeightMap *hm) {
	this->hm = hm;
	edges = new float[8];
	edges[1] = edges[5] = LOOKAHEAD_MIN;
	edges[3] = edges[7] = LOOKAHEAD_MAX;
	target = new float[2];
}

PathPlanner::~PathPlanner() {
	delete edges;
	delete target;
}

void PathPlanner::calcEdges() {
	/*
	edges arranged like so:
	(x2, z2)		(x4, z4)
		|				|
		|				|
		|				|
	(x1, z1)		(x3, z3)
	with top as vehicle forward
	*/
	size_t x1, z1, x2, z2, x3, z3, x4, z4;
	x1 = x2 = x3 = x4 = hm->width / 2;
	z1 = z3 = LOOKAHEAD_MIN * hm->depth / VIEW_DEPTH;
	z2 = z4 = LOOKAHEAD_MAX * hm->depth / VIEW_DEPTH;
	bool keepGoing = true;
	int i1, i2;
	while(percentageBad(x1, z1, x2, z2) < EDGE_THRESHOLD && x1 >= TEMP) {
		x1--;
		x2--;
		for(i1 = -TEMP; i1 <= TEMP; i1++) {
			keepGoing = keepGoing && percentageBad(x1 + i1, z1, x2 - i1, z2) < EDGE_THRESHOLD;
			if(!keepGoing) {
				break;
			}
		}
	}
	if(x1 == -1) {
		// we did bad
		printf("we did bad\n");
	}
	while(percentageBad(x3, z3, x4, z4) < EDGE_THRESHOLD && x3 < hm->width - TEMP) {
		x3++;
		x4++;
		for(i2 = -TEMP; i2 <= TEMP; i2++) {
			keepGoing = keepGoing && percentageBad(x3 + i2, z3, x4 - i2, z4) < EDGE_THRESHOLD;
			if(!keepGoing) {
				break;
			}
		}
	}
	if(x3 == hm->width) {
		// we did bad
		printf("we did bad\n");
	}
	edges[0] = ((float(x1 + i1) - hm->width / 2) * VIEW_WIDTH) / hm->width;
	edges[2] = ((float(x2 - i1) - hm->width / 2) * VIEW_WIDTH) / hm->width;
	edges[4] = ((float(x3 + i2) - hm->width / 2) * VIEW_WIDTH) / hm->width;
	edges[6] = ((float(x4 - i2) - hm->width / 2) * VIEW_WIDTH) / hm->width;
	//	printf("edges: [(%f, %f), (%f, %f), (%f, %f), (%f, %f)]\n", edges[0], edges[1], edges[2], edges[3], edges[4], edges[5], edges[6], edges[7]);
}

float* PathPlanner::getEdges() {
	return edges;
}

float* PathPlanner::getTarget() {
	target[0] = (edges[0] + edges[4]) / 2;
	target[1] = (LOOKAHEAD_MIN + LOOKAHEAD_MAX) / 2;
	return target;
}

float PathPlanner::percentageBad(size_t x1, size_t z1, size_t x2, size_t z2) {
	float numBad = 0;
	int skipped = 0;
	size_t x;
	for(int i = z1; i <= z2; i++) {
		x = size_t(float(x1) + (float(i - z1) / float(z2 - z1)) * (float(x2) - float(x1)));
		if(hm->frequencies[i * hm->width + x] == 0) {
			skipped++;
			continue;
		}
		if(!hm->sobel[i * hm->width + x]) {
			numBad++;
		}
	}
	return numBad / float(z2 - z1 + 1 - skipped);
}
