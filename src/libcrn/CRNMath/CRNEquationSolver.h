/* Copyright 2008-2014 INSA Lyon
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
 * file: CRNEquationSolver.h
 * 
 * \author Jean DUONG
 */


#ifndef CRNEQUATIONSOLVER_HEADER
#define CRNEQUATIONSOLVER_HEADER

#include <CRN.h>
#include <set>

namespace crn
{
	class MatrixDouble;
	class SquareMatrixDouble;

	/****************************************************************************/
	/*! \brief Solvers for systems of linear equations
	 *
	 * Solvers for systems of linear equations
	 * 
	 * \author 	Jean DUONG
	 * \date	March 2013
	 * \version	0.2
	 * \ingroup	math
	 * \todo Cholesky, LU decomposition, etc.
	 */
	namespace LinearSystem
	{	
		/*! \brief Resolution of linear equation system by the determinant method */
		MatrixDouble Cramer(const SquareMatrixDouble &Coefficients, const MatrixDouble &ConstantTerms);
		/*! \brief Resolution of linear equation system by the pivot method */
		MatrixDouble GaussJordan(const SquareMatrixDouble &Coefficients, const MatrixDouble &ConstantTerms);
	}

	/****************************************************************************/
	/*! \brief Solvers for quadratic equations
	 *
	 * Solvers for quadratic equations
	 * 
	 * \author 	Jean DUONG
	 * \date	March 2013
	 * \version	0.2
	 * \ingroup	math
	 */
	namespace QuadraticEquation
	{
		/*! \brief Discriminant of trinom \f$ax^2 + bx + c\f$ */
		double Discriminant(double a, double b, double c) noexcept;
		/*! \brief Real roots of trinom \f$ax^2 + bx + c\f$ */
		std::set<double> RealRoots(double a, double b, double c) noexcept;
	}
}
#endif
