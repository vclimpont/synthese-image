#include "vector3.h"
#include <iostream>

using namespace std;

class Sphere {
public:
	Vector3 center;
	float radius;
	Vector3 albedo;
	
	Sphere(Vector3 _center, float _radius, Vector3 _albedo) {
		center = _center;
		radius = _radius;
		albedo = _albedo;
	}

	void SetCenter(Vector3 _center)
	{
		center = _center;
	}

	void SetAlbedo(Vector3 _albedo)
	{
		albedo = _albedo;
	}

	void SetRadius(float _radius)
	{
		radius = _radius;
	}

	Vector3 GetCenter()
	{
		return center;
	}

	Vector3 GetAlbedo()
	{
		return albedo;
	}

	float GetRadius()
	{
		return radius;
	}
};