/* Copyright 2012-2016 INSA-Lyon
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
 * file: CRNSpectralClustering.cpp
 * \author Yann LEYDIER
 */

#include <iostream>
#include <fstream>
#include <set>

#include <CRNAI/CRNSpectralClustering.h>
#include <CRNAI/CRNClassifResult.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNException.h>
#include <CRNi18n.h>

using namespace crn;

/*! Clustering with local auto scale
 * \throws	ExceptionInvalidArgument	neighborhood <= 1
 * \throws	ExceptionDimension	empty distance matrix
 * \throws	ExceptionRuntime	too many iterations
 * \param[in]	distance_matrix	the distance matrix of the data
 * \param[in]	sigma_neighborhood	the number of neighbors used to compute the local sigma
 * \param[in]	epsilon	the maximal distance between two elements (if distance > epsilon, then the elements cannot be in the same class)
 */
SpectralClustering SpectralClustering::CreateLocalScaleFromNN(const SquareMatrixDouble &distance_matrix, size_t sigma_neighborhood, double epsilon)
{
	if (sigma_neighborhood < 1)
		throw ExceptionInvalidArgument(_("Neighborhood to compute sigma must be >=1."));
	size_t nelem = distance_matrix.GetRows();
	if (!nelem)
		throw ExceptionDimension(_("Empty distance matrix."));
	std::vector<double> sigmas(nelem);
	for (size_t r = 0; r < nelem; ++r)
	{
		std::set<double> dist;
		for (size_t c = 0; c < nelem; ++c)
			if (c != r)
				dist.insert(distance_matrix[r][c]);
		auto it = dist.begin();
		std::advance(it, sigma_neighborhood - 1);
		if (it != dist.end())
			sigmas[r] = *it;
		else
			sigmas[r] = *dist.rbegin();
	}

	SquareMatrixDouble w(nelem, 0);
	for (size_t r = 0; r < nelem; ++r)
		for (size_t c = 0; c < nelem; ++c)
		{
			if ((r == c) || (distance_matrix[r][c] > epsilon))
				w[r][c] = 0;
			else
				w[r][c] = exp(-Sqr(distance_matrix[r][c]) / (2 * sigmas[r] * sigmas[c]));
		}
	return SpectralClustering(w);
}

/*! Clustering with global auto scale
 * \throws	ExceptionInvalidArgument	neighborhood <= 1
 * \throws	ExceptionDimension	empty distance matrix
 * \throws	ExceptionRuntime	too many iterations
 * \param[in]	distance_matrix	the distance matrix of the data
 * \param[in]	sigma_neighborhood	the number of neighbors used to compute the local sigma
 * \param[in]	epsilon	the maximal distance between two elements (if distance > epsilon, then the elements cannot be in the same class)
 */
SpectralClustering SpectralClustering::CreateGlobalScaleFromNN(const SquareMatrixDouble &distance_matrix, size_t sigma_neighborhood, double epsilon)
{
	if (sigma_neighborhood < 1)
		throw ExceptionInvalidArgument(_("Neighborhood to compute sigma must be >=1."));
	size_t nelem = distance_matrix.GetRows();
	if (!nelem)
		throw ExceptionDimension(_("Empty distance matrix."));
	double sigma = 0;
	for (size_t r = 0; r < nelem; ++r)
	{
		std::set<double> dist;
		for (size_t c = 0; c < nelem; ++c)
			if (c != r)
				dist.insert(distance_matrix[r][c]);
		auto it = dist.begin();
		std::advance(it, sigma_neighborhood - 1);
		if (it != dist.end())
			sigma += *it;
		else
			sigma += *dist.rbegin();
	}
	sigma /= double(nelem);
	//std::cout << sigma << std::endl;

	return CreateFixedScale(distance_matrix, sigma, epsilon);
}

/*! Clustering with global auto scale
 * \throws	ExceptionInvalidArgument	dimension <= 1
 * \throws	ExceptionDimension	empty distance matrix
 * \throws	ExceptionRuntime	too many iterations
 * \param[in]	distance_matrix	the distance matrix of the data
 * \param[in]	dimension	the dimension of the original data
 * \param[in]	epsilon	the maximal distance between two elements (if distance > epsilon, then the elements cannot be in the same class)
 */
SpectralClustering SpectralClustering::CreateGlobalScaleFromDimension(const SquareMatrixDouble &distance_matrix, size_t dimension, double epsilon)
{
	if (dimension < 1)
		throw ExceptionInvalidArgument(_("Dimension must be >=1."));
	size_t nelem = distance_matrix.GetRows();
	if (!nelem)
		throw ExceptionDimension(_("Empty distance matrix."));
	double sigma = 0;
	for (size_t r = 0; r < nelem; ++r)
	{
		double m = *std::max_element(distance_matrix[r] + r, distance_matrix[r] + nelem);
		if (m > sigma) sigma = m;
	}
	sigma /= 2 * pow(double(nelem), 1 / double(dimension));
	//std::cout << sigma << std::endl;

	return CreateFixedScale(distance_matrix, sigma, epsilon);
}

