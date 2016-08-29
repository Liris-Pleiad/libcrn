/* Copyright 2014-2015 INSA Lyon
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
 * file: CRNStatisticSample.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNException.h>
#include <CRNMath/CRNMath.h>
#include <CRNStatistics/CRNHistogram.h>
#include <CRNStatistics/CRNStatisticSample.h>
#include <CRNMath/CRNUnivariateGaussianMixture.h>
#include <CRNMath/CRNMultivariateGaussianMixture.h>
#include <CRNStringUTF8.h>
#include <functional>
#include <limits>
#include <CRNi18n.h>

using namespace crn;
using namespace std::placeholders;

/*!
 * \param[in]	v	a vector of doubles
 * \return	mean value of sample
 */
double crn::Mean(const std::vector<double> &v)
{
	const auto s = double(v.size());
	auto m = std::accumulate(v.begin(), v.end(), 0.0);

	if (!std::isinf(m))
		return m / s;
	else
	{
		m = 0.0;
		for (auto x : v)
			m += x / s;
		return m;
	}
}

/*!
 * \param[in]	m	a vector of vector doubles
 * \return mean pattern of sample
 */
std::vector<double> crn::MeanPattern(const std::vector<std::vector<double>> &m)
{
	auto mp = std::vector<double>(m.front().size(), 0.0);

	for (const auto &row : m)
		for (auto k : Range(row))
			mp[k] += row[k];

	auto valid = true;
	for (auto k : Range(mp))
		if (std::isinf(mp[k]))
		{
			valid = false;
			break;
		}

	if (valid)
		std::transform(mp.begin(), mp.end(), mp.begin(), std::bind(std::divides<double>{}, _1, double(m.size())));
	else
	{
		auto s = double(m.size());
		std::vector<double>(m.front().size(), 0.0).swap(mp);

		for (const auto &row : m)
			for (auto k : Range(row))
				mp[k] += row[k] / s;
	}

	return mp;
}

/*!
 * \param[in]	v	a vector of doubles
 * \return deviation of sample
 */
double crn::StdDeviation(const std::vector<double> &v)
{
	return sqrt(Variance(v));
}

/*!
 * \param[in]	v	a vector of doubles
 * \return variance of sample
 */
double crn::Variance(const std::vector<double> &v)
{
	const auto m = Mean(v);
	auto var = 0.0;
	const auto s = double(v.size());

	for (double d : v)
		var += Sqr(d - m);

	if (!std::isinf(var))
		var /= s;
	else
	{
		var = 0.0;
		for (double d : v)
			var += Sqr(d - m) / s;
	}

	return var;
}

/*!
 * \param[in]	m	a matrix of doubles
 * \return covariance for sample
 */
std::vector<std::vector<double>> crn::MakeCovariance(const std::vector<std::vector<double>> &m)
{
	const auto card = m.size();
	const auto dim = m.front().size();
	auto cov = std::vector<std::vector<double>>(dim, std::vector<double>(dim));

	for (size_t i = 0; i < dim; ++i)
		for (auto j = i; j < dim; ++j)
		{
			for (size_t k = 0; k < card; ++k)
				cov[i][j] += m[k][i] * m[k][j];
			cov[i][j] /= double(card); 

			if (j != i)
				cov[j][i] = cov[i][j];
		}
	return cov;
}

/*!
 * \param[in]	v	a vector of doubles
 * \return	mean, variance and standard deviation of sample
 */
