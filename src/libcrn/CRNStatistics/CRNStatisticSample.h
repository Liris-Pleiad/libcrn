/* Copyright 2014-2015 INSA-Lyon
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
 * file: CRNUnivariateStatisticSample.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNSTATISTICSAMPLE_HEADER
#define CRNSTATISTICSAMPLE_HEADER

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <tuple>

#include <CRNMath/CRNMath.h>

	/*! \defgroup stats Statistics
	 * \ingroup	math */

namespace crn
{
	class Histogram;
	class UnivariateGaussianMixture;
	class MultivariateGaussianMixture;

	/*! \addtogroup stats */
	/*@{*/

	/****************************************************************************/
	/*! \brief Data analysis
	 *
	 * Data analysis
	 * 
	 * \author 	Jean DUONG, Yann LEYDIER
	 * \date	Dec 2014
	 * \version	0.1
	 */

	/*! \brief Returns max */
	template<typename T> inline T Max(const std::vector<T> &v) { return *std::max_element(v.begin(), v.end()); }
	/*! \brief Returns max */
	template<typename T> inline T Max(const std::vector<std::vector<T>> &m)
	{
		auto ma = m.front().front();
		for (const auto &row : m)
		{
			auto rmax = *std::max_element(row.begin(), row.end());
			if (rmax > ma) ma = rmax;
		}
		return ma;
	}
	/*! \brief Returns min */
	template<typename T> inline T Min(const std::vector<T> &v) { return *std::min_element(v.begin(), v.end()); }
	/*! \brief Returns min */
	template<typename T> inline T Min(const std::vector<std::vector<T>> &m)
	{
		auto mi = m.front().front();
		for (const auto &row : m)
		{
			auto rmin = *std::max_element(row.begin(), row.end());
			if (rmin < mi) mi = rmin;
		}
		return mi;
	}
	/*! \brief Returns min and max */
	template<typename T> inline std::tuple<T, T> MinMax(const std::vector<T> &v)
	{
		auto res = std::minmax_element(v.begin(), v.end());
		return std::make_tuple(*res.first, *res.second);
	}
	/*! \brief Returns min and max */
	template<typename T> inline std::tuple<T, T> MinMax(const std::vector<std::vector<T>> &m)
	{
		auto mi = m.front().front();
		auto ma = m.front().front();
		for (const auto &row : m)
		{
			auto rmM = std::minmax_element(row.begin(), row.end());
			if (*rmM.first < mi) mi = *rmM.first;
			if (*rmM.second > ma) ma = *rmM.second;
		}
		return std::make_tuple(mi, ma);
	}
	/*! \brief Return index of a maximal */
	template<typename T> inline size_t Argmax(const std::vector<T> &v) { return std::max_element(v.begin(), v.end()) - v.begin(); }
	/*! \brief Return index of a minimal */
	template<typename T> inline size_t Argmin(const std::vector<T> &v) { return std::min_element(v.begin(), v.end()) - v.begin(); }
	/*! \brief Return index of a maximal on a column */
	template<typename T> size_t ColumnArgmax(const std::vector<std::vector<T>> &m, size_t col)
	{
		auto ma = m.front().front();
		auto index = size_t(0);
		for (size_t tmp = 1; tmp < m.size(); ++tmp)
			if (m[tmp][col] > ma)
			{
				ma = m[tmp][col];
				index = tmp;
			}
		return index;
	}
	/*! \brief Return index of a minimal on a column */
	template<typename T> size_t ColumnArgmin(const std::vector<std::vector<T>> &m, size_t col)
	{
		auto mi = m.front().front();
		auto index = size_t(0);
		for (size_t tmp = 1; tmp < m.size(); ++tmp)
			if (m[tmp][col] < mi)
			{
				mi = m[tmp][col];
				index = tmp;
			}
		return index;
	}

