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
 * file: CRNMultivariateGaussianPDF.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNMULTIVARIATEGAUSSIANPDF_HEADER
#define CRNMULTIVARIATEGAUSSIANPDF_HEADER

#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNMath/CRNMultivariateGaussianPDFPtr.h>

namespace crn
{
    /****************************************************************************/
	/*! \brief Multivariate Gaussian distribution
	 *
	 * Model for multivariate Gaussian probability distribution function
	 * 
	 * \author 	Jean DUONG
	 * \date	August 2008
	 * \version	0.1
	 * \ingroup math
	 */
	class MultivariateGaussianPDF: public Object
	{
		public:
			/*! \brief Constructor */
			MultivariateGaussianPDF(const MatrixDouble& mu, const SquareMatrixDouble& sigma);
			MultivariateGaussianPDF(const MultivariateGaussianPDF &) = default;
			MultivariateGaussianPDF(MultivariateGaussianPDF &&) = default;

			/*! \brief Destructor */
			virtual ~MultivariateGaussianPDF() override;

			MultivariateGaussianPDF& operator=(const MultivariateGaussianPDF&) = default;
			MultivariateGaussianPDF& operator=(MultivariateGaussianPDF&&) = default;

			/*! \brief Returns the number of features */
			size_t GetDimension() const noexcept { return dimension; }
			/*! \brief Sets the number of features */
			void SetDimension(size_t d) noexcept;

			/*! \brief Returns the mean of a given density function */
			const MatrixDouble& GetMean() const { return mean; }
			/*! \brief Sets the mean of a given density function */
			void SetMean(const MatrixDouble &obj);

			/*! \brief Returns the variance of a given density function */
			const SquareMatrixDouble& GetVariance() const { return variance; }
			/*! \brief Sets the variance of a given density function */
			void SetVariance(const SquareMatrixDouble &obj);
			
			/*! \brief Evaluates a pattern */
			double ValueAt(const MatrixDouble &x) const;
			/*! \brief Evaluates a pattern */
			double ValueAt(const std::vector<double> &x) const;

			/*! \brief Check if PDF is valid (with finite values) */
			bool IsValid() const;
			
			/*! \brief Dumps a summary to a string */
			String ToString() const;
			        
		private:
			size_t dimension; /*!< the dimension of the data */
			MatrixDouble mean; /*!< the mean pattern */
			SquareMatrixDouble variance; /*!< covariance matrix */
			
			double scale_factor; /*!< internal temporary data */
			SquareMatrixDouble inverse_variance; /*!< the inverse of the covariance matrix */
			
			/*! \brief Internal */
			void updateAuxiliaryAttributes();

			CRN_DECLARE_CLASS_CONSTRUCTOR(MultivariateGaussianPDF)
	};

	template<> struct IsClonable<MultivariateGaussianPDF> : public std::true_type {};
   
}

#endif
