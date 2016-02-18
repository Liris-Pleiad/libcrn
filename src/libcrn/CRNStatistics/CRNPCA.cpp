/* Copyright 2009-2016 Jean DUONG, INSA-Lyon, CoReNum
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
 * file: CRNPCA.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <stdio.h>
#include <iostream>

#include <CRNException.h>
#include <CRNStatistics/CRNPCA.h>
#include <CRNData/CRNDataFactory.h>
#include <CRNUtils/CRNXml.h>
#include <CRNi18n.h>

#include <CRNMath/CRNMath.h>
#include <CRNStatistics/CRNStatisticSample.h>

using namespace crn;

/*!
 * Constructor
 *
 * \param[in]	data matrix of double numbers in which patterns are stored as rows
 * \param[in]	data_reduction_flag	flag to indicate if the data have to be reduced
 */
PCA::PCA(const MatrixDouble &data, bool data_reduction_flag)
{
    dimension = data.GetCols();
    
    size_t nb_patterns = data.GetRows();
    bool status = ((nb_patterns > 1) && (dimension > 1));
    
    if (status)
    {
        means = std::vector<double>(dimension, 0.0);
        deviations = std::vector<double>(dimension, 0.0);
        
        // Simultaneous computation of means and
        // means of squared data (useful for variance further computation)
        std::vector<double> means_of_squares(dimension, 0.0);
        
        for (size_t k = 0; k < nb_patterns; ++k)
            for (size_t d = 0; d < dimension; ++d)
            {
                double val = data[k][d];
                
                means[d] += val / double(nb_patterns);
                means_of_squares[d] += Sqr(val) / double(nb_patterns);
            }
        
        // Check means of squared data
        status = (means_of_squares[0] == means_of_squares[0]);
        
        for (size_t d = 1; d < dimension; ++d)
            status &= (means_of_squares[d] == means_of_squares[d]);
        
        std::vector< std::vector<double> > transformed_data(nb_patterns, std::vector<double>(dimension));
        
        if (status) // Means for squared data are in legal range
        {
            // Deviation computed with König-Huygens formula
            for (size_t d = 0; d < dimension; ++d)
                deviations[d] = sqrt(means_of_squares[d] - Sqr(means[d]));
            
            // Data patterns centered and reduced
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = deviations[d];
                
                if (sigma != 1.0)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = (data[k][d] - mu) / sigma;
                else
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = data[k][d] - mu;
            }
        }
        else
        {
            // Deviation computed in "classical" way
            // Data patterns centered simultameously
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = 0.0;
                
                for (size_t k = 0; k < nb_patterns; ++k)
                {
                    double val = data[k][d];
                    
                    transformed_data[k][d] = val - mu;
                    sigma += Sqr(val - mu) / double(nb_patterns);
                }
                
                sigma = sqrt(sigma);
                deviations[d] = sigma;
                
                if (sigma != 1.0)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] /= sigma;
            }
        }
        
        SquareMatrixDouble cmat(MakeCovariance(transformed_data));
        
        if (dimension == 2)
        {
            if (data_reduction_flag)
                eigensystem = makeCorrelationSpectralEigensystem(cmat[0][1]);
            else
                eigensystem = cmat.MakeSpectralEigensystem();
        }
        else
            eigensystem = cmat.MakeJacobiEigensystem();
    }
}

/*!
 * Constructor
 * 
 * \param[in]	data vector of vectors of double numbers in which patterns are stored
 * \param[in]	data_reduction_flag	flag to indicate if the data have to be reduced
 */
