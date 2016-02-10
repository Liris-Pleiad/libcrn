/* Copyright 2006-2014 Yann LEYDIER, INSA-Lyon
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
 * file: CRNDifferential.cpp
 * \author Yann LEYDIER
 */

#include <CRNImage/CRNDifferential.h>
#include <vector>
#include <math.h>
#include <CRNException.h>
#include <CRNImage/CRNImageRGB.h>
#include <CRNGeometry/CRNPoint2DDouble.h>
#include <CRNi18n.h>

using namespace crn;

Differential::Differential(ImageGray &&src, ImageDoubleGray &&xdiff, ImageDoubleGray &&ydiff, ImageDoubleGray &&xxdiff, ImageDoubleGray &&yydiff):
	srcigray(std::move(src)),
	lx(std::move(xdiff)),
	ly(std::move(ydiff)),
	lxx(std::move(xxdiff)),
	lyy(std::move(yydiff)),
	thres(0),
	lazydata(std::make_unique<std::mutex>())
{
	lx2ly2 = ImageDoubleGray(lx.GetWidth(), lx.GetHeight());
	updateLx2ly2();
}

Differential::Differential(ImageGray &&src, ImageDoubleGray &&xdiff, ImageDoubleGray &&ydiff):
	srcigray(std::move(src)),
	lx(std::move(xdiff)),
	ly(std::move(ydiff)),
	thres(0),
	lazydata(std::make_unique<std::mutex>())
{
	lx2ly2 = ImageDoubleGray(lx.GetWidth(), lx.GetHeight());
	updateLx2ly2();
}

static std::tuple<ImageDoubleGray, ImageDoubleGray> derivate1(const ImageDoubleGray &src)
{
	auto diff1 = MatrixDouble({-1, 0, 1}, Orientation::HORIZONTAL);
	auto lx = src;
	lx.Convolve(diff1);
	diff1.Transpose();
	auto ly = src;
	ly.Convolve(diff1);
	return std::make_tuple(std::move(lx), std::move(ly));
}

static std::tuple<ImageDoubleGray, ImageDoubleGray, ImageDoubleGray, ImageDoubleGray> derivate2(const ImageDoubleGray &src)
{
	auto diff1 = MatrixDouble({-1, 0, 1}, Orientation::HORIZONTAL);
	auto diff2 = MatrixDouble({1, -2, 1}, Orientation::HORIZONTAL);
	auto lx = src;
	lx.Convolve(diff1);
	diff1.Transpose();
	auto ly = src;
	ly.Convolve(diff1);
	auto lxx = src;
	lxx.Convolve(diff2);
	diff2.Transpose();
	auto lyy = src;
	lyy.Convolve(diff2);
	return std::make_tuple(std::move(lx), std::move(ly), std::move(lxx), std::move(lyy));
}

static std::tuple<ImageDoubleGray, ImageDoubleGray> derivate1Gauss(const ImageDoubleGray &src, double sigma)
{
	if (sigma == 0)
		return derivate1(src);

	auto smooth = MatrixDouble::NewGaussianLine(sigma);
	smooth.NormalizeForConvolution();
	auto diff1 = MatrixDouble::NewGaussianLineDerivative(sigma);
	diff1.NormalizeForConvolution();

	auto ly = src;
	ly.Convolve(smooth);
	smooth.Transpose();
	auto lx = src;
	lx.Convolve(smooth);
	lx.Convolve(diff1);
	diff1.Transpose();
	ly.Convolve(diff1);
	return std::make_tuple(std::move(lx), std::move(ly));
}

static std::tuple<ImageDoubleGray, ImageDoubleGray, ImageDoubleGray, ImageDoubleGray> derivate2Gauss(const ImageDoubleGray &src, double sigma)
{
	if (sigma == 0)
		return derivate2(src);

	auto smooth = MatrixDouble::NewGaussianLine(sigma);
	smooth.NormalizeForConvolution();
	auto diff1 = MatrixDouble::NewGaussianLineDerivative(sigma);
	diff1.NormalizeForConvolution();
	auto diff2 = MatrixDouble::NewGaussianLineSecondDerivative(sigma);
	diff2.NormalizeForConvolution();

	auto ly = src;
	ly.Convolve(smooth);
	auto lyy = src;
	lyy.Convolve(smooth);
	smooth.Transpose();
	auto lx = src;
	lx.Convolve(smooth);
	lx.Convolve(diff1);
	diff1.Transpose();
	ly.Convolve(diff1);
	auto lxx = src;
	lxx.Convolve(smooth);
	lxx.Convolve(diff2);
	diff2.Transpose();
	lyy.Convolve(diff2);
	return std::make_tuple(std::move(lx), std::move(ly), std::move(lxx), std::move(lyy));
}

