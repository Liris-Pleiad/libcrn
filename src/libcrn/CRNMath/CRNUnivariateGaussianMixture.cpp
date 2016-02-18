/* Copyright 2008-2016 INSA Lyon, CoReNum
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
 * file: CRNUnivariateGaussianMixture.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNMath/CRNUnivariateGaussianMixture.h>
#include <CRNException.h>
#include <CRNMath/CRNUnivariateGaussianPDF.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace crn;

/*!
 * Destructor
 */
UnivariateGaussianMixture::~UnivariateGaussianMixture()
{
	members.clear();
}

/*!
 * Get a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	integer
 * \return	the member
 */
UnivariateGaussianPDF UnivariateGaussianMixture::GetMember(size_t k) const
{
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::GetMember(size_t k): ") + 
				_("index out of bounds."));
	}
    
	return members[k].first;
}

/*!
 * Get weight for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	integer
 *
 * \return	the weight
 */
double UnivariateGaussianMixture::GetWeight(size_t k) const
{
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::GetWeight(size_t k): ") + 
				_("index out of bounds."));
	}
    
	return members[k].second;
}

/*!
 * Get mean for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	integer
 *
 * \return	the mean
 */
double UnivariateGaussianMixture::GetMean(size_t k) const
{
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::GetMean(size_t k): ") + 
				_("index out of bounds."));
	}
    
	return members[k].first.GetMean();
}

/*!
 * Get variance for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	integer
 *
 * \return	the variance
 */
double UnivariateGaussianMixture::GetVariance(size_t k) const
{
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::GetVariance(size_t k): ") + 
				_("index out of bounds."));
	}
    
	return members[k].first.GetVariance();
}

/*!
 * Add a member in mixture
 *
 * \param[in] 	PDF	pointer to gaussian PDF to add as new member
 * \param[in] 	w	Weight of the new member
 */
void UnivariateGaussianMixture::AddMember(UnivariateGaussianPDF pdf, double w)
{
	members.push_back(std::make_pair(pdf, w));
}

/*!
 * Set a member in mixture
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	PDF	pointer to gaussian PDF to add as new member
 * \param[in] 	w	Weight of the new member
 * \param[in] 	k	index
 */
void UnivariateGaussianMixture::SetMember(UnivariateGaussianPDF pdf, double w, size_t k)
{
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::SetMember("
					"UnivariateGaussianPDF PDF, double w, size_t k): ") + 
				_("index out of bounds."));
	}
	members[k] = std::make_pair(pdf, w);
}

struct pair_comp_mean
{
	pair_comp_mean(bool r):reverse_flag(r){}
	
	inline bool operator()(const std::pair<UnivariateGaussianPDF, double> &e1, const std::pair<UnivariateGaussianPDF, double> &e2) const
	{
		if (reverse_flag)
			return e1.first.GetMean() > e2.first.GetMean();
		else
			return e1.first.GetMean() < e2.first.GetMean();
	}
	
	private:
		bool reverse_flag;
};

struct pair_comp_variance
{
	pair_comp_variance(bool r):reverse_flag(r){}
	
	inline bool operator()(const std::pair<UnivariateGaussianPDF, double> &e1, const std::pair<UnivariateGaussianPDF, double> &e2) const
	{
		if (reverse_flag)
			return e1.first.GetVariance() > e2.first.GetVariance();
		else
			return e1.first.GetVariance() < e2.first.GetVariance();
	}
	
	private:
		bool reverse_flag;
};

struct pair_comp_weight
{
    pair_comp_weight(bool r):reverse_flag(r){}
    
    inline bool operator()(const std::pair<UnivariateGaussianPDF, double> &e1, const std::pair<UnivariateGaussianPDF, double> &e2) const
    {
        if (reverse_flag)
            return e1.second > e2.second;
        else
            return e1.second < e2.second;
    }
    
private:
    bool reverse_flag;
};

/*!
 * Sort members in mixture in increasing mean order
 */
void UnivariateGaussianMixture::SortMembersByMeans(bool reverse)
{
	std::sort(members.begin(), members.end(), pair_comp_mean(reverse));
}

/*!
 * Sort members in mixture in increasing mean order
 */
void UnivariateGaussianMixture::SortMembersByVariances(bool reverse)
{
	std::sort(members.begin(), members.end(), pair_comp_variance(reverse));
}

/*!
 * Sort members in mixture in increasing weight
 */
void UnivariateGaussianMixture::SortMembersByWeights(bool reverse)
{
    std::sort(members.begin(), members.end(), pair_comp_weight(reverse));
}

/*!
 * Evaluates an element
 *
 * \param[in] 	x	data argument to evaluate
 *
 * \return the probability of the element
 */
