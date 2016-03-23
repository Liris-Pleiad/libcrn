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
 * file: CRNMatrixDouble.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNException.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNData/CRNData.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

/*! 
 * Compute the sum of the squared elements
 * 
 * \return	the sum of the squared elements
 */
double MatrixDouble::CumulateSquaredCells() const
{
	double sum = 0.0;
	
	for (size_t r = 0; r < rows; ++r)
	{
		for (size_t c = 0; c < cols; ++c)
		{
			double val = At(r, c);
			sum += val * val;
		}
	}
	
	return sum;
}

/*!
 * Create a line matrix containing a centred Gaussian curve
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	the newly created matrix
 */
MatrixDouble MatrixDouble::NewGaussianLine(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixDouble::NewGaussianLine(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma <= 0.001)
		return MatrixDouble(1, 1, 1);
		
	int hwin = (int)ceil(Thrice(sigma));
	int size = hwin + hwin + 1;
	MatrixDouble mat(1, size, 0);
	
	for (int tmp = 0; tmp < size; ++tmp)
		mat.At(0, tmp) = MeanGauss(double(tmp) - double(hwin), sigma);

	return mat;
}

/*!
 * Create a line matrix containing the derivative of a centred Gaussian curve
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	the newly created matrix
 */
MatrixDouble MatrixDouble::NewGaussianLineDerivative(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixDouble* MatrixDouble::NewGaussianLineDerivative(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma == 0)
	{
		MatrixDouble mat(1, 3, 0);
		mat[0][0] = 1;
		mat[0][2] = -1;
		return mat;
	}
	
	int hwin = (int)ceil(Thrice(sigma));
	int size = hwin + hwin + 1;
	MatrixDouble mat(1, size, 0);
	
	for (int tmp = 0; tmp < size; ++tmp)
	{
		double v = (double)hwin - (double)tmp; // -x
		mat.At(0, tmp) = (v * Gauss(v, sigma) +
					(v - 0.5) * Gauss(v - 0.5, sigma) + 
					(v + 0.5) * Gauss(v + 0.5, sigma)) / 3.0;
	}
	
	return mat;
}

/*!
 * Create a line matrix containing a centred Gaussian curve
 *
 * \throws	ExceptionDomain	sigma is negative
 *
 * \param[in]	sigma	the standard deviation of the Gaussian
 *
 * \return	the newly created matrix
 */
MatrixDouble MatrixDouble::NewGaussianLineSecondDerivative(double sigma)
{
	if (sigma < 0)
		throw ExceptionDomain(StringUTF8("MatrixDouble* MatrixDouble::NewGaussianLineSecondDerivative(double sigma): ") + 
				_("Negative standard deviation"));
	if (sigma == 0)
	{
		MatrixDouble mat(1, 3, -1);
		mat[0][1] = 2;
		return mat;
	}
	int hwin = (int)ceil(6 * sigma);
	int size = hwin + hwin + 1;
	MatrixDouble mat(1, size, 0);
	for (int tmp = 0; tmp < size; ++tmp)
	{
		double v = (double)tmp - (double)hwin; // x
		mat.At(0, tmp) = ((Sqr(v) - Sqr(sigma)) * Gauss(v, sigma) + 
					(Sqr(v - 0.5) - Sqr(sigma)) * Gauss(v - 0.5, sigma) + 
					(Sqr(v + 0.5) - Sqr(sigma)) * Gauss(v + 0.5, sigma)) / 3.0;
	}
	return mat;
}

/*! 
 * Get the product of a column vector by its own transposed on the right side
 *
 * \throws	ExceptionDimension	 the matrix is not a column
 *
 * \return	the product of a column vector by its own transposed on the right side
 */
SquareMatrixDouble MatrixDouble::MakeVectorRightAutoProduct() const
{
	if (GetCols() == 1)
	{
		size_t s = GetRows();
		
		SquareMatrixDouble m(s, 0.0);
					 
		for (size_t r = 0; r < s; ++r)
		{
			double vr = At(r, 0);

			for (size_t c = 0; c < s; ++c)
			{
				m.At(r, c) = vr * At(c, 0);
			}
		}
			
		return m;
	}
	throw ExceptionDimension(StringUTF8("MatrixDouble::MakeVectorRightAutoProduct(): ") + _("not a column vector."));
}

/*!
 * Unsafe load from XML node
 *
 * \author Jean DUONG
 *
 * \throws	ExceptionInvalidArgument	not a MatrixDouble
 * \throws	ExceptionNotFound	cannot find attribute
 * \throws	ExceptionDomain	wrong attribute or cannot find CDATA
 * \throws	ExceptionRuntime	cannot convert CDATA
 *
 * \param[in]	el	the element to load
 */
void MatrixDouble::Deserialize(xml::Element &el)
{
	if (el.GetName() != "MatrixDouble")
	{
		throw ExceptionInvalidArgument(StringUTF8("bool MatrixDouble::Deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	size_t nr = el.GetAttribute<int>("nb_rows", false); // may throw
	size_t nc = el.GetAttribute<int>("nb_columns", false); // may throw

	xml::Node n(el.GetFirstChild());
	xml::Text t(n.AsText()); // may throw

	auto vals = Data::ASCII85Decode<double>(t.GetValue());
	if (vals.size() != nc * nr)
		throw ExceptionRuntime(StringUTF8("bool MatrixDouble::Deserialize(TiXmlElement *el): ") + 
				_("Cannot convert CDATA."));

	rows = nr;
	cols = nc;
	data.swap(vals);
}

/*!
 * Unsafe save
 *
 * \author Jean DUONG
 *
 * \param[in]	parent	the parent element to which we will add the new element
 *
 * \return The newly created element, nullptr if failed.
 */
xml::Element MatrixDouble::Serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement("MatrixDouble"));

	el.SetAttribute("nb_rows", int(rows));
	el.SetAttribute("nb_columns", int(cols));

	auto text = Data::ASCII85Encode(reinterpret_cast<const uint8_t * const>(data.data()), cols * rows * sizeof(double));
	xml::Text t(el.PushBackText(text, false));

	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(MatrixDouble)
	CRN_DATA_FACTORY_REGISTER(U"MatrixDouble", MatrixDouble)
CRN_END_CLASS_CONSTRUCTOR(MatrixDouble)

