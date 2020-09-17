#include "vector3.h"
#include <iostream>

using namespace std;

class Light {
public:
	Vector3 position;
	Vector3 color;

	Light(Vector3 _position, Vector3 _color) {
		position = _position;
		color = _color;
	}

	void SetPosition(Vector3 _position)
	{
		position = _position;
	}

	void SetColor(Vector3 _color)
	{
		color = _color;
	}

	Vector3 GetPosition()
	{
		return position;
	}

	Vector3 GetColor()
	{
		return color;
	}
};