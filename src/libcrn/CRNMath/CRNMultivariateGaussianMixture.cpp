/* Copyright 2008-2015 INSA Lyon, CoReNum
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
 * file: CRNMultivariateGaussianMixture.cpp
 * \author Jean DUONG
 */

#include <iostream>

#include <CRNException.h>
#include <CRNMath/CRNUnivariateRandomTools.h>
#include <CRNMath/CRNMultivariateGaussianPDF.h>
#include <CRNMath/CRNMultivariateGaussianMixture.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNStatistics/CRNStatisticSample.h>
#include <CRNStringUTF8.h>
#include <CRNi18n.h>

using namespace crn;


/*!
 * Destructor
 */
MultivariateGaussianMixture::~MultivariateGaussianMixture()
{
	members.clear();
}

/*!
 * Set up parameters from another mixture's
 *
 * \param[in] 	*m	another mixture
 */
void MultivariateGaussianMixture::SetTo(const MultivariateGaussianMixture& m)
{
	SetDimension(m.GetDimension());

	members.clear();

	for (size_t tmp = 0; tmp < m.GetNbMembers(); tmp++)
	{
		const MultivariateGaussianPDF pdf = m.members[tmp].first;
		double w = m.GetWeight(tmp);

		members.push_back(std::make_pair(pdf, w));
	}	
}


/*!
 * Set dimension
 *
 * \param[in] 	k	dimension of data space
 */
void MultivariateGaussianMixture::SetDimension(size_t k) noexcept
{
	dimension = k;
}

/*!
 * Return a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	index of the density function to return
 * * \return	the kth density function
 */
MultivariateGaussianPDF MultivariateGaussianMixture::GetMember(size_t k) const
{
	if (!isValidMemberIndex(k))
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::GetMember(int k): ") + 
				_("index out of bounds."));

	return members[k].first;
}

/*!
 * Get weight for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	index of a density function
 %
 * \return	the weight of the density function
 */
double MultivariateGaussianMixture::GetWeight(size_t k) const
{
	if (!isValidMemberIndex(k))
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::GetMember(size_t k): ") + 
				_("index out of bounds."));

	return members[k].second;
}

/*!
 * Get mean for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	index of a density function
 %
 * \return	the mean of the density function
 */
MatrixDouble MultivariateGaussianMixture::GetMean(size_t k) const
{
	if (!isValidMemberIndex(k))
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::GetMember(size_t k): ") + 
				_("index out of bounds."));

	return members[k].first.GetMean();
}

/*!
 * Get variance for a member
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	k	index of a density function
 *
 * \return	the variance of the density function
 */
SquareMatrixDouble MultivariateGaussianMixture::GetVariance(size_t k) const
{
	if (!isValidMemberIndex(k))
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::GetMember(size_t k): ") + 
				_("index out of bounds."));

	return members[k].first.GetVariance();
}

/*!
 * Add a member in mixture
 *
 * \throws	ExceptionDimension	incompatible dimensions
 * \param[in] 	spdf	gaussian PDF to add as new member
 * \param[in] 	w	Weight of the new member
 */
void MultivariateGaussianMixture::AddMember(const MultivariateGaussianPDF &pdf, double w)
{
	size_t dim = pdf.GetDimension();

	if (members.empty())
		dimension = dim;

	if (dimension == dim)
		members.push_back(std::make_pair(pdf, w));
	else
		throw ExceptionDimension(_("Incompatible dimensions"));
}

/*!
 * Replace a member in mixture
 *
 * \throws	ExceptionDimension	incompatible dimensions
 * \throws	ExceptionDomain	invalid index
 * \param[in] 	spdf	the new gaussian PDF
 * \param[in] 	w	the new weight
 * \param[in] 	k	index
 */
void MultivariateGaussianMixture::SetMember(const MultivariateGaussianPDF &pdf, double w, size_t k)
{
	if (isValidMemberIndex(k))
	{
		size_t dim = pdf.GetDimension();

		if (dimension == dim)
			members[k] = std::make_pair(pdf, w);
		else
			throw ExceptionDimension(_("Incompatible dimensions"));
	}
	else
		throw ExceptionDomain(_("Invalid index"));
}


/*!
 * Evaluate a pattern
 *
 * \param[in] 	X	the column matrix to evaluate
 *
 * \return	the image of X
 */
double MultivariateGaussianMixture::ValueAt(const MatrixDouble& X) const
{
	double d = 0.0;

	for (size_t k = 0; k < members.size(); ++k)
		d += members[k].second * members[k].first.ValueAt(X);

	return d;
}

