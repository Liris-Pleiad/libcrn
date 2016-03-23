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
 * file: CRNEquationSolver.cpp
 * 
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNMath/CRNEquationSolver.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNException.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h> // Always last of includes !!!

using namespace crn;

/*!
 * Solve linear system using Cramer procedure
 *
 * \throw	ExceptionDimension	incompatible matrix dimensions
 * \throw ExceptionRuntime	Equation has either no solution or an infinity of solutions
 *
 * \param[in]	Coefficients	matrix of equations' coefficients
 * \param[in]	ConstantTerms	column matrix of constant term
 *
 * \return	solutions packed in a column matrix (SMatrixDouble)
 */
MatrixDouble LinearSystem::Cramer(const SquareMatrixDouble &Coefficients, const MatrixDouble &ConstantTerms)
{
	size_t n = Coefficients.GetRows();
	
	if ((ConstantTerms.GetRows() != n) || (ConstantTerms.GetCols() != 1))
	{
		throw ExceptionDimension(StringUTF8("LinearEquationsSystem::CramerSolver("
					"const SquareMatrixDouble *Coefficients, const MatrixDouble *ConstantTerms): ") + 
				_("invalid or incompatible matrix dimensions"));
	}
	else
	{
		double D = Coefficients.Determinant();
		
		if (D == 0.0)
		{
			throw ExceptionRuntime(_("Equation has either no solution or an infinity of solutions."));
		}
		
		MatrixDouble Solutions(n, 1, 0.0);
		
		for (size_t k = 0; k < n; k++)
		{
			SquareMatrixDouble Mk(Coefficients);
			
			for (size_t r = 0; r < n; r++)
			{
				Mk.At(r, k) = ConstantTerms.At(r, 0);
			}
			
			double Dk = Mk.Determinant();
			
			Solutions.At(k, 0) = Dk / D;
		}
		
		return Solutions;
	}
}

/*!
 * Solve linear system using Gauss-Jordan procedure
 *
 * \throw	ExceptionDimension	incompatible matrix dimensions
 * \throw ExceptionRuntime	Equation has either no solution or an infinity of solutions
 *
 * \param[in]	Coefficients	matrix of equations' coefficients
 * \param[in]	ConstantTerms	column matrix of constant terms
 *
 * \return	solutions packed in a column matrix (SMatrixDouble)
 */
MatrixDouble LinearSystem::GaussJordan(const SquareMatrixDouble &Coefficients, const MatrixDouble &ConstantTerms)
{
	size_t n = Coefficients.GetRows();
		
	if (ConstantTerms.GetRows() != n)
	{
		throw ExceptionDimension(StringUTF8("LinearEquationsSystem::GaussJordanSolver("
					"const SquareMatrixDouble *Coefficients, const MatrixDouble *ConstantTerms): ") + 
				_("invalid or incompatible matrix dimensions"));
	}
	else
	{
		USquareMatrixDouble CopyCoefficients = CloneAs<SquareMatrixDouble>(Coefficients);
		UMatrixDouble CopyConstantTerms = CloneAs<MatrixDouble>(ConstantTerms);
	
		for (size_t c = 0; c < n - 1; c++)
		{
			// Search the greatest pivot in column
				
			double Pivot = CopyCoefficients->At(c, c);
			double AbsMaxPivot = fabs(Pivot);
			size_t RowIndex = c;
				
			for (size_t r = c + 1 ; r < n; r++)
			{
				double Candidate = CopyCoefficients->At(r, c);
					
				if (fabs(Candidate) > AbsMaxPivot)
				{
					Pivot = Candidate;
					AbsMaxPivot = fabs(Pivot);
					RowIndex = r;
				}
			}
			
			// If no non-null pivot found, system may have infinite number of solutions
				
			if (Pivot == 0.0)
			{
				throw ExceptionRuntime(_("Equation has either no solution or an infinity of solutions."));
			}
			
			if (RowIndex != c)
			{
				CopyCoefficients->SwapRows(c, RowIndex);
				CopyConstantTerms->SwapRows(c, RowIndex);
			}
				// Elimination
			
			for (size_t r = c + 1; r < n; r++)
			{
				double Coeff = CopyCoefficients->At(r, c);
				
				if (Coeff != 0.0)
				{
					double Scale = - Coeff / Pivot;
					
					for (size_t k = c; k < n; k++)
					{
						CopyCoefficients->IncreaseElement(r, k, CopyCoefficients->At(c, k) * Scale);
					}
					
					CopyConstantTerms->IncreaseElement(r, 0, CopyConstantTerms->At(c, 0) * Scale);
				}
			}
		}
		// End of loop for column
			
		MatrixDouble Solutions(n, 1, 0.0);
			
		Solutions.At(n - 1, 0) = CopyConstantTerms->At(n - 1, 0) / CopyCoefficients->At(n - 1, n - 1);
			
		for (auto r = int(n) - 2; r >= 0; --r)
		{
			double Cumul = 0.0;
			
			for (auto c = int(n) - 1; c > r; --c)
			{
				Cumul += CopyCoefficients->At(r, c) * Solutions.At(c, 0);
			}
				
			Solutions.At(r, 0) = (CopyConstantTerms->At(r, 0) - Cumul) / CopyCoefficients->At(r, r);
		}
			
		return Solutions;
	}
}

/*****************************************************************************/
/*! Discriminant of trinom \f$ax^2 + bx + c\f$
 *
 * \param[in]	a	coefficient for 2nd degree monom
 * \param[in]	b	coefficient for 1st degree monom
 * \param[in]	c	coefficient for null degree monom
 *
 * \return	discriminant value of trinom
 */
double QuadraticEquation::Discriminant(double a, double b, double c) noexcept
{
	return (b * b - 4.0 * a * c);
}

/*****************************************************************************/
/*! Real roots of trinom \f$ax^2 + bx + c\f$
 *
 * \param[in]	a	coefficient for 2nd degree monom
 * \param[in]	b	coefficient for 1st degree monom
 * \param[in]	c	coefficient for null degree monom
 *
 * \return	roots of trinom (stored in std::set)
 */
std::set<double> QuadraticEquation::RealRoots(double a, double b, double c) noexcept
{	
	double Delta = Discriminant(a, b, c);
	
	std::set<double> roots;

	if (a == 0.0)
	{
		roots.insert( - c / b);
	}
	else
	{
		double Inv2a = 1.0 / (2.0 * a);

		if (Delta == 0.0)
		{			
			roots.insert(- b * Inv2a);
		}
		else
		{
			if (Delta > 0.0)
			{
				double SqrtDelta = sqrt(Delta);
				
				roots.insert((- b - SqrtDelta) * Inv2a);
				roots.insert((- b + SqrtDelta) * Inv2a);
			}
		}
	}
	
	return roots;
}
