/* Copyright 2008-2016 INSA Lyon, CoReNum, ENS-Lyon
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
 * file: CRNUnivariateGaussianMixture.h
 * \author Jean DUONG, Yann LEYDIER
 */

 
#ifndef CRNUNIVARIATEGAUSSIANMIXTURE_HEADER
#define CRNUNIVARIATEGAUSSIANMIXTURE_HEADER

#include <vector>
#include <limits>
#include <CRNMath/CRNUnivariateGaussianPDF.h>
#include <CRNMath/CRNMatrixDouble.h>

#include <iostream>

namespace crn
{
	/****************************************************************************/
	/*! \brief Univariate Gaussian mixture
	 *
	 * Model for univariate gaussian mixture
	 * 
	 * \author 	Jean DUONG
	 * \date	August 2008
	 * \version	0.1
	 * \ingroup math
	 */
	class UnivariateGaussianMixture: public Object
	{
		public:
			/*! \brief Default constructor */
			UnivariateGaussianMixture(){}
			UnivariateGaussianMixture(const UnivariateGaussianMixture& m) = default;
			UnivariateGaussianMixture(UnivariateGaussianMixture&&) = default;
			/*!
			 * Create a gaussian mixture to model a given set of data
			 *
			 * \param[in] 	it_begin	iterator pointing to first pair of input data
			 * \param[in] 	it_end      ending iterator
			 * \param[in] 	nbSeeds		the number of density functions
			 *
			 * \return	the inferred gaussian mixture model
			 */
			template<typename ITER> UnivariateGaussianMixture(ITER it_begin, ITER it_end, size_t nb_seeds = 2)
			{
				if (nb_seeds == 1)
				{
					auto mvd = MeanVarDev(it_begin, it_end);

					AddMember(UnivariateGaussianPDF(std::get<0>(mvd), std::get<1>(mvd)), 1.0);
				}
				else
					EM(it_begin, it_end, (unsigned int)(nb_seeds));
			}

			/*! \brief Destructor */
			virtual ~UnivariateGaussianMixture() override;

			UnivariateGaussianMixture& operator=(const UnivariateGaussianMixture&) = delete;
			UnivariateGaussianMixture& operator=(UnivariateGaussianMixture&&) = default;

			/*! \brief Returns the number of density functions */
			size_t GetNbMembers() const noexcept { return members.size(); }

			/*! \brief Returns the weight of a given density function */
			double GetWeight(size_t k) const;
			/*! \brief Returns a given density function */
			UnivariateGaussianPDF GetMember(size_t k) const;
			/*! \brief Returns the mean of a given density function */
			double GetMean(size_t k) const;
			/*! \brief Returns the variance of a given density function */
			double GetVariance(size_t k) const;

			/*! \brief Adds a density function */
			void AddMember(UnivariateGaussianPDF pdf, double Weight);
			/*! \brief Replaces a given density function and its weight */
			void SetMember(UnivariateGaussianPDF pdf, double w, size_t k);
			/*! \brief Sort members by means */
			void SortMembersByMeans(bool reverse = false);
			/*! \brief Sort members by means */
			void SortMembersByVariances(bool reverse = false);
			/*! \brief Sort members by weights */
			void SortMembersByWeights(bool reverse = false);

			/*! \brief Evaluates a pattern */
			double ValueAt(const double x) const;
			/*! \brief Evaluates a pattern for a given density function */
			double ValueAt(const double x, size_t k, bool weight_flag = false) const;

			/*! \brief Expectation Maximization */
			unsigned int EM(const MatrixDouble& patterns, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t maximalIterations = 100);
			/*! \brief Expectation Maximization */
			unsigned int EM(const std::vector<double> &patterns, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t maximalIterations = 100);
			/*! \brief Expectation Maximization */
			template<typename ITER> unsigned int EM(ITER it_begin, ITER it_end, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t maximalIterations = 100);

			/*! \brief Dumps a summary of the mixture to a string */
			String ToString() const;
			/*! \brief Dumps a summary of one element of the mixture to a string */
			String ToString(size_t k) const;

			/*! \brief Maximum log-likelihood estimation */
			double MLLE(const MatrixDouble& Data) const;
			/*! \brief Maximum log-likelihood estimation */
			double MLLE(const std::vector<double> &Data) const; 
			/*! \brief Maximum log-likelihood estimation */
			template<typename ITER> double MLLE(ITER it_begin, ITER it_end) const;

			/*! \brief Bayes Information Criterion estimation */
			double BIC(double l, size_t n) const {return (-2.0 * log(l) + (double)members.size() * log((double)n)) ;}
			/*! \brief Bayes Information Criterion estimation */
			double BIC(const std::vector<double> &Data) const {return (-2.0 * MLLE(Data) + (double)members.size() * log((double)(Data.size()))) ;}

			/*! \brief Creates a data sample following the mixture's probability law */
			std::vector<double> MakeRandomSample(size_t n = 1, size_t m = 100, bool reseed = true) const;

			void Deserialize(xml::Element &el);
			xml::Element Serialize(xml::Element &parent) const;

		private:
			/*! \brief Checks if an index is valid */
			bool isValidMemberIndex(size_t k) const { return k < members.size(); }

			std::vector<std::pair<UnivariateGaussianPDF, double>> members; /*!< the PDFs and their weights */

