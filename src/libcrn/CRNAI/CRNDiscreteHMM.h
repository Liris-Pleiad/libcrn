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
 * file: CRNDiscreteHMM.h
 * \author Jean DUONG, Yann LEYDIER
 */

#ifndef CRNDISCRETEHMM_HEADER
#define CRNDISCRETEHMM_HEADER

#include <CRNObject.h>
#include <CRNMath/CRNMatrixInt.h>
#include <CRNMath/CRNMatrixDoublePtr.h>
#include <CRNMath/CRNSquareMatrixDoublePtr.h>

namespace crn
{
	/*! \brief Discrete HMM class
	 *
	 * Discrete Hidden Markov Model
	 * 
	 * \author 	Jean DUONG
	 * \date	September 2008
	 * \version	0.3
	 * \ingroup math
	 */
	class DiscreteHMM: public Object
	{
		public:
		
			/*! \brief Default constructor */
			DiscreteHMM():nbStates(1),nbSymbols(1),stateTransitionProbability(nullptr),stateGivenSymbolProbability(nullptr),firstStateProbability(nullptr) {}
			/*! \brief Constructor */
			DiscreteHMM(int nstates, int nsymbs);
			/*! \brief Copy constructor */
			DiscreteHMM(const DiscreteHMM &h);

			DiscreteHMM& operator=(const DiscreteHMM&) = delete;
			DiscreteHMM(DiscreteHMM&&) noexcept;
			DiscreteHMM& operator=(DiscreteHMM&&) noexcept;

			/*! \brief Destructor */
			virtual ~DiscreteHMM() override;

			/*! \brief Tests the equality of two models */
			bool operator==(const DiscreteHMM &other) const;

			/*! \brief Returns the number of states */
			size_t GetNbStates() const noexcept { return nbStates; }

			/*! \brief Returns the number of symbols */
			size_t GetNbSymbols() const noexcept { return nbSymbols; }
			
			/*! \brief Returns a reference to the state transition probability matrix */
			SCSquareMatrixDouble GetStateTransitionProbability() const { return stateTransitionProbability; }
			/*! \brief Returns a reference to the state given symbol probability matrix */
			SCMatrixDouble GetStateGivenSymbolProbability() const { return stateGivenSymbolProbability; }
			/*! \brief Returns a reference to the first state probability matrix */
			SCMatrixDouble GetFirstStateProbability() const { return firstStateProbability; }
			/*! \brief Sets the state transition probability matrix */
			void SetStateTransitionProbability(const SquareMatrixDouble& a);
			/*! \brief Sets the state given symbol probability matrix */
			void SetStateGivenSymbolProbability(const MatrixDouble& b);
			/*! \brief Sets the first state probability matrix */
			void SetFirstStateProbability(const MatrixDouble& p);

			/*! \brief Checks if the model is valid */
			bool IsValid() const;
			/*! \brief Dumps a summary to a string */
			String ToString() const;

			/*! \brief Returns the a priori probability of an observed sequence */
			double SequenceProbability(const MatrixInt& observed) const;
			/*! \brief Returns the most likely state state sequence corresponding to an observed sequence */
			UMatrixInt MakeViterbi(const MatrixInt& observed) const;
			/*! \brief Learning from one observed sequence */
			void BaumWelchSingle(const MatrixInt& observed, size_t maxIter);
			/*! \brief Learning from multiple observed sequences */
			void BaumWelchMultiple(const MatrixInt& observationSet, size_t maxIter);

		private:
			/*! \brief Internal */
			UMatrixDouble alpha(const MatrixInt& observed) const;
			/*! \brief Internal */
			UMatrixDouble beta(const MatrixInt& observed) const;
			/*! \brief Internal */
			void forceConsistency();

			size_t nbStates; /*!< The number of states */
			size_t nbSymbols; /*!< The number of symbols */
			SSquareMatrixDouble stateTransitionProbability; /*!< The state transition probability matrix */
			SMatrixDouble stateGivenSymbolProbability; /*!< The state given symbol probability matrix */
			SMatrixDouble firstStateProbability; /*!< The first state probability matrix */
	};
	template<> struct IsClonable<DiscreteHMM> : public std::true_type {};

	CRN_ALIAS_SMART_PTR(DiscreteHMM)
}
#endif
