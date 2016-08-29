/* Copyright 2006-2016 Yann LEYDIER, INSA-Lyon, CoReNum, Université Paris Descartes, ENS-Lyon
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
 * file: SHistogram.h
 * \author Yann LEYDIER, Jean DUONG, Asma OUJI
 */

#ifndef CRNHISTOGRAM_HEADER
#define CRNHISTOGRAM_HEADER

#include <CRNString.h>
#include <CRNStatistics/CRNHistogramPtr.h>
#include <CRNImage/CRNImageFormats.h>
#include <cmath>

namespace crn
{
	/****************************************************************************/
	/*! \brief Mother class for integer histograms
	 *
	 * Histograms are represented as arrays.
	 * Classes are indexed from 0 to (array size)-1.
	 * k-th component in the array is the number of elements (integer value) in class k.
	 *
	 * \author 	Jean Duong
	 * \date	25 August 2006
	 * \version 0.1
	 * \ingroup stats
	 */
	class Histogram: public Object
	{
		public:
			enum class DesignHeuristic {CUSTOM, SQUARE_ROOT, STURGES, SCOTT, FREEDMAN};

			/*! \brief Default constructor */
			Histogram():bins(1, 0),compression(1) { }
			/*! \brief Constructor with size and value */
			Histogram(size_t s, unsigned int v = 0);
			/*! \brief Copy constructor */
			Histogram(const Histogram &src, unsigned int c = 1u);

			/*****************************************************************************/
			/*!
			 * Constructor for this class
			 *
			 * \param[in] 	it_begin iterator pointing to first double-size_t pair of input data contener
			 * \param[in] 	it_end ending iterator
			 * \param[in]	m histogram making heuristic
			 * \param[in]	nb_bins number of default bins
			 *
			 * \author Jean DUONG
			 */
			template<typename ITER> Histogram(ITER it_begin, ITER it_end, DesignHeuristic m = DesignHeuristic::STURGES, size_t nb_bins = (size_t)1)
			{
				double delta;
				double left_bound = std::numeric_limits<double>::infinity();
				double right_bound = -std::numeric_limits<double>::infinity();

				size_t cardinal = 0;

				// First step : check data

				for (auto it = it_begin; it != it_end; ++it)
				{
					double v = it->first;
					size_t c = it->second;

					if (v < left_bound)
						left_bound = v;

					if (v > right_bound)
						right_bound = v;

					cardinal += c;
				}

				double range = right_bound - left_bound;

				// Second step : build histogram

				if (m == DesignHeuristic::SQUARE_ROOT)
				{
					nb_bins = (size_t) sqrt(double(cardinal));
					delta = range / double(nb_bins);
				}
				else if (m == DesignHeuristic::STURGES)
				{
					nb_bins = 1 + (size_t) (log(double(cardinal)) / log(2.0));
					delta = range / double(nb_bins);
				}
				else if (m == DesignHeuristic::SCOTT)
				{
					double m = 0.0;
					double v = 0.0;

					for (auto it = it_begin; it != it_end; ++it)
						m += it->first * ((double)it->second / (double)cardinal);

					for (auto it = it_begin; it != it_end; ++it)
					{
						double diff = it->first - m;

						v += (diff * diff) * ((double)it->second / (double)cardinal);
					}

					delta = 3.5 * sqrt(v) / pow(double(cardinal), (1.0 / 3.0));
					nb_bins = 1 + (size_t) ((right_bound - left_bound) / delta);
				}
				else
				{
					delta = range / double(nb_bins);
				}

				++nb_bins; // Add one safety bin !

				bins = std::vector<unsigned int>(nb_bins + 1, 0u);
				compression = 1;

				// Third step : fill histogram

				if (nb_bins == 1) // trivial case
					SetBin(0, (unsigned int)cardinal);
				else
				{
					for (auto it = it_begin; it != it_end; ++it)
					{
						size_t id = (size_t)((it->first - left_bound) / delta);

						if (id < nb_bins)
							IncBin(id, it->second);
					}
				}
			}

			Histogram& operator=(const Histogram&) = delete;
			Histogram(Histogram&&) = default;
			Histogram& operator=(Histogram&&) = default;

			/*! \brief Destructor */
			~Histogram();

			/*! \brief Returns the number of bins */
			size_t Size() const noexcept {return bins.size();}
			/*! \brief Returns the internal pointer on the bins */
			unsigned int* GetBins() {return &(bins.front());}
			/*! \brief Returns a reference to the internal pointer on the bins */
			const unsigned int* GetBins() const {return &(bins.front());}

			/*! \brief Modify a bin value */
			void SetBin(size_t k, unsigned int v);

			/*! \brief Increments a bin value */
			void IncBin(size_t k, unsigned int i = 1u);

