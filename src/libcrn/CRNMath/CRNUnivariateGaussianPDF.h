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
 * file: CRNUnivariateGaussianPDF.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNUNIVARIATEGAUSSIANPDF_HEADER
#define CRNUNIVARIATEGAUSSIANPDF_HEADER

#include <CRNString.h>
#include <cmath>

namespace crn
{
	/****************************************************************************/
	/*! \brief Univariate Gaussian distribution
	 *
	 * Model for univariate Gaussian probability distribution function
	 * 
	 * \author 	Jean DUONG
	 * \date	October 2008
	 * \version	0.2
	 * \ingroup math
	 */
	class UnivariateGaussianPDF: public Object
	{
		public:
			/*! \brief Constructor */
			UnivariateGaussianPDF(double m, double v);
			UnivariateGaussianPDF(const UnivariateGaussianPDF &) = default;
			UnivariateGaussianPDF(UnivariateGaussianPDF &&) = default;

			/*! \brief Destructor */
			virtual ~UnivariateGaussianPDF() override;

			/*! \brief No assignment allowed */
			UnivariateGaussianPDF& operator=(const UnivariateGaussianPDF&) = default;
			UnivariateGaussianPDF& operator=(UnivariateGaussianPDF&&) = default;

			/*! \brief This is a Clonable object */
			virtual Protocol GetClassProtocols() const noexcept override { return crn::Protocol::Clonable | crn::Protocol::Serializable; }
			/*! \brief Returns the name of the class */
			virtual const String& GetClassName() const override { static const String cn(U"UnivariateGaussianPDF"); return cn; }
			/*! \brief Clones the model */
			virtual UObject Clone() const override { return std::make_unique<UnivariateGaussianPDF>(*this); }

			/*! \brief Returns the mean of a given density function */
			double GetMean() const noexcept { return mean; }
			/*! \brief Sets the mean of a given density function */
			void SetMean(const double mu) noexcept;

			/*! \brief Returns the variance of a given density function */
			double GetVariance() const noexcept { return variance; }
			/*! \brief Sets the variance of a given density function */
			void SetVariance(const double v);

			/*! \brief Returns the standard deviation of a given density function */
			double GetDeviation() const { return sqrt(variance); }

			/*! \brief Evaluates a pattern */
			double ValueAt(const double x) const;

			/*! \brief Dumps a summary to a string */
			virtual String ToString() const override;

			/*! \brief Estimates intersection point between with another PDF, using trinom solving */
			double IntersectionAbscissa(const UnivariateGaussianPDF &pdf) const;
			/*! \brief Estimates intersection point between with another PDF, using dichotomy */
			double IntersectionAbscissaApprox(const UnivariateGaussianPDF &pdf) const;

			/*! \brief Creates a data sample following the PDF's probability law */
			std::vector<double> MakeRandomSample(size_t n = 1, size_t m = 100, bool reseed = true) const;

		private:
			void deserialize(xml::Element &el) override;
			xml::Element serialize(xml::Element &parent) const override;

			double mean; /*!< the mean of the Gaussian */
			double variance; /*!< the variance of the Gaussian */
			double scaleFactor; /*!< internal temporary data */

			/*! \brief Internal */
			void updateAuxiliaryAttributes();

			CRN_DECLARE_CLASS_CONSTRUCTOR(UnivariateGaussianPDF)
			CRN_SERIALIZATION_CONSTRUCTOR(UnivariateGaussianPDF)
	};

	CRN_ALIAS_SMART_PTR(UnivariateGaussianPDF)
}

#endif