	/*! \brief Return mean value of sample */
	double Mean(const std::vector<double> &v);
	/*! \brief Return mean value of sample
	 *
	 * \warning	Use only with small ranges. For big samples, use double version.
	 * \warning	Integral sequences will return an integral mean.
	 * \param[in]	be	begin iterator
	 * \param[in]	en	end iterator
	 * \return	the mean value
	 */
	template<typename ITER> typename std::iterator_traits<ITER>::value_type Mean(ITER be, ITER en)
	{
		using value_type = typename std::iterator_traits<ITER>::value_type;
		auto cumul = std::accumulate(be, en, SumType<value_type>(0));
		cumul /= SumType<value_type>(std::distance(be, en));
		return value_type(cumul);
	}
	/*! \brief Return mean value of sample as a double value
	 *
	 * \param[in]	be	begin iterator
	 * \param[in]	en	end iterator
	 * \return	the mean value
	 */
	template<typename ITER> double MeanAsDouble(ITER be, ITER en)
	{
		const auto s = double(std::distance(be, en));
		auto m = std::accumulate(be, en, 0.0);

		if (!isinf(m))
			return m / s;
		else
		{
			m = 0.0;
			for (auto it = be; it != en; ++it)
				m += double(*it) / s;
			return m;
		}
	}

	/*! \brief Return mean pattern of sample */
	std::vector<double> MeanPattern(const std::vector<std::vector<double>> &m);

	/*!
	 * Average pattern
	 *
	 * \param[in] 	it_begin	iterator pointing to first double-size_t pair of input data
	 * \param[in] 	it_end      ending iterator
	 * \return      mean pattern of sample
	 */
	template<typename ITER> std::vector<double> MeanPattern(ITER it_begin, ITER it_end)
	{
		auto cardinal = 0.0;
		auto dimension = it_begin->first.size();
		auto pattern = std::vector<double>(dimension, 0.0);

		for (auto it = it_begin; it != it_end; ++it)
		{
			const auto& pat = it->first;

			if (pat.size() == dimension)
			{
				const auto weight = double(it->second);

				for (size_t k = 0; k < dimension; ++k)
					pattern[k] +=  weight * pat[k];

				cardinal += weight;
			}
		}

		auto valid = true;

		for (size_t k = 0; k < dimension; ++k)
			if (std::isinf(pattern[k]))
			{
				valid = false;
				break;
			}

		if (valid)
			for (size_t k = 0; k < dimension; ++k)
				pattern[k] /= cardinal;
		else
		{
			for (size_t k = 0; k < dimension; ++k)
				pattern[k] = 0.0;

			for (auto it = it_begin; it != it_end; ++it)
				if (it->first.size() == dimension)
				{
					const auto scale = double(it->second) / double(cardinal);
					const auto& pt = it->first;

					for (size_t k = 0; k < dimension; ++k)
						pattern[k] += pt[k] * scale;
				}
		}

		return pattern;
	}

	/*! \brief Return deviation of sample */
	double StdDeviation(const std::vector<double> &v);
	/*! \brief Return variance of sample */
	double Variance(const std::vector<double> &v);
	/*! \brief Return covariance for sample */
	std::vector<std::vector<double>> MakeCovariance(const std::vector<std::vector<double>> &m);

	template<typename ITER> std::vector<std::vector<double>> MakeCovariance(ITER it_begin, ITER it_end)
	{
		auto card = 0.0;
		const auto dim = it_begin->first.size();
		auto cov = std::vector<std::vector<double>>(dim, std::vector<double>(dim));

		for (auto it = it_begin; it != it_end; ++it)
			card += double(it->second);

		for (size_t i = 0; i < dim; ++i)
			for (auto j = i; j < dim; ++j)
			{
				for (auto it = it_begin; it != it_end; ++it)
					cov[i][j] += it->first[i] * it->first[j] * double(it->second);

				cov[i][j] /= double(card);

				if (j != i)
					cov[j][i] = cov[i][j];
			}

		return cov;
	}