double UnivariateGaussianMixture::ValueAt(const double x) const
{
	double d = 0.0;
	
	for (auto & elem : members)
		d += elem.second * elem.first.ValueAt(x);
	
	return d;
}

/*!
 * Evaluate only for one member of the mixture
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	x	data argument to evaluate
 * \param[in] 	k	member index
 *
 * \return	the probability of the member
 */
double UnivariateGaussianMixture::ValueAt(const double x, size_t k, bool weight_flag) const
{	
	if (!isValidMemberIndex(k))
		throw ExceptionDomain(StringUTF8("UnivariateGaussianMixture::ValueAt(const double x, size_t k): ") + 
				_("index out of bounds."));
	
    double val = members[k].first.ValueAt(x);
    
    if (weight_flag)
        val *= members[k].second;
    
    return val;
}

/*!
 * Maximum log-likelyhood estimator
 *
 * \param[in] 	data	data stored as a column matrix
 *
 * \return	the maximum log-likelyhood of a set of patterns
 */
double UnivariateGaussianMixture::MLLE(const MatrixDouble& data) const
{
	size_t nbPatterns = data.GetRows();
	double e = 0.0;
	
	for (size_t k = 0; k < nbPatterns; ++k)
		e += log(ValueAt(data.At(k, 0)));
    
	return e;
}

/*!
 * Maximum log-likelyhood estimator
 *
 * \param[in] 	data	set of patterns stored as rows of a data matrix
 *
 * \return	the maximum log-likelyhood of a set of patterns
 */
double UnivariateGaussianMixture::MLLE(const std::vector<double> &data) const
{
	size_t nbPatterns = data.size();
	double e = 0.0;
	
	for (size_t k = 0; k < nbPatterns; ++k)
		e += log(ValueAt(data[k]));
    
	return e;
}

/*! 
 * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
 *
 * \param[in] 	patterns			set of patterns stored as rows of a data matrix
 * \param[in] 	nbSeeds				integer
 * \param[in] 	epsilon				double
 * \param[in] 	maximalIterations	integer
 *
 * \return the number of iterations done to optimize the mixture
 */
unsigned int UnivariateGaussianMixture::EM(const MatrixDouble& patterns, size_t nbSeeds, double epsilon, size_t maximalIterations)
{
	unsigned int nbIterations = 0;
	size_t nbMembers = size_t(nbSeeds);
	size_t nbPatterns = patterns.GetRows();
	
	///////////
	// Setup //
	///////////
	
	members.clear();

	double min_value = patterns.GetMin();
	double max_value = patterns.GetMax();
	double delta = (max_value - min_value) / double(nbSeeds);
	double seed = min_value + delta / 2.0;

	// Heuristic setup : gaussians are placed equidistant from Min to Max value
	//                   variance set to catch at least one data value
	
	for (size_t k = 0; k < nbMembers; ++k)
	{
		// Warning : if data distribution is too sparse, variance should be adapted
		
		bool redo = true;
		double var = delta;
		double sigma = sqrt(var);
		size_t counter = 0;
		
		while (redo)
		{
			double v = patterns[counter][0];
			
			redo = (fabs(seed - v) > sigma);
			++counter;
			
			if ((counter >= nbPatterns) && redo)
			{
				var += delta;
				sigma = sqrt(var);
				counter = 0;
			}
		}
		
		AddMember(UnivariateGaussianPDF(seed, var), 1.0);
		
		seed += delta;
	}	
		
	MatrixDouble Proba(nbPatterns, nbMembers, 0.0);
	
	bool Continue = true;
	double likelihood = 0.0;
	
	////////////////
	// Iterations //
	////////////////
	
	while(Continue)
	{
		// Expectation
		
		for (size_t i = 0; i < nbPatterns; ++i)
		{
			double xi = patterns[i][0];
			double Pi = 0.0;
			
			for (size_t k = 0; k < nbMembers; ++k)
			{  
				double pik = this->ValueAt(xi, k);
				
				Proba[i][k] = pik;
				Pi += pik;
			}
			
			Proba.MultRow(i, 1.0 / Pi);
		}
		
		// Maximization
		
		for (size_t k = 0; k < nbMembers; ++k)
		{
			UnivariateGaussianPDF Gk = members[k].first;
			double CumulPk = 0.0;
			double muk = Gk.GetMean();
					
			double mu = 0.0;
			double v = 0.0;             
			
			for (size_t i = 0; i < nbPatterns; ++i)
			{
				double pik = Proba[i][k];
				double xi = patterns[i][0];

				CumulPk += pik;
				mu += pik * xi;
				v += pik * Sqr(xi - muk);
			}                    
			
            // Numeric limit reached. Recomputation needed
            if (std::isinf(mu) || std::isinf(v))
            {
                mu = 0.0;
                v = 0.0;
                
                for (size_t i = 0; i < nbPatterns; ++i)
                {
                    double pik = Proba[i][k];
                    double xi = patterns[i][0];
                    
                    mu += pik * xi / CumulPk;
                    v += pik * Sqr(xi - muk) / CumulPk;
                }
            }
            else
            {
                mu /= CumulPk;
                v /= CumulPk;
            }
            
			members[k].second = double(CumulPk) / double(nbPatterns);
			members[k].first = UnivariateGaussianPDF(mu, v);
		}
		
		double NewLikelihood = MLLE(patterns);
		double LikelihoodDiff = fabs(NewLikelihood - likelihood);
		
		likelihood = NewLikelihood;
		nbIterations++;
		
		Continue = ((nbIterations < maximalIterations) && (LikelihoodDiff > epsilon));
	}

	return nbIterations;
}

