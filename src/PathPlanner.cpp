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
	    |			    |
	    |			    |
	    |			    |
	(x1, z1)----------------(x3, z3)
	with top as vehicle forward
	*/
	size_t x1, z1, x4, z4;
	x1 = 0;
	x4 = PATH_WIDTH;
	z1 = LOOKAHEAD_MIN;
	z4 = LOOKAHEAD_MAX;
	float bestI = 0;
	float bestBad = 1;
	for(int i = 0; i < hm->width - PATH_WIDTH; i++) {
		if(percentageBad(x1 + i, z1, x4 + i, z4) < bestBad) {
			bestI = i;
		} 
	}
	edges[0] = bestI;
	edges[2] = bestI;
	edges[4] = PATH_WIDTH + bestI;
	edges[6] = PATH_WIDTH + bestI;
	// printf("edges: [(%f, %f), (%f, %f), (%f, %f), (%f, %f)]\n", edges[0], edges[1], edges[2], edges[3], edges[4], edges[5], edges[6], edges[7]);
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
	float numBad = 0;
	int skipped = 0;
	for(int i = z1; i <= z2; i++) {
		for(int j = x1; j <= x2; j++) {
			if(hm->frequencies[i * hm->width + j] == 0) {
				skipped++;
			} else if(!hm->sobel[i * hm->width + j]) {
				numBad++;
			}
		}
	}
	return numBad / (float((z2 - z1 + 1) * (x2 - x1 + 1) - skipped) * 3);
}
