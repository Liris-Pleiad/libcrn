/* Copyright 2008-2015 INSA Lyon
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
 * file: CRNGaussianSCHMM.cpp
 * \author Jean DUONG, Yann LEYDIER
 */

#include <CRNi18n.h>
#include <stdio.h>
#include <CRNException.h>
#include <CRNMath/CRNSquareMatrixDouble.h>
#include <CRNMath/CRNMultivariateGaussianMixture.h>
#include <CRNMath/CRNMultivariateGaussianPDF.h>
#include <CRNAI/CRNGaussianSCHMM.h>
#include <CRNStringUTF8.h>
#include <CRNProtocols.h>

using namespace crn;

//GaussianSCHMM::GaussianSCHMM(GaussianSCHMM&&) noexcept = default;
//GaussianSCHMM& GaussianSCHMM::operator=(GaussianSCHMM&&) noexcept = default;

/*!
 * Constructor
 *
 * \param[in] 	nstates	number of states
 * \param[in]	nsymbs	number of symbols
 */
GaussianSCHMM::GaussianSCHMM(size_t nstates, size_t nsymbs):
	nbStates(nstates),
	symbolDimension(nsymbs),
	stateTransitionProbability(nbStates, 1.0 / double(nbStates)),
	firstStateProbability (nbStates, 1, 1.0 / double(nbStates))
{}

/*! 
 * Return a state probability law
 *
 * \param[in]	k	the state
 *
 * \throws	ExceptionDomain	 index value out of range
 *
 * \return	the state probability law
 */
MultivariateGaussianMixture GaussianSCHMM::GetStateGivenSymbolProbability(size_t k) const
{
	if (IsValidIndex(k))
		return stateGivenSymbolProbability[k];
	else
		throw ExceptionDomain(StringUTF8("MultivariateGaussianMixture GaussianSCHMM::GetStateGivenSymbolProbability(size_t k): ") + 
				_("Index out of range"));
}

/*! 
 * Return a state probability for a symbol 
 *
 * \param[in]	k	the state
 * \param[in]	x	the symbol (a column vector)
 *
 * \return	the state probability
 */
double GaussianSCHMM::GetStateGivenSymbolProbability(size_t k, const MatrixDouble& x) const
{
	if (IsValidIndex(k) && (x.GetCols() == 1) && (x.GetRows() == symbolDimension))
		return stateGivenSymbolProbability[k].ValueAt(x);
	else
		return 0.0;
}

/*! 
 * Return all the state probabilities for a symbol
 *
 * \param[in]	x	the symbol (a column vector)
 *
 * \throws	ExceptionDimension	 the vector has bad dimensions
 *
 * \return	all the state probabilities (a column vector)
 */
MatrixDouble GaussianSCHMM::GetStateGivenSymbolProbability(const MatrixDouble& x) const
{
	if ((x.GetCols() == 1) && (x.GetRows() == symbolDimension))
	{
		MatrixDouble p = MatrixDouble(nbStates, 1, 0.0);
		
		for (size_t s = 0; s < nbStates; s++)
			p[s][0] = GetStateGivenSymbolProbability(s, x);
		
		return std::forward<MatrixDouble>(p);
	}
	else
		throw ExceptionDimension(StringUTF8("MatrixDouble GaussianSCHMM::GetStateGivenSymbolProbability(const MatrixDouble& x): ") + 
				_("Illegal input vector dimension"));
}

/*! 
 * Set the state transition probability matrix
 *
 * \param[in]	a	the new state transition probability matrix
 */
void GaussianSCHMM::SetStateTransitionProbability(const SquareMatrixDouble& a)
{
	stateTransitionProbability = a;
	nbStates = a.GetRows();
}

/*! 
 * Set the first state probability matrix
 *
 * \param[in]	p	the new first state probability matrix
 */
void GaussianSCHMM::SetFirstStateProbability(const MatrixDouble& p)
{
	firstStateProbability = p;
	nbStates = p.GetRows();
}