/*! Clustering with local scale
 * \throws	ExceptionInvalidArgument	sigma < 0
 * \throws	ExceptionDimension	empty distance matrix
 * \throws	ExceptionRuntime	too many iterations
 * \param[in]	distance_matrix	the distance matrix of the data
 * \param[in]	sigma	the scale for Gaussian smoothing of the distance matrix
 * \param[in]	epsilon	the maximal distance between two elements (if distance > epsilon, then the elements cannot be in the same class)
 */
SpectralClustering SpectralClustering::CreateFixedScale(const SquareMatrixDouble &distance_matrix, double sigma, double epsilon)
{
	if (sigma < 0.0)
		throw ExceptionInvalidArgument(_("Sigma must be positive."));
	size_t nelem = distance_matrix.GetRows();
	if (!nelem)
		throw ExceptionDimension(_("Empty distance matrix."));

	sigma *= sigma * 2;
	SquareMatrixDouble w(nelem, 0);
	for (size_t r = 0; r < nelem; ++r)
		for (size_t c = 0; c < nelem; ++c)
		{
			if ((r == c) || (distance_matrix[r][c] > epsilon))
				w[r][c] = 0;
			else
				w[r][c] = exp(-Sqr(distance_matrix[r][c]) / sigma);
		}
	return SpectralClustering(w);
}

/*! Computes the projection
 * \throws	ExceptionRuntime	too many iterations
 * \param[in]	w	the affinity matrix
 */
SpectralClustering::SpectralClustering(const SquareMatrixDouble &w)
{
	SquareMatrixDouble d(w.GetRows());
	for (size_t r = 0; r < w.GetRows(); ++r)
	{
		double s = 0;
		for (size_t c = 0; c < w.GetCols(); ++c)
			s += w[r][c];
		if (s != 0) s = 1 / sqrt(s);
		d[r][r] = s;
	}
    auto l = d;
	l.Mult(w);
	l.Mult(d);

	eigenpairs = l.MakeTQLIEigensystem(1000);
}

/*! Returns the eigenvalues (sorted from highest to lowest)
 * \return the eigenvalues sorted from highest to lowest
 */
std::vector<double> SpectralClustering::GetEigenvalues() const
{
	std::vector<double> vals(eigenpairs.size());
	auto vit = vals.begin();
	for (auto eit = eigenpairs.rbegin(); eit != eigenpairs.rend(); ++eit)
		*vit++ = eit->first;
	return vals;
}

/*! Estimates the number of clusters 
 * \throws	ExceptionDomain	limit is <0 or >1
 * \param[in]	limit	the minimal eigenvalue to select. Normally it should be 1.0, but computational errors can make it necessary to use 0.9 for example.
 * \return	the number of classes and optimal dimension of the projected data
 */
size_t SpectralClustering::EstimateClusterNumber(double limit) const
{
	if ((limit < 0.0) || (limit > 1.0))
		throw ExceptionDomain(_("Eigenvalues should be in [0, 1]."));
	size_t n = 1;
	for (auto eit = eigenpairs.rbegin(); eit != eigenpairs.rend(); ++eit)
	{
		if (eit->first < limit)
			break;
		n += 1;
	}
	return n;
}

/*! Projects the data on the first coordinates
 * \throws	ExceptionDimension	ncoordinates < 1
 * \param[in]	ncoordinates	the dimension of the projection space
 * \param[in]	normalize	shall the elements be normalized on the unit circle?
 * \return	a vector of projected elements
 */
std::vector<std::vector<double>> SpectralClustering::ProjectData(size_t ncoordinates, bool normalize) const
{
	if (ncoordinates < 1)
		throw ExceptionDimension(_("Cannot project on less than one coordinate."));
	size_t nelem = eigenpairs.size();
	std::vector<std::vector<double> > data(nelem, std::vector<double>(ncoordinates));
	std::multimap<double, MatrixDouble>::const_reverse_iterator stopit;
	if (ncoordinates >= nelem)
		stopit = eigenpairs.rend();
	else
	{
		stopit = eigenpairs.rbegin();
		std::advance(stopit, ncoordinates);
	}
	size_t coord = 0;
	for (auto eit = eigenpairs.rbegin(); eit != stopit; ++eit)
	{
		for (size_t tmp = 0; tmp < nelem; ++tmp)
			data[tmp][coord] = eit->second[tmp][0];
		coord += 1;
	}
	if (normalize)
	{
		for (auto & elem : data)
		{
			double s = 0;
			for (size_t i = 0; i < ncoordinates; ++i)
				s += Sqr(elem[i]);
			s = sqrt(s);
			for (size_t i = 0; i < ncoordinates; ++i)
				elem[i] /= s;
		}
	}
	return data;
}

