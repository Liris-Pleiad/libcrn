/*! Copyright 2015-2016 Université Paris Descartes, CoReNum
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
 * \file	CRNGenetic.hpp
 * \author	Yann LEYDIER
 */

#ifndef CRN_GENETIC
#define CRN_GENETIC

#include <iterator>
#include <map>
#include <CRNException.h>
#include <random>
#include <algorithm>

namespace crn
{
	enum class GenerationStrategy { KEEP_BEST_PARENT, KEEP_BEST_PARENTS_AND_CHILDREN };
	
	/*! Genetic algorithm
	 * \throws	ExceptionLogic	there is less than 2 individuals
	 * \param[in]	b	iterator on the first individual
	 * \param[in]	e	iterator after the last individual
	 * \param[in]	breed	a function to create two individuals from two parents: std::pair<GENOTYPE, GENOTYPE> breed(const GENOTYPE &, const GENOTYPE &, URNG &rng)
	 * \param[in]	evaluate	a function to evaluate the fitness of an individual: double evaluate(const GENOTYPE &)
	 * \param[in]	stop	a function to verify if the algorithm has found a satisfying solution: bool stop(const std::multimap<double, GENOTYPE> &population)
	 * \param[in]	rng	a random number generator (default value is the default random generator initialized with current time)
	 * \return	the population, sorted by fitness
	 */
	template <typename ITER,
					 typename BREEDING_FUNC,
					 typename EVAL_FUNC,
					 typename STOP_FUNC,
					 typename URNG = std::default_random_engine>
	 std::multimap<double, typename std::iterator_traits<ITER>::value_type> Genetic(ITER b, ITER e,
								 BREEDING_FUNC breed,
								 EVAL_FUNC evaluate,
								 STOP_FUNC stop,
								 GenerationStrategy keep_parents = GenerationStrategy::KEEP_BEST_PARENT,
								 URNG &&rng = std::default_random_engine{size_t(std::chrono::system_clock::now().time_since_epoch().count())})
	 {
		 using GENOTYPE = typename std::iterator_traits<ITER>::value_type;

		 // evaluate the initial population
		 auto population = std::multimap<double, GENOTYPE>{};
		 for (; b < e; ++b)
			 population.emplace(evaluate(*b), *b);
		 if (population.size() < 2)
			 throw ExceptionLogic("Parthenogenesis is not allowed, at least two individuals are needed.");

		 // iterate generations
		 while (!stop(population))
		 {
			 auto newpopulation = std::multimap<double, GENOTYPE>{};
			 //auto it = population.cbegin();
			 // create a randomized order of the population
			 auto ranpos = std::vector<size_t>(population.size());
			 std::iota(ranpos.begin(), ranpos.end(), 0);
			 std::shuffle(ranpos.begin(), ranpos.end(), rng);
			 // breeding loop
			 for (size_t tmp = 0; tmp < population.size(); )
			 {
				 if (tmp + 1 >= ranpos.size()) // if the number of individuals is odd, the least fitting individual is not bred :o(
					 break;
				 // pick two random individuals
				 auto it1 = std::next(population.begin(), ranpos[tmp++]);
				 auto it2 = std::next(population.begin(), ranpos[tmp++]);
				 auto children = std::pair<GENOTYPE, GENOTYPE>{};
				 if (tmp >= ranpos.size())
				 { // if cannot pick a third, just breed what we have since they are the last remaining individuals.
					 children = breed(it1->second, it2->second, rng);
				 }
				 else
				 { // pick a third random individual
					 auto it3 = std::next(population.begin(), ranpos[tmp]);
					 // breed the best two, the least fitting one will be part of next love triangle
					 if (it1->first < it2->first)
					 {
						 if (it2->first < it3->first)
						 {
							 children = breed(it1->second, it2->second, rng);
							 // third individual's index stays in place in the list to be reused in next loop
						 }
						 else
						 {
							 children = breed(it1->second, it3->second, rng);
							 ranpos[tmp] = ranpos[tmp - 1]; // second individual's index stored for next loop
						 }
					 }
					 else
					 {
						 if (it1->first < it3->first)
						 {
							 children = breed(it1->second, it2->second, rng);
							 // third individual's index stays in place in the list to be reused in next loop
						 }
						 else
						 {
							 children = breed(it2->second, it3->second, rng);
							 ranpos[tmp] = ranpos[tmp - 2]; // first individual's index stored for next loop
						 }
					 }
				 }

				 // evaluate children
				 auto fitness = evaluate(children.first);
				 newpopulation.emplace(fitness, std::move(children.first));
				 fitness = evaluate(children.second);
				 newpopulation.emplace(fitness, std::move(children.second));
			 } // breeding loop

			 if (keep_parents == GenerationStrategy::KEEP_BEST_PARENT)
			 { // keep only the best parent
				 newpopulation.insert(*population.begin());
				 newpopulation.erase(--newpopulation.end());
				 population.swap(newpopulation);
			 }
			 else
			 { // keep the best individuals, parents and children included
				 const auto s = population.size();
				 std::move(newpopulation.begin(), newpopulation.end(), std::inserter(population, population.end()));
				 population.erase(std::next(population.begin(), s), population.end());
			 }

		 }
		 return population;
	 }

	/////////////////////////////////////////////////////////////////////
	// Breeding functors
	/////////////////////////////////////////////////////////////////////

	/*! \brief Crossover functor
	 *
	 * Creates two vectors from two parent vectors using complementary random strings from each one.
	 */
	struct CrossOver
	{
		/*!
		 * \param[in]	idv1	first parent
		 * \param[in]	idv2	second parent
		 * \throws	ExceptionDimension	parents do not have the same size
		 * \throws	ExceptionInvalidArgument	parents are empty
		 * \return	a pair of children
		 */
		template<typename T, typename URNG> inline std::pair<std::vector<T>, std::vector<T>> operator()(const std::vector<T> &idv1, const std::vector<T> &idv2, URNG &rng) const
		{
			if (idv1.size() != idv2.size())
				throw ExceptionDimension("The individuals must have the same size.");
			const auto s = idv1.size();
			if (!s)
				throw ExceptionInvalidArgument("The individuals must not be empty.");

			auto ran = std::uniform_int_distribution<size_t>(0, s - 1);
			const auto cut = ran(rng); // position of the cut
			auto child1 = std::vector<T>(s);
			auto child2 = std::vector<T>(s);
			for (auto tmp = size_t(0); tmp < cut; ++tmp)
			{ // copy first string
				child1[tmp] = idv1[tmp];
				child2[tmp] = idv2[tmp];
			}
			for (auto tmp = cut; tmp < s; ++tmp)
			{ // copy second string from the other parent
				child1[tmp] = idv2[tmp];
				child2[tmp] = idv1[tmp];
			}
			return std::make_pair(std::move(child1), std::move(child2));
		}
	};

	/////////////////////////////////////////////////////////////////////
	// Stop functors
	/////////////////////////////////////////////////////////////////////

	/*! \brief Simple counter to stop an genetic algorithm
	*/
	struct GenerationCounter
	{
		constexpr GenerationCounter(size_t cnt):generation(cnt) {}
		template<typename T> inline bool operator()(const T &)
		{ return --generation == 0; }
		private:
		size_t generation;
	};

	/*! \brief Stops when the best individual has a fitness lower than a threshold
	*/
	struct FitnessThreshold
	{
		constexpr FitnessThreshold(double thresh):threshold(thresh) {}
		template<typename T> inline bool operator()(const std::multimap<double, T> &population) const
		{ return population.begin()->first < threshold; }
		private:
		double threshold;
	};
}
#endif

