// Arquivo vector3d.hpp
// Criado em 03/10/2020 as 13:13 por Acrisio
// Definição da classe Vector3D

#pragma once
#ifndef _STDA_VECTOR3D_HPP
#define _STDA_VECTOR3D_HPP

namespace stdA {

	class Matrix3D;

	class Vector3D {

		public:
			Vector3D(float _x, float _y, float _z);
			virtual ~Vector3D();

		public:
			Vector3D& normalize();
			Vector3D& negate();

			float length();
			float dot(Vector3D& _vector3d);
			float dot(float _x, float _y, float _z);
			
			Vector3D& copy(Vector3D& _vector3d);
			Vector3D clone();

			Vector3D& addScalar(float _value);
			Vector3D& subScalar(float _value);
			Vector3D& multiplyScalar(float _value);
			Vector3D& divideScalar(float _value);

			Vector3D& add(Vector3D& _vector3d);
			Vector3D& add(float _x, float _y, float _z);

			Vector3D& sub(Vector3D& _vector3d);
			Vector3D& sub(float _x, float _y, float _z);

			Vector3D& multiply(Vector3D& _vector3d);
			Vector3D& multiply(float _x, float _y, float _z);

			Vector3D& divide(Vector3D& _vector3d);
			Vector3D& divide(float _x, float _y, float _z);

			Vector3D& cross(Vector3D& _vector3d);
			Vector3D& cross(float _x, float _y, float _z);

			float distanceTo(Vector3D& _vector3d);
			float distanceTo(float _x, float _y, float _z);

			float distanceXZTo(Vector3D& _vector3d);
			float distanceXZTo(float _x, float _z);

			float angleTo(Vector3D& _vector3d);
			float angleTo(float _x, float _y, float _z);

			bool isEqual(Vector3D& _vector3d);
			bool isEqual(float _x, float _y, float _z);

			Vector3D& applyMatrix3(Matrix3D& _matrix3d);
			Vector3D& applyMatrix4(Matrix3D& _matrix3d);

		public:
			float m_x;
			float m_y;
			float m_z;
	};
}

#endif // !_STDA_VECTOR3D_HPP
