/* Copyright 2015 Université Paris Descartes
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
 * file: CRNGradientShapeContext.h
 * \author Yann LEYDIER
 */

#ifndef CRNGradientShapeContext_H
#define CRNGradientShapeContext_H

#include <CRNImage/CRNImageGradient.h>
#include <CRNGeometry/CRNPoint2DInt.h>
#include <CRNAI/CRNkMedoids.h>
#include <CRNAI/CRNBipartite.h>
#include <array>
#include <vector>
#include <algorithm>

namespace crn
{
	/****************************************************************************/
	/*! \brief Gradient shape context factory
	 *
	 * \author 	Yann LEYDIER
	 * \date		October 2015
	 * \version 0.1
	 */
	template<size_t ngrad, size_t ntheta, size_t nrho> class GradientShapeContext
	{
		public:
			static constexpr auto size = ngrad * ntheta * nrho;
			static constexpr auto toffset = ngrad;
			static constexpr auto roffset = toffset * ntheta;

			struct SC
			{
				SC(): X(0), Y(0) { std::fill(histo.begin(), histo.end(), 0); }
				SC(const Point2DInt &p): X(p.X), Y(p.Y) { std::fill(histo.begin(), histo.end(), 0); }
				SC(const SC&) = default;
				SC(SC&&) = default;
				SC& operator=(const SC&) = default;
				SC& operator=(SC&&) = default;
				int X, Y;
				std::array<int, size> histo;
			};

			static std::vector<SC> CreateFixed(const ImageGradient &igr, size_t npoints, size_t ndummy);
			static std::vector<SC> CreateRatio(const ImageGradient &igr, size_t divisor, size_t ndummy);
			static double Distance(const std::vector<SC> &img1, const std::vector<SC> &img2);

		private:
			template<typename Sc> static int dist(const Sc &p1, const Sc &p2);
	};

	template<size_t ngrad, size_t ntheta, size_t nrho> 
		std::vector<typename GradientShapeContext<ngrad, ntheta, nrho>::SC> GradientShapeContext<ngrad, ntheta, nrho>::CreateFixed(const ImageGradient &igr, size_t npoints, size_t ndummy)
		{
			// collect significant points
			auto pts = std::vector<Point2DInt>{};
			FOREACHPIXEL(x, y, igr)
				if (igr.IsSignificant(x, y))
					pts.emplace_back(int(x), int(y));
			auto distmat = std::vector<std::vector<double>>(pts.size(), std::vector<double>(pts.size(), 0));
			for (auto p1 : Range(pts))
				for (auto p2 = p1 + 1; p2 < pts.size(); ++p2)
					distmat[p1][p2] = distmat[p2][p1] = sqrt(Sqr(pts[p1].X - pts[p2].X) + Sqr(pts[p1].Y - pts[p2].Y));
			// select a few points
			auto meds = std::vector<size_t>{};
			if (pts.size() <= npoints)
			{
				meds.reserve(pts.size());
				std::iota(meds.begin(), meds.end(), 0);
			}
			else
				meds = std::move(std::get<2>(kmedoids::Run(kmedoids::init::Central{npoints}, kmedoids::update::Local{}, distmat, 100)));

			// compute context for the selected points
			auto res = std::vector<SC>{};
			for (auto tp : Range(meds))
			{
				const auto p = meds[tp];
				auto refp = SC{pts[p]};
				const auto maxdist = log(1 + *std::max_element(distmat[p].begin(), distmat[p].end())) + 1;
				for (auto p2 : Range(pts))
				{
					if (p == p2)
						continue;
					const auto &op = pts[p2];
					const auto vec = pixel::Polar2D<double, Angle<ByteAngle>>(pixel::Cart2D<int>(refp.X - op.X, refp.Y - op.Y));
					const auto r = size_t(log(1 + vec.rho) * nrho / maxdist);
					const auto t = size_t(vec.theta.value * ntheta / 256);
					refp.histo[r * roffset + t * toffset + igr.At(op.X, op.Y).theta.value * ngrad / 256] += 1;
				}
				// normalize
				for (auto &g : refp.histo)
				{
					g = (g * 1000) / int(pts.size());
				}

				res.push_back(std::move(refp));
			}
			// add dummy points
			res.insert(res.end(), ndummy, SC{});
			return res;
		}

	template<size_t ngrad, size_t ntheta, size_t nrho> 
		std::vector<typename GradientShapeContext<ngrad, ntheta, nrho>::SC> GradientShapeContext<ngrad, ntheta, nrho>::CreateRatio(const ImageGradient &igr, size_t divisor, size_t ndummy)
		{
			auto npoints = size_t(0);
			for (auto tmp : Range(igr))
				if (igr.IsSignificant(tmp))
					npoints += 1;
			return CreateFixed(igr, crn::Max(size_t(1), npoints / divisor), ndummy);
		}

	template<size_t ngrad, size_t ntheta, size_t nrho> 
		double GradientShapeContext<ngrad, ntheta, nrho>::Distance(const std::vector<typename GradientShapeContext<ngrad, ntheta, nrho>::SC> &img1, const std::vector<typename GradientShapeContext<ngrad, ntheta, nrho>::SC> &img2)
		{
			const auto s = Max(img1.size(), img2.size());
			auto distmat = std::vector<std::vector<double>>(s, std::vector<double>(s, 0));
			for (auto p1 : Range(img1))
				for (auto p2 : Range(img2))
					distmat[p1][p2] = dist(img1[p1], img2[p2]);
			return std::get<0>(Hungarian(distmat)) / double(Min(img1.size(), img2.size()));
		}

	template<size_t ngrad, size_t ntheta, size_t nrho> 
		template<typename Sc> int GradientShapeContext<ngrad, ntheta, nrho>::dist(const Sc &p1, const Sc &p2)
		{
			if (p1.X == 0 && p1.Y == 0)
				return 0;
			if (p2.X == 0 && p2.Y == 0)
				return 0;
			auto diff = 0;
			for (auto tmp : Range(p1.histo))
				diff += Abs(p1.histo[tmp] - p2.histo[tmp]);
			return diff;
		}
}

#endif

