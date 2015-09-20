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

// Definitely a CUDA candidate
void PathPlanner::calcEdges() {
	/*
	edges arranged like so:
	(x2, z2)----------------(x4, z4)
	    |                       |
	    |                       |
	    |                       |
	(x1, z1)----------------(x3, z3)
	with top as vehicle forward
	*/
	const int pathWidth = PATH_WIDTH * hm->width / VIEW_WIDTH;
	const int lookaheadMin = LOOKAHEAD_MIN * hm->depth / VIEW_DEPTH;
	const int lookaheadMax = LOOKAHEAD_MAX * hm->depth / VIEW_DEPTH;
	const float middleI = (hm->width - pathWidth) / 2;
	float bestI = 0;
	float bestBad = 2;
	float thisBad;
	for(int i = 0; i < hm->width - pathWidth; i++) {
		thisBad = percentageBad(i, lookaheadMin, pathWidth + i, lookaheadMax) + float((i - middleI) * (i - middleI)) / (middleI * middleI);
		if(thisBad < bestBad) {
			bestBad = thisBad;
			bestI = i;
		} 
	}
	// set x values
	edges[0] = (VIEW_WIDTH / hm->width) * (-hm->width/2 + bestI);
	edges[2] = (VIEW_WIDTH / hm->width) * (-hm->width/2 + bestI);
	edges[4] = (VIEW_WIDTH / hm->width) * (-hm->width/2 + pathWidth + bestI);
	edges[6] = (VIEW_WIDTH / hm->width) * (-hm->width/2 + pathWidth + bestI);
	printf("edges: [(%f, %f), (%f, %f), (%f, %f), (%f, %f)]\n", edges[0], edges[1], edges[2], edges[3], edges[4], edges[5], edges[6], edges[7]);
}

float* PathPlanner::getEdges() {
	return edges;
}

float* PathPlanner::getTarget() {
	target[0] = (edges[0] + edges[4]) / 2;
	target[1] = (LOOKAHEAD_MIN + LOOKAHEAD_MAX) / 2;
	return target;
}

// Definitely a CUDA candidate
float PathPlanner::percentageBad(size_t x1, size_t z1, size_t x2, size_t z2) {
	int numBad = 0;
	int skipped = 0;
	for(int i = z1; i <= z2; i++) {
		for(int j = x1; j <= x2; j++) {
			if(hm->frequencies[i * hm->width + j] == 0) {
				skipped++;
				numBad++;
			} else if(!hm->sobel[i * hm->width + j]) {
				numBad++;
			}
		}
	}
	printf("x1: %d, z1: %d, x2: %d, z2: %d\n", x1, z1, x2, z2);
	return float(numBad) / (float((z2 - z1 + 1) * (x2 - x1 + 1) - skipped) * 3);
}
