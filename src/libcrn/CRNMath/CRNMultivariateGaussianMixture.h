/* Copyright 2008-2015 INSA Lyon, CoReNum, Université Paris Descartes
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
 * file: CRNMultivariateGaussianMixture.h
 * \author Jean DUONG, Yann LEYDIER
 */

 
#ifndef CRNMULTIVARIATEGAUSSIANMIXTURE_HEADER
#define CRNMULTIVARIATEGAUSSIANMIXTURE_HEADER

#include <CRNString.h>
#include <CRNMath/CRNMultivariateGaussianPDF.h>
#include <CRNMath/CRNMultivariateGaussianMixturePtr.h>
#include <limits>

namespace crn
{
	/****************************************************************************/
	/*! \brief Multivariate gaussian mixture
	 *
	 * Model for multivariate gaussian mixture
	 * 
	 * \author 	Jean DUONG
	 * \date	August 2008
	 * \version	0.2
	 * \ingroup math
	 */
	class MultivariateGaussianMixture: public Object
	{
		public:
			/*! \brief Default constructor */
			MultivariateGaussianMixture():dimension(1){}
			/*! \brief Default constructor */
			MultivariateGaussianMixture(size_t d):dimension(d){}
			MultivariateGaussianMixture(const MultivariateGaussianMixture&) = default;
			MultivariateGaussianMixture(MultivariateGaussianMixture&&) = default;
        
            /*!
             * Create a gaussian mixture to model a given set of data
             *
             * \param[in] 	it_begin	iterator pointing to first (vector of double, size_t) pair of input data
             * \param[in] 	it_end      ending iterator
             * \param[in] 	nbSeeds		the number of density functions
             *
             * \return	the inferred gaussian mixture model
             */
            template<typename ITER> MultivariateGaussianMixture(ITER it_begin, ITER it_end, size_t nb_seeds = 2)
            {
                if (nb_seeds == 1)
                    AddMember(MultivariateGaussianPDF(MatrixDouble(MeanPattern(it_begin, it_end)), SquareMatrixDouble(MakeCovariance(it_begin, it_end))), 1.0);
                else
                    EM(it_begin, it_end, (unsigned int)(nb_seeds));
            }
        
			/*! \brief Clones the model */
			virtual UObject Clone() const override { return std::make_unique<MultivariateGaussianMixture>(*this); }
			
			MultivariateGaussianMixture& operator=(const MultivariateGaussianMixture&) = default;
			MultivariateGaussianMixture& operator=(MultivariateGaussianMixture&&) = default;

			/*! \brief Destructor */
			virtual ~MultivariateGaussianMixture() override;
			
			/*! \brief This is a Clonable object */
			virtual Protocol GetClassProtocols() const noexcept override { return Protocol::Clonable; }
			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const override { static const String cn(U"MultivariateGaussianMixture"); return cn; }

			/*! \brief Set mixture from another one */
			void SetTo(const MultivariateGaussianMixture& m);
			/*! \brief Returns the number of features */
			size_t GetDimension() const noexcept { return dimension; }
			/*! \brief Sets the number of features */
			void SetDimension(size_t k) noexcept;
			/*! \brief Returns the number of density functions */
			size_t GetNbMembers() const noexcept { return members.size(); }
			
			/*! \brief Returns the weight of a given density function */
			double GetWeight(size_t k) const;
			/*! \brief Returns a given density function */
			MultivariateGaussianPDF GetMember(size_t k) const;
			/*! \brief Returns the mean of a given density function */
			MatrixDouble GetMean(size_t k) const;
			/*! \brief Returns the variance of a given density function */
			SquareMatrixDouble GetVariance(size_t k) const;
			
			/*! \brief Adds a density function */
			void AddMember(const MultivariateGaussianPDF &pdf, double Weight);
			/*! \brief Replaces a given density function and its weight */
			void SetMember(const MultivariateGaussianPDF &pdf, double w, size_t k);
			
			/*! \brief Evaluates a pattern */
			double ValueAt(const MatrixDouble& X) const;
			/*! \brief Evaluates a pattern for a given density function */
			double ValueAt(const MatrixDouble& X, size_t k, bool w = false) const;
			/*! \brief Evaluates a pattern for a given density function */
			double ValueAt(const std::vector<double> &x, size_t k, bool w = false) const;

			/*! \brief Expectation Maximization */
			unsigned int EM(const MatrixDouble& patterns, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t MaximalIterations = 100);
        
			unsigned int EM(const std::vector<std::vector<double> > &patterns, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t MaximalIterations = 100);
        
