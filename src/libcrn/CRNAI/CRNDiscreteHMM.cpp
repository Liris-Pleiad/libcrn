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
 * file: CRNDiscreteHMM.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNi18n.h>
#include <stdio.h>
#include <CRNException.h>
#include <CRNMath/CRNMatrixDouble.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNAI/CRNDiscreteHMM.h>

using namespace crn;

DiscreteHMM::DiscreteHMM(DiscreteHMM&&) noexcept = default;
DiscreteHMM& DiscreteHMM::operator=(DiscreteHMM&&) noexcept = default;

/*!
 * Copy constructor
 *
 * \param[in] 	h	another HMM
 */
DiscreteHMM::DiscreteHMM(const DiscreteHMM &h)
{
	nbStates = h.GetNbStates();
	nbSymbols = h.GetNbSymbols();
	
	stateTransitionProbability = h.GetStateTransitionProbability()->CloneAs<SquareMatrixDouble>();
	stateGivenSymbolProbability = h.GetStateGivenSymbolProbability()->CloneAs<MatrixDouble>();
	firstStateProbability = h.GetFirstStateProbability()->CloneAs<MatrixDouble>();
}

/*!
 * Constructor
 *
 * \param[in] 	nstates	number of states for HMM
 * \param[in]	nsymbs	number of symbols for HMM
*/
DiscreteHMM::DiscreteHMM(int nstates, int nsymbs):
	nbStates(nstates),
	nbSymbols(nsymbs)
{
	double InvNbStates = 1.0 / double(nbStates);
	double InvNbSymbols = 1.0 / double(nbSymbols);
	
	stateTransitionProbability = std::make_shared<SquareMatrixDouble>(nbStates, InvNbStates);
	stateGivenSymbolProbability = std::make_shared<MatrixDouble>(nbStates, nbSymbols, InvNbSymbols);
	firstStateProbability = std::make_shared<MatrixDouble>(nbSymbols, 1, InvNbSymbols);
}

/*!
 * Destructor
 */
DiscreteHMM::~DiscreteHMM()
{
}

/*! 
 * Set the state transition probability matrix. A copy of the matrix is made.
 *
 * \param[in]	a the new state transition probability matrix
 */
void DiscreteHMM::SetStateTransitionProbability(const SquareMatrixDouble& a)
{
	stateTransitionProbability = a.CloneAs<SquareMatrixDouble>();
	nbStates = a.GetRows();
}

/*! 
 * Set the state given symbol probability matrix. A copy of the matrix is made.
 *
 * \param[in]	b the new state given symbol probability matrix
 */
void DiscreteHMM::SetStateGivenSymbolProbability(const MatrixDouble& b)
{
	stateGivenSymbolProbability = b.CloneAs<MatrixDouble>();
	nbStates = b.GetRows();
	nbSymbols = b.GetCols();
}

/*! 
 * Set the first state probability matrix. A copy of the matrix is made.
 *
 * \param[in]	p the new first state probability matrix
 */
void DiscreteHMM::SetFirstStateProbability(const MatrixDouble& p)
{
	firstStateProbability = p.CloneAs<MatrixDouble>();
	nbStates = p.GetRows();
}

/*!
* Check if model is valid
*
* \return true if the model is valid, false else
*/
bool DiscreteHMM::IsValid() const
{
	bool status = false;
	
	if (stateTransitionProbability && stateGivenSymbolProbability && firstStateProbability)
	{
		if ((stateTransitionProbability->GetRows() == nbStates) && 
				(stateTransitionProbability->GetCols() == nbStates) && 
				(stateGivenSymbolProbability->GetRows() == nbStates) && 
				(stateGivenSymbolProbability->GetCols() == nbSymbols) && 
				(firstStateProbability->GetRows() == nbSymbols) && 
				(firstStateProbability->GetCols() == 1))
		{
			status = true;
		}
	}
	
	return status;
}

/*!
 * Check if two models are equal
 *
 * \param[in]	obj	the other HMM
 *
 * \return	true if models are equal, false else
 */
bool DiscreteHMM::equals(const Object &obj) const
{
	const DiscreteHMM& h = (const DiscreteHMM&) obj;
	
	return ((h.GetStateTransitionProbability())->Equals(*stateTransitionProbability) && 
	(h.GetStateGivenSymbolProbability())->Equals(*stateGivenSymbolProbability) && 
	(h.GetFirstStateProbability())->Equals(*firstStateProbability));
}