PCA::PCA(const std::vector< std::vector<double> > &data, bool data_reduction_flag)
{
    dimension = data.front().size();
    
    size_t nb_patterns = data.size();
    bool status = ((nb_patterns > 1) && (dimension > 1));
    
    for (size_t k = 1; k < nb_patterns; ++k)
        if (data[k].size() != dimension)
        {
            status = false;
            break;
        }
    
    if (status)
    {
        means = std::vector<double>(dimension, 0.0);
        deviations = std::vector<double>(dimension, 0.0);
        
        // Simultaneous computation of means and
        // means of squared data (useful for variance further computation)
        std::vector<double> means_of_squares(dimension, 0.0);
        
        for (size_t k = 0; k < nb_patterns; ++k)
            for (size_t d = 0; d < dimension; ++d)
            {
                double val = data[k][d];
                
                means[d] += val / double(nb_patterns);
                means_of_squares[d] += Sqr(val) / double(nb_patterns);
            }
        
        // Check means of squared data
        status = (means_of_squares[0] == means_of_squares[0]);
        
        for (size_t d = 1; d < dimension; ++d)
            status &= (means_of_squares[d] == means_of_squares[d]);
        
        std::vector< std::vector<double> > transformed_data(nb_patterns, std::vector<double>(dimension));
        
        if (status) // Means for squared data are in legal range
        {
            // Deviation computed with König-Huygens formula
            for (size_t d = 0; d < dimension; ++d)
                deviations[d] = sqrt(means_of_squares[d] - Sqr(means[d]));
            
            // Centered and reduced versions of data patterns
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = deviations[d];
                
                if ((sigma != 1.0) && (sigma != 0.0) && data_reduction_flag)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = (data[k][d] - mu) / sigma;
                else
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = data[k][d] - mu;
            }
        }
        else
        {
            // Deviation computed in "classical" way
            // Centered data patterns are computed simultameously
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = 0.0;
                
                for (size_t k = 0; k < nb_patterns; ++k)
                {
                    double val = data[k][d];
                    
                    transformed_data[k][d] = val - mu;
                    sigma += Sqr(val - mu) / double(nb_patterns);
                }
                
                sigma = sqrt(sigma);
                deviations[d] = sigma;
                
                if ((sigma != 1.0) && (sigma != 0.0) && data_reduction_flag)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] /= sigma;
            }
        }

        SquareMatrixDouble cmat(MakeCovariance(transformed_data));
                
        if (dimension == 2)
        {
            if (data_reduction_flag)
                eigensystem = makeCorrelationSpectralEigensystem(cmat[0][1]);
            else
                eigensystem = cmat.MakeSpectralEigensystem();
        }
        else
            eigensystem = cmat.MakeJacobiEigensystem();
    }
}
			
/*!
 * Constructor
 * 
 * \param[in]	data collection of vectors of double numbers
 * \param[in]	cards pattern cardinals
 * \param[in]	data_reduction_flag	flag to indicate if the data have to be reduced
 */