static ImageDoubleGray& sumrgb(ImageDoubleGray &r, ImageDoubleGray &g, ImageDoubleGray &b)
{
	for (auto tmp : Range(r))
		r.At(tmp) += g.At(tmp) + b.At(tmp);
	return r;
}

static ImageDoubleGray& absmax(ImageDoubleGray &r, ImageDoubleGray &g, ImageDoubleGray &b)
{
	for (auto tmp : Range(r))
		r.At(tmp) = AbsMax(AbsMax(r.At(tmp), g.At(tmp)), b.At(tmp));
	return r;
}

static ImageDoubleGray& absmax(ImageDoubleGray &i1, ImageDoubleGray &i2)
{
	for (auto tmp : Range(i1))
		i1.At(tmp) = AbsMax(i1.At(tmp), i2.At(tmp));
	return i1;
}

static ImageDoubleGray& absmin(ImageDoubleGray &i1, ImageDoubleGray &i2)
{
	for (auto tmp : Range(i1))
		i1.At(tmp) = AbsMin(i1.At(tmp), i2.At(tmp));
	return i1;
}

void halfdiffleft(ImageDoubleGray &i)
{
	for (size_t y = 0; y < i.GetHeight(); ++y)
	{
		for (size_t x = i.GetWidth() - 1; x >= 1; --x)
			i.At(x, y) -= i.At(x - 1, y);
		i.At(0, y) = i.At(1, y);
	}
}

void halfdiffright(ImageDoubleGray &i)
{
	for (size_t y = 0; y < i.GetHeight(); ++y)
	{
		for (size_t x = 0; x < i.GetWidth() - 1; ++x)
			i.At(x, y) -= i.At(x + 1, y);
		i.At(i.GetWidth() - 1, y) = i.At(i.GetWidth() - 2, y);
	}
}

void halfdifftop(ImageDoubleGray &i)
{
	for (size_t y = i.GetHeight() - 1; y >= 1; --y)
	{
		for (size_t x = 0; x < i.GetWidth(); ++x)
			i.At(x, y) -= i.At(x, y - 1);
	}
	for (size_t x = 0; x < i.GetWidth(); ++x)
		i.At(x, 0) = i.At(x, 1);
}

void halfdiffbottom(ImageDoubleGray &i)
{
	for (size_t y = 0; y < i.GetHeight() - 1; ++y)
	{
		for (size_t x = 0; x < i.GetWidth(); ++x)
			i.At(x, y) -= i.At(x, y + 1);
	}
	for (size_t x = 0; x < i.GetWidth(); ++x)
		i.At(x, i.GetHeight() - 1) -= i.At(x, i.GetHeight() - 2);
}

/*! Convolution with Gaussian derivatives.
 * \throws	ExceptionDomain	sigma<0
 * \param[in]	src	the image to derivate
 * \param[in]	proj	the method of projection from R, G and B derivatives to a single derivative. If proj = ABSMAX, the 2nd derivatives are not cached and will be approximated.
 * \param[in]	sigma	the standard deviation of the Gaussian (if null, the derivation convolutions are [-1 0 1] and [1 -2 1])
 * \return	a new Differential object
 */
Differential Differential::NewGaussian(const ImageRGB &src, RGBProjection proj, double sigma)
{
	if (proj == RGBProjection::ABSMAX)
	{
		auto ig = ImageDoubleGray(src.GetWidth(), src.GetHeight());
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).r;
		auto rdiff = derivate1Gauss(ig, sigma);
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).g;
		auto gdiff = derivate1Gauss(ig, sigma);
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).b;
		auto bdiff = derivate1Gauss(ig, sigma);
		return Differential(MakeImageGray(src), std::move(absmax(std::get<0>(rdiff), std::get<0>(gdiff), std::get<0>(bdiff))),
				std::move(absmax(std::get<1>(rdiff), std::get<1>(gdiff), std::get<1>(bdiff))));
	}
	else
	{
		auto ig = ImageDoubleGray(src.GetWidth(), src.GetHeight());
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).r;
		auto rdiff = derivate2Gauss(ig, sigma);
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).g;
		auto gdiff = derivate2Gauss(ig, sigma);
		for (auto tmp : Range(src))
			ig.At(tmp) = src.At(tmp).b;
		auto bdiff = derivate2Gauss(ig, sigma);
		return Differential(MakeImageGray(src), std::move(sumrgb(std::get<0>(rdiff), std::get<0>(gdiff), std::get<0>(bdiff))),
				std::move(sumrgb(std::get<1>(rdiff), std::get<1>(gdiff), std::get<1>(bdiff))),
				std::move(sumrgb(std::get<2>(rdiff), std::get<2>(gdiff), std::get<2>(bdiff))),
				std::move(sumrgb(std::get<3>(rdiff), std::get<3>(gdiff), std::get<3>(bdiff))));
	}
}