/*!
 * Forward function in Viterbi approximation
 *
 * \param[in]	observed	the observed sequence
 *
 * \return	the forward probability matrix
 */
UMatrixDouble DiscreteHMM::alpha(const MatrixInt& observed) const
{
	size_t i, j, t;
	size_t N = nbStates;
	size_t T = observed.GetCols();
			
	UMatrixDouble FWD = std::make_unique<MatrixDouble>(T, N, 0.0);
	// Just some convenient short variable names to fit literature and make it easier to handle ! =)
	SSquareMatrixDouble A = stateTransitionProbability;
	SMatrixDouble B = stateGivenSymbolProbability;
	SMatrixDouble P = firstStateProbability;

	// First alpha for each state.

	for (i = 0; i < N; i++)
	{
		FWD->At(0, i) = P->At(i, 0) * B->At(i, observed.At(0, 0));
	}

	// Iteration.

	for (t = 0; t < T - 1; t++)
	{
		for (j = 0; j < N; j++)
		{
			double Sum = 0.0;

			for (i = 0; i < N; i++)
			{
				Sum += FWD->At(t, i) * A->At(i, j);
			}

			FWD->At(t + 1, j) = Sum * B->At(j, observed.At(t + 1, 0));
		}
	}
        
	return std::forward<UMatrixDouble>(FWD);
}

/*!
* Backward function in Viterbi approximation
*
* \param[in]	observed	the observed sequence
*
* \return the backward probability matrix
*/
UMatrixDouble DiscreteHMM::beta(const MatrixInt& observed) const
{
	size_t i,j;
	size_t N = nbStates;
	size_t T = observed.GetCols();
        
	UMatrixDouble BKD = std::make_unique<MatrixDouble>(T, N, 0.0);
	SSquareMatrixDouble A = stateTransitionProbability;
	SMatrixDouble B = stateGivenSymbolProbability;

	// Last beta for each state.
			
	for (i = 0; i < N; i++)
	{
			BKD->At(T - 1, i) = 1.0;
	}

	// Iteration.
    
	for (int t = int(T) - 2; t >= 0; t--)
	{
		for (i = 0; i < N; i++)
		{
			double Sum = 0.0;

			for (j = 0; j < N; j++)
			{
				Sum += A->At(i, j) * 
				B->At(j, observed.At(t+1, 0)) * 
				BKD->At(t + 1, j);
			}
                    
			BKD->At(t, i) = Sum;
		}
	}
        
	return std::forward<UMatrixDouble>(BKD);
}

/*!
* Probability estimation for a given observation sequence
*
* \param[in]	observed	the observed sequence
*
* \return	the a priori probability of the observed sequence
*/
double DiscreteHMM::SequenceProbability(const MatrixInt& observed) const
{
	size_t k;
	size_t N = nbStates;
	size_t T = observed.GetCols();
	SMatrixDouble Alpha = alpha(observed);
	double P = 0.0;

	for (k = 0; k < N; k++)
	{
		P += Alpha->At(T - 1, k);
	}
        
	return P;
}

