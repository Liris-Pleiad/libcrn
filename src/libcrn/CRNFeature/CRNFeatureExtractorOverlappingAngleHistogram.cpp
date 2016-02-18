/* Copyright 2012-2016 CoReNum, INSA-Lyon
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
 * file: CRNFeatureExtractorOverlappingAngleHistogram.cpp
 * \author Yann LEYDIER
 */

#include <CRNFeature/CRNFeatureExtractorOverlappingAngleHistogram.h>
#include <CRNException.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNImage/CRNImageGray.h>
#include <CRNImage/CRNImageGradient.h>
#include <CRNBlock.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

using namespace crn;

static inline size_t chooseangle(const crn::Angle<ByteAngle> &a, const std::vector<crn::Angle<ByteAngle> > &angles)
{
	std::vector<uint8_t> distances(angles.size());
	for (size_t tmp = 0; tmp < angles.size(); ++tmp)
		distances[tmp] = crn::AngularDistance(a, angles[tmp]);
	return std::min_element(distances.begin(), distances.end()) - distances.begin();
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorOverlappingAngleHistogram::Extract(Block &b)
{
	const size_t ncenters = 9;
	// find center and std deviation
	uint64_t xsum = 0, ysum = 0, x2sum = 0, y2sum = 0, vsum = 0;
	SImageGray ig(b.GetGray());
	FOREACHPIXEL(x, y, *ig)
	{
		int val = 255 - ig->At(x, y);
		vsum += val;
		xsum += x * val;
		ysum += y * val;
		x2sum += x * x * val;
		y2sum += y * y * val;
	}
	if (!xsum) // white image
		return std::make_shared<MatrixDouble>(ncenters * nbangles, 1, 0.0);
	int xbar;
	int ybar;
	double xvar;
	double yvar;
	if (vsum == 0)
	{
		xbar = int(ig->GetWidth()) / 2;
		ybar = int(ig->GetHeight()) / 2;
		xvar = int(ig->GetWidth()) / 6;
		yvar = int(ig->GetHeight()) / 6;
	}
	else
	{
		xbar = int(xsum / vsum);
		ybar = int(ysum / vsum);
		xvar = (double(x2sum) - double(xsum * xsum) / double(vsum)) / double(vsum);
		yvar = (double(y2sum) - double(ysum * ysum) / double(vsum)) / double(vsum);
	}

	// compute centers (sqrt(1.4×var) is the distance where the Gaussian evaluates to 0.5)
	std::vector<crn::Point2DInt> centers(ncenters);
	centers[0] = crn::Point2DInt(xbar, ybar);
	centers[1] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar);
	centers[2] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar - int(sqrt(distance * yvar)));
	centers[3] = crn::Point2DInt(xbar, ybar - int(sqrt(distance * yvar)));
	centers[4] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar - int(sqrt(distance * yvar)));
	centers[5] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar);
	centers[6] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar + int(sqrt(distance * yvar)));
	centers[7] = crn::Point2DInt(xbar, ybar + int(sqrt(distance * yvar)));
	centers[8] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar + int(sqrt(distance * yvar)));

	// precompute Gaussian
	int maxx = int(2*ig->GetWidth());
	int maxy = int(2*ig->GetHeight());
	std::vector<std::vector<double> > gauss(maxx, std::vector<double>(maxy));
	const double sigmax = 2.0 * xvar / 4.0;
	const double sigmay = 2.0 * yvar / 4.0;
	for (int x = 0; x < maxx; ++x)
		for (int y = 0; y < maxy; ++y)
			gauss[x][y] = exp(-((x * x) + (y * y)) / (sigmax * sigmay));

	// compute features
	SImageGradient igr(b.GetGradient());
	int cnt = 0;
	std::vector<double> feats(ncenters * nbangles, 0);
	FOREACHPIXEL(x, y, *igr)
	{
		if (igr->IsSignificant(x, y))
		{
			cnt += 1;
			size_t angle = chooseangle(igr->At(x, y).theta, angletab);
			for (size_t tmp = 0; tmp < ncenters; ++tmp)
			{
				feats[tmp * nbangles + angle] += gauss[crn::Abs(int(x) - centers[tmp].X)][crn::Abs(int(y) - centers[tmp].Y)];
			}
		}
	}
	if (cnt != 0)
		for (auto & feat : feats)
			feat /= cnt;

	return std::make_shared<MatrixDouble>(feats);
}

/*****************************************************************************/
/*!
 * Returns a CRNPROTOCOL_FEATURE object computed from a block
 *
 * \param[in]	b	the block to process
 * \param[in]	mask	the mask of the connected component to process
 * \return a SHistogram containing the profiles
 */