/*! Alternate half derivatives (left and top for 1st derivatives and right and bottom for 2nd derivatives)
 * \param[in]	src	the image to derivate
 * \param[in]	proj	the method of projection from R, G and B derivatives to a single derivative
 * \return	a new Differential object
 */
Differential Differential::NewHalfDiff(const ImageRGB &src, RGBProjection proj)
{
	auto irx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		irx.At(tmp) = src.At(tmp).r;
	auto iry = irx;
	halfdiffleft(irx);
	halfdifftop(iry);
	auto igx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		igx.At(tmp) = src.At(tmp).g;
	auto igy = igx;
	halfdiffleft(igx);
	halfdifftop(igy);
	auto ibx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		ibx.At(tmp) = src.At(tmp).b;
	auto iby = ibx;
	halfdiffleft(ibx);
	halfdifftop(iby);
	if (proj == RGBProjection::ABSMAX)
	{
		auto& lx = absmax(irx, igx, ibx);
		auto& ly = absmax(iry, igy, iby);
		auto lxx = lx;
		halfdiffright(lxx);
		auto lyy = ly;
		halfdiffbottom(lyy);
		return Differential(MakeImageGray(src), std::move(lx), std::move(ly), std::move(lxx), std::move(lyy));
	}
	else
	{
		auto& lx = sumrgb(irx, igx, ibx);
		auto& ly = sumrgb(iry, igy, iby);
		auto lxx = lx;
		halfdiffright(lxx);
		auto lyy = ly;
		halfdiffbottom(lyy);
		return Differential(MakeImageGray(src), std::move(lx), std::move(ly), std::move(lxx), std::move(lyy));
	}
}

/*! AbsMax half derivatives. The 2nd derivatives are not cached and will be approximated.
 * \param[in]	src	the image to derivate
 * \param[in]	proj	the method of projection from R, G and B derivatives to a single derivative
 */
Differential Differential::NewHalfDiffAbsMax(const ImageRGB &src, RGBProjection proj)
{
	auto irx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		irx.At(tmp) = src.At(tmp).r;
	auto itmp = irx;
	auto iry = irx;
	halfdiffleft(irx);
	halfdiffright(itmp);
	absmax(irx, itmp);
	itmp = iry;
	halfdifftop(iry);
	halfdiffbottom(itmp);
	absmax(iry, itmp);

	auto igx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		igx.At(tmp) = src.At(tmp).g;
	itmp = igx;
	auto igy = igx;
	halfdiffleft(igx);
	halfdiffright(itmp);
	absmax(igx, itmp);
	itmp = igy;
	halfdifftop(igy);
	halfdiffbottom(itmp);
	absmax(igy, itmp);

	auto ibx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		ibx.At(tmp) = src.At(tmp).b;
	itmp = ibx;
	auto iby = ibx;
	halfdiffleft(ibx);
	halfdiffright(itmp);
	absmax(ibx, itmp);
	itmp = iby;
	halfdifftop(iby);
	halfdiffbottom(itmp);
	absmax(iby, itmp);

	if (proj == RGBProjection::ABSMAX)
		return Differential(MakeImageGray(src), std::move(absmax(irx, igx, ibx)), std::move(absmax(iry, igy, iby)));
	else
		return Differential(MakeImageGray(src), std::move(sumrgb(irx, igx, ibx)), std::move(sumrgb(iry, igy, iby)));
}

/*! AbsMin half derivatives. The 2nd derivatives are not cached and will be approximated.
 * \param[in]	src	the image to derivate
 * \param[in]	proj	the method of projection from R, G and B derivatives to a single derivative
 */
