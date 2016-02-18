/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum, Université Paris Descartes
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
 * file: SHistogram cpp
 * \author Yann LEYDIER, Jean DUONG, Asma OUJI, Frank LeBOURGEOIS
 */

#include <CRNStatistics/CRNHistogram.h>
#include <CRNData/CRNData.h>
#include <CRNException.h>
#include <CRNImage/CRNImageBW.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNIO/CRNIO.h>
#include <CRNi18n.h>

using namespace crn;

/*****************************************************************************/
/*!
 * Constructor with size and value.
 *
 * \param[in] 	s Size for this histogram
 * \param[in] 	v Value to set up all the components for this histogram (default value = 0)
 *
 * \author Jean DUONG
 */
Histogram::Histogram(size_t s, unsigned int v):
	bins(s, v),
	compression(1)
{ }

/*****************************************************************************/
/*!
 * Constructor for this class
 *
 * \param[in] 	src Source histogram
 * \param[in]	c compression ratio
 *
 * \author Jean DUONG
 */
Histogram::Histogram(const Histogram &src, unsigned int c):
	compression(c)
{

	if (c == 1)
	{
		bins = src.bins;
	}
	else
	{
		size_t q = src.Size() / compression;
		size_t r = src.Size() % compression;
		size_t newsize;
		if (r > 0)
		{
			newsize = q + 1;
		}
		else
		{
			newsize = q;
		}

		datatype(newsize, 0).swap(bins);
		size_t k = 0;

		for (size_t i = 0; i < q; i++)
		{
			unsigned int sum = 0;

			for (unsigned int j = 0; j < c; j++)
			{
				sum += src.bins[k];
				k++;
			}

			bins[i] = sum;
		}

		if (r > 0)
		{
			unsigned int sum = 0;

			for (size_t j = k; j < src.Size(); j++)
			{
				sum += src.bins[k];
				k++;
			}

			bins[q] = sum;
		}
	}
}

/*****************************************************************************/
/*!
 * Set k-th bin to value v for this histogram
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k The index of the component to be modified
 * \param[in] 	v The new value for k-th component
 *
 * \author Jean DUONG
 */
void Histogram::SetBin(size_t k, unsigned int v)
{
	if (k >= bins.size())
	{
		throw ExceptionDomain(StringUTF8("Histogram::SetBin(size_t k, unsigned int v): ") +
				_("invalid index."));
	}

	bins[k] = v;
}

/*****************************************************************************/
/*!
 * Increments k-th bin of i for this histogram
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k The index of the component to be modified
 * \param[in] 	i The increment (default value = 1)
 *
 * \author Yann LEYDIER
 */
void Histogram::IncBin(size_t k, unsigned int i)
{
	if (k >= bins.size())
	{
		throw ExceptionDomain(StringUTF8("Histogram::IncBin(size_t k, unsigned int i): ") +
				_("invalid index."));
	}

	bins[k] += i;
}

/*****************************************************************************/
/*!
 * Gets k-th bin from this histogram
 *
 * \throws	ExceptionDomain	index out of bounds
 *
 * \param[in]	k	the index of the bin to retrieve
 *
 * \return content of the bin
 *
 * \author Jean DUONG
 */
unsigned int Histogram::GetBin(size_t k) const
{
	if (k >= bins.size())
	{
		throw ExceptionDomain(StringUTF8("Histogram::GetBin(size_t k): ") +
				_("invalid index."));
	}

	return bins[k];
}


/*****************************************************************************/
/*!
 * Cumulate all bins form this histogram
 *
 * \return the sum of all the bins
 *
 * \author Jean DUONG
 */
unsigned int Histogram::CumulateBins() const
{
	unsigned int Cumul = 0;

	for (auto & elem : bins)
	{
		Cumul += elem;
	}

	return Cumul;
}

/*****************************************************************************/
/*!
 * Mean value of the bin indexes in this histogram
 *
 * \return	the mean value of the bin indexes
 *
 * \author Jean DUONG
 */
double Histogram::Mean() const
{
	double m = 0.0;
	unsigned int p = 0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		m += double(k * bins[k]);
		p += bins[k];
	}

	m /= p;

	return m;
}

/*****************************************************************************/
/*!
 *  Variance of the bin indexes in this histogram
 *
 * \param[in] 	m The mean value previously computer
 *
 * \return	the variance of the bins indexes
 *
 * \author Jean DUONG
 */
