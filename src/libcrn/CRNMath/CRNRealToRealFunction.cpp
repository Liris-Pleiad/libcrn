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
 * file: RealToRealFunction.cpp
 * \author Jean DUONG
 */

#include <CRNMath/CRNRealToRealFunction.h>
#include <CRNi18n.h>

using namespace crn;

/*! Constructor
 *
 * \param[in]	l	left end point
 * \param[in]	r	right end point
 */
RealToRealFunction::RealToRealFunction(double l, double r):
	left_endpoint(l),
	right_endpoint(r)
{
}

/*! Constructor
 *
 * \param[in]	F	a function to copy
 */
RealToRealFunction::RealToRealFunction(const RealToRealFunction& F)
{
	left_endpoint = F.GetLeftEndpoint();
	right_endpoint = F.GetRightEndpoint();

	image = CloneAs<MatrixDouble>(*F.GetImage());
}

/*!
 * Destructor
 */
RealToRealFunction::~RealToRealFunction(){}


/*! 
 * Set left endpoint for function domain
 *
 * \param[in]	d	the new end point
 */
void RealToRealFunction::SetLeftEndpoint(double d) noexcept
{
	left_endpoint = d;
}

/*! 
 * Set right endpoint for function domain
 *
 * \param[in]	d	the new end point
 */
void RealToRealFunction::SetRightEndpoint(double d) noexcept
{
	right_endpoint = d;
}

/*! 
 * Perform numerical integration over the entire domain using trapezoids
 *
 * \return	the integral value
 */
double RealToRealFunction::TrapezoidIntegral() const
{
	size_t nb_data = image->GetCols();
	
	double step = double(right_endpoint - left_endpoint) / double(nb_data - 1);
	double I = (At(0) + At(nb_data - 1)) / 2.0;

	for (size_t c = 1; c < nb_data - 1; c++)
	{
		I += At(c);
	}


	I *= step;

	return I;
}

/*! 
 * Define value of the k-th point in domain
 *
 * \param[in]	k	index
 * \param[in]	v	value
 */
void RealToRealFunction::SetElement(size_t k, double v)
{
	image->At(0, k) = v;
}

/*! 
 * Define the number of data points in domain
 *
 * \param[in]	s	new size
 */
void RealToRealFunction::SetImageSize(size_t s)
{
	image = std::make_shared<MatrixDouble>(1, s, 0.0);
}

/*! 
 * Compute value of the image of a given point
 *
 * \param[in]	x	the point to evaluate
 * \return	the computed image
 */
double RealToRealFunction::Evaluate(double x) const
{
	double y = 0.0;

	if ((x >= left_endpoint) || (x <= right_endpoint))
	{
	        size_t nbBins = GetImageSize();
		double step = double(right_endpoint - left_endpoint) / double(nbBins - 1);
	     
		int k = (int)((x - left_endpoint) / step);
		
		double x_l = left_endpoint + k * step;
		double x_r = x_l + step;
		double y_l = At(k);
		double y_r = At(k + 1);
		
		y = ((x - x_l) * (y_r - y_l)) / (x_r - x_l) + y_l;
	}

	return y;
}

/*! 
 * Get the possible preimages of a given image value
 *
 * \throws	ExceptionNotFound	no preimage found
 *
 * \param[in]	y	the image
 *
 * \return	a line matrix containing the possible preimages
 */
MatrixDouble RealToRealFunction::MakePreimage(double y) const
{
	size_t n = GetImageSize();
	size_t Count = 0;

	for (size_t k = 1; k < n; k++)
	{
		double diff_l = y - At(k - 1);
		double diff_r = y - At(k);

		if (diff_l * diff_r <= 0)
		{
			Count++;

			if (diff_r ==0)
			{
				Count--;
			}
		}

	}

	if (Count == 0)
	{
		throw ExceptionNotFound(_("No preimage found."));
	}

	MatrixDouble P(1, Count, 0.0);

	double step = double(right_endpoint - left_endpoint) / double(n - 1);
	double x_l = left_endpoint;
	double x_r = left_endpoint;
	double y_r = At(0);

	Count = 0;
	double Last = HUGE_VAL;

	for (size_t k = 1; k < n; k++)
	{
		x_l = x_r;
		double y_l = y_r;
		x_r += step;
		y_r = At(k);

		if ((y - y_l)*(y - y_r) <= 0.0)
		{
			double x = 0.0;

			if (x_r != x_l)
			{
				x = ((x_r - x_l) * (y - y_l)) / (y_r - y_l) + x_l;
			}
			else
			{
				x = (x_l + x_r) / 2.0;
			}

			if (x != Last)
			{
				Last = x;
				P.At(0, Count) = x;
				Count++;
			}

		}
	}


	return P;
}