Differential Differential::NewHalfDiffAbsMin(const ImageRGB &src, RGBProjection proj)
{
	auto irx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		irx.At(tmp) = src.At(tmp).r;
	auto itmp = irx;
	auto iry = irx;
	halfdiffleft(irx);
	halfdiffright(itmp);
	absmin(irx, itmp);
	itmp = iry;
	halfdifftop(iry);
	halfdiffbottom(itmp);
	absmin(iry, itmp);

	auto igx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		igx.At(tmp) = src.At(tmp).g;
	itmp = igx;
	auto igy = igx;
	halfdiffleft(igx);
	halfdiffright(itmp);
	absmin(igx, itmp);
	itmp = igy;
	halfdifftop(igy);
	halfdiffbottom(itmp);
	absmin(igy, itmp);

	auto ibx = ImageDoubleGray(src.GetWidth(), src.GetHeight());
	for (auto tmp : Range(src))
		ibx.At(tmp) = src.At(tmp).b;
	itmp = ibx;
	auto iby = ibx;
	halfdiffleft(ibx);
	halfdiffright(itmp);
	absmin(ibx, itmp);
	itmp = iby;
	halfdifftop(iby);
	halfdiffbottom(itmp);
	absmin(iby, itmp);

	if (proj == RGBProjection::ABSMAX)
		return Differential(MakeImageGray(src), std::move(absmax(irx, igx, ibx)), std::move(absmax(iry, igy, iby)));
	else
		return Differential(MakeImageGray(src), std::move(sumrgb(irx, igx, ibx)), std::move(sumrgb(iry, igy, iby)));
}

/*! Convolution with Gaussian derivatives
 * \throws	ExceptionDomain	sigma<0
 * \param[in]	src	the image to derivate
 * \param[in]	sigma	the standard deviation of the Gaussian (if null, the derivation convolutions are [-1 0 1] and [1 -2 1])
 * \return	a new Differential object
 */
Differential Differential::NewGaussian(const ImageDoubleGray &src, double sigma)
{
	auto diff = derivate2Gauss(src, sigma);
	return Differential(Downgrade<ImageGray>(src), std::move(std::get<0>(diff)), std::move(std::get<1>(diff)), std::move(std::get<2>(diff)), std::move(std::get<3>(diff)));
}

/*! Alternate half derivatives (left and top for 1st derivatives and right and bottom for 2nd derivatives)
 * \param[in]	src	the image to derivate
 * \return	a new Differential object
 */
Differential Differential::NewHalfDiff(const ImageDoubleGray &src)
{
	auto lx = src;
	halfdiffleft(lx);
	auto lxx = lx;
	halfdiffright(lxx);
	auto ly = src;
	halfdifftop(ly);
	auto lyy = ly;
	halfdiffbottom(lyy);
	return Differential(Downgrade<ImageGray>(src), std::move(lx), std::move(ly), std::move(lxx), std::move(lyy));
}

/*! AbsMax half derivatives. The 2nd derivatives are not cached and will be approximated.
 * \param[in]	src	the image to derivate
 * \return	a new Differential object
 */
Differential Differential::NewHalfDiffAbsMax(const ImageDoubleGray &src)
{
	auto lx = src;
	auto itmp = src;
	halfdiffleft(lx);
	halfdiffright(itmp);
	absmax(lx, itmp);
	auto ly = src;
	itmp = src;
	halfdifftop(ly);
	halfdiffbottom(itmp);
	absmax(ly, itmp);
	return Differential(Downgrade<ImageGray>(src), std::move(lx), std::move(ly));
}

/*! AbsMin half derivatives. The 2nd derivatives are not cached and will be approximated.
 * \param[in]	src	the image to derivate
 * \return	a new Differential object
 */
Differential Differential::NewHalfDiffAbsMin(const ImageDoubleGray &src)
{
	auto lx = src;
	auto itmp = src;
	halfdiffleft(lx);
	halfdiffright(itmp);
	absmin(lx, itmp);
	auto ly = src;
	itmp = src;
	halfdifftop(ly);
	halfdiffbottom(itmp);
	absmin(ly, itmp);
	return Differential(Downgrade<ImageGray>(src), std::move(lx), std::move(ly));
}

/*!
 * Updates the square gradient module image
 */
void Differential::updateLx2ly2()
{
	for (auto tmp : Range(lx2ly2))
		lx2ly2.At(tmp) = Sqr(GetLx().At(tmp)) + Sqr(GetLy().At(tmp));
	AutoThreshold();
}

