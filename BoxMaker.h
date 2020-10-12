#pragma once
#include <iostream>
#include <math.h>
#include <vector>
#include "vector3.h"
#include "sphere.h"
#include "box.h"

using namespace std;

class BoxMaker {
public:
	Vector3 minCoords;
	Vector3 maxCoords;
	Box initialBox;

	BoxMaker(Vector3 _minCoords, Vector3 _maxCoords)
	{
		minCoords = _minCoords;
		maxCoords = _maxCoords;
		initialBox = Box(minCoords, maxCoords, NULL);
	}

	void AddSpheresToInitialBox(std::vector<Sphere> spheres)
	{
		for (Sphere s : spheres)
		{
			initialBox.addSphere(s);
		}
	}

	void SplitBoxOfLargestAxis(Box* boxToSplit, int maxCoord)
	{
		Vector3 minCoords1 = boxToSplit->minCoords;
		Vector3 maxCoords1 = boxToSplit->maxCoords;

		Vector3 minCoords2 = boxToSplit->minCoords;
		Vector3 maxCoords2 = boxToSplit->maxCoords;

		switch (maxCoord)
		{
		case 0:
			maxCoords1.x = boxToSplit->GetHalfX();
			minCoords2.x = boxToSplit->GetHalfX();
			break;
		case 1:
			maxCoords1.y = boxToSplit->GetHalfY();
			minCoords2.y = boxToSplit->GetHalfY();
			break;
		case 2:
			maxCoords1.z = boxToSplit->GetHalfZ();
			minCoords2.z = boxToSplit->GetHalfZ();
			break;
		}

		Box* box1 = CreateBox(minCoords1, maxCoords1, boxToSplit);
		box1->FindSpheresInTheBox();
		boxToSplit->addBox(box1);

		Box* box2 = CreateBox(minCoords2, maxCoords2, boxToSplit);
		box2->FindSpheresInTheBox();
		boxToSplit->addBox(box2);
	}

	void SplitBox(Box* boxToSplit)
	{
		if (boxToSplit->spheres.size() > 5)
		{
			int maxCoord = boxToSplit->GetMaxCoord();

			SplitBoxOfLargestAxis(boxToSplit, maxCoord);

			//std::cout << "---- boxToSplit ---- " << boxToSplit->minCoords << " " << boxToSplit->maxCoords << " " << boxToSplit->boxes.size() << " ---- ";

			for (Box* b : boxToSplit->boxes)
			{
				SplitBox(b);
			}
		}
	}

	void DisplaySpheresOfBox(Box* boxToDisplay)
	{
		std::cout << boxToDisplay->minCoords << " " << boxToDisplay->spheres.size() << " " << boxToDisplay->boxes.size() << "  |||||  ";

		for (Box* b : boxToDisplay->boxes)
		{
			DisplaySpheresOfBox(b);
		}
	}

	Box* CreateBox(Vector3 minCoords, Vector3 maxCoords, Box* parent)
	{
		return new Box(minCoords, maxCoords, parent);
	}
};