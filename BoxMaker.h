#pragma once
#include <iostream>
#include <math.h>
#include <vector>
#include "vector3.h"
#include "sphere.cpp"
#include "box.h"

using namespace std;

class BoxMaker {
public:
	Vector3 minCoords;
	Vector3 maxCoords;

	BoxMaker(Vector3 _minCoords, Vector3 _maxCoords)
	{
		minCoords = _minCoords;
		maxCoords = _maxCoords;
	}

	void SplitBox(Box* boxToSplit)
	{
		if (GetSpheresFromBox(boxToSplit).size() > 1)
		{
			Vector3 minCoords1 = boxToSplit->minCoords;
			Vector3 maxCoords1 = Vector3(boxToSplit->GetHalfX(), boxToSplit->GetHalfY(), boxToSplit->maxCoords.z);
			Box box1 = CreateBox(minCoords1, maxCoords1, boxToSplit);
			boxToSplit->addBox(box1);

			Vector3 minCoords2 = Vector3(boxToSplit->GetHalfX(), boxToSplit->minCoords.y, boxToSplit->minCoords.z);
			Vector3 maxCoords2 = Vector3(boxToSplit->maxCoords.x, boxToSplit->GetHalfY(), boxToSplit->maxCoords.z);
			Box box2 = CreateBox(minCoords2, maxCoords2, boxToSplit);
			boxToSplit->addBox(box2);

			Vector3 minCoords3 = Vector3(boxToSplit->minCoords.x, boxToSplit->GetHalfY(), boxToSplit->minCoords.z);
			Vector3 maxCoords3 = Vector3(boxToSplit->GetHalfX(), boxToSplit->maxCoords.y, boxToSplit->maxCoords.z);
			Box box3 = CreateBox(minCoords3, maxCoords3, boxToSplit);
			boxToSplit->addBox(box3);

			Vector3 minCoords4 = Vector3(boxToSplit->GetHalfX(), boxToSplit->GetHalfY(), boxToSplit->minCoords.z);
			Vector3 maxCoords4 = boxToSplit->maxCoords;
			Box box4 = CreateBox(minCoords4, maxCoords4, boxToSplit);
			boxToSplit->addBox(box4);

			SplitBox(&box1);
			SplitBox(&box2);
			SplitBox(&box3);
			SplitBox(&box4);
		}
	}

	Box CreateBox(Vector3 minCoords, Vector3 maxCoords, Box* parent)
	{
		return Box(minCoords, maxCoords, parent);
	}

	std::vector<Sphere> GetSpheresFromBox(Box* box)
	{

	}
};