/*****************************************************************************/
/*!
 * Returns the x second derivate.
 * Computes it if needed.
 * \return the newly created image
 */
const ImageDoubleGray& Differential::GetLxx()
{
	std::lock_guard<std::mutex> l(*lazydata);
	if (lxx.Size() != lx.Size())
	{
		lxx = GetLx();
		MatrixDouble sobx = MatrixDouble::NewGaussianLineDerivative(0);
		lxx.Convolve(sobx);
	}
	return lxx;
}

/*****************************************************************************/
/*!
 * Returns the y second derivate.
 * Computes it if needed.
 * \return the newly created image
 */
const ImageDoubleGray& Differential::GetLyy()
{
	std::lock_guard<std::mutex> l(*lazydata);
	if (lyy.Size() != ly.Size())
	{
		lyy = GetLy();
		MatrixDouble soby = MatrixDouble::NewGaussianLineDerivative(0);
		soby.Transpose();
		lyy.Convolve(soby);
	}
	return lyy;
}

/*****************************************************************************/
/*!
 * Returns the xy derivate.
 * Computes it if needed.
 * \return the newly created image
 */
const ImageDoubleGray& Differential::GetLxy()
{
	std::lock_guard<std::mutex> l(*lazydata);
	if (lxy.Size() != lx.Size())
	{
		lxy = GetLx();
		MatrixDouble soby = MatrixDouble::NewGaussianLineDerivative(0);
		soby.Transpose();
		lxy.Convolve(soby);
	}
	return lxy;
}

/*****************************************************************************/
/*!
 * Returns the yx derivate.
 * Computes it if needed.
 * \return the newly created image
 */
const ImageDoubleGray& Differential::GetLyx()
{
	std::lock_guard<std::mutex> l(*lazydata);
	if (lyx.Size() != ly.Size())
	{
		lyx = GetLy();
		MatrixDouble sobx = MatrixDouble::NewGaussianLineDerivative(0);
		lyx.Convolve(sobx);
	}
	return lyx;
}

/*****************************************************************************/
/*!
 * Returns the first derivate of the normal to the isophotes. sqrt(Lx²+Ly²).
 * Computes it if needed.
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeLw()
{
	auto lw = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
		lw.At(tmp) = sqrt(lx2ly2.At(tmp));
	return lw;
}

/*****************************************************************************/
/*!
 * Returns the second derivate of the tangent to the isophotes. (Lx²*Lyy + Ly²*Lxx - Lx*Ly*(Lxy + Lyx)) / (Lx² + Ly²)
 * Computes it if needed.
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeLvv()
{
	auto lvv = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		auto n = lx2ly2.At(tmp);
		if (n != 0.0)
			lvv.At(tmp) =  
					(Sqr(GetLx().At(tmp)) * GetLyy().At(tmp) 
					 + Sqr(GetLy().At(tmp)) * GetLxx().At(tmp)
					 - GetLx().At(tmp) * GetLy().At(tmp) * (GetLxy().At(tmp) + GetLyx().At(tmp)))
					/ n;
	}
	return lvv;
}

/*****************************************************************************/
/*!
 * Returns the second derivative of the normal to the isophotes. (Lx²*Lxx + Ly²*Lyy + Lx*Ly*(Lxy + Lyx)) / (Lx² + Ly²)
 * Computes it if needed.
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeLww()
{
	auto lww = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		double n = lx2ly2.At(tmp);
		if (n != 0.0)
			lww.At(tmp) =
					(Sqr(GetLx().At(tmp)) * GetLyy().At(tmp) 
					 + Sqr(GetLy().At(tmp)) * GetLxx().At(tmp)
					 + GetLx().At(tmp) * GetLy().At(tmp) * (GetLxy().At(tmp) + GetLyx().At(tmp)))
					/ n;
	}
	return lww;
}

/*****************************************************************************/
/*!
 * Returns the cross derivative of the tangent and the normal to the isophotes. (Lx*Ly*(Lyy-Lxx) + Lxy*Lx² - Lyx*Ly²) / (Lx² + Ly²)
 * Computes it if needed.
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeLvw()
{
	auto lvw = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		auto n = lx2ly2.At(tmp);
		if (n != 0.0)
			lvw.At(tmp) =
					(GetLx().At(tmp) * GetLy().At(tmp) * (GetLyy().At(tmp) - GetLxx().At(tmp))
					 + GetLxy().At(tmp) * Sqr(GetLx().At(tmp))
					 - GetLyx().At(tmp) * Sqr(GetLy().At(tmp))
					)
					/ n;
	}
	return lvw;
}

/*****************************************************************************/
/*!
 * Returns the laplacian image. Lxx + Lyy
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeLaplacian()
{
	auto tmpimg = GetLxx();
	for (auto tmp : Range(lx2ly2))
		tmpimg.At(tmp) += GetLyy().At(tmp);
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the isophote curvature image. Lvv / Lw
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeIsophoteCurvature()
{
	auto tmpimg = MakeLvv();
	for (auto tmp : Range(lx2ly2))
	{
		const auto n = lx2ly2.At(tmp);
		if (n != 0.0)
			tmpimg.At(tmp) /= sqrt(n);
	}
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the flowline curvature image. Lvw / Lw
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeFlowlineCurvature()
{
	auto tmpimg = MakeLvw();
	for (auto tmp : Range(lx2ly2))
	{
		auto n = lx2ly2.At(tmp);
		if (n != 0.0)
			tmpimg.At(tmp) /= sqrt(n);
	}
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the edge image. Lww / Lw
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeEdge()
{
	auto tmpimg = MakeLww();
	for (auto tmp : Range(lx2ly2))
	{
		const auto n = lx2ly2.At(tmp);
		if (n != 0.0)
			tmpimg.At(tmp) /= sqrt(n);
	}
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the corner image. Lvv * Lw²
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeCorner()
{
	auto tmpimg = MakeLvv();
	for (auto tmp : Range(lx2ly2))
		tmpimg.At(tmp) *= lx2ly2.At(tmp);
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the Gaussian curvature image. Lxx*Lyy - Lxy*Lyx
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeGaussianCurvature()
{
	auto tmpimg = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
		tmpimg.At(tmp) = GetLxx().At(tmp) * GetLyy().At(tmp) - GetLxy().At(tmp) * GetLyx().At(tmp);
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Returns the gradient module image
 *
 * \return the newly created image, nullptr if failed.
 */
