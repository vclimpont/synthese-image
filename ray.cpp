#include "vector3.cpp"
#include <iostream>

using namespace std;

class Ray {
public:
	Vector3 position;
	Vector3 direction;

	Ray(Vector3 _position, Vector3 _direction) {
		position = _position;
		direction = _position;
	}

	void SetPosition(Vector3 _position)
	{
		position = _position;
	}

	void SetDirection(Vector3 _direction)
	{
		direction = _direction;
	}

	Vector3 GetPosition()
	{
		return position;
	}

	Vector3 GetDirection()
	{
		return direction;
	}
};