#pragma once
#include <iostream>
#include <math.h>
#include "vector3.h"
#include <vector>

using namespace std;

class Box {
public:
	Box* parentBox;
	std::vector<Box> boxes;
	Vector3 minCoords;
	Vector3 maxCoords;

	Box(Vector3 _minCoords, Vector3 _maxCoords, Box* _parentBox)
	{
		minCoords = _minCoords;
		maxCoords = _maxCoords;
		parentBox = _parentBox;
	}

	void addBox(Box box)
	{
		boxes.push_back(box);
	}

	float GetHalfX()
	{
		return (maxCoords.x - minCoords.x) / 2;
	}

	float GetHalfY()
	{
		return (maxCoords.y - minCoords.y) / 2;
	}

	float GetHalfZ()
	{
		return (maxCoords.z - minCoords.z) / 2;
	}
};