/*!
* Forward function in Viterbi approximation
*
* \param[in]	Observed	the observed sequence
*
* \return the forward probability matrix
*/
MatrixDouble GaussianSCHMM::alpha(const MatrixDouble& Observed) const
{
	size_t i, j, t;
	size_t N = nbStates;
	size_t T = Observed.GetRows();

	MatrixDouble FWD(T, N, 0.0);
	MatrixDouble A = stateTransitionProbability;
	const std::vector<MultivariateGaussianMixture> &B = stateGivenSymbolProbability;
	MatrixDouble P = firstStateProbability;

	// First alpha for each state.

	for (i = 0; i < N; i++)
	{		
		FWD[0][i] = P.At(i, 0) * B[i].ValueAt(Observed.MakeRowAsColumn(0));
	}

	// Iteration.

	for (t = 0; t < T - 1; t++)
	{
		for (j = 0; j < N; j++)
		{
			double Sum = 0.0;

			for (i = 0; i < N; i++)
				Sum += FWD[t][i] * A[i][j];

			FWD[t + 1][j] = Sum * B[j].ValueAt(Observed.MakeRowAsColumn(t + 1));
		}
	}

	return std::forward<MatrixDouble>(FWD);
}

/*!
* Backward function in Viterbi approximation
*
* \param[in]	Observed	the observed sequence
*
* \return  the backward probability matrix
*/
MatrixDouble GaussianSCHMM::beta(const MatrixDouble& Observed) const
{
	size_t i,j;
	size_t N = nbStates;
	size_t T = Observed.GetRows();

	MatrixDouble BKD(T, N, 0.0);
	MatrixDouble A = stateTransitionProbability;
	const std::vector<MultivariateGaussianMixture> &B = stateGivenSymbolProbability;

	// Last beta for each state.

	for (i = 0; i < N; i++)
		BKD[T - 1][i] = 1.0;

	// Iteration.

	for (int t = int(T) - 2; t >= 0; t--)
	{
		for (i = 0; i < N; i++)
		{
			double Sum = 0.0;

			for (j = 0; j < N; j++)
				Sum += A[i][j] * B[j].ValueAt(Observed.MakeRowAsColumn(t + 1)) *  BKD[t + 1][j];

			BKD[t][i] = Sum;
		}
	}

	return std::forward<MatrixDouble>(BKD);
}

/*!
 * HMM model training with Baum-Welch procedure. Training with one observation sequence.
 * 
 * \param[in]	Observed	the observed sequence
 * \param[in]	MaxIter	the maximum number of iterations
 */