			/*! \brief Get a bin value */
			unsigned int GetBin(size_t k) const;

			/*! \brief Access a bin value (fast and unsafe) */
			unsigned int& operator[](size_t k) { return bins[k]; }
			/*! \brief Access a bin value (fast and unsafe) */
			const unsigned int& operator[](size_t k) const { return bins[k]; }

			/*! \brief Cumulate all bins */
			unsigned int CumulateBins() const;

			/*! \brief Mean value on class indexes */
			double Mean() const;

			/*! \brief Variance on class indexes, having mean value */
			double Variance(double m) const;
			/*! \brief Standard deviation on class indexes, having mean value */
			double Deviation(double m) const {return sqrt(Variance(m));}

			/*! \brief Variance on class indexes */
			double Variance() const;
			/*! \brief Standard deviation on class indexes */
			double Deviation() const {return sqrt(Variance());}

			/*! \brief Maximal count value */
			unsigned int Max() const;

			/*! \brief Minimal count value */
			unsigned int Min() const;

			/*! \brief First class having a maximal count value */
			size_t Argmax() const;

			/*! \brief First class having a minimal count value */
			size_t Argmin() const;

			/*! \brief Set maximal count value to m */
			void SetCeiling(unsigned int m);

			/*! \brief Scale maximal count to m */
			void ScaleMaxTo(unsigned int m);

			/*! \brief Smoothing histogram */
			void AverageSmoothing(size_t d);
			/*! \brief Smoothing circular histogram */
			void CircularAverageSmoothing(size_t d);

			/*! \brief Returns the modes */
			std::vector<size_t> Modes() const;
			/*! \brief Returns the stable modes */
			std::vector<size_t> StableModes() const;

			/*! \brief Intersection between two histograms */
			Histogram MakeIntersection(const Histogram &h) const;

			/*! \brief Intersection divergence between two histograms */
			double IntersectionDivergence(const Histogram &h) const;

			/*! \brief Correlation between two histograms */
			double Correlation(const Histogram &h) const;

			/*! \brief Correlation between two histograms */
			double Chi2(const Histogram &h) const;

			/*! \brief Minkowski distance between two histograms */
			double MinkowskiDistance(const Histogram &h, double r) const;

			/*! \brief Jeffrey divergence between two histograms */
			double JeffreyDivergence(const Histogram &h) const;

			/*! \brief Match distance between two histograms */
			double MatchDistance(const Histogram &h) const;

			/*! \brief Kolmogorov-Smirnov distance between two histograms */
			double KolmogorovSmirnovDistance(const Histogram &h) const;

			/*! \brief Earth Mover's Distance */
			double EMD(const Histogram &h) const;

			/*! \brief Circular Earth Mover's Distance */
			double CEMD(const Histogram &h) const;

			/*! \brief Appends an histogram to the current */
			void Append(const Histogram &h);

			/*! \brief Resizes the histogram */
			void Resize(size_t newsize);

			/*! \brief Dumps bins to a string */
			String ToString() const;

			/*! \brief returns the histogram image */
			ImageBW MakeImageBW(size_t height) const;

			/*! \brief returns the circular histogram image */
			ImageBW MakeRadialImageBW(size_t radius) const;

			/*! \brief Cumulates the values from 0 to end */
			void Cumulate();

			/*! \brief Computes the Fisher threshold */
			size_t Fisher() const;

			/*! \brief Computes the entropy threshold */
			size_t EntropyThreshold() const;

			/*! \brief the entropy */
			double Entropy() const;

			/*! \brief Computes the median value */
			size_t MedianValue() const;

			/*! \brief Creates an histogram from population */
			Histogram MakePopulationHistogram() const;

			std::vector<unsigned int> Std() && { return std::move(bins); }

			/*! \brief Initializes the object from an XML element. Unsafe. */
			void Deserialize(xml::Element &el);
			/*! \brief Dumps the object to an XML element. Unsafe. */
			xml::Element Serialize(xml::Element &parent) const;

		private:
			using datatype = std::vector<unsigned int>; /*!< Inner data representation */
			datatype bins; /*!< Classes for this histogram. */
			unsigned int compression; /*!< Compression ratio for this histogram. */

			CRN_DECLARE_CLASS_CONSTRUCTOR(Histogram)
			CRN_SERIALIZATION_CONSTRUCTOR(Histogram)
	};
	template<> struct IsSerializable<Histogram> : public std::true_type {};
	template<> struct IsClonable<Histogram> : public std::true_type {};
	inline double Distance(const Histogram &h1, const Histogram &h2) { return h1.MinkowskiDistance(h2, 1); }
	template<> struct IsMetric<Histogram> : public std::true_type {};


	/*! \brief Size of an histogram */
	inline size_t Size(const Histogram &h) noexcept { return h.Size(); }
}
#endif
