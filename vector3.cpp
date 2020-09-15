#include "vector3.h"

	float Vector3::lengthSquare() {
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}
	float Vector3::length() {
		return sqrt(this->lengthSquare());
	}

	float Vector3::dot(float f) {
		return dot(*this, f);
	}

	float Vector3::dot(Vector3 v2) {
		return dot(*this, v2);
	}

	Vector3 Vector3::cross(Vector3 v) {
		return cross(*this, v);
	}

	Vector3 Vector3::normalized() {
		return normalize(*this);
	}
	Vector3 Vector3::operator+(const Vector3 v) {
		return Vector3(this->x + v.x, this->y + v.y, this->z + v.z);
	}
	Vector3 Vector3::operator+(float f) {
		return Vector3(this->x + f, this->y + f, this->z + f);
	}
	Vector3 Vector3::operator-(const Vector3 v) {
		return Vector3(this->x - v.x, this->y - v.y, this->z - v.z);
	}
	Vector3 Vector3::operator-(float f) {
		return Vector3(this->x - f, this->y - f, this->z - f);
	}
	Vector3 Vector3::operator/(const float f) {
		if (f == 0)
			throw string("Division by zero");
		return Vector3(this->x / f, this->y / f, this->z / f);
	}
	Vector3 Vector3::operator*(const float f) {
		Vector3 vector;
		vector.x = this->x * f;
		vector.y = this->y * f;
		vector.z = this->z * f;
		return vector;
	}
	Vector3 Vector3::operator*(const Vector3 v) {
		return this->cross(v);
	}