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
 * file: CRNSquareMatrixDouble.cpp
 * 
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNException.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNMath/CRNMath.h>
#include <CRNMath/CRNEquationSolver.h>
#include <CRNData/CRNDataFactory.h>

#include <stdio.h>
#include <iostream>
#include <limits>
#include <CRNi18n.h>

using namespace crn;

SquareMatrixDouble::SquareMatrixDouble(const Matrix<double> &m):
	MatrixDouble(m)
{
	if (rows != cols)
		throw ExceptionDimension(StringUTF8("SquareMatrixDouble::SquareMatrixDouble(const Matrix<double> &m): ") + _("the matrix is not square."));
}

SquareMatrixDouble::SquareMatrixDouble(Matrix<double> &&m):
	MatrixDouble(std::move(m))
{
	if (rows != cols)
		throw ExceptionDimension(StringUTF8("SquareMatrixDouble::SquareMatrixDouble(const Matrix<double> &m): ") + _("the matrix is not square."));
}

SquareMatrixDouble::SquareMatrixDouble(const std::vector<std::vector<double>> &m):
	MatrixDouble(m)
{
	if (rows != cols)
		throw ExceptionDimension(StringUTF8("SquareMatrixDouble::SquareMatrixDouble(const std::vector<std::vector<double>> &m): ") + _("the matrix is not square."));
}

SquareMatrixDouble::SquareMatrixDouble(std::vector<std::vector<double>> &&m):
	MatrixDouble(std::move(m))
{
	if (rows != cols)
		throw ExceptionDimension(StringUTF8("SquareMatrixDouble::SquareMatrixDouble(const std::vector<std::vector<double>> &m): ") + _("the matrix is not square."));
}

// Special matrix constructors
#define MAX_GAUSS_W 40
#define MULT 1

/*! 
 * Create Gaussian matrix
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
SquareMatrixDouble SquareMatrixDouble::NewGaussian(double sigma)
{
	if (sigma == 0)
		return SquareMatrixDouble(1, 1);

	size_t d = MAX_GAUSS_W;
	// compute the width of the matrix
	for (size_t tmp = 1; tmp < MAX_GAUSS_W; ++tmp)
		if (Gauss(double(tmp), sigma) < 0.1)
		{
			d = tmp;
			break;
		}
	SquareMatrixDouble mat(d + d + 1);
	
	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = MULT * MeanGauss(double(d) - double(r), sigma);
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = MULT * MeanGauss(double(r) - double(d), sigma);
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = MULT * MeanGauss(double(d) - double(c), sigma);
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = MULT * MeanGauss(double(c) - double(d), sigma);
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;

	return mat;
}

/*! 
 * Create Gaussian X derivation Sobel mask
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
SquareMatrixDouble SquareMatrixDouble::NewGaussianSobelX(double sigma)
{
	if (sigma == 0)
	{
		SquareMatrixDouble m(3, 0);
		m[1][0] = 1;
		m[1][2] = -1;
		return m;
	}

	size_t d = MAX_GAUSS_W;
	// compute the width of the matrix
	for (size_t tmp = 1; tmp < MAX_GAUSS_W; tmp++)
		if (Gauss(double(tmp), sigma) < 0.1)
		{
			d = tmp;
			break;
		}
    
	SquareMatrixDouble mat(d + d + 1);
	
	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = MULT * MeanGauss(double(d) - double(r), sigma);
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = MULT * MeanGauss(double(r) - double(d), sigma);
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = MULT * MeanGauss(double(d) - double(c), sigma);
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = MULT * MeanGauss(double(c) - double(d), sigma);
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;

	// derivate
	for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.cols; c++)
		{
			mat[r][c] *= double(d) - double(c);
			mat[r][c] /= double(d);
		}

	return mat;
}

/*! 
 * Create Gaussian Y derivation Sobel mask
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	a new matrix
 */