/*!
* Viterbi procedure to estimate the best state chain given an observation chain
*
* \param[in]	observed	the observed sequence
*
* \return the most likely state sequence corresponding to the observed sequence
*/
UMatrixInt DiscreteHMM::MakeViterbi(const MatrixInt& observed) const
{
	size_t i, j, k;
	size_t T = observed.GetRows();
	size_t N = nbStates;

	SSquareMatrixDouble A = stateTransitionProbability;
	SMatrixDouble B = stateGivenSymbolProbability;
	SMatrixDouble P = firstStateProbability;

	SMatrixDouble delta = std::make_shared<MatrixDouble>(T, N, 0.0);
	SMatrixInt phi = std::make_shared<MatrixInt>(T, N);

	// Setup.

	for (i = 0; i < N; i++)
	{
		delta->At(0, i) = P->At(i, 0) * B->At(i, observed.At(0, 0));
		phi->At(0, i) = 0;
	}

	// Recursion.

	for (size_t t = 1; t < T; t++)
	{
		// New delta values.

		double NewDelta = 0.0;
		double MaxDelta = 0.0;
		size_t Index = 0;

		for (j = 0; j < N; j++)
		{
			for (i = 0; i < N; i++)
			{
				NewDelta = delta->At(t - 1, i) * A->At(i, j);

			if (NewDelta > MaxDelta)
			{
				MaxDelta = NewDelta;
				Index = i;
			}
		}

		delta->At(t, j) = MaxDelta * B->At(j, observed.At(t, 0));
		phi->At(t, j) = int(Index);
		}
	}

	// Backtracking.

	UMatrixInt Path = std::make_unique<MatrixInt>(T, 1);
	size_t CurrentState;

	SMatrixDouble LastDelta = std::make_shared<MatrixDouble>(N, 1, 0.0);

	for (k = 0; k < N; k++)
	{
		LastDelta->At(k, 0) = delta->At(T - 1, k);
	}

	CurrentState = LastDelta->ArgmaxInColumn(0);

	Path->At(T - 1, 0) = int(CurrentState);

	for (int t = int(T) - 2; t >= 0; t--)
	{
		CurrentState = phi->At(t + 1, CurrentState);
		Path->At(t, 0) = int(CurrentState);
	}

	return std::forward<UMatrixInt>(Path);
}

/*!
 * HMM model training with Baum-Welch procedure. Training with one observation sequence.
 * 
 * \param[in]	observed	the observed sequence as a line matrix
 * \param[in]	maxIter	the maximum number of iterations
 */
void DiscreteHMM::BaumWelchSingle(const MatrixInt& observed, size_t maxIter)
{
	size_t Count = 1;
	bool Continue = true;

	while (Continue)
	{
		SDiscreteHMM H = CloneAs<DiscreteHMM>();

		size_t i, j, k, t;
		size_t N = nbStates;
		size_t K = nbSymbols;
		size_t T = observed.GetCols();

		SquareMatrixDouble A = *stateTransitionProbability->CloneAs<SquareMatrixDouble>();
		MatrixDouble B = *stateGivenSymbolProbability->CloneAs<MatrixDouble>();
		MatrixDouble P = *firstStateProbability->CloneAs<MatrixDouble>();

		UMatrixDouble Alpha(alpha(observed));
		UMatrixDouble Beta(beta(observed));
		SMatrixDouble Gamma = std::make_shared<MatrixDouble>(T, N, 0.0);

		std::vector<SSquareMatrixDouble> Xi(T);
		
		for (size_t date = 0; date < T; date++)
		{
			Xi[date] = std::make_shared<SquareMatrixDouble>(N, 0.0);
		}
		
		// Compute sequence probability via Alpha.

		double Prob = 0.0;

		for (k = 0; k < N; k++)
		{
			Prob += Alpha->At(T - 1, k);
		}

		///////////////////////////////////
		// Computation for Xi and Gamma. //
		///////////////////////////////////

		for (t = 0; t < T - 1; t++)
		{
			for (i = 0; i < N; i++)
			{ 
				for (j = 0; j < N; j++)
				{
					double z = (Alpha->At(t, i) * 
							A.At(i, j) * 
							B.At(j, observed.At(t + 1, 0)) * 
							Beta->At(t + 1, j)) / Prob;

					Xi[t]->At(i, j) = z;
					
					Gamma->IncreaseElement(t, i, z);
				}
			}                        
		}

		// Last gamma vector (cf. Rabiner & Juang).

		for (i = 0; i < N; i++)
		{
			double alpha = Alpha->At(T - 1, i);
			double beta = Beta->At(T - 1, i);
			double g = alpha * beta / Prob;

			Gamma->IncreaseElement(T - 1, i, g);
		}

		////////////////////
		// Re-estimation. //
		////////////////////

		// Re-estimation for P.

		P = Gamma->MakeRow(0);

		// Re-estimation for A.

		A.SetAll(0.0);

		SMatrixDouble InvScales = std::make_shared<MatrixDouble>(N, 1, 0.0);

		for (t = 0; t < T - 1; t++)
		{            	 
			for (i = 0; i < N; i++)
			{
				InvScales->IncreaseElement(i, 0, Gamma->At(t, i));

				for (j = 0; j < N; j++)
				{
					A.IncreaseElement(i, j, Xi[t]->At(i, j));
				}
			}
		}
		Xi.clear();

		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				double v = A.At(i, j);

				A.At(i, j) = v / InvScales->At(i, 0);
			}
		}

		// Re-estimation for B.

		for (j = 0; j < N; j++)
		{
			for (k = 0; k < K; k++)
			{
				double Numerator = 0.0;
				double Denominator = 0.0;

				for (t = 0; t < T; t++)
				{
					double g = Gamma->At(t, j);

					if (observed.At(t, 0) == int(k))
					{
						Numerator += g;
					}

					Denominator += g;
				}

				B.At(j, k) = Numerator / Denominator;
			}
		}

		firstStateProbability = P.CloneAs<MatrixDouble>();
		stateTransitionProbability = A.CloneAs<SquareMatrixDouble>();
		stateGivenSymbolProbability = B.CloneAs<MatrixDouble>();

		//////////////////////////////
		// End for Baum-Welch step. //
		//////////////////////////////

		Count++;
		Continue = !((this->Equals(*H)) || (Count > maxIter));
	}

	// XXX I forgot if this thing is still useful !!!
	//this.updateCumuls();

}