	/*! \brief Return mean, variance and standard deviation of sample */
	std::tuple<double, double, double> MeanVarDev(const std::vector<double> &v);
	/*!
	 * Mean, variance and deviation for a sample
	 *
	 * \param[in] 	it_begin	iterator pointing to first pair of input data
	 * \param[in] 	it_end      ending iterator
	 * \return      mean pattern of sample
	 */
	template<typename ITER> std::tuple<double, double, double> MeanVarDev(ITER it_begin, ITER it_end)
	{
		// Compute cumul and cumul of squares in a same loop
		auto m = 0.0;
		auto m_2 = 0.0;
		auto s = 0.0;

		for (auto it = it_begin; it != it_end; ++it)
		{
			auto val = it->first;
			auto cnt = it->second;

			m += val * cnt;
			m_2 += Sqr(val) * cnt;
			s += double(cnt);
		}

		if (!std::isinf(m))
		{
			m /= s;
			m_2 /= s;
		}
		else // If cumul too large, re-compute mean with values downscaled by cardinal
		{
			m = 0.0;
			m_2 = 0.0;

			for (auto it = it_begin; it != it_end; ++it)
			{
				auto val = it->first;
				auto cnt = it->second;

				m += val * cnt / s;
				m_2 += Sqr(val) * cnt / s;
			}
		}

		if (!std::isinf(m_2))
		{
			auto var = m_2 - Sqr(m);
			return std::make_tuple(m, var, sqrt(var));
		}
		else // Konig-Huygens formula for variance cannot be used (too large values in cumul of squares)
		{
			auto var = 0.0;

			for (auto it = it_begin; it != it_end; ++it)
				var += Sqr(it->first - m) * it->second;

			if (!std::isinf(var))
				var /= s;
			else
			{
				var = 0.0;

				for (auto it = it_begin; it != it_end; ++it)
					var += Sqr(it->first - m) * it->second / s;
			}

			return std::make_tuple(m, var, sqrt(var));
		}
	}

	/*! \brief Return quantile values of sample */
	std::vector<double> Quantiles(const std::vector<double> &v, size_t q, bool sort_flag = true);

	/*! \brief Median value */
	template<typename T> T MedianValue(const std::vector<T> &v)
	{
		auto sv = v;
		std::sort(sv.begin(), sv.end());
		return sv[sv.size() / 2];
	}

	/*! \brief Test if all data values are equal */
	template<typename T> inline bool AllEqual(const std::vector<T> &v) { return std::all_of(v.begin(), v.end(), [&v](const T &x){ return x == v.front(); }); }
	/*! \brief Test if all data values are equal */
	template<typename T> inline bool AllEqual(const std::vector<std::vector<T>> &m)
	{
		auto refval = m.front().front();
		for (const auto &row : m)
			if (std::any_of(row.begin(), row.end(), [refval](const T &x){ return x != refval; }))
				return false;
		return true;
	}

	/*! \brief Returns count histogram */
	Histogram MakeHistogram(const std::vector<double> &v, size_t nb_bins);
	/*! \brief Returns count histogram (#bins = sqrt(pop) ) */
	Histogram MakeHistogramSquareRoot(const std::vector<double> &v);
	/*! \brief Returns count histogram (#bins = 1+log_2(pop) ) */
	Histogram MakeHistogramSturges(const std::vector<double> &v);
	/*! \brief Returns count histogram (#bins = 2n^(1/3) ) */
	Histogram MakeHistogramRice(const std::vector<double> &v);
	/*! \brief Returns count histogram (bin width = 3.5 * stddev / pop^(1/3)) */
	Histogram MakeHistogramScott(const std::vector<double> &v);
	/*! \brief Returns count histogram (bin width = 2 * IQR(v) / pop^(1/3)) */
	Histogram MakeHistogramFreedmanDiaconis(const std::vector<double> &v, bool sort_flag = true);

	/*! \brief Return Gaussian mixture model modeling current (univariate) sample */
	UnivariateGaussianMixture MakeGaussianMixtureModel(const std::vector<double> &v, size_t nb_seeds = 2);
	/*! \brief Return Gaussian mixture model modeling current (multivariate) sample */
	MultivariateGaussianMixture MakeGaussianMixtureModel(const std::vector<std::vector<double>> &patterns, size_t nb_seeds = 2);

	/*@}*/

}

#endif