SquareMatrixDouble SquareMatrixDouble::NewGaussianSobelY(double sigma)
{
	if (sigma == 0)
	{
		SquareMatrixDouble m(3, 0);
		m[0][1] = 1;
		m[2][1] = -1;
		return m;
	}

	size_t d = MAX_GAUSS_W;
	// compute the width of the matrix
	for (size_t tmp = 1; tmp < MAX_GAUSS_W; tmp++)
		if (Gauss(double(tmp), sigma) < 0.1)
		{
			d = tmp;
			break;
		}
	SquareMatrixDouble mat(d + d + 1);

	// fill central column
	for (size_t r = 0; r < d; r++)
		mat[r][d] = MULT * MeanGauss(double(d) - double(r), sigma);
	for (size_t r = d; r < mat.rows; r++)
		mat[r][d] = MULT * MeanGauss(double(r) - double(d), sigma);
	// fill central row
	for (size_t c = 0; c < d; c++)
		mat[d][c] = MULT * MeanGauss(double(d) - double(c), sigma);
	for (size_t c = d; c < mat.cols; c++)
		mat[d][c] = MULT * MeanGauss(double(c) - double(d), sigma);
	// fill first quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill second quadrant
	for (size_t r = 0; r < d; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill third quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = 0; c < d; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;
	// fill fourth quadrant
	for (size_t r = d + 1; r < mat.rows; r++)
		for (size_t c = d + 1; c < mat.cols; c++)
			mat[r][c] = mat[r][d] * mat[d][c] / MULT;

	// derivate
	for (size_t r = 0; r < mat.rows; r++)
		for (size_t c = 0; c < mat.cols; c++)
		{
			mat[r][c] *= double(d) - double(r);
			mat[r][c] /= double(d);
		}

	return mat;
}

/*! 
 * Check if matrix is upper triangular
 *
 * \return	true if the matrix is upper triangular, false else
 */
bool SquareMatrixDouble::IsUpperTriangular() const
{
	bool Status = true;
	size_t r = 1;
	
	while ((Status) && (r < rows))
	{
		size_t c = 0;
		
		while ((Status) && (c < r))
		{
			Status = (At(r, c) == 0.0);
			++c;
		}
		
		++r;
	}
	
	return Status;
}

/*! 
 * Check if matrix is lower triangular
 *
 * \return	true if the matrix is lower triangular
 */
bool SquareMatrixDouble::IsLowerTriangular() const
{
	bool Status = true;
	size_t r = 0;
	
	while ((Status) && (r < rows - 1))
	{
		size_t c = r + 1;
		
		while ((Status) && (c < cols))
		{
			Status = (At(r, c) == 0.0);
			++c;
		}
		
		++r;
	}
	
	return Status;
}

/*!
 * Check if matrix is upper Hessenberg
 *
 * \return	true if the matrix has upper Hessenberg form, false else
 */
bool SquareMatrixDouble::IsUpperHessenberg() const
{
    auto status = true;
    auto c = 0;
    
    while (status && (c < int(cols) - 2))
    {
        auto r = c + 2;
        
        while (status && (r < int(rows)))
        {
            status = status && !At(r, c);
            ++r;
        }
        
        ++c;
    }
    
    return status;
}

/*! 
 * Check if matrix is diagonal
 *
 * \return	true if the matrix is diagonal
 */
bool SquareMatrixDouble::IsDiagonal() const
{
	bool Status = true;
	size_t r = 0;
	
	while ((Status) && (r < rows - 1))
	{
		size_t c = r + 1;
		
		while ((Status) && (c < cols))
		{
			Status = (At(r, c) == 0.0) && (At(c, r) == 0.0);
			++c;
		}
		
		++r;
	}
	
	return Status;
}

/*! 
 * Sum of the elements on the diagonal
 *
 * \return	the trace of the matrix
 */
double SquareMatrixDouble::Trace() const
{
	double sum = 0;
	
	for (size_t r = 0; r < rows; ++r)
		sum += At(r, r);
	
	return sum;
}

/*! 
 * Product of the elements on the diagonal
 *
 * \return	the diagonal product
 */
double SquareMatrixDouble::DiagonalProduct() const
{
	double P = 1.0;
	
	for (size_t r = 0; r < rows; ++r)
		P *= At(r, r);
	
	return P;
}

/*! 
 * Transposes the matrix
 */
SquareMatrixDouble& SquareMatrixDouble::Transpose()
{
	for (size_t l = 0; l < rows - 1; ++l)
		for (size_t c = l + 1; c < cols; ++c)
			std::swap(At(l, c), At(c, l));
	return *this;
}

/*! 
 * Create minor matrix
 *
 * \throws	ExceptionDomain	row or column out of range
 *
 * \param[in]	r	row index
 * \param[in]	c	column index
 *
 * \return	a new matrix, less the column and row
 */
SquareMatrixDouble SquareMatrixDouble::MakeMinor(size_t r, size_t c) const
{
	if ((r >= rows) || (c >= cols))
		throw ExceptionDomain(StringUTF8("SquareMatrixDouble::MakeMinor(size_t r, size_t c): ") + 
				_("row or column index out of range"));

	SquareMatrixDouble M(rows - 1);

	size_t i = 0;	

	for (size_t rindex = 0; rindex < rows; ++rindex)
	{
		if (rindex != r)
		{
			size_t j = 0;

			for (size_t cindex = 0; cindex < cols; ++cindex)
			{
				if (cindex != c)
				{
					M[i][j] = At(rindex, cindex);
					++j;
				}
			}

			++i;
		}
	}

	return M;
}

/*! 
 * Cofactor
 *
 * \throws	ExceptionDomain	row or column out of range
 *
 * \param[in]	r	row index
 * \param[in]	c	column index
 *
 * \return	the cofactor at the given row and column
 */
double SquareMatrixDouble::Cofactor(size_t r, size_t c) const
{	
	if ((r >= rows) || (c >= cols))
		throw ExceptionDomain(StringUTF8("SquareMatrixDouble::Cofactor(size_t r, size_t c): ") + 
				_("row or column index out of range"));

	SquareMatrixDouble M = MakeMinor(r, c);
	double d = M.Determinant();

	if ((r + c) % 2 == 1) // Parity test
		d = -d;

	return d;
}

/*! 
 * Determinant
 *
 * \return	the determinant of the matrix
 */
double SquareMatrixDouble::Determinant() const
{	
	// Case 1x1 matrix
	if (rows == 1)
		return At(0, 0);

	// Case 2x2 matrix
	if (rows == 2)
		return At(0, 0) * At(1, 1) - At(0, 1) * At(1, 0);

	// Case 3x3 matrix (Sarrus rule)
	if (rows == 3)
	{
		double det = 0.0;

		det += At(0, 0) * At(1, 1) * At(2, 2);
		det += At(1, 0) * At(2, 1) * At(0, 2);
		det += At(2, 0) * At(0, 1) * At(1, 2);
		det -= At(0, 2) * At(1, 1) * At(2, 0);
		det -= At(1, 2) * At(2, 1) * At(0, 0);
		det -= At(2, 2) * At(0, 1) * At(1, 0);

		return det;
	}

	// Case triangular matrices
	if (IsUpperTriangular() || IsLowerTriangular())
		return DiagonalProduct();

	//////////////////
	// General case //
	//////////////////

	double det = 0.0;

	size_t r = 0;
	size_t c = 0;
	size_t MaxNullInRow = 0;
	size_t MaxNullInColumn = 0;					

	// Search most sparse row (r) and most sparse column (c)

	for (size_t k = 0; k < rows; ++k)
	{
		size_t nr = CountNullCellsInRow(k);
		size_t nc = CountNullCellsInColumn(k);

		if (nr > MaxNullInRow)
		{
			MaxNullInRow = nr;
			r = k;
		}

		if (nc > MaxNullInColumn)
		{
			MaxNullInColumn = nc;
			c = k;
		}
	}

	// Expansion by minors

	if (MaxNullInRow > MaxNullInColumn)
	{		
		for (size_t k = 0; k < cols; ++k)
		{
			double Erk = At(r, k);

			if (Erk != 0.0)
				det += Erk * Cofactor(r, k);
		}
	}
	else
	{
		for (size_t k = 0; k < rows; k++)
		{
			double Ekc = At(k, c);

			if (Ekc != 0.0)
				det += Ekc * Cofactor(k, c);
		}
	}

	return det;
}

/*! 
 * Inversion using determinants computation
 *
 * \return	the inverse matrix
 */
SquareMatrixDouble SquareMatrixDouble::MakeInverse() const
{
	double dt = Determinant();

	if (dt != 0.0)
	{				
		SquareMatrixDouble C(rows);

		for (size_t r = 0; r < rows; ++r)
			for (size_t c = 0; c < cols; ++c)
				C[r][c] = Cofactor(r, c);

		C.Transpose();
		C *= 1.0 / dt;

		return C;
	}

	throw ExceptionRuntime(_("The matrix cannot be inversed."));
}

/*! 
 * Make identity matrix
 *
 * \param[in]	n	the dimension
 *
 * \return	a new matrix
 */
SquareMatrixDouble SquareMatrixDouble::NewIdentity(size_t n)
{
	SquareMatrixDouble id(n);

	for (size_t r = 0; r < n; ++r)
		for (size_t c = 0; c < n; ++c)
		{
			if (r == c)
				id[r][c] = 1.0;
			else
				id[r][c] = 0.0;
		}

	return id;
}

/*! 
 * Inversion using Gauss-Jordan elimination
 *
 * \throws	ExceptionRuntime	matrix cannot be inversed
 *
 * \return	the inverse matrix
 */
SquareMatrixDouble SquareMatrixDouble::MakeGaussJordanInverse() const
{
	size_t n = rows;
	SquareMatrixDouble Id = NewIdentity(n);
	SquareMatrixDouble M = *this;

	// Transform this matrix to triangular matrix

	for (size_t c = 0; c < n - 1; ++c)
	{
		// Search the greatest pivot in column

		double Pivot = M[c][c];
		double AbsMaxPivot = fabs(Pivot);
		size_t RowIndex = c;

		for (size_t r = c + 1 ; r < n; ++r)
		{
			double Candidate = M[r][c];

			if (fabs(Candidate) > AbsMaxPivot)
			{
				Pivot = Candidate;
				AbsMaxPivot = fabs(Pivot);
				RowIndex = r;
			}
		}

		// If no non-null pivot found, matrix is non inversible

		if (Pivot == 0.0)
			throw ExceptionRuntime(crn::StringUTF8(_("Matrix cannot be inversed. Null pivot at column: ")) + c);

		if (RowIndex != c)
		{
			M.SwapRows(c, RowIndex);
			Id.SwapRows(c, RowIndex);
		}

		// Elimination

		for (size_t r = c + 1; r < n; ++r)
		{
			double Coeff = M[r][c];

			if (Coeff != 0.0)
			{
				double Scale = - Coeff / Pivot;

				for (size_t k = 0; k < n; ++k)
				{
					M[r][k] += M[c][k] * Scale;
					Id[r][k] += Id[c][k] * Scale;
				}					
			}
		}
	}

	// Transform this matrix to diagonal

	for (int c = int(n) - 1; c > 0; --c)
	{
		double DCoeff = M[c][c];

		if (DCoeff != 0.0)
		{
			for (int r = c - 1; r >= 0; --r)
			{
				double Coeff = M[r][c];

				if (Coeff != 0.0)
				{
					double Scale = - Coeff / DCoeff;

					for (size_t k = 0; k < n; ++k)
					{
						Id[r][k] += Id[c][k] * Scale;
						M[r][k] += M[c][k] * Scale;
					}
				}
			}
		}
	}

	// Transform to id

	for (size_t r = 0; r < n; r++)
		Id.MultRow(r, 1.0 / M[r][r]);

	return Id;
}

/*! 
 * Get the lower triangular factor in Cholesky decomposition.
 * Matrix is supposed symetric semi-definite positive.
 *
 * Property : M = L * transpose(L) with
 *
 * M : the input matrix
 * L : the lower triangular factor found
 *  
 * \return	the lower triangular factor in Cholesky decomposition
 */
SquareMatrixDouble SquareMatrixDouble::MakeCholesky() const
{
	size_t n = GetRows();
	SquareMatrixDouble L(n, 0.0);

	L[0][0] = sqrt(At(0, 0));

	if (n > 2)
	{
		for (int j = 0; j < int(n); ++j)
		{
			auto Sum = 0.0;

			for (int k = 0; k <= j - 1; ++k)
				Sum += Sqr(L[j][k]);

			L[j][j] = sqrt(At(j, j) - Sum);

			for (int i = j + 1 ; i < int(n); ++i)
			{
				Sum = 0.0;

				for (int k = 0; k <= j - 1; ++k)
					Sum += L[i][k] * L[j][k];

				L[i][j] = (At(i, j) - Sum) / L[j][j];
			}
		}
	}
	else
	{
		L[1][0] = At(0, 1) / L[0][0];
		L[1][1] = sqrt(At(1, 1) - Sqr(At(0, 1)) / At(0, 0));
	}

	return L;
}

/*!
 * Get the upper Hessenberg form of matrix.
 *
 * \return	the upper Hessenberg form of matrix
 */
SquareMatrixDouble SquareMatrixDouble::MakeUpperHessenberg() const
{
    const auto n = Min(int(rows), int(cols)); // Vector space dimension
    auto hess_mat(*this);
    
    std::vector<std::vector<double>> reflector_seeds;
    
    reflector_seeds.reserve(n - 2);
    
    for (auto k = 0; k <= n - 3; ++k)
    {
        // Generate the Househoder reflector Pk
        std::vector<double> u_k;
        
        u_k.reserve(n - k);
        
        for (auto i = k + 1; i < n; ++i)
            u_k.push_back(hess_mat.At(i, k));
        
        u_k[0] += SignOf(u_k[0]) * Pythagoras(u_k.begin(), u_k.end());
        Scale(u_k.begin(), u_k.end(), 1.0 / Pythagoras(u_k.begin(), u_k.end()));
        reflector_seeds.push_back(u_k);
        
        // Left product by P_k
        for (auto j = k; j < n; ++j)
        {
            auto scale_current_column = 0.0;
            auto it = u_k.begin();
            
            for (auto i = k + 1; i < n; ++i)
            {
                scale_current_column += (*it) * hess_mat.At(i, j);
                ++it;
            }
            
            it = u_k.begin();
            
            for (auto i = k + 1; i < n; ++i)
            {
                hess_mat.At(i, j) -= 2 * (*it) * scale_current_column;
                ++it;
            }
        }
        
        // Right product by P_k
        for (auto i = 0; i < n; ++i)
        {
            auto scale_current_row = 0.0;
            auto it = u_k.begin();
            
            for (auto j = k + 1; j < n; ++j)
            {
                scale_current_row += hess_mat.At(i, j) * (*it);
                ++it;
            }
            
            it = u_k.begin();
            
            for (auto j = k + 1; j < n; ++j)
            {
                hess_mat.At(i, j) -= 2 * scale_current_row * (*it);
                ++it;
            }
        }
    }
    
    return hess_mat;
}


/*!
 * 2x2 symetric matrix diagonalization using spectral theorem
 * 
 * Eigenpairs are returned in a multimap
 * sorted according decreasing eigenvalues.
 * 
 * \return eigenpairs
 */
std::multimap<double, MatrixDouble> SquareMatrixDouble::MakeSpectralEigensystem() const
{
	std::multimap<double, MatrixDouble> eigen_pairs;

	auto g_1 = At(0, 0);
	auto g_2 = At(0, 1);
	auto g_3 = At(1, 1);

	MatrixDouble eigen_vector_1((size_t)2, (size_t)1);
	MatrixDouble eigen_vector_2((size_t)2, (size_t)1);

	if (g_2 == 0.0) // Trivial case (matrix already diagonal)
	{
		eigen_vector_1[0][0] = 1.0;
		eigen_vector_1[1][0] = 0;
		eigen_vector_2[0][0] = 0.0;
		eigen_vector_2[1][0] = 1.0;

		eigen_pairs.insert(std::make_pair(g_1, eigen_vector_1));
		eigen_pairs.insert(std::make_pair(g_3, eigen_vector_2));
	}
	else
	{
		auto delta = Sqr(g_1 - g_3) + 4 * Sqr(g_2);
		auto sqrt_delta = sqrt(delta);

		auto eigen_value_1 = (g_1 + g_3 + sqrt_delta) / 2.0;
		auto eigen_value_2 = (g_1 + g_3 - sqrt_delta) / 2.0;

		eigen_vector_1[0][0] = 1.0;
		eigen_vector_1[1][0] = (eigen_value_1 - g_1) / g_2;

		// For symetric matrix, eigenvectors are orthogonal: (u, v) and (-v, u)

		eigen_vector_2[0][0] = - eigen_vector_1[1][0];
		eigen_vector_2[1][0] = eigen_vector_1[0][0];

		eigen_pairs.insert(std::make_pair(eigen_value_1, eigen_vector_1));
		eigen_pairs.insert(std::make_pair(eigen_value_2, eigen_vector_2));
	}

	return eigen_pairs;
}

/*!
 * Jacobi method to perform matrix diagonalization
 * 
 * Assumptions :
 * 
 * 1 - Matrix is symetric
 * 2 - Matrix has n distinct eigenvalues
 *     (n is the dimension of vector space)
 * 2 - n > 2
 * 
 * Eigenpairs are returned in a multimap
 * sorted according eigenvalues.
 * 
 * \return eigenpairs
 */
std::multimap<double, MatrixDouble> SquareMatrixDouble::MakeJacobiEigensystem(size_t MaxIteration) const
{	
	size_t MaxIter = 100;

	size_t i, j, k, n = GetRows();

	SquareMatrixDouble Eigenvectors(n, 0.0);
	SquareMatrixDouble A = *this;

	for (k = 0; k < n; k++)
		Eigenvectors[k][k] = 1.0;

	k = 0;
	bool Loop = true;

	// Some variable matrices.

	SquareMatrixDouble Omega(n, 0.0);
	SquareMatrixDouble NewA(n, 0.0);
	SquareMatrixDouble B(n, 0.0);

	//////////
	// Loop //
	//////////

	while ((k < MaxIter) && Loop)
	{
		size_t p = 1, q = 0;

		// Search optimal p and q indices.

		double M = 0.0;

		for (i = 0; i < n; ++i)
			for (j = 0; j < n; ++j)
				if (i != j)
				{
					double a = fabs(A[i][j]);

					if (a > M)
					{
						M = a;
						p = i;
						q = j;
					}
				}

		// Build rotation.

		Omega.SetAll(0.0);

		for (i = 0; i < n; i++)
			Omega[i][i] = 1.0;

		double Xi = (A[q][q] - A[p][p]) / (2.0 * A[p][q]);
		double t = 1.0;

		if (Xi != 0.0)
		{
			std::set<double> Roots = QuadraticEquation::RealRoots(1.0, 2.0 * Xi, -1.0);

			t = Min(*Roots.begin(), *Roots.rbegin());
		}

		double c = 1.0 / sqrt(1 + t * t);
		double s = t / sqrt(1 + t * t);

		Omega[p][p] = c;
		Omega[q][q] = c;
		Omega[p][q] = s;
		Omega[q][p] = -s;

		// Iterate eigenvectors.

		Eigenvectors *= Omega;

		// Iterate A.

		B = A * Omega;
		Omega.Transpose();
		NewA = Omega * B;
		Loop = (NewA != A);
		A = NewA;
		++k;
	}

	///////////////////////
	// End of main loop. //
	///////////////////////

	// Pack pairs of eigenvalues with corresponding eigenvectors
	// in descending order according to eigenvalues.

	std::multimap<double, MatrixDouble> EigenPairs;
	std::vector<bool> Flags(n);

	for (k = 0; k < n; ++k)
		Flags[k] = true;

	for (i = 0; i < n; ++i)
	{	
		double MaxEigenvalue = -1.0; // Eigenvalues are all positive
		size_t Argmax = 0;

		for (j = 0; j < n; ++j)
			if (Flags[j])
			{
				double Value = A[j][j];

				if (Value > MaxEigenvalue)
				{
					MaxEigenvalue = Value;
					Argmax = j;
				}
			}

		Flags[Argmax] = false;

		EigenPairs.insert(std::make_pair(A[Argmax][Argmax], Eigenvectors.MakeColumn(Argmax)));
	}

	return EigenPairs;
}

/*! 
 * Diagonalization for positive symmetric matrix
 * 
 * \throws	ExceptionRuntime	too many iterations
 * \return	eigenpairs
 */
std::multimap<double, MatrixDouble> SquareMatrixDouble::MakeTQLIEigensystem(size_t maxiter) const
{
	SquareMatrixDouble z(1);
	std::vector<double> eigenvalues, offdiag;
	tred2(z, eigenvalues, offdiag);

	// tqli
	for (int i = 1; i < int(rows); ++i) // renumber the off-diagonal
		offdiag[i - 1] = offdiag[i];
	offdiag[rows - 1] = 0.0;
	for (int l = 0; l < int(rows); ++l) 
	{
		size_t iter = 0;
		int m;
		do 
		{
			for (m = l; m < int(rows) - 1; ++m) 
			{ // look for a single small subdiagonal element to split the matrix
				double dd = Abs(eigenvalues[m]) + Abs(eigenvalues[m + 1]);
				if (Abs(offdiag[m]) <= std::numeric_limits<double>::epsilon() * dd)
					break;
			}
			if (m != l) 
			{
				if (iter++ >= maxiter)
					throw ExceptionRuntime(StringUTF8("std::multimap<double, SMatrixDouble> SquareMatrixDouble::MakeTQLIEigensystem(size_t maxiter) const: ") + _("Too many iterations."));
				double g = (eigenvalues[l + 1] - eigenvalues[l]) / (2.0 * offdiag[l]);
				double r = Pythagoras(g, 1.0);
				g = eigenvalues[m] - eigenvalues[l] + offdiag[l] / (g + ((g < 0) ? -Abs(r) : Abs(r)));
				double s = 1.0, c = 1.0, p = 0.0;
				int i;
				for (i = m - 1; i >= l; --i) 
				{ // plane rotation followed by Givens rotations to restore tridiagonal form
					double f = s * offdiag[i];
					double b = c * offdiag[i];
					r = Pythagoras(f, g);
					offdiag[i + 1] = r;
					if (r == 0.0) 
					{ // recover from underflow
						eigenvalues[i + 1] -= p;
						offdiag[m] = 0.0;
						break;
					}
					s = f / r;
					c = g / r;
					g = eigenvalues[i + 1] - p;
					r = (eigenvalues[i] - g) * s + 2.0 * c * b;
					p = s * r;
					eigenvalues[i + 1] = g + p;
					g = c * r - b;
					for (int k = 0; k < int(rows); ++k) 
					{
						f = z[k][i + 1];
						z[k][i + 1] = s * z.At(k, i) + c * f;
						z.At(k, i) = c * z.At(k, i) - s * f;
					}
				}
				if ((r == 0.0) && (i >= l))
					continue;
				eigenvalues[l] -= p;
				offdiag[l] = g;
				offdiag[m] = 0.0;
			}
		} while (m != l);
	}

	// export results
	std::multimap<double, MatrixDouble> out;
	for (size_t tmp = 0; tmp < eigenvalues.size(); ++tmp)
	{
		MatrixDouble ev(rows, 1, 0.0);
		for (size_t r = 0; r < rows; ++r)
			ev[r][0] = z[r][tmp];
		out.insert(std::make_pair(eigenvalues[tmp], ev));
	}
	return out;
}

/*! Tridiagonalization
 * \param[out]	z	the transform matrix
 * \param[out]	diag	the diagonal
 * \param[out]	offdiag	the off-diagonal
 */
void SquareMatrixDouble::tred2(SquareMatrixDouble &z, std::vector<double> &diag, std::vector<double> &offdiag) const
{
	// initialize output variables
	z = *this;
	std::vector<double>(rows).swap(diag);
	std::vector<double>(rows).swap(offdiag);

	for (int i = int(rows) - 1; i > 0; --i)
	{
		int l = i - 1;
		double h = 0.0;
		if (l > 0)
		{
			double scale = 0.0;
			for (int k = 0; k < i; ++k)
				scale += Abs(z.At(i, k));
			if (scale == 0.0) // skip transformation
				offdiag[i] = z.At(i, l);
			else
			{
				for (int k = 0; k < i; ++k)
				{
					z.At(i, k) /= scale;
					h += Sqr(z.At(i, k));
				}
				double f = z.At(i, l);
				double g = (f > 0.0) ? -sqrt(h) : sqrt(h);
				offdiag[i] = scale * g;
				h -= f * g;
				z.At(i, l) = f - g;
				f = 0.0;
				for (int j = 0; j < i; ++j)
				{
					z.At(j, i) = z.At(i, j) / h;
					g = 0.0;
					for (int k = 0; k < j + 1; ++k)
						g += z.At(j, k) * z.At(i, k);
					for (int k = j + 1; k < i; ++k)
						g += z.At(k, j) * z.At(i, k);
					offdiag[j] = g / h;
					f += offdiag[j] * z.At(i, j);
				}
				double hh = f / (h + h);
				for (int j = 0; j < i; ++j)
				{
					f = z.At(i, j);
					offdiag[j] = g = offdiag[j] - hh * f;
					for (int k = 0; k < j + 1; ++k)
						z.At(j, k) -= f * offdiag[k] + g * z.At(i, k);
				}
			}
		} 
		else
			offdiag[i] = z.At(i, l);
		diag[i] = h;
	}
	diag[0] = 0.0;
	offdiag[0] = 0.0;
	for (size_t i = 0; i < rows; ++i) 
	{ // begin accumulation of transformation matrices
		if (diag[i] != 0.0) 
		{
			for (size_t j = 0; j < i; ++j) 
			{
				double g = 0.0;
				for (size_t k = 0; k < i; ++k)
					g += z.At(i, k) * z.At(k, j);
				for (size_t k = 0; k < i; ++k)
					z.At(k, j) -= g * z.At(k, i);
			}
		}
		diag[i] = z.At(i, i);
		z.At(i, i) = 1.0; // reset row and column of z to identity matrix for next iteration
		for (size_t j = 0; j < i; ++j)
			z.At(j, i) = z.At(i, j) = 0.0;
	}

}


/*! Finds all eigenvalues of a [1..n][1..n] upper Hessenberg matrix.
 * 
 * \param[in] mat	the input matrix
 * \param[in] max_iter	the maximal number of iterations
 * 
 * \return	eigenvalues stored in a vector of complex numbers
 * 
 * Implementation from "Numerical Recipes in C"
*/
std::vector<std::complex<double>> SquareMatrixDouble::Eigenvalues(size_t max_iter) const
{
	const auto n = int(GetCols());
	
	if (IsUpperTriangular())
	{
		std::vector<std::complex<double>> eigenvalues;
    
		for (auto k = 0; k < n; ++k)
			eigenvalues.push_back(std::complex<double>(At(k, k)));
			
		return eigenvalues;
	}
	
	// Implementation done for [1..n][1..n] matrices.
	// Some tricky index shift needed for [0..n-1][0..n-1] matrices.
	// Still to be done!
	// For now we just store [0..n-1][0..n-1] input matrix in a wider
	// [0..n][0..n] matrix to comply with code proposed in
	// "Numerical Recipes in C" (0th row and 0th colums not touched)
	SquareMatrixDouble a(n + 1, 0.0);
	
	if (IsUpperHessenberg())
		for (auto r = 0; r < n; ++r)
			for (auto c = 0; c < n; ++c)
				a[r + 1][c + 1] = At(r, c);
	else
	{
		const auto& hess_mat = MakeUpperHessenberg();
		
		for (auto r = 0; r < n; ++r)
			for (auto c = 0; c < n; ++c)
				a[r + 1][c + 1] = hess_mat.At(r, c);
	}
	
	std::vector<double> wr(n + 1);
	std::vector<double> wi(n + 1);
	
	int nn = n, m, l;
	double z, y, x, w, v, u, t = 0.0, s, r, q, p;

	// Compute matrix norm for possible use in
	// locating single small subdiagonal element.
	auto anorm = 0.0;
	
	for (auto i = 1; i <= n; ++i)
		for (auto j = Max(i - 1, 1); j <= n; ++j)
			anorm += Abs(a[i][j]);

	// Gets changed only by an exceptional shift.
	while (nn >= 1)
	{
		// Begin search for next eigenvalue.
		int its = 0;
		
		do
		{
			for (l = nn; l >= 2; --l)
			{   // Begin iteration: look for single small subdiagonal element.
                s = Abs(a[l - 1][l - 1]) + Abs(a[l][l]);
				
				if (s == 0.0)
                    s = anorm;
				if (Abs(a[l][l - 1]) + s == s) // Because s and a[l][l-1] may have very different magnitudes
                    break;
			}
			
			x = a[nn][nn];
			
            if (l == nn)
			{   // One root found.
				wr[nn] = x + t;
				wi[nn--] = 0.0;
            }
			else
			{
				y = a[nn - 1][nn - 1];
				w = a[nn][nn - 1] * a[nn - 1][nn];
				
				if (l == (nn - 1))
				{   // Two  roots found...
					p = (y - x) / 2.0;
					q = Sqr(p) + w;
					z = sqrt(Abs(q));
					x += t;

					if (q >= 0.0)
					{   // ... a real  pair.
						//z = p + SIGN(z, p);
						z = p + Abs(z) * SignOf(p);
						wr[nn - 1] = wr[nn] = x + z;

						if (z)
                            wr[nn] = x - w / z;
							
						wi[nn - 1] = wi[nn] = 0.0;
					}
					else
					{   // ... a complex pair.
						wr[nn - 1] = wr[nn] = x + p;
						wi[nn - 1] = -(wi[nn] = z);
					}
					
					nn -= 2;
				}
				else
				{   // No roots found.  Continue iteration.
					if (its == 30)
						std::cout << "ARGH !!!" << std::endl;
						
					if (!(its % 10))
					{   // Form exceptional shift.
						t += x;

						for (auto i = 1; i <= nn; ++i)
							a[i][i]-= x;
						
						s = Abs(a[nn][nn - 1]) + Abs(a[nn - 1][nn - 2]);
						y = x = 0.75 * s;
						w = -0.4375 * Sqr(s);
					}
				
					++its;
			
					for (m = (nn - 2); m >= l; --m)
					{   // Form shift and then look for 2 consecutive
						// small subdiagonal elements.
						z = a[m][m];
						r = x - z;
						s = y - z;
						p = (r * s - w) / a[m + 1][m] + a[m][m + 1];
                        q = a[m + 1][m + 1] - z - r - s;
						r = a[m + 2][m + 1];
						s = Abs(p) + Abs(q) + Abs(r);
                        
						// Scale to prevent overflow or underflow.
						p /= s;
						q /= s;
						r /= s;
						
						if (m == l)
                            break;
						
						u = Abs(a[m][m - 1]) * (Abs(q) + Abs(r));
						v = Abs(p) * (Abs(a[m - 1][m - 1]) + Abs(z) + Abs(a[m + 1][m + 1]));
						
						if (u + v == v)
                            break;
					}
			
					for (auto i = m + 2; i <= nn; ++i)
					{
						a[i][i - 2] = 0.0;
						
						if (i != (m + 2))
                            a[i][i - 3] = 0.0;
					}
			
					for (auto k = m; k <= nn - 1; ++k)
					{   // Double  QR  step on rows l to nn
						// and  columns m to nn
						if (k != m) 
						{
							p = a[k][k - 1];
                            
							// Begin setup of Householder vector
							q = a[k + 1][k - 1];
							r = 0.0;
							
							if (k != (nn - 1))
								r = a[k + 2][k - 1];
							
							// Scale to prevent overflow or underflow
							if ((x = Abs(p) + Abs(q) + Abs(r)) != 0.0)
							{   
								p /= x;
								q /= x;
								r /= x;
							}
						}
						
						//if ((s = SIGN(Pythagoras(p, q, r) ,p)) != 0.0)
						if ((s = Pythagoras(p, q, r) * SignOf(p)) != 0.0)
						{
							if (k == m)
							{
								if (l != m)
									a[k][k - 1] = -a[k][k - 1];
							}
							else
								a[k][k - 1] = -s * x;
								
							p += s;
							x = p / s;
							y = q / s;
							z = r / s;
							q /= p;
							r /= p;
							
							for (auto j = k; j <= nn; ++j)
							{   // Row modifcation.
								p = a[k][j] + q * a[k + 1][j];
								
								if (k != (nn - 1))
								{
									p += r * a[k + 2][j];
									a[k + 2][j] -= p * z;
								}
								
								a[k + 1][j] -= p * y;
								a[k][j] -= p * x;
							}
							
                            auto mmin = nn < k + 3 ? nn : k + 3;
						
                            for (auto i = l; i <= mmin; ++i)
                            {   // Column  modifcation.
                                p = x * a[i][k] + y * a[i][k + 1];
							
                                if (k != (nn - 1))
                                {
                                    p += z*a[i][k + 2];
                                    a[i][k + 2] -= p * r;
                                }
							
                                a[i][k + 1] -= p * q;
                                a[i][k] -= p;
                            }
                        }
                    }
				}
			}
		}
		while (l < nn - 1);
	}
    
    std::vector<std::complex<double>> eigenvalues;
    
    for (auto k = 1; k <=n; ++k)
		eigenvalues.push_back(std::complex<double>(wr[k], wi[k]));
    
    // Sort eigenvalues by magnitude
	
	std::sort(eigenvalues.begin(), eigenvalues.end(), [](const std::complex<double> &c1, const std::complex<double> &c2)
	{
		return norm(c1) < norm(c2);
	});
	
    return eigenvalues;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(SquareMatrixDouble)
	CRN_DATA_FACTORY_REGISTER(U"SquareMatrixDouble", SquareMatrixDouble)
CRN_END_CLASS_CONSTRUCTOR(SquareMatrixDouble)