/*!
 * Evaluate a pattern only for one member of the mixture
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	X	the column matrix to evaluate
 * \param[in] 	k	member index
 * \param[in]   w   flag to indicate if weight is used
 *
 * \return the image of X though the kth density function
 */
double MultivariateGaussianMixture::ValueAt(const MatrixDouble& X, size_t k, bool w) const
{	
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::ValueAt(const MatrixDouble& X, size_t k): ") + 
				_("index out of bounds."));
	}
	else if ((X.GetRows() != dimension) || (X.GetCols() != 1))
	{
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::ValueAt(const MatrixDouble* X, size_t k): ") + 
				_("incompatible dimensions."));	
	}

    if (w)
        return members[k].second * members[k].first.ValueAt(X);
    else
        return members[k].first.ValueAt(X);
}

/*!
 * Evaluate a pattern only for one member of the mixture
 *
 * \throw	ExceptionDomain	index out of bounds
 *
 * \param[in] 	x	pattern vector to evaluate
 * \param[in] 	k	member index
 * \param[in]   w   flag to indicate if weight is used
 *
 * \return the image of X though the kth density function
 */
double MultivariateGaussianMixture::ValueAt(const std::vector<double> &x, size_t k, bool w) const
{	
	if (!isValidMemberIndex(k))
	{
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::ValueAt(const std::vector<double> &x, size_t k): ") + 
				_("index out of bounds."));
	}
	else if (x.size() != dimension)
	{
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture::ValueAt(const std::vector<double> &x, size_t k): ") + 
				_("incompatible dimensions."));	
	}
    
    double val = members[k].first.ValueAt(x);

    if (w)
        val *= members[k].second;
        
    return val;
}

/*!
 * Maximum log-likelihood estimator
 *
 * \param[in] 	Data	set of patterns stored as rows of a data matrix
 *
 * \return	the likelihood that the mixture modelizes the set of patterns
 */
double MultivariateGaussianMixture::MLLE(const MatrixDouble& Data) const
{
	size_t nbPatterns = Data.GetRows();
	double E = 0.0;

	MatrixDouble x(dimension, 1, 0.0);

	for (size_t k = 0; k < nbPatterns; ++k)
	{	
		for (size_t c = 0; c < dimension; ++c)
			x.At(c, 0) = Data[k][c];

		E += log(ValueAt(x));
	}

	return E;
}

/*!
 * Maximum log-likelihood estimator
 *
 * \param[in] 	Data	set of patterns stored as rows of a data matrix
 *
 * \return	the likelihood that the mixture modelizes the set of patterns
 */
double MultivariateGaussianMixture::MLLE(const std::vector< std::vector<double> > &data) const
{
    double E = 0.0;
    
    for (size_t k = 0; k < data.size(); ++k)
        E += log(ValueAt(data[k]));
    
    return E;
}

/*! 
 * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
 *
 * \param[in] 	patterns	set of patterns stored as rows of a data matrix
 * \param[in] 	nbSeeds		the number of density functions
 * \param[in] 	epsilon		the minimum likelihood gain between two iterations
 * \param[in] 	maximalIterations		the maximum number of iterations
 *
 * \return	the number of iterations done to optimize the mixture
 */