std::tuple<double, double, double> crn::MeanVarDev(const std::vector<double> &v)
{
	// Compute cumul and cumul of squares in a same loop
	auto m = 0.0;
	auto m_2 = 0.0;
	const auto s = double(v.size());

	for (double x : v)
	{
		m += x;
		m_2 += Sqr(x);
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

		for (double x : v)
		{
			m += x / s;
			m_2 += Sqr(x) / s;
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
		for (double x : v)
			var += Sqr(x - m);

		if (!std::isinf(var))
			var /= s;
		else
		{
			var = 0.0;
			for (double x : v)
				var += Sqr(x - m) / s;
		}
		return std::make_tuple(m, var, sqrt(var));
	}
}

/*!
 * Get quantile values
 *
 * \throws	ExceptionDomain	illegal range
 *
 * \param[in]   v   statistic sample
 * \param[in]   q   number of bins required
 * \param[in]   sort_flag   shall the statistic sample be sorted?
 *
 * \return quantile values of sample
 */
std::vector<double> crn::Quantiles(const std::vector<double> &v, size_t q, bool sort_flag)
{
	if ((q < 3) || (q > v.size()))
		throw ExceptionDomain(StringUTF8("StatisticSample::Quantiles(const std::vector<double>&, size_t, bool): ") + _("Illegal range."));

	auto qt = std::vector<double>(q - 1);
	const auto jump = v.size() / q;
	auto index = jump;

	if (sort_flag)
	{
		auto w = v;
		std::sort(w.begin(), w.end());
		for(size_t k = 0; k < q - 1; ++k)
		{
			qt[k] = w[index];
			index += jump;
		}
	}
	else
		for(size_t k = 0; k < q - 1; ++k)
		{
			qt[k] = v[index];
			index += jump;
		}

	return qt;
}

/*!
 * \param[in]	v	a vector of doubles
 * \param[in]	nb_bins	the desired size of the histogram
 * \returns count histogram
 */
Histogram crn::MakeHistogram(const std::vector<double> &v, size_t nb_bins)
{
	auto mM = MinMax(v);
	auto left_bound = std::get<0>(mM);
	auto right_bound = std::get<1>(mM);
	auto range = right_bound - left_bound;
	auto delta = range / double(nb_bins);
	auto h = Histogram(nb_bins);
	if (nb_bins == 1) // trivial case
		h.SetBin(0, (unsigned int)v.size());
	else
		for (auto d : v)
		{			
			auto id = size_t((d - left_bound) / delta);
			if (id >= nb_bins)
				id = nb_bins - 1;
			h.IncBin(id);
		}
	return h;
}

/*!
 * \param[in]	v	a vector of doubles
 * \returns count histogram (Square root rule: nb bins = sqrt(pop))
 */
Histogram crn::MakeHistogramSquareRoot(const std::vector<double> &v)
{
    return MakeHistogram(v, size_t(sqrt(double(v.size()))));
}

/*!
 * \param[in]	v	a vector of doubles
 * \returns count histogram (Sturges rule: nb bins = 1 + log_2(pop))
 */
Histogram crn::MakeHistogramSturges(const std::vector<double> &v)
{
    return MakeHistogram(v, 1 + size_t(log2(double(v.size()))));
}

/*!
 * \param[in]	v	a vector of doubles
 * \returns count histogram (Riche rule: nb bins = 2 * pop^(1/3))
 */
Histogram crn::MakeHistogramRice(const std::vector<double> &v)
{
    return MakeHistogram(v, size_t(2 * pow(double(v.size()), 1.0 / 3.0)));
}

/*!
 * \param[in]	v	a vector of doubles
 * \returns count histogram (Scott rule: bin width = 3.5 * stddev / pop^(1/3))
 */
Histogram crn::MakeHistogramScott(const std::vector<double> &v)
{
	const auto delta = 3.5 * StdDeviation(v) / pow(double(v.size()), 1.0 / 3.0);
	const auto& mM = MinMax(v);
	return MakeHistogram(v, 1 + size_t((std::get<1>(mM) - std::get<0>(mM)) / delta));
}

/*!
 * \param[in]	v	a vector of doubles
 * \returns count histogram (Freedman-Diaconis rule: bin width = 2 * IQR(v) / pop^(1/3))
 */
Histogram crn::MakeHistogramFreedmanDiaconis(const std::vector<double> &v, bool sort_flag)
{
    const auto& qt = Quantiles(v, size_t(4), sort_flag);
    const auto delta = 2 * (qt.back() - qt.front()) / pow(double(v.size()), 1.0 / 3.0);
    const auto& mM = MinMax(v);
    return MakeHistogram(v, 1 + size_t((std::get<1>(mM) - std::get<0>(mM)) / delta));
}

/*!
 * \param[in]	v	a vector of doubles
 * \param[in]	nb_seeds	the number of desired Gaussians
 * \return Gaussian mixture model modeling an univariate sample
 */
UnivariateGaussianMixture crn::MakeGaussianMixtureModel(const std::vector<double> &v, size_t nb_seeds)
{
	UnivariateGaussianMixture gaussian_mixture_model;

	if (nb_seeds == 1)
	{
		auto mvd = MeanVarDev(v);
		gaussian_mixture_model.AddMember(UnivariateGaussianPDF(std::get<0>(mvd), std::get<1>(mvd)), 1.0);
	}
	else
		gaussian_mixture_model.EM(v, nb_seeds);

	return gaussian_mixture_model;
}

/*!
 * Create a gaussian mixture to model a given set of data
 *
 * \param[in]	patterns	a matrix of doubles
 * \param[in]	nb_seeds	the number of desired Gaussians
 * \return Gaussian mixture model modeling a multivariate sample
 */
MultivariateGaussianMixture crn::MakeGaussianMixtureModel(const std::vector<std::vector<double>> &patterns, size_t nb_seeds)
{
	MultivariateGaussianMixture gaussian_mixture_model;

	if (nb_seeds == 1)
		gaussian_mixture_model.AddMember(MultivariateGaussianPDF(MatrixDouble(MeanPattern(patterns)), SquareMatrixDouble(MakeCovariance(patterns))), 1.0);
	else
		gaussian_mixture_model.EM(patterns, (unsigned int)(nb_seeds));

	return gaussian_mixture_model;
}