double Histogram::Variance(double m) const
{
	double v = 0.0;
	unsigned int p=0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		double d = double(k) - m;

		v += d * d * bins[k];
		p += bins[k];
	}

	v /= p;

	return v;
}

/*****************************************************************************/
/*!
 *  Variance of the bin indexes in this histogram
 *
 * \return	the variance of the bin indexes
 *
 * \author Jean DUONG
 */
double Histogram::Variance() const
{
	double m = Mean();
	double v = 0.0;
	unsigned int p=0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		double d = double(k) - m;

		v += d * d * bins[k];
		p += bins[k];
	}

	v /= p;

	return v;
}

/*****************************************************************************/
/*!
 * Maximal count value for this histogram
 *
 * \return	the maximal count value
 *
 * \author Jean DUONG
 */
unsigned int Histogram::Max() const
{
	unsigned int m = 0;

	for (auto & elem : bins)
	{
		m = crn::Max(m, elem);
	}

	return m;
}

/*****************************************************************************/
/*!
 * Minimal count value for this histogram
 *
 * \return	the minimal count value
 *
 * \author Jean DUONG
 */
unsigned int Histogram::Min() const
{
	unsigned int m = bins[0];

	for (auto & elem : bins)
	{
		m = crn::Min(m, elem);
	}

	return m;
}

/*****************************************************************************/
/*!
 * First class having maximal count value for this histogram
 *
 * \return the index of the first class having the maximal count value
 *
 * \author Jean DUONG
 */
size_t Histogram::Argmax() const
{
	unsigned int m = 0;
	size_t a = 0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		unsigned int v = bins[k];

		if (v > m)
		{
			m = bins[k];
			a = k;
		}
	}

	return a;
}

/*****************************************************************************/
/*!
 * First class having minimal count value for this histogram
 *
 * \return the index of the first class having the minimal count value
 *
 * \author Jean DUONG
 */
size_t Histogram::Argmin() const
{
	unsigned int m = bins[0];
	size_t a = 0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		unsigned int v = bins[k];

		if (v < m)
		{
			m = bins[k];
			a = k;
		}
	}

	return a;
}

/*****************************************************************************/
/*!
 * Set maximal value for counts
 *
 * \param[in] 	m The maximal value
 *
 * \author Jean DUONG
 */
void Histogram::SetCeiling(unsigned int m)
{
	for (auto & elem : bins)
	{
		elem = crn::Min(m,elem);

	}
}

/*****************************************************************************/
/*!
 * Scale to make maximal count reach a given value
 *
 * \param[in] 	m The desired maximal value
 *
 * \author Jean DUONG
 */
void Histogram::ScaleMaxTo(unsigned int m)
{
	unsigned int c = Max(); /* Maximal value for this histogram */

	if (c == 0)
	{
		CRNWarning(String(U"Histogram::ScaleMaxTo(int m): ") +
				_("flat histogram."));
		return;
	}

	double s = (double) m / (double) c; /* Scale factor */

	for (auto & elem : bins)
	{
		unsigned int v = (unsigned int)(elem * s);

		elem = v;
	}
}

/*****************************************************************************/
/*!
 * Smoothing : replace k-th count by average value computed in
 * a sliding window centered on k-th class
 *
 * \param[in] 	d The sliding window's half-width
 *
 * \author Jean DUONG
 */
void Histogram::AverageSmoothing(size_t d)
{
	datatype H(bins.size());

	for (size_t k = 0; k < bins.size(); k++)
	{
		int left = crn::Max(0, int(k) - int(d));
		int right = crn::Min(int(bins.size()) - 1, int(k) + int(d));

		uint64_t m = 0;
		uint64_t n = right - left + 1;

		for (int j = left; j <= right; j++)
		{
			m += bins[j];
		}

		H[k] = (unsigned int)(m / n);
	}

	bins.swap(H);
}

/*****************************************************************************/
/*!
 * Smoothing : replace k-th count by average value computed in
 * a sliding window centered on k-th class
 *
 * \param[in] 	d The sliding window's half-width
 *
 * \author Yann LEYDIER
 */