PCA::PCA(const std::vector< std::vector<double> > &data, const std::vector<size_t> &cards, bool data_reduction_flag)
{	
    dimension = data.front().size();
    
    size_t nb_patterns = data.size();
    double sample_cardinal = 0.0;
    bool status = ((nb_patterns > 1) && (dimension > 1));
    
    for (size_t k = 1; k < nb_patterns; ++k)
    {
        if (data[k].size() != dimension)
        {
            status = false;
            break;
        }
        
        sample_cardinal += double(cards[k]);
    }
    
    if (status)
    {
        means = std::vector<double>(dimension, 0.0);
        deviations = std::vector<double>(dimension, 0.0);
        
        // Simultaneous computation of means and
        // means of squared data (useful for variance further computation)
        std::vector<double> means_of_squares(dimension, 0.0);
        
        for (size_t k = 0; k < nb_patterns; ++k)
            for (size_t d = 0; d < dimension; ++d)
            {
                double val = data[k][d];
                double wgt = double(cards[k]);
                
                means[d] += val * wgt / sample_cardinal;
                means_of_squares[d] += Sqr(val) * wgt / sample_cardinal;
            }
        
        // Check means of squared data
        status = (means_of_squares[0] == means_of_squares[0]);
        
        for (size_t d = 1; d < dimension; ++d)
            status &= (means_of_squares[d] == means_of_squares[d]);
        
        std::vector< std::vector<double> > transformed_data(nb_patterns, std::vector<double>(dimension));
        
        if (status) // Means for squared data are in legal range
        {
            // Deviation computed with König-Huygens formula
            for (size_t d = 0; d < dimension; ++d)
                deviations[d] = sqrt(means_of_squares[d] - Sqr(means[d]));
            
            // Data patterns centered and reduced
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = deviations[d];
                
                if (sigma != 1.0)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = (data[k][d] - mu) / sigma;
                else
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] = data[k][d] - mu;
            }
        }
        else
        {
            // Deviation computed in "classical" way
            // Data patterns centered simultameously
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = 0.0;
                
                for (size_t k = 0; k < nb_patterns; ++k)
                {
                    double val = data[k][d];
                    
                    transformed_data[k][d] = val - mu;
                    sigma += Sqr(val - mu) * double(cards[k]) / sample_cardinal;
                }
                
                sigma = sqrt(sigma);
                deviations[d] = sigma;
                
                if (sigma != 1.0)
                    for (size_t k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] /= sigma;
            }
        }
        
        SquareMatrixDouble cmat(MakeCovariance(transformed_data));
        
        if (dimension == 2)
        {
            if (data_reduction_flag)
                eigensystem = makeCorrelationSpectralEigensystem(cmat[0][1]);
            else
                eigensystem = cmat.MakeSpectralEigensystem();
        }
        else
            eigensystem = cmat.MakeJacobiEigensystem();
    }
}

/*!
 * Constructor
 * 
 * \param[in]	data map of vectors of double numbers with cardinals
 * \param[in]	data_reduction_flag	flag to indicate if the data have to be reduced
 */
PCA::PCA(const std::map< std::vector<double>, size_t > &data, bool data_reduction_flag)
{	
    dimension = data.begin()->first.size();
    
    size_t nb_patterns = data.size();
    double sample_cardinal = 0.0;
    bool status = ((nb_patterns > 1) && (dimension > 1));
    
    for (const auto& weighted_pattern:data)
    {
        if (weighted_pattern.first.size() != dimension)
        {
            status = false;
            break;
        }
        
        sample_cardinal += double(weighted_pattern.second);
    }

    if (status)
    {
        means = std::vector<double>(dimension, 0.0);
        deviations = std::vector<double>(dimension, 0.0);
        
        // Simultaneous computation of means and
        // means of squared data (useful for variance further computation)
        std::vector<double> means_of_squares(dimension, 0.0);
        
        for (const auto& weighted_pattern:data)
            for (size_t d = 0; d < dimension; ++d)
            {
                double val = weighted_pattern.first[d];
                double wgt = double(weighted_pattern.second);
                
                means[d] += val * wgt / sample_cardinal;
                means_of_squares[d] += Sqr(val) * wgt / sample_cardinal;
            }
        
        // Check means of squared data
        status = (means_of_squares[0] == means_of_squares[0]);
        
        for (size_t d = 1; d < dimension; ++d)
            status &= (means_of_squares[d] == means_of_squares[d]);
        
        std::vector< std::vector<double> > transformed_data(nb_patterns, std::vector<double>(dimension));
        
        if (status) // Means for squared data are in legal range
        {
            // Deviation computed with König-Huygens formula
            for (size_t d = 0; d < dimension; ++d)
                deviations[d] = sqrt(means_of_squares[d] - Sqr(means[d]));
            
            // Data patterns centered and reduced
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = deviations[d];
                
                if (sigma != 1.0)
                {
                    size_t k = 0;
                    
                    for (const auto& weighted_pattern:data)
                    {
                        transformed_data[k][d] = (weighted_pattern.first[d] - mu) / sigma;
                        ++k;
                    }
                }
                else
                {
                    size_t k = 0;
                    
                    for (const auto& weighted_pattern:data)
                    {
                        transformed_data[k][d] = weighted_pattern.first[d] - mu;
                        ++k;
                    }
                }
            }
        }
        else
        {
            // Deviation computed in "classical" way
            // Data patterns centered simultameously
            for (size_t d = 0; d < dimension; ++d)
            {
                double mu = means[d];
                double sigma = 0.0;
                size_t k = 0;
                
                for (const auto& weighted_pattern:data)
                {
                    double val = weighted_pattern.first[d];
                    
                    transformed_data[k][d] = val - mu;
                    sigma += Sqr(val - mu) * double(weighted_pattern.second) / sample_cardinal;
                    ++k;
                }
                
                sigma = sqrt(sigma);
                deviations[d] = sigma;
                
                if (sigma != 1.0)
                    for (k = 0; k < nb_patterns; ++k)
                        transformed_data[k][d] /= sigma;
            }
        }
        
        SquareMatrixDouble cmat(MakeCovariance(transformed_data));
        
        if (dimension == 2)
        {
            if (data_reduction_flag)
                eigensystem = makeCorrelationSpectralEigensystem(cmat[0][1]);
            else
                eigensystem = cmat.MakeSpectralEigensystem();
        }
        else
            eigensystem = cmat.MakeJacobiEigensystem();
    }

}