            /*!
             * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
             *
             * \param[in] 	it_begin	iterator pointing to first (vector of double, size_t) pair of input data
             * \param[in] 	it_end      ending iterator
             * \param[in] 	nbSeeds		the number of density functions
             * \param[in] 	epsilon		the minimum likelihood gain between two iterations
             * \param[in] 	maximalIterations		the maximum number of iterations
             *
             * \return	the number of iterations done to optimize the mixture
             */
            template<typename ITER> unsigned int EM(ITER it_begin, ITER it_end, size_t nbSeeds = 2, double epsilon = std::numeric_limits<double>::epsilon(), size_t MaximalIterations = 100)
            {
                unsigned int nbIterations = 0;
                size_t nbMembers = nbSeeds;
                size_t nbKeys = std::distance(it_begin, it_end);
                size_t nbPatterns = 0;
                
                for (auto it = it_begin; it != it_end; ++it)
                    nbPatterns += it->second;
                
                dimension = it_begin->first.size();
                MatrixDouble proba(nbPatterns, nbMembers, 0.0);
                
                ///////////
                // Setup //
                ///////////
                
                members.clear();
                
                MatrixDouble mu(MeanPattern(it_begin, it_end), Orientation::VERTICAL);
                SquareMatrixDouble sigma(MakeCovariance(it_begin, it_end));
                
                std::vector<MatrixDouble> seeds;
                //std::vector<double> deltas;
                
                if (nbMembers > 1)
                {
                    for (size_t d = 0; d < dimension; ++d)
                    {
                        auto min = std::numeric_limits<double>::infinity();
                        auto max = -std::numeric_limits<double>::infinity();
                        auto it = it_begin;
                        
                        for (size_t k = 0; k < nbKeys; ++k)
                        {
                            double v(it->first[d]);
                            
                            min = Min(min, v);
                            max = Max(max, v);
                            ++it;
                        }
                        
                        double delta = (max - min) / (double(nbMembers) - 1.0);
                        
                        MatrixDouble sample = MatrixDouble(nbMembers, 1, 0.0);
                        
                        for (size_t k = 0; k < nbMembers; ++k)
                            sample[k][0] = min + double(k) * delta;
                        
                        seeds.push_back(sample);
                    }
                }
                else
                {
                    for (size_t d = 0; d < dimension; ++d)
                    {
                        MatrixDouble sample = MatrixDouble(1, 1, mu[d][0]);
                        
                        seeds.push_back(sample);
                    }
                }
                
                for (size_t k = 0; k < nbMembers; ++k)
                {
                    // Set up mean
                    
                    MatrixDouble m(dimension, 1, 0.0);
                    
                    for (size_t d = 0; d < dimension; ++d)
                        m[d][0] = seeds[d][k][0];
                    
                    // Create member
                    
                    AddMember(MultivariateGaussianPDF(m, sigma), 1.0);
                }
                
                ////////////////
                // Iterations //
                ////////////////
                
                // Some computational variables
                
                bool reloop = true;
                double likelihood = - std::numeric_limits<double>::infinity();
                
                //std::vector<double> xi;
                
                SquareMatrixDouble m(dimension, 0.0);
                MultivariateGaussianMixture backup;
                
                while (reloop && (nbIterations < MaximalIterations))
                {
                    backup = *this;
                    
                    // Step E : Expectation
                    
                    auto it = it_begin;
                    
                    for (size_t i = 0; i < nbKeys; ++i)
                    {
                        const auto& xi = it->first;
                        
                        for (auto count = 0; count < it->second; ++count)
                        {
                            double pi = 0.0;
                            
                            for (size_t k = 0; k < nbMembers; ++k)
                            {
                                double pik = ValueAt(xi, k);
                                
                                proba[i][k] = pik;
                                pi += pik;
                            }
                            
                            proba.MultRow(i, 1.0 / pi);
                        }
                        
                        ++it;
                    }
                    
                    //Step M : Maximisation
                    
                    for (size_t k = 0; k < nbMembers; ++k)
                    {
                        double cumulPk = 0.0;
                        mu.SetAll(0.0);
                        sigma.SetAll(0.0);
                        
                        auto mean_k = GetMean(k);
                        auto it = it_begin;
                        
                        std::vector<double> tmp_pattern(dimension);
                        
                        for (size_t i = 0; i < nbKeys; ++i)
                        {
                            double pik = proba[i][k];
                            
                            for (auto count = 0; count < it->second; ++count)
                            {
                                cumulPk += pik;
                                
                                // Retrieve i-th pattern
                                for (size_t j = 0; j < dimension; ++j)
                                {
                                    double val = it->first[j];
                                    
                                    mu[j][0] += val * pik;
                                    tmp_pattern[j] = val - mean_k[j][0];
                                }
                                
                                for (size_t r = 0; r < dimension; ++r)
                                    for (size_t c = 0; c < dimension; ++c)
                                        m[r][c] = tmp_pattern[r] * tmp_pattern[c];
                                
                                m.Mult(pik);
                                sigma.Add(m);
                            }
                            
                            ++it;
                        }
                        
                        double invCumulPk = 1.0 / cumulPk;
                        
                        mu.Mult(invCumulPk);
                        sigma.Mult(invCumulPk);
                        
                        members[k].second = double(cumulPk) / double(nbPatterns);
                        members[k].first = MultivariateGaussianPDF(mu, sigma);
                    }
                    
                    double newLikelihood = MLLE(it_begin, it_end);
                    double likelihoodDiff = newLikelihood - likelihood;
                    
                    likelihood = newLikelihood;
                    nbIterations++;
                    
                    // Global output for each loop
                    
                    if (likelihoodDiff < 0.0)
                        SetTo(backup);
                    else
                        reloop = IsValid() && (likelihoodDiff > epsilon);
                }
                
                return nbIterations;
            }

			/*! \brief Dumps a summary of the mixture to a string */
			virtual String ToString() const override;
			/*! \brief Dumps a summary of one element of the mixture to a string */
			String ToString(size_t k) const;

			/*! \brief Maximum log-likelihood estimation */
			double MLLE(const MatrixDouble& data) const;
			/*! \brief Maximum log-likelihood estimation */
			double MLLE(const std::vector< std::vector<double> > &data) const;
            /*! \brief Maximum log-likelihood estimation */
            template<typename ITER> double MLLE(ITER it_begin, ITER it_end) const
            {
                double e = 0.0;
                
                for (auto it = it_begin; it != it_end; ++it)
                    e += double(it->second) * log(ValueAt(it->first));
                
                return e;
            }

			/*! \brief Test if mixture is valid i.e. if all paratemers have finite values */
			bool IsValid() const;

		private:
			/*! \brief Checks if an index is valid */
			bool isValidMemberIndex(size_t k) const { return k < members.size(); }

			std::vector<std::pair<MultivariateGaussianPDF, double>> members; /*!< the Gaussians and their coefficient */
			size_t dimension; /*!< the dimension of the data */
	};
    
}

#endif
