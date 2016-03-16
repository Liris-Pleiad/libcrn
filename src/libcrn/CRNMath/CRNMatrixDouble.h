/* Copyright 2007-2016 Yann LEYDIER, CoReNum, INSA-Lyon, ENS-Lyon
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
 * file: CRNMatrixDouble.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNMATRIXDOUBLE_HEADER
#define CRNMATRIXDOUBLE_HEADER

#include <CRNMath/CRNMatrix.h>
#include <CRNMath/CRNMatrixDoublePtr.h>
#include <CRNMath/CRNMath.h>

namespace crn
{
	class SquareMatrixDouble;

	/*! \brief double matrix class
	 *
	 * Matrix containing doubles
	 * 
	 * \author 	Yann LEYDIER
	 * \date	August 2007
	 * \version	0.2
	 * \ingroup math
	 */
	class MatrixDouble: public Matrix<double>
	{
		public:
			using Matrix<double>::Matrix;
			MatrixDouble(const Matrix<double> &m) : Matrix(m) {}
			MatrixDouble(Matrix<double> &&m) : Matrix(std::move(m)) {}
			
			/*! \brief	Defines a default constructor from xml element */
			MatrixDouble(xml::Element &el) : Matrix(1, 1) { Deserialize(el); }
			
			MatrixDouble(const MatrixDouble &) = default;
			MatrixDouble(MatrixDouble &&) = default;
			
			MatrixDouble& operator=(const MatrixDouble &) = default;
			MatrixDouble& operator=(MatrixDouble &&) = default;
			
			/*! \brief Destructor */
			virtual ~MatrixDouble() override {}

			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const override { static const String cn(U"MatrixDouble"); return cn; }
			/*! \brief Returns the protocols of the class: ComplexObject, VectorOverR, Clonable, Ring, Serializable and Metric */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::VectorOverR | crn::Protocol::Clonable | crn::Protocol::Ring | crn::Protocol::Serializable | crn::Protocol::Metric; }

			virtual UObject Clone() const override { return std::make_unique<MatrixDouble>(*this); }
	
			/*! \brief Computes the sum of the squared elements */
			double CumulateSquaredCells() const;

			/*! \brief Creates a line matrix with a centered Gaussian */
			static MatrixDouble NewGaussianLine(double sigma);
			/*! \brief Creates a line matrix with the derivative of a centered Gaussian */
			static MatrixDouble NewGaussianLineDerivative(double sigma);
			/*! \brief Creates a line matrix with the second derivative of a centered Gaussian */
			static MatrixDouble NewGaussianLineSecondDerivative(double sigma);
			/*! \brief Get the product of a column vector by its own transposed on the right side */
			SquareMatrixDouble MakeVectorRightAutoProduct() const;

		private:
			void deserialize(xml::Element &el) override;
			xml::Element serialize(xml::Element &parent) const override;

		protected:
			CRN_DECLARE_CLASS_CONSTRUCTOR(MatrixDouble)
	};
	namespace protocol
	{
		template<> struct IsSerializable<MatrixDouble> : public std::true_type {};
	}

	template<> struct TypeInfo<MatrixDouble>
	{
		using SumType = MatrixDouble;
		using DiffType = MatrixDouble;
		using DecimalType = MatrixDouble;
	};


}

#endif