void Histogram::CircularAverageSmoothing(size_t d)
{
	if (d > bins.size())
	{
		CRNWarning(String(U"Histogram::CircularAverageSmoothing(size_t d): ") +
				_("window larger than histogram. Cropping."));
		d = bins.size(); // wew, dirty !
	}

	datatype H(bins.size());

	for (size_t k = 0; k < bins.size(); k++)
	{
		int left = int(k) - int(d);
		int right = int(k + d);
		double m = 0.0;
		double n = double(right) - double(left) + 1;

		for (int j = left; j <= right; j++)
		{
			int index = j;
			if (index < 0) index = int(bins.size()) + index;
			if (index >= int(bins.size())) index -= int(bins.size());
			m += (double) bins[index] / n;
		}

		H[k]=(unsigned int)m;
	}

	bins.swap(H);
}

/*****************************************************************************/
/*!
 * Search modes for this histogram
 *
 * \author Yann LEYDIER
 *
 * \return	a vector containing the index of the classes that are modes
 */
std::vector<size_t> Histogram::Modes() const
{
	std::vector<size_t> modes;
	if (bins.empty())
		return modes;
	if (bins.size() == 1)
	{
		modes.push_back(0);
		return modes;
	}
	// first value is a mode ?
	if (bins[0] > bins[1])
		modes.push_back(0);
	for (size_t b = 1; b < bins.size() - 1; ++b)
	{
		if (bins[b] > bins[b - 1])
		{ // increasing
			size_t first = b;
			if (bins[b] < bins[b + 1])
			{ // still increasing, pass
				continue;
			}
			while (bins[b] == bins[b + 1])
			{ // plateau
				b += 1;
				if (b == bins.size() - 1)
				{ // reached the end of the histogram
					modes.push_back((first + bins.size() - 1) / 2);
					break;
				}
				else if (b == bins.size() - 2)
				{ // reached the penultimate
					if (bins[b] == bins[b + 1])
					{ // plateau till the end
						modes.push_back((first + bins.size() - 1) / 2);
					}
					if (bins[b] > bins[b + 1])
					{ // peak
						modes.push_back((first + b) / 2);
					}
					break;
				}
			}
			if ((b < bins.size() - 1) && (bins[b] > bins[b + 1]))
			{ // peak !
				modes.push_back((first + b) / 2);
			}
		}
	}
	if (bins[bins.size() - 2] < bins[bins.size() - 1])
		modes.push_back(bins.size() - 1);

	return modes;
}

/*! Returns the stable modes through iterative smoothing
 *
 * \warning	May be very slow on histograms containing huge values
 *
 * \author Yann LEYDIER
 *
 * \return	a vector containing the index of the classes that are stable modes
 */
std::vector<size_t> Histogram::StableModes() const
{
	// copy histogram
	Histogram s(*this);

	std::map<size_t, size_t> mcount; // key = number of modes, value = number of occurrences
	std::map<size_t, std::vector<size_t> > msum; // key = number of modes, value = sum of the modes

	std::vector<size_t> modes(s.Modes());
	size_t nmodes = modes.size();
	size_t niter = 0;
	while (nmodes > 1)
	{
		auto cit = mcount.find(nmodes);
		if (cit != mcount.end())
		{ // update
			cit->second += 1;
			auto sit = msum.find(nmodes);
			std::transform(sit->second.begin(), sit->second.end(), modes.begin(), sit->second.begin(), std::plus<size_t>());
		}
		else
		{ // add
			mcount.insert(std::make_pair(nmodes, 1));
			msum.insert(std::make_pair(nmodes, modes));
		}
		// smooth
		s.AverageSmoothing(1);
		modes = s.Modes();
		nmodes = modes.size();
		niter += 1;
	}

	// if there is only one iteration, then it means there is only one mode
	if (niter == 1)
	{
		return modes;
	}

	// look for most stable mode number
	nmodes = 0;
	size_t maxpop = 0;
	for (auto & elem : mcount)
	{
		if (elem.second > maxpop)
		{
			maxpop = elem.second;
			nmodes = elem.first;
		}
	}

	// compute the vector of modes
	if (nmodes == 0)
		return std::vector<size_t>();

	modes.swap(msum[nmodes]);
	std::transform(modes.begin(), modes.end(), modes.begin(), std::bind2nd(std::divides<size_t>(), maxpop));
	return modes;
}

/*****************************************************************************/
/*!
 * Intersection between two histograms
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 * \return	the intersection of the two histograms
 * \author Jean DUONG
 */
Histogram Histogram::MakeIntersection(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::Intersection(const Histogram &h): ") +
				_("histograms must have same size."));
	}

	Histogram i(bins.size());

	for (size_t k = 0; k < bins.size(); k++)
	{
		i.bins[k] = (unsigned int)(crn::Min(h.GetBin(k),bins[k]));
	}

	return i;
}