unsigned int MultivariateGaussianMixture::EM(const MatrixDouble& patterns, size_t nbSeeds, double epsilon, size_t maximalIterations)
{
	unsigned int nbIterations = 0;
	size_t nbMembers = nbSeeds;
	size_t nbPatterns = patterns.GetRows();

	dimension = patterns.GetCols();
	MatrixDouble proba(nbPatterns, nbMembers, 0.0);

	///////////
	// Setup //
	///////////

	members.clear();

	MatrixDouble mu = patterns.MakeColumnMeans().Transpose();
	SquareMatrixDouble sigma = patterns.MakeCovariance();

	std::vector<MatrixDouble> g_samples;
	//std::vector<double> deltas;

	if (nbMembers > 1)
	{
		for (size_t d = 0; d < dimension; ++d)
		{
			MatrixDouble ck = patterns.MakeColumn(d);

			double min = ck.GetMin();
			double max = ck.GetMax();
			double delta = (max - min) / (double(nbMembers) - 1.0);

			MatrixDouble sample(nbMembers, 1, 0.0);

			for (size_t k = 0; k < nbMembers; k++)
				sample[k][0] = min + double(k) * delta;

			g_samples.push_back(sample);
		}
	}
	else
	{
		for (size_t d = 0; d < dimension; ++d)
		{
			MatrixDouble sample(1, 1, mu[d][0]);

			g_samples.push_back(sample);
		}
	}

	for (size_t k = 0; k < nbMembers; ++k)
	{
		// Set up mean

		MatrixDouble m(dimension, 1, 0.0);

		for (size_t d = 0; d < dimension; ++d)
			m[d][0] = g_samples[d][k][0];

		// Create member.
		AddMember(MultivariateGaussianPDF(m, sigma), 1.0);
	}
    
	////////////////
	// Iterations //
	////////////////

	// Some computational variables

	bool reloop = true;
	double likelihood = - std::numeric_limits<double>::infinity();

	std::vector<double> xi(dimension);

	SquareMatrixDouble m(dimension, 0.0);
	MultivariateGaussianMixture backup;

	while (reloop && (nbIterations < maximalIterations))
	{
		backup = *this;

		// Step E : Expectation

		for (size_t i = 0; i < nbPatterns; ++i)
		{
			// Retrieve i-th pattern
			for (size_t j = 0; j < dimension; ++j)
				xi[j] = patterns[i][j];

			double pi = 0.0;

			for (size_t k = 0; k < nbMembers; ++k)
			{
				double pik = ValueAt(xi, k);
				
				proba[i][k] = pik;
				pi += pik;
			}

			proba.MultRow(i, 1.0 / pi);
		}

		//Step M : Maximisation

		for (size_t k = 0; k < nbMembers; ++k)
		{
			double cumulPk = 0.0;
			mu.SetAll(0.0);
			sigma.SetAll(0.0);

			auto mean_k = GetMean(k);

			for (size_t i = 0; i < nbPatterns; ++i)
			{
				double pik = proba[i][k];
				cumulPk += pik;

				// Retrieve i-th pattern
				for (size_t j = 0; j < dimension; ++j)
				{
					double val = patterns[i][j];

					mu[j][0] += val * pik;
					xi[j] = val - mean_k[j][0];
				}

				for (size_t r = 0; r < dimension; ++r)
					for (size_t c = 0; c < dimension; ++c)
						m[r][c] = xi[r] * xi[c];

				m.Mult(pik);
				sigma.Add(m);
			}

			double invCumulPk = 1.0 / cumulPk;

			mu.Mult(invCumulPk);
			sigma.Mult(invCumulPk);

			members[k].second = double(cumulPk) / double(nbPatterns);
			members[k].first = MultivariateGaussianPDF(mu, sigma);			
		}

		double newLikelihood = MLLE(patterns);	
		double likelihoodDiff = newLikelihood - likelihood;

		likelihood = newLikelihood;
		nbIterations++;

		// Global output for each loop

		//std::cout << "lle = " << newLikelihood << std::endl;

		if (likelihoodDiff < 0.0)
			SetTo(backup);
		else
			reloop = IsValid() && (likelihoodDiff > epsilon);
	}

	return nbIterations;
}

/*!
 * Expectation-Maximization algorithm for Gaussian mixture estimation optimization
 *
 * \param[in] 	patterns	vector of patterns stored as vectors of doubles
 * \param[in] 	nbSeeds		the number of density functions
 * \param[in] 	epsilon		the minimum likelihood gain between two iterations
 * \param[in] 	maximalIterations		the maximum number of iterations
 *
 * \return	the number of iterations done to optimize the mixture
 */
