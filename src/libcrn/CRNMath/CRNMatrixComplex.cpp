/* Copyright 2013-2014 INSA Lyon
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
 * file: CRNMatrixComplex.cpp
 * \author Yann LEYDIER
 */

#include <CRNMath/CRNMatrixComplex.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNProtocols.h> 
#include <CRNi18n.h> 

using namespace crn;

/*! \internal */
static void doFFT(std::complex<double> *sig, int n, bool direct)
{
	// dimensions
	double logsize = log2(n);
	if (logsize != ceil(logsize))
		throw ExceptionDimension(_("FFT: signal size is not a power of 2."));
	int m = int(logsize);

	// bit reversal
	int i2 = n >> 1;
	int j = 0;
	for (int i = 0; i < n-1 ; i++)
	{
		if (i < j)
			swap(sig[i], sig[j]);
		int k = i2;
		while (k <= j) 
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	// compute the FFT
	std::complex<double> c(-1.0, 0.0);
	int l2 = 1;
	for (int l = 0; l < m; l++) 
	{
		int l1 = l2;
		l2 <<= 1;
		std::complex<double> u(1.0, 0.0);
		for (j = 0; j < l1; j++) 
		{
			for (int i = j; i < n; i += l2) 
			{
				int i1 = i + l1;
				std::complex<double> t1 = u * sig[i1];
				sig[i1] = sig[i] - t1; 
				sig[i] += t1;
			}

			u = u * c;
		}

		double ima = sqrt((1.0 - c.real()) / 2.0);
		if (direct)
			ima = -ima;
		c = std::complex<double>(sqrt((1.0 + c.real()) / 2.0), ima);
	}

	// scaling for forward transform
	if (direct)
	{
		for (int i = 0; i < n; i++)
			sig[i] /= n;      
	}	
}

/*! Fast Fourier transform
 * \date		April 2013
 * \version 0.1
 * \ingroup math
 * \throws	ExceptionDimension	signal does not have a power of 2 size
 * \param[in]	sig	the signal to process
 * \param[in]	direct	direct=true, inverse=false
 */
void crn::FFT(std::vector<std::complex<double> > &sig, bool direct)
{
	doFFT(sig.data(), int(sig.size()), direct);
}

/*! Grows the matrix to power of 2 sizes (for FFT) 
 * \param[in]	make_square	shall the resulting matrix be square?
 * \param[in]	fill_value	the value used to fill the blanks
 */
void MatrixComplex::GrowToPowerOf2(bool make_square, const std::complex<double> &fill_value)
{
	// find power of 2 row size
	size_t newr = rows;
	double logr = log2(int(newr));
	while (logr != ceil(logr))
	{
		newr += 1;
		logr = log2(int(newr));
	}
	// find power of 2 col size
	size_t newc = cols;
	double logc = log2(int(newc));
	while (logc != ceil(logc))
	{
		newc += 1;
		logc = log2(int(newc));
	}
	// make a square matrix?
	if (make_square)
	{
		newc = newr = Max(newr, newc);
	}
	// copy the data
	datatype newdata(newr * newc, fill_value);
	for (size_t r = 0; r < rows; ++r)
		std::copy(data.begin() + r * cols, data.begin() + (r + 1) * cols, newdata.begin() + r * newc);

	std::swap(rows, newr);
	std::swap(cols, newc);
	data.swap(newdata);
}

/*! \return	a matrix containing the modules of the inner values */
MatrixDouble MatrixComplex::MakeModule() const
{
	MatrixDouble m(rows, cols);
	for (size_t r = 0; r < rows; ++r)
		for (size_t c = 0; c < cols; ++c)
			m[r][c] = std::abs(At(r, c));
	return m;
}

/* Inplace fast Fourier transform
 *
 * \warning	Sizes must be power of 2
 *
 * \throws	ExceptionDimension	signal does not have a power of 2 size
 * \param[in]	direct	direct=true, inverse=false
 */
void MatrixComplex::FFT(bool direct)
{
	if ((rows == 1) || (cols == 1))
	{ // line vector
		crn::FFT(data, direct);
	}
	else
	{ // 2D transform
		// lines
		for (size_t r = 0; r < rows; ++r)
			doFFT(data.data() + r * cols, int(cols), direct);
		// columns
		Transpose();
		try
		{
			for (size_t r = 0; r < rows; ++r)
				doFFT(data.data() + r * cols, int(cols), direct);
			Transpose();
		}
		catch (...)
		{ // transpose back to ensure exception safety
			Transpose();
			throw;
		}
	}
}

/*! Cross correlation 
 * \param[in]	other	the matrix to match
 * \param[in]	fill1	the fill pattern for current matrix (to grow to power of 2 dimensions)
 * \param[in]	fill2	the fill pattern for the other matrix (to grow to power of 2 dimensions)
 * \return	the best match displacement and power
 */
std::pair<Point2DInt, double> MatrixComplex::CrossCorrelation(const MatrixComplex &other, const std::complex<double> &fill1, const std::complex<double> &fill2)
{
	// find nearest power of 2 dimensions
	size_t w = crn::Max(GetCols(), other.GetCols());
	size_t h = crn::Max(GetRows(), other.GetRows());
	double logs = log2(int(w));
	while (logs != ceil(logs))
	{
		w += 1;
		logs = log2(int(w));
	}
	if (w < 2) w = 2;
	logs = log2(int(h));
	while (logs != ceil(logs))
	{
		h += 1;
		logs = log2(int(h));
	}
	if (h < 2) h = 2;
	// compute cross correlation
	MatrixComplex c1(h, w, fill1);
	for (size_t r = 0; r < GetRows(); ++r)
		for (size_t c = 0; c < GetCols(); ++c)
			c1[r][c] = At(r, c);
	c1.FFT(true);
	UMatrixComplex c2(std::make_unique<MatrixComplex>(h, w, fill2));
	for (size_t r = 0; r < other.GetRows(); ++r)
		for (size_t c = 0; c < other.GetCols(); ++c)
			(*c2)[r][c] = other[r][c];
	c2->FFT(true);
	for (size_t r = 0; r < h; ++r)
		for (size_t c = 0; c < w; ++c)
			c1[r][c] *= std::conj((*c2)[r][c]);
	c2 = nullptr; // free memory
	c1.FFT(true);
	// find best match
	Point2DInt p;
	double maxc = 0;
	for (size_t r = 0; r < h; ++r)
		for (size_t c = 0; c < w; ++c)
		{
			double corr = std::norm(c1[r][c]);
			if (corr > maxc)
			{
				p.X = int(c);
				p.Y = int(r);
				maxc = corr;
			}
		}
	int hw = (int)w/2;
	int hh = (int)h/2;
	if (p.X >= hw)
		p.X = (p.X % hw) - hw;
	if (p.Y >= hh)
		p.Y = (p.Y % hh) - hh;
	return std::make_pair(p, maxc);	
}

CRN_BEGIN_CLASS_CONSTRUCTOR(MatrixComplex)
	Cloner::Register<MatrixComplex>();
CRN_END_CLASS_CONSTRUCTOR(MatrixComplex)
