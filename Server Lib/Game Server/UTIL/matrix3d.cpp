// Arquivo matrix3d.cpp
// Criado em 03/10/2020 as 18:12 por Acrisio
// Implementação da classe Martix3D

#include "matrix3d.hpp"

using namespace stdA;

Matrix3D::Matrix3D(Vector3D _v1, Vector3D _v2, Vector3D _v3, Vector3D _v4)
	: m_v1(_v1), m_v2(_v2), m_v3(_v3), m_v4(_v4) {
}

Matrix3D::~Matrix3D() {
}

Matrix3D Matrix3D::crossMatrix(Matrix3D& _m1, Matrix3D& _m2) {
	return Matrix3D(
		_m1.m_v1.applyMatrix3(_m2),
		_m1.m_v2.applyMatrix3(_m2),
		_m1.m_v3.applyMatrix3(_m2),
		_m1.m_v4.applyMatrix4(_m2)
	);
}