/*****************************************************************************/
/*!
 * Intersection distance between two histograms
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 * \return	the intersection distance
 * \author Jean DUONG
 */
double Histogram::IntersectionDivergence(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDomain(StringUTF8("Histogram::IntersectionDivergence("
					"const Histogram &h): ") + _("histograms must have same size."));
	}

	double cumulMin = 0.0;
	double cumulH = h.CumulateBins();

	for (size_t k = 0; k < bins.size(); k++)
	{
		cumulMin += crn::Min(h.GetBin(k),bins[k]);
	}

	return (1.0 - (cumulMin / cumulH));
}

/*****************************************************************************/
/*!
 * Correlation between two histograms
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 * \return	the correlation
 * \author Jean DUONG
 */
double Histogram::Correlation(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::Correlation("
						"const Histogram &h): ") + _("histograms must have same size."));
	}

	int offset_1 = (int)(CumulateBins() / bins.size());
	int offset_2 = (int)(h.CumulateBins() / bins.size());

	double cumul_numer = 0.0;
	double cumul_denom_1 = 0.0;
	double cumul_denom_2 = 0.0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		int v_1 = int(bins[k]) - offset_1;
		int v_2 = int(h.bins[k]) - offset_2;

		cumul_numer += v_1 * v_2;
		cumul_denom_1 += v_1 * v_1;
		cumul_denom_2 += v_2 * v_2;
	}

	cumul_numer /= sqrt(cumul_denom_1);
	cumul_numer /= sqrt(cumul_denom_2);

	return cumul_numer;
}


/*****************************************************************************/
/*!
 * Chi2 distance between two histograms
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 * \return	the correlation
 * \author Jean DUONG
 */
double Histogram::Chi2(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::IntersectionDivergence("
						"const Histogram &h): ") + _("histograms must have same size."));
		return HUGE_VAL;
	}

	double cumul_numer = 0.0;
	double cumul_denom = 0.0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		auto v_1 = int(bins[k]);
		auto v_2 = int(h.bins[k]);
		int diff = v_1 - v_2;

		cumul_numer += diff * diff;
		cumul_denom += v_1 + v_2;
	}

	return cumul_numer / cumul_denom;
}

/*****************************************************************************/
/*!
 * Minkowski distance between two histograms.
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] h the other histogram
 * \param[in]	r	power of the norm
 *
 * \return	the Minkowski distance
 *
 * \author Jean DUONG
 */
double Histogram::MinkowskiDistance(const Histogram &h, double r) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::MinkowskiDistance("
					"const Histogram &h, double r): ") +
				_("histograms must have same size."));
	}

	double cumul = 0.0;
	double invR = 1.0 / (double)(r);

	for (size_t k = 0; k < bins.size(); k++)
	{
		cumul += pow(fabs((double)(h.GetBin(k)) - (double)(bins[k])),r);
	}

	return pow(cumul, invR);
}

/*****************************************************************************/
/*!
 * Jeffrey divergence between two histograms.
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 *
 * \return	Jeffrey's divergence
 *
 * \author Jean DUONG
 */
double Histogram::JeffreyDivergence(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::JeffreyDistance(const Histogram &h): ") +
				_("histograms must have same size."));
	}

	double cumul = 0.0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		double h1 = bins[k];
		double h2 = h.GetBin(k);
		double m = (h1 + h2) / 2.0;

		cumul += h1 * log(h1 / m) + h2 * log(h2 / m);
	}

	return cumul;
}

/*****************************************************************************/
/*!
 * Match distance between two histograms.
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 *
 * \return	the match distance
 *
 * \author Jean DUONG
 */
double Histogram::MatchDistance(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::MatchDistance(const Histogram &h): ") +
				_("histograms must have same size."));
	}

	double cumul = 0.0;
	double cumulH1  = 0.0;
	double cumulH2  = 0.0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		cumulH1 += bins[k];
		cumulH2 += h.GetBin(k);
		cumul += fabs(cumulH1 - cumulH2);
	}

	return cumul;
}

/*****************************************************************************/
/*!
 * Kolmogorov-Smirnov distance between two histograms.
 * The two histograms must have same size.
 *
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h Histogram
 *
 * \return	Kolmogorov-Smirnov distance
 *
 * \author Jean DUONG
 */
