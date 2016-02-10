/* Copyright 2008-2015 INSA Lyon
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
 * file: FuzzyLogic.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNMath/CRNFuzzyLogic.h>
#include <CRNMath/CRNRealToRealFunction.h>

using namespace crn;

/*! 
 * Compute the compatibility function between a regular function and a approximated function
 *
 * \param[in]	f	the regular function
 * \param[in]	g	the approximated function
 * \param[in]	nbBins	the number of slices in resulting function
 *
 * \return	a real to real function
 */
RealToRealFunction FuzzyLogic::MakeCompatibility(const RealToRealFunction& f, const RealToRealFunction& g, size_t nbBins)
{
	RealToRealFunction c;
	c.SetImageSize(nbBins);

	double delta = 1.0 / double(nbBins - 1);
	double x = 0.0;

	for (size_t k = 0; k < nbBins; k++)
	{
		double sup = 0.0;
		try
		{
			MatrixDouble preF = f.MakePreimage(x);
			for (size_t i = 0; i < preF.GetCols(); i++)
			{
				double y = g.Evaluate(preF.At(0, i));

				if (y > sup)
				{
					sup = y;
				}
			}	
		}
		catch (ExceptionNotFound&) { }
		c.SetElement(k, sup);
		x += delta;
	}

	c.SetLeftEndpoint(0.0);
	c.SetRightEndpoint(1.0);

	return c;
}

/*! Defuzzification over an interval using center of gravity method
 *
 * \param[in]	min	beginning of the interval
 * \param[in]	max	end of the interval
 * \param[in]	f	a truth function
 *
 * \return	the truth value after defuzzification
 */
double FuzzyLogic::DefuzzificationCOG(double min, double max, const RealToRealFunction& f)
{
	double denominator = f.TrapezoidIntegral();

	if (denominator != 0.0)
	{
		URealToRealFunction newF = f.CloneAs<RealToRealFunction>();

		size_t n = newF->GetImageSize();
		double x = 0.0;
		double delta = 1.0 / double(n - 1);

		for (size_t k = 0; k < n; k++)
		{
			newF->SetElement(k, newF->At(k) * x);
			x += delta;
		}

		return newF->TrapezoidIntegral() / denominator;
	}
	else
	{
		return 0.0;
	}
}