ImageDoubleGray Differential::MakeGradientModule()
{
	return MakeLw();
}

/*****************************************************************************/
/*!
 * Returns the gradient curvature image
 *
 * \return the newly created image.
 */
ImageGray Differential::MakeGradientCurvature()
{
	return MakeImageGradient().MakeCurvature();
}

/*****************************************************************************/
/*!
 * Returns the gradient image
 *
 * \return the newly created image
 */
ImageGradient Differential::MakeImageGradient()
{
	auto img = ImageGradient(GetLx().GetWidth(), GetLx().GetHeight());
	for (auto i : Range(img))
		img.At(i) = pixel::Cart2D<double>(GetLx().At(i), GetLy().At(i));
	img.SetMinModule((int)sqrt(thres));
	return img;
}

/*! Returns the first eigenvalue of the Hessian as an image. (Lxx+Lyy + sqrt(|(Lxx+Lyy)² - 4*(Lxx*Lyy - Lxy*Lyx)|)) / 2
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeKappa1()
{
	auto tmpimg = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		auto lxxlyy = GetLxx().At(tmp) + GetLyy().At(tmp);
		tmpimg.At(tmp) = 
				(lxxlyy + sqrt(Abs(
													 Sqr(lxxlyy) - 4 * (
														 GetLxx().At(tmp) * GetLyy().At(tmp)
														 -GetLxy().At(tmp) * GetLyx().At(tmp))
													))
				) / 2;
	}
	return tmpimg;
}

/*! Returns the second eigenvalue of the Hessian as an image. (Lxx+Lyy - sqrt(|(Lxx+Lyy)² - 4*(Lxx*Lyy - Lxy*Lyx)|)) / 2
 *
 * \return the newly created image
 */
ImageDoubleGray Differential::MakeKappa2()
{
	auto tmpimg = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		auto lxxlyy = GetLxx().At(tmp) + GetLyy().At(tmp);
		tmpimg.At(tmp) = 
				(lxxlyy - sqrt(Abs(
													 Sqr(lxxlyy) - 4 * (
														 GetLxx().At(tmp) * GetLyy().At(tmp)
														 -GetLxy().At(tmp) * GetLyx().At(tmp))
													))
				) / 2;
	}
	return tmpimg;
}

