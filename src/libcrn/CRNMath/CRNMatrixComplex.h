/* Copyright 2013-2016 INSA-Lyon, ENS-Lyon
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
 * file: CRNMatrixComplex.h
 * \author Yann LEYDIER
 */

#ifndef CRNMATRIXComplex_HEADER
#define CRNMATRIXComplex_HEADER

#include <CRNMath/CRNMatrix.h>
#include <complex>

namespace crn
{
	/*! \brief Fast Fourier transform */
	void FFT(std::vector<std::complex<double> > &sig, bool direct);

	class MatrixDouble;
	class Point2DInt;

	/****************************************************************************/
	/*! \brief Complex matrix class
	 *
	 * Matrix containing complex numbers
	 * 
	 * \author 	Yann LEYDIER
	 * \date		April 2013
	 * \version 0.2
	 * \ingroup math
	 */
	class MatrixComplex: public Matrix<std::complex<double>>
	{
		public:
			using Matrix<std::complex<double>>::Matrix;
			MatrixComplex(const Matrix<std::complex<double>> &m) : Matrix(m) {}
			MatrixComplex(Matrix<std::complex<double>> &&m) : Matrix(std::move(m)) {}
			MatrixComplex(const MatrixComplex &) = default;
			MatrixComplex(MatrixComplex &&) = default;

			/*! \brief Destructor */
			virtual ~MatrixComplex() override = default;

			MatrixComplex& operator=(const MatrixComplex &) = default;
			MatrixComplex& operator=(MatrixComplex &&) = default;

			/*! \brief Grows the matrix to power of 2 sizes (for FFT) */
			void GrowToPowerOf2(bool make_square, const std::complex<double> &fill_value = std::complex<double>(0, 0));

			/*! \brief Returns a matrix containing the modules of the inner values */
			MatrixDouble MakeModule() const;

			/*! \brief Inplace fast Fourier transform */
			void FFT(bool direct);

			/*! \brief Cross correlation */
			std::pair<Point2DInt, double> CrossCorrelation(const MatrixComplex &other, const std::complex<double> &fill1 = std::complex<double>(0, 0), const std::complex<double> &fill2 = std::complex<double>(0, 0));

			CRN_DECLARE_CLASS_CONSTRUCTOR(MatrixComplex)
	};
	template<> struct IsClonable<MatrixComplex> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(MatrixComplex)

	/*! \brief Fast Fourier transform */
	inline void FFT(MatrixComplex &m, bool direct) { m.FFT(direct); }

	template<> struct TypeInfo<MatrixComplex>
	{
		using SumType = MatrixComplex;
		using DiffType = MatrixComplex;
		using DecimalType = MatrixComplex;
	};
}

#endif

