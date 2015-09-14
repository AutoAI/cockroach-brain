// PathPlanner.cpp

#include "PathPlanner.hpp"

#include <iostream>

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
	while(percentageBad(x1, z1, x2, z2) < EDGE_THRESHOLD && x1 > -1) {
		x1--;
		x2--;
	}
	if(x1 == -1) {
		// we did bad
		printf("we did bad\n");
	}
	while(percentageBad(x3, z3, x4, z4) < EDGE_THRESHOLD && x3 < hm->width) {
		x3++;
		x4++;
	}
	if(x3 == hm->width) {
		// we did bad
		printf("we did bad\n");
	}
	edges[0] = ((x1 - hm->width / 2) * VIEW_WIDTH) / hm->width;;
	edges[2] = ((x2 - hm->width / 2) * VIEW_WIDTH) / hm->width;;
	edges[4] = ((x3 - hm->width / 2) * VIEW_WIDTH) / hm->width;;
	edges[6] = ((x4 - hm->width / 2) * VIEW_WIDTH) / hm->width;;
}

float* PathPlanner::getEdges() {
	printf("edges = [(%f, %f), (%f, %f), (%f, %f), (%f, %f)]\n", x1, z1, x2, z2, x3, z3, x4, z4);
	return edges;
}

float* PathPlanner::getTarget() {
	target[0] = (edges[0] + edges[2]) / 2;
	target[1] = (LOOKAHEAD_MIN + LOOKAHEAD_MAX) / 2;
	return target;
}

float PathPlanner::percentageBad(size_t x1, size_t z1, size_t x2, size_t z2) {
	float numBad = 0;
	for(int i = z1; i <= z2; i++) {
		if(!hm->sobel[i * hm->width + x1]) {
			numBad++;
		}
	}
	return numBad / float(z2 - z1 + 1);
}