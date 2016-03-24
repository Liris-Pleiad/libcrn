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
 * file: CRNGaussianSCHMM.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNGAUSSIANSCHMM_HEADER
#define CRNGAUSSIANSCHMM_HEADER

#include <CRNObject.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNSquareMatrixDoublePtr.h>
#include <CRNMath/CRNMultivariateGaussianMixturePtr.h>

namespace crn
{
	/*! \brief Semi-continuous gaussian HMM class
	 *
	 * Semi-continuous gaussian Hidden Markov Model
	 * 
	 * \author 	Jean DUONG
	 * \date	September 2008
	 * \version	0.2
	 * \ingroup math
	 */
	class GaussianSCHMM: public Object
	{
		public:
		
			/*! \brief Constructor */
			GaussianSCHMM(size_t nstates, size_t nsymbs);
			/*! \brief Copy constructor */
			GaussianSCHMM(const GaussianSCHMM& h) = default;
			/*! \brief Destructor */
			virtual ~GaussianSCHMM() override {}

			GaussianSCHMM& operator=(const GaussianSCHMM&) = default;
			GaussianSCHMM(GaussianSCHMM&&) = default;
			GaussianSCHMM& operator=(GaussianSCHMM&&) = default;

			/*! \brief Returns the number of states */
			size_t GetNbStates() const noexcept { return nbStates; }
			/*! \brief Returns the dimension of observable symbol space */
			size_t GetSymbolDimension() const noexcept { return symbolDimension; }

			/*! \brief Returns the state transition probability matrix */
			SquareMatrixDouble GetStateTransitionProbability() const {return stateTransitionProbability; }
			/*! \brief Returns the first state probability matrix */
			MatrixDouble GetFirstStateProbability() const { return firstStateProbability; }
			/*! \brief Returns all the state probability laws */
			const std::vector<MultivariateGaussianMixture>& GetStateGivenSymbolProbability() const { return stateGivenSymbolProbability; }

			/*! \brief Returns a state probability law */
			MultivariateGaussianMixture GetStateGivenSymbolProbability(size_t k) const;
			/*! \brief Returns all the state probabilities for a symbol */
			MatrixDouble GetStateGivenSymbolProbability(const MatrixDouble& x) const;
			/*! \brief Returns a state probability for a symbol */
			double GetStateGivenSymbolProbability(size_t k, const MatrixDouble& x) const;
			
			/*! \brief Sets the state transition probability matrix */
			void SetStateTransitionProbability(const SquareMatrixDouble& a);
			/*! \brief Sets the first state probability matrix */
			void SetFirstStateProbability(const MatrixDouble& p);

			/*! \brief Learning from one sequence */
			void BaumWelchSingle(const MatrixDouble& observed, unsigned int maxIter);

		private:
			size_t nbStates; /*!< The number of states */
			size_t symbolDimension; /*!< The dimension of the symbols */
					
			SquareMatrixDouble stateTransitionProbability; /*!< Matrix A in Rabiner */
			std::vector<MultivariateGaussianMixture> stateGivenSymbolProbability; /*!< Gaussian mixtures models (one model per state) */
			MatrixDouble firstStateProbability; /*!< Vector Pi in Rabiner */

			/*\brief Internal */
			inline bool IsValidIndex(size_t k) const noexcept { return k < nbStates; }
			/*\brief Internal */
			MatrixDouble alpha(const MatrixDouble &observed) const;
			/*\brief Internal */
			MatrixDouble beta(const MatrixDouble &observed) const;

			CRN_DECLARE_CLASS_CONSTRUCTOR(GaussianSCHMM)
	};
	template<> struct IsClonable<GaussianSCHMM> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(GaussianSCHMM)
}
#endif
