// Arquivo matrix3d.hpp
// Criado em 03/10/2020 as 17:04 por Acrisio
// Definição da clase Matrix3D

#pragma once
#ifndef _STDA_MATRIX3D_HPP
#define _STDA_MATRIX3D_HPP

#include "vector3d.hpp"

namespace stdA {

	class Matrix3D {
		public:
			Matrix3D(Vector3D _v1, Vector3D _v2, Vector3D _v3, Vector3D _v4);
			virtual ~Matrix3D();

			static Matrix3D crossMatrix(Matrix3D& _m1, Matrix3D& _m2);

		public:
			Vector3D m_v1;
			Vector3D m_v2;
			Vector3D m_v3;
			Vector3D m_v4;
	};
}

#endif // !_STDA_MATRIX3D_HPP
