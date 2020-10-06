#pragma once
#include <iostream>
#include <math.h>
#include "vector3.h"
#include "sphere.h"
#include <vector>

using namespace std;

class Box {
public:
	Box* parentBox;
	std::vector<Box> boxes;
	std::vector<Sphere> spheres;
	Vector3 minCoords;
	Vector3 maxCoords;

	Box()
	{
		minCoords = Vector3(0, 0, 0);
		maxCoords = Vector3(0, 0, 0);
		parentBox = NULL;
	}
	Box(Vector3 _minCoords, Vector3 _maxCoords, Box* _parentBox)
	{
		minCoords = _minCoords;
		maxCoords = _maxCoords;
		parentBox = _parentBox;
	}

	void FindSpheresInTheBox()
	{
		for (Sphere s : Box::parentBox->spheres)
		{
			if (BoxIntersectSphere(s))
			{
				addSphere(s);
			}
		}
	}

	bool BoxIntersectSphere(Sphere sphere)
	{
		Vector3 C1 = minCoords;
		Vector3 C2 = maxCoords;
		Vector3 S = sphere.GetCenter();
		float R = sphere.GetRadius();
		float dist_squared = R * R;

		if (S.x < C1.x) dist_squared -= squared(S.x - C1.x);
		else if (S.x > C2.x) dist_squared -= squared(S.x - C2.x);
		if (S.y < C1.y) dist_squared -= squared(S.y - C1.y);
		else if (S.y > C2.y) dist_squared -= squared(S.y - C2.y);
		if (S.z < C1.z) dist_squared -= squared(S.z - C1.z);
		else if (S.z > C2.z) dist_squared -= squared(S.z - C2.z);
		return dist_squared > 0;
	}

	float squared(float v)
	{
		return v * v;
	}

	void addBox(Box box)
	{
		boxes.push_back(box);
	}

	void addSphere(Sphere s)
	{
		spheres.push_back(s);
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