double Histogram::KolmogorovSmirnovDistance(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::KolmogorovSmirnovDistance("
					"const Histogram &h): ")+ _("histograms must have same size."));
	}

	double max = 0.0;
	double cumulH1  = 0.0;
	double cumulH2  = 0.0;

	for (size_t k = 0; k < bins.size(); k++)
	{
		cumulH1 += bins[k];
		cumulH2 += h.GetBin(k);
		double diff = fabs(cumulH1 - cumulH2);

		if (diff > max)
		{
			max = diff;
		}
	}

	return max;
}

/*!
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h	the histogram to compare to
 *
 * \return	Earth Mover's Distance
 *
 * \author Yann LEYDIER
 */
double Histogram::EMD(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::EMD(const Histogram &h): ")+ _("histograms must have same size."));
	}
	auto h1 = *this;
	h1.Cumulate();
	auto h2 = h;
	h2.Cumulate();
	auto dist = 0.0;
	for (auto tmp : crn::Range(h1))
		dist += double(Abs(h1[tmp] - h2[tmp]));
	return dist / double(h1.Size());
}

/*! Earth Mover's Distance for histograms of angles
 * \throws	ExceptionDimension	histograms do not have the same size
 * \param[in] 	h	the histogram to compare to
 *
 * \return	Circular Earth Mover's Distance
 *
 * \author Yann LEYDIER
 */
double Histogram::CEMD(const Histogram &h) const
{
	if (bins.size() != h.Size())
	{
		throw ExceptionDimension(StringUTF8("Histogram::CEMD(const Histogram &h): ")+ _("histograms must have same size."));
	}
	auto dist = std::numeric_limits<double>::max();
	// compute EMD for each possible cumulative histogram
	for (auto k : crn::Range(h))
	{
		// compute cumulative histogram
		auto h1 = Histogram(Size());
		auto h2 = Histogram(Size());
		// sum from k to end
		auto sum1 = 0u;
		auto sum2 = 0u;
		for (auto i = k; i < Size(); ++i)
		{
			sum1 += (*this)[i];
			sum2 += h[i];
		}
		// cumulate from k to end and 0 to i (i < k)
		for (auto i = size_t(0); i < k; ++i)
		{
			h1[i] = sum1;
			h2[i] = sum2;
			for (auto j = size_t(0); j <= i; ++j)
			{
				h1[i] += (*this)[j];
				h2[i] += h[j];
			}
		}
		// cumulate from k to i (i >= k)
		for (auto i = k; i < Size(); ++i)
			for (auto j = k; j <= i; ++j)
			{
				h1[i] += (*this)[j];
				h2[i] += h[j];
			}
		// compute EMPD
		auto d = 0.0;
		for (auto tmp : crn::Range(h1))
			d += double(Abs(h1[tmp] - h2[tmp]));
		// keep min
		if (d < dist)
			dist = d;
	}
	return dist / double(Size());
}

/*****************************************************************************/
/*!
 * Destructor
 *
 * \author Jean DUONG
 */
Histogram::~Histogram()
{ }

/*****************************************************************************/
/*!
 * Append an histogram
 *
 * \param[in]	h	the histogram to append
 *
 * \author Yann LEYDIER
 */
void Histogram::Append(const Histogram &h)
{
	bins.insert(bins.end(), h.bins.begin(), h.bins.end());
}

/*****************************************************************************/
/*!
 * Resize the histogram
 *
 * \throw	ExceptionDomain	null new size
 * \param[in]	newsize	the new size
 *
 * \author Yann LEYDIER
 *
 * \todo take care of float comparison.
 * \todo using int calculation ? (better for android port)
 */
void Histogram::Resize(size_t newsize)
{
	if (newsize == 0)
		throw crn::ExceptionDomain(_("Cannot resize histogram with null size."));

	if (newsize == bins.size())
		return;

	datatype newbins(newsize);

	double ratio = (double)bins.size() / (double)newsize;
	double begin = 0;
	for (size_t tmp = 0; tmp < newsize; tmp++)
	{
		double val = 0;
		auto end = double(tmp + 1) * ratio;
		if (end >= double(bins.size())) end = double(bins.size()) - 0.0001; // yay... -_-
		double tb = begin;
		while (floor(tb) != floor(end))
		{
			double te = ceil(tb);
			if (te == tb) te += 1;
			val += (te - tb) * bins[(size_t)floor(tb)];
			tb = te;
		}
		val += (end - tb) * bins[(size_t)floor(tb)];
		if ((end - begin) != 0)
			val /= (end - begin);
		//GET: using round in place of direct cast
		//GETnewbins[tmp] = (unsigned int)val;
		newbins[tmp] = (unsigned int)round(val);
		begin = end;
	}

	bins.swap(newbins);
}