/*!
 * 2x2 correlation matrix diagonalization using spectral theorem
 * 
 * Eigenpairs are returned in a multimap sorted according eigenvalues.
 * 
 * \return vector of eigenpairs
 */
std::multimap<double, MatrixDouble> PCA::makeCorrelationSpectralEigensystem(double g) const
{
	std::multimap<double, MatrixDouble> eigen_pairs;
	
	double delta = 4 * g * g;
	
	double m_1 = sqrt(delta) / (2 * g);
	
	double fraction = 1.0 / sqrt(1.0 + m_1 * m_1);
	
	MatrixDouble eigen_vector_1((size_t)2, (size_t)1);
	MatrixDouble eigen_vector_2((size_t)2, (size_t)1);
	
	eigen_vector_1.At(0, 0) = fraction;
	eigen_vector_1.At(1, 0) = m_1 * fraction;
	
	eigen_vector_2.At(0, 0) = - m_1 * fraction;
	eigen_vector_2.At(1, 0) = fraction;
	
	double eigen_value_1 = (2 + sqrt(delta)) / 2.0;
	double eigen_value_2 = (2 - sqrt(delta)) / 2.0;
	
	eigen_pairs.insert(std::make_pair(eigen_value_1, eigen_vector_1));
	eigen_pairs.insert(std::make_pair(eigen_value_2, eigen_vector_2));
	
	return eigen_pairs;
}

/*! 
 * Returns the mean value of d-th feature
 *
 * \throws  ExceptionDimension	incompatible patter dimensions
 * \param[in]   d   index of desired value
 * \return  double value
 */
double PCA::GetMean(size_t d) const
{
    if (d >= dimension)
        throw ExceptionDimension(StringUTF8("PCA::GetMean(size_t d): ") + _("Index out of dimension range."));
    
    return means[d];
}

/*!
 * Returns the deviation of d-th feature
 *
 * \throws  ExceptionDimension	incompatible patter dimensions
 * \param[in]   d   index of desired value
 * \return  double value
 */
double PCA::GetDeviation(size_t d) const
{
    if (d >= dimension)
        throw ExceptionDimension(StringUTF8("PCA::GetMean(size_t d): ") + _("Index out of dimension range."));
    
    return deviations[d];
}

/*!
 * Perform projection of given patterns using current PCA attributes.
 * Patterns are translated to make centroid become the center of space,
 * then projected on pricipal componants from most to least
 * significant ones.
 *
 * \throws	ExceptionDimension	incompatible patter dimensions
 * \throws	ExceptionDomain	incompatible output dimension
 * \param[in] patterns	matrix of double numbers in which patterns are stored
 * \param[in] nb_features	the number of principal componants used for projection
 *
 * \return a matrix containing projected patterns
 */
