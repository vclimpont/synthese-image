#include "vector3.h"
#include <iostream>

using namespace std;

class Light {
public:
	Vector3 position;
	Vector3 color;
	float intensity;

	Light(Vector3 _position, Vector3 _color, float _intensity) {
		position = _position;
		color = _color;
		intensity = _intensity;
	}

	void SetPosition(Vector3 _position)
	{
		position = _position;
	}

	void SetColor(Vector3 _color)
	{
		color = _color;
	}

	void SetIntensity(float _intensity)
	{
		intensity = _intensity;
	}

	Vector3 GetPosition()
	{
		return position;
	}

	Vector3 GetColor()
	{
		return color;
	}

	float GetIntensity()
	{
		return intensity;
	}
};