SObject FeatureExtractorOverlappingAngleHistogram::ExtractWithMask(Block &b, ImageIntGray &mask)
{
	const size_t ncenters = 9;
	const int ccid = b.GetName().ToInt();
	const int ox = b.GetRelativeBBox().GetLeft(); // offset from parent block
	const int oy = b.GetRelativeBBox().GetTop();

	// find center and std deviation
	uint64_t xsum = 0, ysum = 0, x2sum = 0, y2sum = 0, vsum = 0;
	SImageGray ig(b.GetGray());
	FOREACHPIXEL(x, y, *ig)
	{
		if (mask.At(x + ox, y + oy) != ccid)
			continue;
		int val = 255 - ig->At(x, y);
		vsum += val;
		xsum += x * val;
		ysum += y * val;
		x2sum += x * x * val;
		y2sum += y * y * val;
	}
	if (!xsum) // white image
		return std::make_shared<MatrixDouble>(ncenters * nbangles, 1, 0.0);
	int xbar;
	int ybar;
	double xvar;
	double yvar;
	if (vsum == 0)
	{
		xbar = int(ig->GetWidth()) / 2;
		ybar = int(ig->GetHeight()) / 2;
		xvar = int(ig->GetWidth()) / 6;
		yvar = int(ig->GetHeight()) / 6;
	}
	else
	{
		xbar = int(xsum / vsum);
		ybar = int(ysum / vsum);
		xvar = (double(x2sum) - double(xsum * xsum) / double(vsum)) / double(vsum);
		yvar = (double(y2sum) - double(ysum * ysum) / double(vsum)) / double(vsum);
	}

	// compute centers (sqrt(1.4×var) is the distance where the Gaussian evaluates to 0.5)
	std::vector<crn::Point2DInt> centers(ncenters);
	centers[0] = crn::Point2DInt(xbar, ybar);
	centers[1] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar);
	centers[2] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar - int(sqrt(distance * yvar)));
	centers[3] = crn::Point2DInt(xbar, ybar - int(sqrt(distance * yvar)));
	centers[4] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar - int(sqrt(distance * yvar)));
	centers[5] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar);
	centers[6] = crn::Point2DInt(xbar + int(sqrt(distance * xvar)), ybar + int(sqrt(distance * yvar)));
	centers[7] = crn::Point2DInt(xbar, ybar + int(sqrt(distance * yvar)));
	centers[8] = crn::Point2DInt(xbar - int(sqrt(distance * xvar)), ybar + int(sqrt(distance * yvar)));

	// precompute Gaussian
	int maxx = int(2*ig->GetWidth());
	int maxy = int(2*ig->GetHeight());
	std::vector<std::vector<double> > gauss(maxx, std::vector<double>(maxy));
	const double sigmax = 2.0 * xvar / 4.0;
	const double sigmay = 2.0 * yvar / 4.0;
	for (int x = 0; x < maxx; ++x)
		for (int y = 0; y < maxy; ++y)
			gauss[x][y] = exp(-((x * x) + (y * y)) / (sigmax * sigmay));

	// compute features
	SImageGradient igr(b.GetGradient());
	int cnt = 0;
	std::vector<double> feats(ncenters * nbangles, 0);
	FOREACHPIXEL(x, y, *igr)
	{
		if (mask.At(x + ox, y + oy) != ccid)
			continue;
		if (igr->IsSignificant(x, y))
		{
			cnt += 1;
			size_t angle = chooseangle(igr->At(x, y).theta, angletab);
			for (size_t tmp = 0; tmp < ncenters; ++tmp)
			{
				feats[tmp * nbangles + angle] += gauss[crn::Abs(int(x) - centers[tmp].X)][crn::Abs(int(y) - centers[tmp].Y)];
			}
		}
	}
	if (cnt != 0)
		for (auto & feat : feats)
			feat /= cnt;

	return std::make_shared<MatrixDouble>(feats);
}

/*****************************************************************************/
/*!
 * Initializes the object from an XML element. Unsafe.
 * \throws	ExceptionInvalidArgument	not a FeatureExtractorProjection
 * \throws	ExceptionNotFound	attribute not found
 * \throws	ExceptionDomain	wrong attribute
 */
void FeatureExtractorOverlappingAngleHistogram::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool FeatureExtractorOverlappingAngleHistogram::deserialize("
					"xml::Element &el): ") + _("Wrong XML element."));
	}
	int n = el.GetAttribute<int>("nbangles", false); // may throw
	double d = el.GetAttribute<double>("distance", false); // may throw
	nbangles = n;
	distance = d;
	initTables();
}

/*****************************************************************************/
/*!
 * Dumps the object to an XML element. Unsafe.
 */
xml::Element FeatureExtractorOverlappingAngleHistogram::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("nbangles", nbangles);
	el.SetAttribute("distance", distance);
	return el;
}

/*! creates tables to speed up the process */
void FeatureExtractorOverlappingAngleHistogram::initTables()
{
	std::vector<Angle<ByteAngle> >(nbangles).swap(angletab);
	for (size_t tmp = 0; tmp < nbangles; ++tmp)
		angletab[tmp] = uint8_t((tmp * 255) / nbangles);
}

CRN_BEGIN_CLASS_CONSTRUCTOR(FeatureExtractorOverlappingAngleHistogram)
	CRN_DATA_FACTORY_REGISTER(U"FeatureExtractorOverlappingAngleHistogram", FeatureExtractorOverlappingAngleHistogram)
CRN_END_CLASS_CONSTRUCTOR(FeatureExtractorOverlappingAngleHistogram)