MatrixDouble PCA::Transform(const MatrixDouble &patterns, const size_t nb_features) const
{
	if (patterns.GetCols() != dimension)
	{
		throw ExceptionDimension(StringUTF8("PCA::ApplyTransform(const MatrixDouble &patterns, const size_t nb_features): ") + _("Incompatible input pattern dimensions."));
	}
	else if (nb_features > dimension)
	{
		throw ExceptionDomain(StringUTF8("PCA::ApplyTransform(const MatrixDouble &patterns, const size_t nb_features): ") + _("Incompatible output dimensions."));
	}
	
	size_t nb_patterns = patterns.GetRows();
	MatrixDouble new_patterns(nb_patterns, nb_features);
	
	std::multimap<double, MatrixDouble>::const_reverse_iterator rev_iter;

	std::vector<double> translated_pattern(dimension);
	
	for (size_t p = 0; p < nb_patterns; p++)
	{
		for (size_t k = 0; k < dimension; k++)
			translated_pattern[k] = (patterns[p][k] - means[k]);

		rev_iter = eigensystem.rbegin();
		
		for (size_t f = 0; f < nb_features; f++)
		{
			// Scalar product <pattern|eigenvector>
			
			double cumul = 0.0;
			MatrixDouble eigen_vector = rev_iter->second;
			
			for (size_t k = 0; k < dimension; ++k)
				cumul += translated_pattern[k] * eigen_vector[k][0];
			
			new_patterns.At(p, f) = cumul;
			++rev_iter;
		}
	}
	
	return new_patterns;
}

/*!
 * Perform projection of given patterns using current PCA attributes.
 * Patterns are translated to make centroid become the center of space,
 * then projected on principal componants from most to least
 * significant ones.
 *
 * \param[in] data	vector of multivariate patterns
 * \param[in] nb_features	the number of principal componants used for projection
 *
 * \return a vector containing projected multivariate patterns
 */
std::vector<std::vector<double>> PCA::Transform(const std::vector< std::vector<double> > &data, const size_t nb_features) const
{
	size_t new_dimension = nb_features;
	
	if ((nb_features <= 0) || (nb_features > dimension))
		new_dimension = dimension;
	
	size_t nb_patterns = data.size();

	std::vector< std::vector<double> > new_patterns(nb_patterns, std::vector<double>(new_dimension));	
	
	// Reverse iterator to cross the ordered eigensystem
	std::multimap<double, MatrixDouble>::const_reverse_iterator rev_iter;
    // A vector to store temporary single translated pattern
    std::vector<double> translated_pattern(dimension);
    
	for (size_t p = 0; p < nb_patterns; ++p)
	{
		for (size_t k = 0; k < dimension; ++k)
			translated_pattern[k] = (data[p][k] - means[k]);

		rev_iter = eigensystem.rbegin();
		
		for (size_t f = 0; f < new_dimension; ++f)
		{
			// Scalar product <pattern|eigenvector>
			
			double cumul = 0.0;
			const auto& eigen_vector = rev_iter->second;
			
			for (size_t k = 0; k < dimension; ++k)
				cumul += translated_pattern[k] * eigen_vector[k][0];
			
			new_patterns[p][f] = cumul;
			++rev_iter;
		}
	}
	
	return new_patterns;
}

/*!
 * Apply reverse transform to get given patterns' pre-images.
 * No dimension change allowed for this version.
 *
 * \param[in] data	vector of multivariate patterns
 *
 * \return a vector containing projected multivariate patterns
 */