/*!
* HMM model training with Baum-Welch procedure. Training with many observation sequences.
* 
* \param[in]	observationSet	the observed sequences. Each sequence is a line of the matrix.
* \param[in]	maxIter maximal number of iterations
*/
void DiscreteHMM::BaumWelchMultiple(const MatrixInt& observationSet, size_t maxIter)
{
	size_t Count = 1;
	bool Continue=true;
	size_t i, j, k, m, n, t;
	size_t N = nbStates;
	size_t M = nbSymbols;
	size_t nbPatterns = observationSet.GetRows();

	while (Continue)
	{
		SDiscreteHMM H = CloneAs<DiscreteHMM>();

		SSquareMatrixDouble A = stateTransitionProbability;
		SMatrixDouble B = stateGivenSymbolProbability;
		SMatrixDouble P = firstStateProbability;

		SMatrixDouble CumulNumeratorA = std::make_shared<MatrixDouble>(N, N, 0.0);
		SMatrixDouble CumulNumeratorB = std::make_shared<MatrixDouble>(N, M, 0.0);
		SMatrixDouble CumulDenominatorA = std::make_shared<MatrixDouble>(N, N, 0.0);
		SMatrixDouble CumulDenominatorB = std::make_shared<MatrixDouble>(N, M, 0.0);
		SMatrixDouble CumulP = std::make_shared<MatrixDouble>(N, 1, 0.0);

		//////////////////////////
		// Loop over all sequences. //
		/////////////////////////

		for (k = 0; k < nbPatterns; k++)
		{
			MatrixInt Sample = observationSet.MakeRowAsColumn(k);
			size_t T = Sample.GetRows();

			UMatrixDouble Alpha(alpha(Sample));
			UMatrixDouble Beta(beta(Sample));

			double Prob = 0.0;

			for (i = 0; i < N; i++)
			{
				Prob += Alpha->At(T - 1, i);
			}

			std::vector<SSquareMatrixDouble> Xi;
			std::vector<SMatrixDouble> Gamma;

			
			/////////////////////////////////////
			// Computation for Xi and Gamma. //
			/////////////////////////////////////

			for (t = 0; t < T - 1; t++)
			{
				SSquareMatrixDouble xi = std::make_shared<SquareMatrixDouble>(N);
				SMatrixDouble gamma = std::make_shared<MatrixDouble>(N, 1, 0.0);

				for (i = 0; i < N; i++)
				{
					for (j = 0; j < N; j++)
					{
						double z = (Alpha->At(t, i)*
						A->At(i, j)*
						B->At(j, Sample.At(t + 1, 0))*
						Beta->At(t + 1, j)) / Prob;

						xi->At(i, j) = z;
						gamma->IncreaseElement(i, 0, z);
					}
				}
				
				Xi.push_back(xi);
				Gamma.push_back(gamma);
			}

			// Last gamma vector.

			{
				SMatrixDouble gamma = std::make_shared<MatrixDouble>(N, 1, 0.0);
				double Denominator = 0.0;

				for (i = 0; i < N; i++)
				{
					double alpha = Alpha->At(T - 1, i);
					double beta = Beta->At(T - 1, i);
					Denominator += alpha * beta;
				}

				for (i = 0; i < N; i++)
				{
					double alpha=Alpha->At(T - 1, i);
					double beta=Beta->At(T - 1, i);
					gamma->At(i, 0) = (alpha * beta) / Denominator;
				}

				Gamma.push_back(gamma);
			}

			/////////////////
			// Accumulation. //
			/////////////////

			// Accumulation for A.

			for (m = 0; m < N; m++)
			{
				for (n = 0; n < N; n++)
				{
					for (t = 0; t < T - 1; t++)
					{
						CumulNumeratorA->IncreaseElement(m, n, Xi[t]->At(m, n));
						CumulDenominatorA->IncreaseElement(m, n, Gamma[t]->At(m, 0));
					}
				}
			}

			// Accumulation for B;

			for (n = 0; n < N; n++)
			{
				for (m = 0; m < M; m++)
				{
					for (t = 0; t < T; t++)
					{
						double g = Gamma[t]->At(n, 0);

						if (Sample.At(t, 0) == int(m))
						{
							CumulNumeratorB->IncreaseElement(n, m, g);
						}

						CumulDenominatorB->IncreaseElement(n, m, g);
					}
				}
			}

			// Accumulation for P.

			for (n = 0; n < N; n++)
			{
				CumulP->IncreaseElement(n, 0, Gamma[0]->At(n, 0));
			}
		}

		///////////////////////////
		// Levinson's re-estimation. //
		//////////////////////////

		for (m = 0; m < N; m++)
		{
			for (n = 0; n < N; n++)
			{
				if (A->At(m, n) != 0.0)
				{
					A->At(m, n) = CumulNumeratorA->At(m, n) / CumulDenominatorA->At(m, n);
				}
			}
		}

		for (n = 0; n < N; n++)
		{
			for (m = 0; m < M; m++)
			{
				if (B->At(n,m) != 0.0)
				{
					B->At(n, m) = CumulNumeratorB->At(n, m) / CumulDenominatorB->At(n, m);
				}
			}
		}

		CumulP->Mult(1.0 / double(nbSymbols));

		for (n = 0; n < N; n++)
		{
			P->At(n, 0) = CumulP->At(n, 0);
		}

		////////////////////////
		// Maintain consistency. //
		////////////////////////

		//forceConsistency();

		//////////////////////////////
		// End for Baum-Welch step. //
		//////////////////////////////

		Count++;

		Continue = !((Equals(*H)) || (Count > maxIter));
	}

	// ???
	/*
	this.updateCumuls();
	this.firstStateProbabilityCumul.display();
	this.stateTransitionProbabilityCumul.display();
	this.stateGivenSymbolProbabilityCumul.display();
	*/
}