/*! 
 * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
 *
 * \param[in] 	patterns	set of patterns stored as standard vector
 * \param[in] 	nbSeeds		number of seeds
 * \param[in] 	epsilon		precision
 * \param[in] 	maximalIterations	maximal number of iterations
 *
 * \return the number of iterations done to optimize the mixture
 */
unsigned int UnivariateGaussianMixture::EM(const std::vector<double> &patterns, size_t nbSeeds, double epsilon, size_t maximalIterations)
{
	unsigned int nbIterations = 0;
	size_t nbMembers = size_t(nbSeeds);
	size_t nbPatterns = patterns.size();
	auto spatterns = patterns;
	
	///////////
	// Setup //
	///////////
	
	members.clear();
	std::sort(spatterns.begin(), spatterns.end());

	double min_value = spatterns.front();
	double max_value = spatterns.back();
	double delta = (max_value - min_value) / double(nbSeeds);
	double seed = min_value + delta / 2.0;

	// Heuristic setup : gaussians are placed equidistant from Min to Max value
	//                   variance set to catch at least one data value
	
	for (size_t k = 0; k < nbMembers; ++k)
	{
		// Warning : if data distribution is too sparse, variance should be adapted
		
		bool redo = true;
		double var = delta;
		double sigma = sqrt(var);
		size_t counter = 0;
		
		while (redo)
		{
			double v = spatterns[counter];
			
			redo = (fabs(seed - v) > sigma);
			++counter;
			
			if ((counter >= nbPatterns) && redo)
			{
				var += delta;
				sigma = sqrt(var);
				counter = 0;
			}
		}
		
		AddMember(UnivariateGaussianPDF(seed, var), 1.0);
		seed += delta;
	}	
		
	MatrixDouble Proba(nbPatterns, nbMembers, 0.0);
	
	bool Continue = true;
	double likelihood = 0.0;
	
	////////////////
	// Iterations //
	////////////////
	
	while(Continue)
	{		
		// Expectation
		
		for (size_t i = 0; i < nbPatterns; ++i)
		{
			double xi = spatterns[i];
			double Pi = 0.0;
			
			for (size_t k = 0; k < nbMembers; ++k)
			{  
				double pik = this->ValueAt(xi, k);
				
				Proba[i][k] = pik;
				Pi += pik;
			}
            
			Proba.MultRow(i, 1.0 / Pi);
		}
		
		// Maximization
		
		for (size_t k = 0; k < nbMembers; ++k)
		{
			UnivariateGaussianPDF Gk = members[k].first;
			double CumulPk = 0.0;
			double muk = Gk.GetMean();
					
			double mu = 0.0;
			double v = 0.0;             
			
			for (size_t i = 0; i < nbPatterns; ++i)
			{
				double pik = Proba[i][k];
				double xi = spatterns[i];

				CumulPk += pik;
				mu += pik * xi;
				v += pik * Sqr(xi - muk);
			}                    
			
            if (std::isfinite(mu) && std::isfinite(v))
            {
                mu /= CumulPk;
                v /= CumulPk;
            }
            else // Numeric limit reached. Recomputation needed
            {
                mu = 0.0;
                v = 0.0;
                
                for (size_t i = 0; i < nbPatterns; ++i)
                {
                    double pik = Proba[i][k];
                    double xi = spatterns[i];
                    
                    mu += pik * xi / CumulPk;
                    v += pik * Sqr(xi - muk) / CumulPk;
                }
            }
            
			members[k].second = double(CumulPk) / double(nbPatterns);
			members[k].first = UnivariateGaussianPDF(mu, v);
		}
		
		double NewLikelihood = MLLE(spatterns);
		double LikelihoodDiff = fabs(NewLikelihood - likelihood);
		
		likelihood = NewLikelihood;
		nbIterations++;
		
		Continue = ((nbIterations < maximalIterations) && (LikelihoodDiff > epsilon));
	}
	
	return nbIterations;
}

