#include "vector3.h"
#include <iostream>

using namespace std;

class Sphere {
public:
	Vector3 center;
	float radius;
	
	Sphere(Vector3 _center, float _radius) {
		center = _center;
		radius = _radius;
	}

	void SetCenter(Vector3 _center)
	{
		center = _center;
	}

	void SetRadius(float _radius)
	{
		radius = _radius;
	}

	Vector3 GetCenter()
	{
		return center;
	}

	float GetRadius()
	{
		return radius;
	}
};