/*!
 * Sets probability matrix to a have a sum equals to one.
 */
void DiscreteHMM::forceConsistency()
{
	size_t N = nbStates;
	size_t K = nbSymbols;
	SSquareMatrixDouble A = stateTransitionProbability;
	SMatrixDouble B = stateGivenSymbolProbability;

	// Sum over each row in A should be 1.

	for (size_t l = 0; l < N; l++)
	{
		double Cumul=0.0;

		for (size_t c = 0; c < N; c++)
		{
			Cumul += A->At(l, c);
		}
		
		if (Cumul != 1.0)
		{
			for (size_t c = 0; c < N; c++)
			{
				double a = A->At(l, c)/Cumul;
				A->At(l, c) = a;
			}
		}
	}

	// Sum over row line in B should be 1.

	for (size_t l = 0; l < N; l++)
	{
		double Cumul = 0.0;

		for (size_t c = 0; c < K; c++)
		{
			Cumul += B->At(l, c);
		}
		
		if (Cumul != 1.0)
		{
			for (size_t c = 0; c < K; c++)
			{
				double b = B->At(l,c)/Cumul;
				B->At(l, c) = b;
			}
		}	
	}

}


/*!
* Dump a summary to a string
*
* \return a string containing a summary of the model
*/
String DiscreteHMM::ToString() const
{
	String s;
	s += U"Discrete HMM : ";
	s += String(nbStates);
	s += U" states ";
	s += String(nbSymbols);
	s += U"symbols\n\nState transion probability\n\n";
	s += stateTransitionProbability->ToString();
	s += U"\nState given symbol probability\n\n";
	s += stateGivenSymbolProbability->ToString();
	s += U"\nFirst state probability\n\n";
	s += firstStateProbability->ToString();
	return s;
}
