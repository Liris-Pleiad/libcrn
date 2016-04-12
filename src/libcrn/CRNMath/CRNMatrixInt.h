/* Copyright 2006-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
 * 
 * This file is part of libcrn.
 * 
 * libcrn is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * libcrn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with libcrn.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * file: CRNMatrixInt.h
 * \author Yann LEYDIER, Jean DUONG
 */

#ifndef CRNMATRIXINT_HEADER
#define CRNMATRIXINT_HEADER

#include <CRNMath/CRNMatrix.h>
#include <CRNMath/CRNMatrixIntPtr.h>

namespace crn
{
	/****************************************************************************/
	/*! \brief Integer matrix class
	 *
	 * Matrix contening integers
	 * 
	 * \author 	Yann LEYDIER
	 * \date		23 August 2006
	 * \version 0.3
	 * \ingroup math
	 */
	class MatrixInt: public Matrix<int>
	{
		public:
			using Matrix<int>::Matrix;
			MatrixInt(const Matrix<int> &m) : Matrix(m) {}
			MatrixInt(Matrix<int> &&m) : Matrix(std::move(m)) {}
			MatrixInt(const MatrixInt &) = default;
			MatrixInt(MatrixInt &&) = default;

			/*! \brief Destructor */
			virtual ~MatrixInt() override = default;
			
			MatrixInt& operator=(const MatrixInt &) = default;
			MatrixInt& operator=(MatrixInt &&) = default;

			// Special matrix constructors
			/*! \brief Creates a Gaussian mask given standard deviation */
			static MatrixInt NewGaussian(double sigma);
			/*! \brief Creates a 3x3 Gaussian mask */
			static MatrixInt NewGaussian3();
			/*! \brief Creates a Gaussian Sobel X derivation mask */
			static MatrixInt NewGaussianSobelX(double sigma);
			/*! \brief Creates a Gaussian Sobel Y derivation mask */
			static MatrixInt NewGaussianSobelY(double sigma);
			/*! \brief Creates a Sobel X derivation mask */
			static MatrixInt NewSobelX();
			/*! \brief Creates a Sobel Y derivation mask */
			static MatrixInt NewSobelY();

			// Distance transform half matrices
			/*! \brief Creates the first D4 DT mask */
			static MatrixInt NewD4DT1();
			/*! \brief Creates the second D4 DT mask */
			static MatrixInt NewD4DT2();
			/*! \brief Creates the first D8 DT mask */
			static MatrixInt NewD8DT1();
			/*! \brief Creates the second D8 DT mask */
			static MatrixInt NewD8DT2();
			/*! \brief Creates the first chamfer DT mask */
			static MatrixInt NewChamferDT1();
			/*! \brief Creates the second chamfer DT mask */
			static MatrixInt NewChamferDT2();

			CRN_DECLARE_CLASS_CONSTRUCTOR(MatrixInt)
	};
	template<> struct IsClonable<MatrixInt> : public std::true_type {};

	template<> struct TypeInfo<MatrixInt>
	{
		using SumType = MatrixInt;
		using DiffType = MatrixInt;
		using DecimalType = MatrixInt;
	};

}
#endif