/*****************************************************************************/
/*! Simulate a random sample following the mixture's low
*
* \param[in]	n	size of sample
* \param[in]	m	temporary sample size
* \param[in]	reseed shall the random seed be reinitialized?
*
* \return a set of patterns generated by the mixture's low
*/
std::vector<double> UnivariateGaussianMixture::MakeRandomSample(size_t n, size_t m, bool reseed) const
{
	size_t nbPDF = GetNbMembers();
	
	auto patterns = std::vector<double>();
	
	std::vector<int> pop;
	std::vector<double> cumul_weights;
	std::vector<double> index_sample;

	// Create uniform sample to choose indexes
	// This should become a function !!!
	
	if (reseed)
	{
		srand((unsigned int)time(nullptr));
	}
		
	for (size_t k = 0; k < n; ++k)
	{	
		index_sample.push_back(((double)(rand())) / ((double)(RAND_MAX)));
	}
	
	// Cumulative weights from the mixture
	
	double mass = 0.0;
		
	for (size_t k = 0; k < nbPDF; k++)
	{
		mass += GetWeight(k);
		
		cumul_weights.push_back(mass);
	}
		
	for (size_t k = 0; k < nbPDF; ++k)
	{
		cumul_weights[k] /= mass;
		pop.push_back(0);
	}
	
	// Build indexes to indicate which PDF of the mixture is used to generate each pattern
	
	for (size_t k = 0; k < n; k++)
	{
		double d = index_sample[k];
		bool continue_flag = true;
		size_t Id = 0;
		
		while (continue_flag)
		{
			if ((cumul_weights[Id] >= d) || (Id == nbPDF - 1))
			{
				continue_flag = false;
			}
			else
			{
				++Id;
			}
		}
		
		++pop[Id];
	}

	// Create patterns
		
	for (size_t p = 0; p < nbPDF; ++p)
	{
		int subsample_pop = pop[p];
		
		auto subsample = GetMember(p).MakeRandomSample(subsample_pop, m, reseed);
		
		for (int r = 0; r < subsample_pop; ++r)
		{
			patterns.push_back(subsample[r]);
		}
	}

  return patterns;	
}

/*! 
 * Dump the mixture parameters to a string
 *
 * \return	the string
 */
String UnivariateGaussianMixture::ToString() const
{
	String s;
    
	for (size_t k = 0; k < members.size(); ++k)
	{
		s += ToString(k) + U"\n";
	}

	return s;
}

/*! 
 * Dump the parameters for one member in the mixture to a string
 *
 * \param[in] 	k	integer
 *
 * \return	the string
 */
 String UnivariateGaussianMixture::ToString(size_t k) const
{
	String s;
	
	if (isValidMemberIndex(k))
	{
		s += U"Member : ";
		s += k;
		s += U" [0..";
		s += members.size() - 1;
		s += U"]\nWeight   = ";
		s += members[k].second;
		s += U"\nMean     = ";
		s += (members[k].first).GetMean();
		s += U"\nVariance = ";
		s += (members[k].first).GetVariance();
	}
	
	return s;
}

/*!
 * Unsafe load from XML node
 *
 * \author Jean DUONG
 *
 * \throws	ExceptionInvalidArgument	not a UnivariateGaussianMixture
 * \throws	ExceptionNotFound	cannot find attribute
 * \throws	ExceptionDomain	wrong attribute
 *
 * \param[in]	el	the element to load
 */
void UnivariateGaussianMixture::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool UnivariateGaussianMixture::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	
	std::vector<std::pair<UnivariateGaussianPDF, double> > newmembers;
	
	for (xml::Element sub_el = el.BeginElement(); sub_el != el.EndElement(); ++sub_el)
	{
		double w = sub_el.GetAttribute<double>("weight", false); // may throw
		
		UnivariateGaussianPDF pdf = UnivariateGaussianPDF(sub_el); // may throw
		
		newmembers.push_back(std::make_pair(pdf, w));
	}

	members.swap(newmembers);
}

/*!
 * Unsafe save
 *
 * \author Jean DUONG
 *
 * \param[in]	parent	the parent element to which we will add the new element
 *
 * \return The newly created element
 */
xml::Element UnivariateGaussianMixture::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	
	for (auto & elem : members)
	{
		xml::Element sub_el(elem.first.Serialize(el));
		sub_el.SetAttribute("weight", elem.second);
	}
	
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(UnivariateGaussianMixture)
	CRN_DATA_FACTORY_REGISTER(U"UnivariateGaussianMixture", UnivariateGaussianMixture)
CRN_END_CLASS_CONSTRUCTOR(UnivariateGaussianMixture)

