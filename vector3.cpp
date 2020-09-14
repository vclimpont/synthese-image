#include <iostream>
#include <math.h>

using namespace std;

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3() {
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3(float vectorX, float vectorY, float vectorZ) {
		x = vectorX;
		y = vectorY;
		z = vectorZ;
	}
	float lengthSquare() {
		return this->x * this->x + this->y * this->y + this->z * this->z;
	}
	float length() {
		return sqrt(this->lengthSquare());
	}
	static float dot(Vector3 v1, float f) {
		return v1.x * f + v1.y * f + v1.z * f;
	}
	float dot(float f) {
		return dot(*this, f);
	}
	static Vector3 cross(Vector3 v1, Vector3 v2) {
		Vector3 vector;
		vector.x = v1.y * v2.z - v1.z * v2.y;
		vector.y = v1.z * v2.x - v1.x * v2.z;
		vector.z = v1.x * v2.y - v1.y * v2.x;
		return vector;
	}
	Vector3 cross(Vector3 v) {
		return cross(*this, v);
	}
	static Vector3 normalize(Vector3 v1) {
		try {
			return v1 / v1.length();
		}
		catch (string err) {
			cout << err + "\n";
		}
	}
	Vector3 normalized() {
		return normalize(*this);
	}
	Vector3 operator+(const Vector3 v) {
		return Vector3(this->x + v.x, this->y + v.y, this->z + v.z);
	}
	Vector3 operator+(float f) {
		return Vector3(this->x + f, this->y + f, this->z + f);
	}
	Vector3 operator-(const Vector3 v) {
		return Vector3(this->x - v.x, this->y - v.y, this->z - v.z);
	}
	Vector3 operator-(float f) {
		return Vector3(this->x - f, this->y - f, this->z - f);
	}
	Vector3 operator/(const float f) {
		if (f == 0)
			throw string("Division by zero");
		return Vector3(this->x / f, this->y / f, this->z / f);
	}
	Vector3 operator*(const float f) {
		Vector3 vector;
		vector.x = this->x * f;
		vector.y = this->y * f;
		vector.z = this->z * f;
		return vector;
	}
	Vector3 operator*(const Vector3 v) {
		return this->cross(v);
	}
	friend std::ostream& operator<<(std::ostream& output, const Vector3& vector) {
		output << "(" << vector.x << " ; " << vector.y << " ; " << vector.z << ")";
		return output;
	}
};