			CRN_DECLARE_CLASS_CONSTRUCTOR(UnivariateGaussianMixture)
			CRN_SERIALIZATION_CONSTRUCTOR(UnivariateGaussianMixture)
	};
	template<> struct IsSerializable<UnivariateGaussianMixture> : public std::true_type {};
	template<> struct IsClonable<UnivariateGaussianMixture> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(UnivariateGaussianMixture)

	/*!
	 * Maximum log-likelihood estimator
	 *
	 * \param[in] 	it_begin	iterator pointing to first double-size_t pair of input data
	 * \param[in] 	it_end      ending iterator
	 *
	 * \return	the maximum log-likelihood of a set of patterns
	 */
	template<typename ITER> double UnivariateGaussianMixture::MLLE(ITER it_begin, ITER it_end) const
	{
		double e = 0.0;

		for (auto it = it_begin; it != it_end; ++it)
			e += double(it->second) * log(ValueAt(it->first));

		return e;
	}

	/*!
	 * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
	 *
	 * \param[in] 	it_begin	iterator pointing to first double-size_t pair of input data
	 * \param[in] 	it_end  ending iterator
	 * \param[in] 	nbSeeds		number of seeds
	 * \param[in] 	epsilon		precision
	 * \param[in] 	maximalIterations	maximal number of iterations
	 *
	 * \return the number of iterations done to optimize the mixture
	 */
	template<typename ITER> unsigned int UnivariateGaussianMixture::EM(ITER it_begin, ITER it_end, size_t nbSeeds, double epsilon, size_t maximalIterations)
	{
		unsigned int nbIterations = 0;
		size_t nbMembers = size_t(nbSeeds);
		size_t nbPatterns = 0;
		size_t nbKeys = std::distance(it_begin, it_end);

		///////////
		// Setup //
		///////////

		auto min_value = std::numeric_limits<double>::infinity();
		auto max_value = -std::numeric_limits<double>::infinity();

		for (auto it = it_begin; it != it_end; ++it)
		{
			auto v = it->first;

			min_value = Min(min_value, v);
			max_value = Max(max_value, v);
			nbPatterns += it->second;
		}

		members.clear();


		double delta = (max_value - min_value) / double(nbSeeds);
		double Seed = min_value + delta / 2.0;

		// Heuristic setup : gaussians are placed equidistant from Min to Max value
		//                   variance set to catch at least one data value

		for (size_t k = 0; k < nbMembers; ++k)
		{
			// Warning : if data distribution is too sparse, variance should be adapted

			bool redo = true;
			double var = delta;
			double sigma = sqrt(var);

			auto it = it_begin;

			while (redo)
			{
				redo = (Abs(Seed - it->first) > sigma);
				++it;

				if ((it == it_end) && redo)
				{
					var += delta;
					sigma = sqrt(var);
					it = it_begin;
				}
			}

			AddMember(UnivariateGaussianPDF(Seed, var), 1.0);
			Seed += delta;
		}

		// Heuristic setup : mobile centroids



		MatrixDouble Proba(nbKeys, nbMembers, 0.0);

		bool Continue = true;
		double likelihood = 0.0;

		////////////////
		// Iterations //
		////////////////

		while(Continue)
		{
			// Expectation ("E" step)

			auto it = it_begin;

			for (size_t i = 0; i < nbKeys; ++i)
			{
				double xi = it->first;
				double Pi = 0.0;

				for (size_t k = 0; k < nbMembers; ++k)
				{
					double pik = this->ValueAt(xi, k);

					Proba[i][k] = pik;
					Pi += pik;
				}

				Proba.MultRow(i, 1.0 / Pi);
				++it;
			}

			// Maximization ("M" step)

			for (size_t k = 0; k < nbMembers; ++k)
			{
				const auto& Gk = members[k].first;
				double CumulPk = 0.0;
				double muk = Gk.GetMean();

				double mu = 0.0;
				double v = 0.0;

				it = it_begin;

				for (size_t i = 0; i < nbKeys; ++i)
				{
					double pik = Proba[i][k];
					double xi = it->first;
					double ni = double(it->second);

					CumulPk += ni * pik;
					mu += ni * pik * xi;
					v += ni * pik * Sqr(xi - muk);
					++it;
				}

				if (std::isfinite(mu) && std::isfinite(v))
				{
					mu /= CumulPk;
					v /= CumulPk;
				}
				else // Numeric limit reached. Recomputation needed
				{
					it = it_begin;
					mu = 0.0;
					v = 0.0;

					for (size_t i = 0; i < nbKeys; ++i)
					{
						double pik = Proba[i][k];
						double xi = it->first;
						double ni = double(it->second);

						mu += ni * pik * xi / CumulPk;
						v += ni * pik * Sqr(xi - muk) / CumulPk;
						++it;
					}
				}

				members[k].second = CumulPk / double(nbPatterns);
				members[k].first = UnivariateGaussianPDF(mu, v);
			}

			double NewLikelihood = MLLE(it_begin, it_end);
			double LikelihoodDiff = fabs(NewLikelihood - likelihood);

			likelihood = NewLikelihood;
			++nbIterations;

			Continue = ((nbIterations < maximalIterations) && (LikelihoodDiff > epsilon));
		}

		return nbIterations;
	}

}

#endif