void GaussianSCHMM::BaumWelchSingle(const MatrixDouble& Observed, unsigned int MaxIter)
{
	unsigned int Count = 1;
	bool Continue = true;

	while (Continue)
	{
		// One step of Baum-Welch algorithm

		size_t i, j, k, t;
		size_t N = nbStates;
		size_t T = Observed.GetRows();

		// Some variables to make code clearer.

		SquareMatrixDouble A = stateTransitionProbability;
		std::vector<MultivariateGaussianMixture> &B = stateGivenSymbolProbability;
		MatrixDouble P = firstStateProbability;

		// Useful numerical table
		// - forward function
		// - backward function
		// - Xi (needed for transition reestimation)
		// - Gamma (needed for transition reestimation)

		MatrixDouble Alpha(alpha(Observed));
		MatrixDouble Beta(beta(Observed));
		std::vector<SquareMatrixDouble> Xi;
		MatrixDouble Gamma(T, N, 0.0);

		for (k = 0; k < T; k++)
			Xi.push_back(SquareMatrixDouble(N));
		
		// Compute sequence probability via Alpha
		// (needed for transition reestimation)

		double Prob = 0.0;

		for (k = 0; k < N; k++)
			Prob += Alpha[T - 1][k];

		// Computation for Xi and Gamma

		for (t = 0; t < T - 1; t++)
		{
			for (i = 0; i < N; i++)
			{ 
				for (j = 0; j < N; j++)
				{
					double z = (Alpha[t][i] * A[i][j] * B[j].ValueAt(Observed.MakeRowAsColumn(t + 1)) *  Beta[t + 1][j]) / Prob;
					Xi[t][i][j] = z;
					Gamma[t][i] += z;
				}
			}                        
		}
		
		// Last gamma vector (cf. Rabiner & Juang).

		for (i = 0; i < N; i++)
		{
			double alpha = Alpha[T - 1][i];
			double beta = Beta[T - 1][i];
			double g = alpha * beta / Prob;

			Gamma[T - 1][i] += g;
		}

		// Re-estimation

		// Re-estimation for P.

		P = MatrixDouble(Gamma.MakeRowAsColumn(0));

		firstStateProbability = P;

		// Re-estimation for A.

		A.SetAll(0.0);

		MatrixDouble InvScales(N, 1, 0.0);

		for (t = 0; t < T - 1; t++)
		{            	 
			for (i = 0; i < N; i++)
			{
				InvScales[i][0] += Gamma[t][i];

				for (j = 0; j < N; j++)
					A[i][j] += Xi[t].At(i, j);
			}
		}

		for (i = 0; i < N; i++)
		{
			for (j = 0; j < N; j++)
			{
				double v = A.At(i, j);

				A[i][j] = v / InvScales[i][0];
			}
		}

		stateTransitionProbability = A;

		// Re-estimation for B : 
		//
		// - reestimation for mixture weights
		// - reestimation for means
		// - reestimation for variances

		// The "new" gamma in Rabiner's
		
		//std::vector<std::vector<MatrixDouble>> ExtendedGamma(T, std::vector<MatrixDouble>(N));
		std::vector< std::vector< std::vector<double> > > ExtendedGamma(T, std::vector< std::vector<double> >(N));
		
		for (t = 0; t < T; t++)
		{
			MatrixDouble Ot = Observed.MakeRowAsColumn(t);

			for (j = 0; j < N; j++)
			{
				size_t Mj = B[j].GetNbMembers(); // Number of members in j-th mixture
				
				//MatrixDouble V(Mj, 1, 0.0);
				std::vector<double> V(Mj);
				
				double Denominator = B[j].ValueAt(Ot);

				for (k = 0; k < Mj; k++)
				{
					double Numerator = B[j].ValueAt(Ot, k);

					V[k] = Gamma[t][j] * Numerator / Denominator;
				}

				ExtendedGamma[t][j] = V;
			}
		}

		for (j = 0; j < N; j++)
		{
			size_t Mj = B[j].GetNbMembers();

			for (k = 0; k < Mj; k++)
			{
				double c_jk = 0.0;
				MatrixDouble mu_jk(symbolDimension, 1, 0.0);
				SquareMatrixDouble U_jk(symbolDimension, 0.0);

				// k-th distribution in j-th mixture
				MultivariateGaussianPDF Gjk = B[j].GetMember(k);
				MatrixDouble mu_jk_old = Gjk.GetMean();

				// Scaling factors
				double c_jk_scale = 0.0;
				double mu_jk_scale = 0.0;
				double U_jk_scale = 0.0;

				for (t = 0; t < T; t++)
				{
					MatrixDouble Ot = Observed.MakeRowAsColumn(t);
					//double gamma_tjk = ExtendedGamma[t][j].At(k, 0);
					double gamma_tjk = ExtendedGamma[t][j][k];

					// increase c_jk
					c_jk += gamma_tjk;

					// increase mu_jk
					//mu_jk.add(Ot.getScaled(gamma_tjk));
					MatrixDouble tm = Ot;
					tm *= gamma_tjk;
					mu_jk += tm;

					// increase U_jk
					//SMatrixDouble* TranslatedPattern = Ot.getSubstracted(mu_jk_old);
					MatrixDouble TranslatedPattern = Ot;
					TranslatedPattern -= mu_jk_old;
					//U_jk.add(TranslatedPattern.getRightAutoProduct().getScaled(gamma_tjk));
					SquareMatrixDouble tsm = TranslatedPattern.MakeVectorRightAutoProduct();
					tsm *= gamma_tjk;
					U_jk += tsm;
					
					// Increase scale factors

					for (size_t m = 0; m < Mj; m++)
						//c_jk_scale += ExtendedGamma[t][j].At(m, 0);
						c_jk_scale += ExtendedGamma[t][j][m];

					mu_jk_scale += gamma_tjk;
					U_jk_scale += gamma_tjk;
				}

				c_jk /= c_jk_scale;
				mu_jk *= 1.0 / mu_jk_scale;
				U_jk *= 1.0 / U_jk_scale;

				stateGivenSymbolProbability[j].SetMember(MultivariateGaussianPDF(mu_jk, U_jk), c_jk, k);
			}
		}

		ExtendedGamma.clear();

		Count++;
		//Continue = !((this.equivalent(H)) || (Count > MaxIter));
		Continue = !(Count > MaxIter);
	}
}

CRN_BEGIN_CLASS_CONSTRUCTOR(GaussianSCHMM)
	Cloner::Register<GaussianSCHMM>();
CRN_END_CLASS_CONSTRUCTOR(GaussianSCHMM)