unsigned int MultivariateGaussianMixture::EM(const std::vector<std::vector<double> > &patterns, size_t nbSeeds, double epsilon, size_t maximalIterations)
{
    unsigned int nbIterations = 0;
    size_t nbMembers = nbSeeds;
    size_t nbPatterns = patterns.size();
    
    dimension = patterns.front().size();
    MatrixDouble proba(nbPatterns, nbMembers, 0.0);
    
    ///////////
    // Setup //
    ///////////
    
    members.clear();
    
    MatrixDouble mu(MeanPattern(patterns), Orientation::VERTICAL);
    SquareMatrixDouble sigma(MakeCovariance(patterns));
    
    std::vector<MatrixDouble> g_samples;
    //std::vector<double> deltas;
    
    if (nbMembers > 1)
    {
        for (size_t d = 0; d < dimension; ++d)
        {
            double min = std::numeric_limits<double>::infinity();
            double max = -std::numeric_limits<double>::infinity();
            
            for (size_t k = 0; k < nbPatterns; ++k)
            {
                double v = patterns[k][d];
                
                min = Min(min, v);
                max = Max(max, v);
            }
            
            double delta = (max - min) / (double(nbMembers) - 1.0);
            
            MatrixDouble sample = MatrixDouble(nbMembers, 1, 0.0);
            
            for (size_t k = 0; k < nbMembers; ++k)
                sample[k][0] = min + double(k) * delta;
            
            g_samples.push_back(sample);
        }
    }
    else
    {
        for (size_t d = 0; d < dimension; ++d)
        {
            MatrixDouble sample = MatrixDouble(1, 1, mu[d][0]);
            
            g_samples.push_back(sample);
        }
    }
    
    for (size_t k = 0; k < nbMembers; ++k)
    {
        // Set up mean

        MatrixDouble m(dimension, 1, 0.0);
        
        for (size_t d = 0; d < dimension; ++d)
            m[d][0] = g_samples[d][k][0];
        
        // Create member
        
        AddMember(MultivariateGaussianPDF(m, sigma), 1.0);        
    }
    
    ////////////////
    // Iterations //
    ////////////////
    
    // Some computational variables
    
    bool reloop = true;
    double likelihood = - std::numeric_limits<double>::infinity();
    
    std::vector<double> xi;
    
    SquareMatrixDouble m(dimension, 0.0);
    MultivariateGaussianMixture backup;
    
    while (reloop && (nbIterations < maximalIterations))
    {
        backup = *this;
        
        // Step E : Expectation
        
        for (size_t i = 0; i < nbPatterns; ++i)
        {
            xi = patterns[i];
            
            double pi = 0.0;
            
            for (size_t k = 0; k < nbMembers; ++k)
            {
                double pik = ValueAt(xi, k);
                
                proba[i][k] = pik;
                pi += pik;
            }
            
            proba.MultRow(i, 1.0 / pi);
        }
        
        //Step M : Maximisation
        
        for (size_t k = 0; k < nbMembers; ++k)
        {
            double cumulPk = 0.0;
            mu.SetAll(0.0);
            sigma.SetAll(0.0);
            
            auto mean_k = GetMean(k);
            
            for (size_t i = 0; i < nbPatterns; ++i)
            {
                double pik = proba[i][k];
                cumulPk += pik;
                
                // Retrieve i-th pattern
                for (size_t j = 0; j < dimension; ++j)
                {
                    double val = patterns[i][j];
                    
                    mu[j][0] += val * pik;
                    xi[j] = val - mean_k[j][0];
                }
                
                for (size_t r = 0; r < dimension; ++r)
                    for (size_t c = 0; c < dimension; ++c)
                        m[r][c] = xi[r] * xi[c];
                
                m.Mult(pik);
                sigma.Add(m);
            }
            
            double invCumulPk = 1.0 / cumulPk;
            
            mu.Mult(invCumulPk);
            sigma.Mult(invCumulPk);
            
            members[k].second = double(cumulPk) / double(nbPatterns);
            members[k].first = MultivariateGaussianPDF(mu, sigma);			
        }
        
        double newLikelihood = MLLE(patterns);
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

/*! 
 * Dump the mixture parameters to a string
 *
 * \return the string
 */
String MultivariateGaussianMixture::ToString() const
{
	String s;
    
	for (size_t k = 0; k < members.size(); k++)
		s += ToString(k) + U"\n";

	return s;
}

/*! 
 * Dump the parameters of one member in the mixture to a string
 *
 * \param[in] 	k	the index of the density function
 *
 * \return the string
 */
String MultivariateGaussianMixture::ToString(size_t k) const
{
	String s;
    
	if (isValidMemberIndex(k))
	{
		s += U"Member : ";
		s += k;
		s += U" [0..";
		s += members.size() - 1;
		s += U"]\nWeight = ";
		s += members[k].second;
		s += U"\n\nMean = \n";
		s += ((members[k].first).GetMean()).ToString();
		s += U"\nVariance = \n";
		s += ((members[k].first).GetVariance()).ToString();
	}
	return s;
}

/*! 
 * Test if mixture is valid i.e. if all paratemers have finite values
 *
 * \return true if success, false else
 */

bool MultivariateGaussianMixture::IsValid() const
{
	bool status = true;

	size_t k = 0;

	while ((k < GetNbMembers()) && status)
	{
		double w = GetWeight(k);

		status = GetMember(k).IsValid() && (w == w);
		k++;
	}

	return status;
}

