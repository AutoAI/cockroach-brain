// PathPlanner.hpp

#pragma once

#define LOOKAHEAD_MIN 1.0f // meters
#define LOOKAHEAD_MAX 2.0f
#define EDGE_THRESHOLD .5f // [0, 1]

#include "HeightMap.hpp"

class PathPlanner {
public:
	PathPlanner(HeightMap *hm);
	~PathPlanner();

	void calcEdges();
	float* getEdges(); // x1-z1-x2-z2-x3-z3-x4-z4
	float* getTarget(); // x-z

private:
	HeightMap* hm;
	float* edges;
	float* target;

	float percentageBad(size_t x1, size_t z1, size_t x2, size_t z2);
};