/*! Returns the Hessian corner image. Lxx*Lyy - Lxy*Lyx - s * (Lxx+Lyy)²
 * \param[in]	s	a value between 0.04 and 0.15
 * \return	the newly created image
 */
ImageDoubleGray Differential::MakeHessianCorner(double s)
{
	auto tmpimg = ImageDoubleGray(lx2ly2.GetWidth(), lx2ly2.GetHeight());
	for (auto tmp : Range(lx2ly2))
	{
		tmpimg.At(tmp) = GetLxx().At(tmp) * GetLyy().At(tmp)
				- GetLxy().At(tmp) * GetLyx().At(tmp)
				- s * Sqr(GetLxx().At(tmp) + GetLyy().At(tmp));
	}
	return tmpimg;
}

/*****************************************************************************/
/*!
 * Computes the best mingrad value
 *
 * \return	The computed minimal gradient magnitude value
 * \todo find the right division factor for the deviation
 */
double Differential::AutoThreshold()
{
	int max = 0;
	for (auto tmp : Range(srcigray))
	{
		if (srcigray.At(tmp) > max)
		{
			max = srcigray.At(tmp);
		}
	}
	double S1 = 0, S2 = 0, C1 = 0, C2 = 0;
	for (auto tmp : Range(srcigray))
	{
		int w1 = srcigray.At(tmp); // weight on light pixels
		int w2 = max - w1; // weight on dark pixels
		S1 += lx2ly2.At(tmp) * w1;
		S2 += lx2ly2.At(tmp) * w2;
		C1 += w1;
		C2 += w2;
	}

	double m1 = S1 / C1;
	double m2 = S2 / C2;
	thres = (m1 + m2) / 2;

	return thres;
}

/*****************************************************************************/
/*! 
 * Diffuses the gradient.
 * All other buffers are freed in order to be recomputed if needed.
 * \author	Yann LEYDIER
 *
 * \param[in]		maxiter	Maximum number of iterations
 * \param[in]		maxdiv	Maximum divergence allowed to modify a pixel, value in ]0.0, 2.0] or >2 to always modify
 */
void Differential::Diffuse(size_t maxiter, double maxdiv)
{
	// clear second derivatives
	lxx = lxy = lyy = ImageDoubleGray{};

	ImageDoubleGray tmplx = lx;
	ImageDoubleGray tmply = ly;
	for (size_t iter = 0; iter < maxiter; ++iter)
	{
		ImageDoubleGray div;
		if (maxdiv <= 2.0)
			div = MakeDivergence();
		for (size_t y = 1; y < lx.GetHeight() - 1; ++y)
			for (size_t x = 1; x < lx.GetWidth() - 1; ++x)
			{
				auto cond = true;
				if (maxdiv <= 2.0)
					cond = Abs(div.At(x, y)) < maxdiv;
				if (cond)
				{
					double gx = (1.0/16.0) * (lx.At(x - 1, y - 1) + lx.At(x - 1, y + 1) + 
							lx.At(x + 1, y - 1) + lx.At(x + 1, y + 1)) +
						(2.0/16.0) * (lx.At(x - 1, y) + lx.At(x + 1, y) + 
								lx.At(x, y - 1) + lx.At(x, y + 1)) +
						(4.0/16.0) * lx.At(x, y);
					double gy = (1.0/16.0) * (ly.At(x - 1, y - 1) + ly.At(x - 1, y + 1) + 
							ly.At(x + 1, y - 1) + ly.At(x + 1, y + 1)) +
						(2.0/16.0) * (ly.At(x - 1, y) + ly.At(x + 1, y) + 
								ly.At(x, y - 1) + ly.At(x, y + 1)) +
						(4.0/16.0) * ly.At(x, y);
					tmplx.At(x, y) = gx;
					tmply.At(x, y) = gy;
					lx2ly2.At(x, y) = Sqr(gx) + Sqr(gy);
				}
			}
		std::swap(lx, tmplx);
		std::swap(ly, tmply);
	}

	AutoThreshold();
}

/*!
 * Creates a continuous skeleton from gradient divergence.
 *
 * \return	an image of the skeleton
 */
