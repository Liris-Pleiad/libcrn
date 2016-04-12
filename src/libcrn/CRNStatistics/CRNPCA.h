/* Copyright 2009-2016 Jean DUONG, CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNPCA.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNPCA_HEADER
#define CRNPCA_HEADER

#include <CRNMath/CRNSquareMatrixDouble.h>
#include <map>

namespace crn
{
	/*! \brief Class to perform Principal Componant Analysis
	 *
	 * Principal Componant Analysis
	 * 
	 * \author 	Jean DUONG
	 * \date	September 2009
	 * \version	0.1
	 * \ingroup stats
	 */
	class PCA: public Object
	{
		public:
			/*! \brief Constructor */
			PCA(const MatrixDouble &data, bool data_reduction_flag = true);
			/*! \brief Constructor */
			PCA(const std::vector< std::vector<double> > &data, bool data_reduction_flag = true);
			/*! \brief Constructor from weighted vectors */
			PCA(const std::vector< std::vector<double> > &data, const std::vector<size_t> &cards, bool data_reduction_flag = true);
			/*! \brief Constructor from map */
			PCA(const std::map< std::vector<double>, size_t > &data, bool data_reduction_flag = true);
			/*! \brief Constructor from iterators */
			template<typename ITER> PCA(ITER begin, ITER end, bool data_reduction_flag = true);
			/*! \brief Constructor */
			PCA(const PCA&) = default;
			PCA(PCA&&) = default;
			/*! \brief Destructor */
			virtual ~PCA() override = default;

			PCA& operator=(const PCA&) = default;
			PCA& operator=(PCA&&) = default;

			/*! \brief Returns the dimension of feature space */
			size_t GetDimension() const noexcept { return dimension; }
			/*! \brief Returns the mean values computed for sample data */
			const std::vector<double>& GetMeans() const noexcept { return means; }
			/*! \brief Returns the mean value of d-th feature computed for sample data */
			double GetMean(size_t d) const;

			/*! \brief Returns the deviation computed for sample data */
			const std::vector<double>& GetDeviations() const noexcept { return deviations; }
			/*! \brief Returns the deviation of d-th feature computed for sample data */
			double GetDeviation(size_t d) const;

			/*! \brief Returns the eigensystem computed on covariance matrix of last sample data */
			const std::multimap<double, MatrixDouble>& GetEigensystem() const noexcept { return eigensystem; }

			/*! \brief Apply transform to given patterns */
			MatrixDouble Transform(const MatrixDouble &patterns, size_t nb_features = 1u) const;
			/*! \brief Apply transform to given patterns */
			std::vector<std::vector<double>> Transform(const std::vector< std::vector<double> > &data, const size_t nb_features = 0u) const;

			/*! \brief Apply reverse transform to get given patterns' pre-images. */
			std::vector<std::vector<double>> ReverseTransform(const std::vector< std::vector<double> > &data) const;

			void Deserialize(xml::Element &el);
			xml::Element Serialize(xml::Element &parent) const;
		private:

			/*! \brief Optimized 2x2 correlation matrix diagonalization */
			std::multimap<double, MatrixDouble> makeCorrelationSpectralEigensystem(double g) const;

			size_t dimension;
			std::vector<double> means;
			std::vector<double> deviations;
			std::multimap<double, MatrixDouble> eigensystem;

			CRN_DECLARE_CLASS_CONSTRUCTOR(PCA)
		public : PCA(xml::Element& el):means(1,1),dimension(1),deviations(1,1){Deserialize(el);}
	};
	template<> struct IsSerializable<PCA> : public std::true_type {};
	template<> struct IsClonable<PCA> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(PCA)

	/*!
	 * Constructor
	 * 
	 * Performs principal component analysis over patterns given as a value-cardinal pairs
	 *
	 * \param[in] begin	iterator pointing at the begining of the data collection
	 * \param[in] end	ending iterator
	 * \param[in] data_reduction_flag	flag to indicate if data should be reduced for PCA estimate
	 */

	template<typename ITER> PCA::PCA(ITER begin, ITER end, bool data_reduction_flag)
	{
		std::vector< std::vector<double> > altered_data;
		std::vector<double> scales;
		dimension = (begin->first).size();
		size_t nb_patterns = 0;
		size_t nb_prototypes = 0;

		// Compute means

		for (ITER it = begin; it != end; ++it)
		{
			nb_patterns += it->second;
			++nb_prototypes;
		}

		means = std::vector<double>(dimension);

		for (ITER it = begin; it != end; ++it)
		{
			std::vector<double> pat = it->first;

			double scale = (double) it->second / (double) nb_patterns;

			for (size_t ft = 0; ft < dimension; ++ft)
				means[ft] += pat[ft] * scale;
		}

		// Data centering

		for (ITER it = begin; it != end; ++it)
		{
			std::vector<double> pat = it->first;
			size_t pop = it->second;

			std::vector<double> new_pat;

			for (size_t ft = 0; ft < dimension; ++ft)
				new_pat.push_back(pat[ft] - means[ft]);

			altered_data.push_back(new_pat);
			scales.push_back((double) pop / (double) (nb_patterns - 1));
		}

		// Compute standard deviation on centered data

		deviations = std::vector<double>(dimension);

		for (size_t k = 0; k < nb_prototypes; ++k)
		{
			std::vector<double> pat = altered_data[k];
			double scale = scales[k];

			for (size_t ft = 0; ft < dimension; ++ft)
			{
				double val = pat[ft];
				deviations[ft] += scale * val * val;
			}
		}

		for (size_t ft = 0; ft < dimension; ++ft)
			deviations[ft] = sqrt(deviations[ft]);

		// Data reduction

		if (data_reduction_flag)
		{	
			for (size_t ft = 0; ft < dimension; ++ft)
			{
				double ratio = 1.0 / deviations[ft];

				for (size_t k = 0; k < nb_prototypes; ++k)
					(altered_data[k])[ft] *= ratio;
			}
		}

		// Covariance matrix

		SquareMatrixDouble cmat(dimension, 0.0);
		double scale_correction = (double)(nb_patterns - 1) / (double) nb_patterns;

		for (size_t k = 0; k < nb_prototypes; ++k)
		{
			std::vector<double> pat = altered_data[k];
			double scale = (double) scales[k] * scale_correction;

			for (size_t i = 0; i < dimension; ++i)
				for (size_t j = i; j < dimension; ++j)
					cmat.IncreaseElement(i, j, scale * pat[i] * pat[j]);
		}

		for (size_t i = 0; i < dimension; ++i)
			for (size_t j = i + 1; j < dimension; ++j)
				cmat.At(j, i) = cmat.At(i, j);

		if (dimension == 2)
		{
			if (data_reduction_flag)
				eigensystem = makeCorrelationSpectralEigensystem(cmat.At(0, 1));
			else
				eigensystem = cmat.MakeSpectralEigensystem();
		}
		else
			eigensystem = cmat.MakeJacobiEigensystem();
	}
}

#endif