std::vector<std::vector<double>> PCA::ReverseTransform(const std::vector< std::vector<double> > &data) const
{
	size_t nb_patterns = data.size();
	std::vector< std::vector<double> > new_patterns;
	SquareMatrixDouble mat(dimension, 0.0);
	
	size_t j = 0;
	
	for (auto rev_iter = eigensystem.rbegin(); rev_iter != eigensystem.rend(); ++rev_iter)
	{
		MatrixDouble vect = rev_iter->second;
		
		for (size_t i = 0; i < dimension; ++i)
			mat[i][j] = vect[i][0];
		
		++j;
	}

	for (size_t p = 0; p < nb_patterns; ++p)
	{
		std::vector<double> pattern = data[p];
		std::vector<double> new_pattern(dimension);
	
		for (size_t i = 0; i < dimension; ++i)
		{
			double cumul = 0.0;
			
			for (size_t k = 0; k < dimension; ++k)
				cumul += mat[i][k] * pattern[k];
			
			new_pattern[i] = cumul + means[i];
		}

		new_patterns.push_back(new_pattern);
	}
	
	return new_patterns;
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
void PCA::deserialize(xml::Element &el)
{
	if (el.GetName() != GetClassName().CStr())
	{
		throw ExceptionInvalidArgument(StringUTF8("bool PCA::deserialize(xml::Element &el): ") + 
				_("Wrong XML element."));
	}
	
	std::multimap<double, MatrixDouble> newsystem;
	
	xml::Element sub_el(el.GetFirstChildElement("eigenpair"));
	while(sub_el)
	{
		double w = sub_el.GetAttribute<double>("eigenvalue", false); // may throw
		
		
		xml::Element mat_el(sub_el.GetFirstChildElement("MatrixDouble"));
		MatrixDouble mat(mat_el); // may throw
		
		newsystem.insert(std::make_pair(w, mat));
		
		sub_el = sub_el.GetNextSiblingElement("eigenpair");
	}
	
    SMatrixDouble mmat, dmat;
	sub_el = el.GetFirstChildElement("MatrixDouble");
	while (sub_el)
	{
		StringUTF8 role(sub_el.GetAttribute<StringUTF8>("role"));
		if (role == "means")
            mmat = std::make_shared<MatrixDouble>(sub_el); // may throw
		else if (role == "deviations")
			dmat = std::make_shared<MatrixDouble>(sub_el); // may throw
		sub_el = sub_el.GetNextSiblingElement("MatrixDouble");
	}
	if (!mmat || !dmat)
		throw crn::ExceptionNotFound(StringUTF8("bool PCA::deserialize(xml::Element &el): ") + _("Incomplete PCA xml element."));
    
    dimension = mmat->GetCols();
    means = std::vector<double>(dimension);
    deviations = std::vector<double>(dimension);
    
    for (size_t d = 0; d < dimension; ++d)
    {
        means[d] = mmat->At(0, d);
        deviations[d] = dmat->At(0, d);
    }
    
	eigensystem.swap(newsystem);
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
xml::Element PCA::serialize(xml::Element &parent) const
{
	xml::Element el(parent.PushBackElement(GetClassName().CStr()));
	el.SetAttribute("dimension", int(dimension));
	
    MatrixDouble mmat(1, dimension);
    MatrixDouble dmat(1, dimension);
    
    for (size_t d = 0; d < dimension; ++d)
    {
        mmat.At(0, d) = means[d];
        dmat.At(0, d) = deviations[d];
    }
    
	xml::Element s(mmat.Serialize(el));
	s.SetAttribute("role", "means");
	s = dmat.Serialize(el);
	s.SetAttribute("role", "deviations");

	std::multimap<double, MatrixDouble>::const_reverse_iterator rev_iter;
	
	for (rev_iter = eigensystem.rbegin(); rev_iter != eigensystem.rend(); ++rev_iter)
	{
		xml::Element sub_el(el.PushBackElement("eigenpair"));
		
		sub_el.SetAttribute("eigenvalue", rev_iter->first);
		
		rev_iter->second.Serialize(sub_el);		
	}
	
	return el;
}

CRN_BEGIN_CLASS_CONSTRUCTOR(PCA)
	CRN_DATA_FACTORY_REGISTER(U"PCA", PCA)
CRN_END_CLASS_CONSTRUCTOR(PCA)

