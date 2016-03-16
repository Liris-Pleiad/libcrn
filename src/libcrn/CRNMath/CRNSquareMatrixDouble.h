/* Copyright 2008-2016 INSA-Lyon, ENS-Lyon
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
 * file: CRNSquareMatrixDouble.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNSQUAREMATRIXDOUBLE_HEADER
#define CRNSQUAREMATRIXDOUBLE_HEADER

#include <CRNMath/CRNMatrixDouble.h>
#include <map>

namespace crn
{
	/****************************************************************************/
	/*! \brief Square double matrix class
	 *
	 * Square matrix containing doubles
	 * 
	 * \author 	Jean DUONG
	 * \date	February 2008
	 * \version	0.2
	 * \ingroup math
	 */
	class SquareMatrixDouble: public MatrixDouble
	{	
		public:
			/*! \brief Constructor */
			SquareMatrixDouble(size_t size, double value = 0.0) : MatrixDouble(size, size, value) {}
			SquareMatrixDouble(const Matrix<double> &m);
			SquareMatrixDouble(Matrix<double> &&m);
			SquareMatrixDouble(const std::vector<std::vector<double>> &m);
			SquareMatrixDouble(std::vector<std::vector<double>> &&m);
			SquareMatrixDouble(const SquareMatrixDouble &) = default;
			SquareMatrixDouble(SquareMatrixDouble &&) = default;
			
			/*! \brief Destructor */
			virtual ~SquareMatrixDouble() override {}
			
			SquareMatrixDouble& operator=(const SquareMatrixDouble &) = default;
			SquareMatrixDouble& operator=(SquareMatrixDouble &&) = default;

			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const override { static const String cn(U"SquareMatrixDouble"); return cn; }
			
			virtual UObject Clone() const override { return std::make_unique<SquareMatrixDouble>(*this); }
			
			/*********************************************************************/
			/* Special matrix constructors                                       */
			/*********************************************************************/
			/*! \brief Create Gaussian matrix */		
			static SquareMatrixDouble NewGaussian(double sigma);
			/*! \brief Create Gaussian Sobel X derivation mask */		
			static SquareMatrixDouble NewGaussianSobelX(double sigma);
			/*! \brief Create Gaussian Sobel Y derivation mask */		
			static SquareMatrixDouble NewGaussianSobelY(double sigma);
			/*! \brief Create identity matrix */		
			static SquareMatrixDouble NewIdentity(size_t n);

			/*! \brief Check if matrix is upper triangular */
			bool IsUpperTriangular() const;
			/*! \brief Check if matrix is lower triangular */
			bool IsLowerTriangular() const;
            /*! \brief Check if matrix is upper Hessenberg */
            bool IsUpperHessenberg() const;
			/*! \brief Check if matrix is diagonal */
			bool IsDiagonal() const;
			/*! \brief Trace */
			double Trace() const;
			/*! \brief Diagonal product */
			double DiagonalProduct() const;
			/*! \brief Transposition */
			virtual SquareMatrixDouble& Transpose() override;
			/*! \brief Minor matrix */
			SquareMatrixDouble MakeMinor(size_t r, size_t c) const;
			/*! \brief Cofactor */
			double Cofactor(size_t r, size_t c) const;
			/*! \brief Determinant */		
			double Determinant() const;
			/*! \brief Invert using determinants */
			SquareMatrixDouble MakeInverse() const;
			/*! \brief Invert using Gauss-Jordan elimination */
			SquareMatrixDouble MakeGaussJordanInverse() const;
			/*! \brief Get the lower triangular factor in Cholesky decomposition */
			SquareMatrixDouble MakeCholesky() const;
            /*! \brief Get the upper Hessenberg form of matrix */
            SquareMatrixDouble MakeUpperHessenberg() const;
			
			/*! \brief Perform diagonalization for 2x2 symmetric matrix */
			std::multimap<double, MatrixDouble> MakeSpectralEigensystem() const;
			/*! \brief Perform diagonalization for symmetric matrix */
			std::multimap<double, MatrixDouble> MakeJacobiEigensystem(size_t MaxIteration = 100) const;
			/*! \brief Perform diagonalization for positive symmetric matrix */
			std::multimap<double, MatrixDouble> MakeTQLIEigensystem(size_t maxiter = 30) const;
			/*! \brief Extract eigenvalues for matrix of real (eigenvalues may be complex) */
			std::vector<std::complex<double>> Eigenvalues(size_t max_iter = 30) const;
			
		private:
			/*! \brief Tridiagonalization */
			void tred2(SquareMatrixDouble &z, std::vector<double> &diag, std::vector<double> &offdiag) const;

			CRN_DECLARE_CLASS_CONSTRUCTOR(SquareMatrixDouble)
		public:
			SquareMatrixDouble(xml::Element &el):MatrixDouble(1, 1) { Deserialize(el); }
	};
	namespace protocol
	{
		template<> struct IsSerializable<SquareMatrixDouble> : public std::true_type {};
	}

	template<> struct TypeInfo<SquareMatrixDouble>
	{
		using SumType = SquareMatrixDouble;
		using DiffType = SquareMatrixDouble;
		using DecimalType = SquareMatrixDouble;
	};

}

#include <CRNMath/CRNSquareMatrixDoublePtr.h>
#endif