/*****************************************************************************/
/*!
 * Unsafe load from XML node
 *
 * \author Yann LEYDIER
 *
 * \throws	ExceptionInvalidArgument	not an Histogram
 * \throws	ExceptionNotFound	cannot find CDATA
 * \throws	ExceptionDomain	cannot find CDATA
 * \throws	ExceptionRuntime	cannot convert CDATA
 *
 * \param[in]	el	the element to load
 */
void Histogram::deserialize(xml::Element &el)
{
	bins.clear();
	if (el.GetValue() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool Histogram::deserialize(xml::Element &el): ") +
				_("Wrong XML element."));
	}
	xml::Node n(el.GetFirstChild());
	if (!n)
	{
		throw ExceptionNotFound(StringUTF8("bool Histogram::deserialize(xml::Element &el): ") +
				_("Cannot get CDATA."));
	}
	xml::Text t = n.AsText(); // may throw
	auto v = Data::ASCII85Decode<unsigned int>(t.GetValue());
	if (v.empty())
	{
		throw ExceptionRuntime(StringUTF8("bool Histogram::deserialize(xml::Element &el): ") +
				_("Cannot convert CDATA."));
	}
	bins.swap(v);
}

/*****************************************************************************/
/*!
 * Unsafe save
 *
 * \author Yann LEYDIER
 *
 * \param[in]	parent	the parent element to which we will add the new element
 *
 * \return The newly created element
 */
xml::Element Histogram::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	auto text = Data::ASCII85Encode(reinterpret_cast<const uint8_t * const>(bins.data()), bins.size() * sizeof(datatype::value_type));
	xml::Text t(el.PushBackText(text, false));

	return el;
}

/*!
 * Dumps the bins to a string
 *
 * \return	a string containing the bins
 */
String Histogram::ToString() const
{
	String s(GetClassName());
	s += U": ";
	for (auto & elem : bins)
	{
		s += elem;
		s += U" ";
	}
	return s;
}

/*****************************************************************************/
/*!
 * histogram image
 *
 * \author Asma OUJI
 *
 * \param[in]	height the histogram image height
 *
 * \return The newly created image.
 */
ImageBW Histogram::MakeImageBW(size_t height) const
{
	auto bw = ImageBW(bins.size(), height, pixel::BWWhite);
	auto val = double(height) / double(Max());
	for (size_t k = 0; k < bins.size(); k++)
	{
		auto bin = GetBin(k);
		for (int i = 0; i < int(double(bin) * val) - 1; ++i)
		{
			bw.At(k, height - 1 - i) = pixel::BWBlack;
		}
	}
	return bw;
}

/*!
 * Cumulates the values from 0 to end
 */
void Histogram::Cumulate()
{
	unsigned int sum = 0;
	for (size_t k = 0; k < bins.size(); k++)
	{
		sum += GetBin(k);
		SetBin(k, sum);
	}
}

/*****************************************************************************/
/*!
 * Creates a radial histogram image
 *
 * \author Yann LEYDIER
 *
 * \param[in]	radius the histogram image radius
 *
 * \return The newly created image.
 */
ImageBW Histogram::MakeRadialImageBW(size_t radius) const
{
	Rect r;
	double factor = crn::Twice(M_PI) / double(bins.size());
	double max = Max();
	for (size_t tmp = 0; tmp < bins.size(); tmp++)
	{
		double tx = double(GetBin(tmp) * radius) / max * cos(double(tmp) * factor);
		double ty = double(GetBin(tmp) * radius) / max * sin(double(tmp) * factor);
		r |= Rect((int)tx, (int)ty);
	}
	if (!r.GetWidth() || !r.GetHeight())
		return ImageBW{};
	ImageBW img = ImageBW(r.GetWidth(), r.GetHeight(), pixel::BWWhite);
	for (size_t tmp = 0; tmp < bins.size(); tmp++)
	{
		double tx = double(GetBin(tmp) * radius) / max * cos(double(tmp) * factor);
		double ty = double(GetBin(tmp) * radius) / max * sin(double(tmp) * factor);
		img.DrawLine(-r.GetLeft(), -r.GetTop(), (int)tx - r.GetLeft(),
				(int)ty - r.GetTop(), pixel::BWBlack);
	}
	return img;
}