ImageDoubleGray Differential::MakeDivergence()
{
	const size_t w = GetLx().GetWidth();
	auto axe = ImageDoubleGray(w, GetLx().GetHeight(), 0.0);
	// precompute normalized derivatives
	auto precline = std::vector<Point2DDouble>(w);
	auto currline = std::vector<Point2DDouble>(w);
	for (size_t x = 1; x < w - 1; ++x)
	{
		auto n = sqrt(GetLx2Ly2().At(x, 0));
		precline[x].X = GetLx().At(x, 0) / n;
		precline[x].Y = GetLy().At(x, 0) / n;
		n = sqrt(GetLx2Ly2().At(x, 1));
		currline[x].X = GetLx().At(x, 1) / n;
		currline[x].Y = GetLy().At(x, 1) / n;
	}
	auto nextline = std::vector<Point2DDouble>(w);
	// compute divergence (cannot be parallelized!)
	for (size_t y = 1; y < GetLx().GetHeight() - 1; ++y)
	{
		for (size_t x = 1; x < w - 1; ++x)
		{
			// update normalized derivatives
			auto n = sqrt(GetLx2Ly2().At(x, y + 1));
			nextline[x].X = GetLx().At(x, y + 1) / n;
			nextline[x].Y = GetLy().At(x, y + 1) / n;

			// compute divergence
			auto dx = AbsMaxSameSign(currline[x + 1].Y - currline[x].Y, currline[x].Y - currline[x - 1].Y);
			auto dy = AbsMaxSameSign(nextline[x].X - currline[x].X, currline[x].X - precline[x].X);
			axe.At(x, y) = dx + dy;
		}
		// shift normalized derivatives
		precline.swap(currline);
		currline.swap(nextline);
	}
	return axe;
}

static inline double norm(double a, double b)
{
	return sqrt(Sqr(a) + Sqr(b));
}
/*!
 * Canny's edge detector image
 *
 * \return Canny's edge detector image
 */
ImageDoubleGray Differential::MakeCanny()
{
	auto l1 = ImageDoubleGray(GetLx().GetWidth(), GetLx().GetHeight());
	for (size_t i = 0; i < GetLx().GetHeight(); i++)
	{
		for (size_t j = 0; j < GetLx().GetWidth(); j++)
		{
			double xc = GetLx().At(j, i);
			double yc = GetLy().At(j, i);
			if ((Abs(xc) < 0.01) && (Abs(yc) < 0.01)) 
				continue;
			double g = norm (xc, yc);
			double g1, g2, g3, g4, xx, yy;

			/* Follow the gradient direction, as indicated by the direction of
				 the vector (xc, yc); retain pixels that are a local maximum. */
			if (Abs(yc) > Abs(xc))
			{
				/* The Y component is biggest, so gradient direction is basically UP/DOWN */
				xx = Abs(xc)/Abs(yc);
				yy = 1.0;

				g2 = norm(GetLx().At(j, i-1), GetLy().At(j, i-1));
				g4 = norm(GetLx().At(j, i+1), GetLy().At(j, i+1));
				if (xc * yc > 0.0)
				{
					g3 = norm(GetLx().At(j+1, i+1), GetLy().At(j+1, i+1));
					g1 = norm(GetLx().At(j-1, i-1), GetLy().At(j-1, i-1));
				} 
				else
				{
					g3 = norm(GetLx().At(j-1, i+1), GetLy().At(j-1, i+1));
					g1 = norm(GetLx().At(j+1, i-1), GetLy().At(j+1, i-1));
				}

			} 
			else
			{
				/* The X component is biggest, so gradient direction is basically LEFT/RIGHT */
				xx = Abs(yc) / Abs(xc);
				yy = 1.0;

				g2 = norm(GetLx().At(j+1, i), GetLy().At(j+1, i));
				g4 = norm(GetLx().At(j-1, i), GetLy().At(j-1, i));
				if (xc * yc > 0.0)
				{
					g3 = norm(GetLx().At(j-1, i-1), GetLy().At(j-1, i-1));
					g1 = norm(GetLx().At(j+1, i+1), GetLy().At(j+1, i+1));
				}
				else
				{
					g1 = norm(GetLx().At(j+1, i-1), GetLy().At(j+1, i-1));
					g3 = norm(GetLx().At(j-1, i+1), GetLy().At(j-1, i+1));
				}
			}

			/* Compute the interpolated value of the gradient magnitude */
			if ( (g > (xx*g1 + (yy-xx)*g2)) && (g > (xx*g3 + (yy-xx)*g4)) )
			{
				l1.At(j, i) = g;
			} 
			else
			{
				l1.At(j, i) = 0;
			}

		}
	} 
	return l1;
}

