// Arquivo vector3d.cpp
// Criado em 03/10/2020 as 13:32 por Acrisio
// Implemeta��o da classe Vector3D

#include "vector3d.hpp"
#include "matrix3d.hpp"
#include <algorithm>

#include <cmath>

using namespace stdA;

Vector3D::Vector3D(float _x, float _y, float _z) : m_x(_x), m_y(_y), m_z(_z) {
}

Vector3D::~Vector3D() {}

Vector3D& Vector3D::normalize() {
	return divideScalar( length() );
}

Vector3D& Vector3D::negate() {
	return multiplyScalar(-1);
}

float Vector3D::length() {
	return sqrt( m_x * m_x + m_y * m_y + m_z * m_z );
}

float Vector3D::dot(Vector3D& _vector3d) {
	return dot(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

float Vector3D::dot(float _x, float _y, float _z) {
	return m_x * _x + m_y * _y + m_z * _z;
}

Vector3D& Vector3D::copy(Vector3D& _vector3d) {
	
	m_x = _vector3d.m_x;
	m_y = _vector3d.m_y;
	m_z = _vector3d.m_z;

	return *this;
}

Vector3D Vector3D::clone() {
	return Vector3D(m_x, m_y, m_z);
}

Vector3D& Vector3D::addScalar(float _value) {
	
	m_x += _value;
	m_y += _value;
	m_z += _value;

	return *this;
}

Vector3D& Vector3D::subScalar(float _value) {
	
	m_x -= _value;
	m_y -= _value;
	m_z -= _value;

	return *this;
}

Vector3D& Vector3D::multiplyScalar(float _value) {
	
	m_x *= _value;
	m_y *= _value;
	m_z *= _value;

	return *this;
}

Vector3D& Vector3D::divideScalar(float _value) {
	
	m_x /= _value;
	m_y /= _value;
	m_z /= _value;

	return *this;
}

Vector3D& Vector3D::add(Vector3D& _vector3d) {
	return add(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

Vector3D& Vector3D::add(float _x, float _y, float _z) {
	
	m_x += _x;
	m_y += _y;
	m_z += _z;

	return *this;
}

Vector3D& Vector3D::sub(Vector3D& _vector3d) {
	return sub(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

Vector3D& Vector3D::sub(float _x, float _y, float _z) {
	
	m_x -= _x;
	m_y -= _y;
	m_z -= _z;

	return *this;
}

Vector3D& Vector3D::multiply(Vector3D& _vector3d) {
	return multiply(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

Vector3D& Vector3D::multiply(float _x, float _y, float _z) {
	
	m_x *= _x;
	m_y *= _y;
	m_z *= _z;

	return *this;
}

Vector3D& Vector3D::divide(Vector3D& _vector3d) {
	return divide(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

Vector3D& Vector3D::divide(float _x, float _y, float _z) {
	
	m_x /= _x;
	m_y /= _y;
	m_z /= _z;

	return *this;
}

Vector3D& Vector3D::cross(Vector3D& _vector3d) {
	return cross(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

Vector3D& Vector3D::cross(float _x, float _y, float _z) {
	
	float x = m_x, y = m_y, z = m_z;

	m_x = y * _z - z * _y;
	m_y = z * _x - x * _z;
	m_z = x * _y - y * _x;

	return *this;
}

float Vector3D::distanceTo(Vector3D& _vector3d) {
	return distanceTo(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

float Vector3D::distanceTo(float _x, float _y, float _z) {

	float dx = m_x - _x,
		  dy = m_y - _y,
		  dz = m_z - _z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

float Vector3D::distanceXZTo(Vector3D& _vector3d) {
	return distanceXZTo(_vector3d.m_x, _vector3d.m_z);
}

float Vector3D::distanceXZTo(float _x, float _z) {
	return distanceTo(_x, 0.f, _z);
}

float Vector3D::angleTo(Vector3D& _vector3d) {
	return angleTo(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

float Vector3D::angleTo(float _x, float _y, float _z) {
	
	float theta = dot(_x, _y, _z) / (length() * Vector3D(_x, _y, _z).length());

	return acos(std::clamp(theta, -1.f, 1.f));
}

bool Vector3D::isEqual(Vector3D& _vector3d){
	return isEqual(_vector3d.m_x, _vector3d.m_y, _vector3d.m_z);
}

bool Vector3D::isEqual(float _x, float _y, float _z){
	return (m_x == _x) && (m_y == _y) && (m_z == _z);
}

Vector3D& stdA::Vector3D::applyMatrix3(Matrix3D& _matrix3d) {
	
	float x = m_x, y = m_y, z = m_z;

	m_x = _matrix3d.m_v1.m_x * x + _matrix3d.m_v2.m_x * y + _matrix3d.m_v3.m_x * z;
	m_y = _matrix3d.m_v1.m_y * x + _matrix3d.m_v2.m_y * y + _matrix3d.m_v3.m_y * z;
	m_z = _matrix3d.m_v1.m_z * x + _matrix3d.m_v2.m_z * y + _matrix3d.m_v3.m_z * z;

	return *this;
}

Vector3D& Vector3D::applyMatrix4(Matrix3D& _matrix3d) {
	
	float x = m_x, y = m_y, z = m_z;

	m_x = _matrix3d.m_v1.m_x * x + _matrix3d.m_v2.m_x * y + _matrix3d.m_v3.m_x * z + _matrix3d.m_v4.m_x;
	m_y = _matrix3d.m_v1.m_y * x + _matrix3d.m_v2.m_y * y + _matrix3d.m_v3.m_y * z + _matrix3d.m_v4.m_y;
	m_z = _matrix3d.m_v1.m_z * x + _matrix3d.m_v2.m_z * y + _matrix3d.m_v3.m_z * z + _matrix3d.m_v4.m_z;

	return *this;
}