/*****************************************************************************/
/*!
 * Computes the Fisher threshold
 *
 * \author Yann LEYDIER
 *
 * \return The index of the cut
 */
size_t Histogram::Fisher() const
{
	std::vector<bool> notempty(bins.size());
	std::vector<double> tab(bins.size());

	for (size_t tmp = 0; tmp < bins.size(); tmp++)
	{
		if (bins[tmp] > 0)
		{
			notempty[tmp] = true;
			tab[tmp] = double((tmp + 1) * bins[tmp]);
		}
		else
		{
			notempty[tmp] = false;
			tab[tmp] = 0;
		}
	}

	double s1 = 0, s2 = 0;
	long n1 = 0, n2 = 0;
	for (size_t tmp = 0; tmp < bins.size(); tmp++)
	{
		if (notempty[tmp])
		{
			s2 += tab[tmp];
			n2 += bins[tmp];
		}
	}
	size_t index = 0;
	double f(0.0);
	do
	{
		if (notempty[index])
		{
			s1 += tab[index];
			n1 += bins[index];
			s2 -= tab[index];
			n2 -= bins[index];
		}
		if ((n1 * n2) == 0)
			f = -20.0;
		else
			f = double(index) - ((s1 / double(n1)) + (s2 / double(n2))) / 2.0;
		index += 1;
	} while ((index < bins.size()) && (f <= 0.0));

	if (index)
		return index - 1;
	else
		return 0;
}

/*****************************************************************************/
/*!
 * Computes the entropy threshold for bimodal histogram
 *
 * \author Frank LeBOURGEOIS
 *
 * \return the threshold
 */
size_t Histogram::EntropyThreshold() const
{
	std::vector<double> entropy(bins.size());
	for (size_t tmp = 0; tmp < bins.size(); tmp++)
	{
		if (bins[tmp])
			entropy[tmp] = bins[tmp] * log((double)bins[tmp]);
		else
			entropy[tmp] = 0;
	}
	double max = 0;
	size_t thresh = 0;
	for (size_t t = 0; t < bins.size(); t++)
	{
		double s1 = 0, s2 = 0, n1 = 0, n2 = 0;
		for (size_t tmp = 0; tmp <= t; tmp++)
			if (bins[tmp])
			{
				s1 += entropy[tmp];
				n1 += bins[tmp];
			}
		for (size_t tmp = t + 1; tmp < bins.size(); tmp++)
			if (bins[tmp])
			{
				s2 += entropy[tmp];
				n2 += bins[tmp];
			}
		double en = 0;
		if ((n1 * n2) != 0)
			en = -(s1/n1) - (s2/n2) + log(n1*n2);
		if (en > max)
		{
			max = en;
			thresh = t;
		}
	}
	return thresh;
}

/*****************************************************************************/

/*!
 * Compute the entropy
 *
 * \author Jean DUONG
 *
 * \return the entropy
 */

double Histogram::Entropy() const
{
	auto e = 0.0;
	auto c = double(CumulateBins());

	for (auto & elem : bins)
	{
		auto b_k = double(elem);

		if (b_k != 0)
		{
			auto p_k = double(b_k / c);

			e += p_k * log(p_k);
		}
	}

	return -e;
}

/*****************************************************************************/

/*!
 * Computes the median value
 *
 * \author Yann LEYDIER
 *
 * \return The median value
 */
size_t Histogram::MedianValue() const
{
	if (bins.empty())
		throw ExceptionUninitialized(_("The histogram is empty."));
	unsigned int mid = CumulateBins() / 2;
	unsigned int sum = 0;
	size_t index;
	for (index = 0; index < bins.size(); index++)
	{
		sum += bins[index];
		if (sum >= mid)
			break;
	}
	return index;
}

/*!
 * Creates an histogram from population
 *
 * \author Yann LEYDIER
 *
 * \return	a new histogram
 */
Histogram Histogram::MakePopulationHistogram() const
{
	Histogram poph(Max() + 1);
	for (auto & elem : bins)
	{
		poph.IncBin(elem);
	}
	return poph;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(Histogram)
	CRN_DATA_FACTORY_REGISTER(U"Histogram", Histogram)
CRN_END_CLASS_CONSTRUCTOR(Histogram)

