/* Copyright 2012-2016 CoReNum, INSA-Lyon, Université Paris Descartes, ENS-Lyon
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
 * file: CRNPathFinding.h
 * \author Yann LEYDIER
 */

#ifndef CRNPathFinding_HEADER
#define CRNPathFinding_HEADER

#include <CRNTypes.h>
#include <memory>
#include <vector>
#include <list>

namespace crn
{
	/*! \brief	A node in an A* graph */
	template<typename Node> struct AStarNode
	{
		/*! \brief	Constructor */
		AStarNode():cumulCost(0), distToEnd(0), totalCost(0) { }
		Node node; /*!< The content of the node */
		double cumulCost; /*!< The cumulated cost to the node */
		double distToEnd; /*!< The estimated distance to the end of the path */
		double totalCost; /*!< The cumulated cost + the cost of the estimated distance to the end */
		std::weak_ptr<AStarNode> parent; /*!< Back reference to the parent node */
	};

	/*! \brief A* path finding
	 *
	 * Searches the best path between two nodes with a heuristic.
	 *
	 * \param[in]	first	start node
	 * \param[in]	last	end node
	 * \param[in]	stepcost	computes the cost from a node to one of its neighbors: double stepcost(const Node&, const Node&)
	 * \param[in]	heuristic	estimates the cost from a node to another: double heuristic(const Node&, const Node&)
	 * \param[in]	get_neighbors	returns the list of neighbors to a node: std::vector<Node> get_neighbors(const Node&)
	 * \return	a vector of Nodes containing the shortest path or an empty vector if no path was found.
	 *
	 * \author Yann LEYDIER
	 * \date	June 2012
	 * \version 0.1
	 * \ingroup ai
	 */
	template<
		typename Node, 
		typename StepCostFunc,
		typename DistanceEstimationFunc,
		typename NeighborFunc,
		typename std::enable_if<!traits::HasLT<Node>::value, int>::type = 0
		>
	std::vector<Node> AStar(const Node &first, const Node &last, const StepCostFunc &stepcost, const DistanceEstimationFunc &heuristic, const NeighborFunc &get_neighbors)
	{
		using NodePtr = std::shared_ptr<AStarNode<Node>>;
		std::vector<NodePtr> openset;
		std::vector<NodePtr> closeset;
		// fist node
		NodePtr n(std::make_shared<AStarNode<Node>>());
		n->node = first;
		n->distToEnd = heuristic(n->node, last);
		n->totalCost = n->distToEnd;
		openset.push_back(std::move(n));
		while (!openset.empty())
		{
			// pick up the node with lowest cost
			auto minit = std::min_element(openset.begin(), openset.end(), [](const NodePtr &n1, const NodePtr &n2){ return n1->cumulCost < n2->cumulCost; });
			NodePtr currentnode = *minit;
			//openset.erase(minit);
			std::swap(*minit, openset.back()); openset.pop_back();
			closeset.push_back(currentnode);
			if (currentnode->node == last)
			{ // destination reached!
				std::vector<Node> path;
				// fill the path
				while (currentnode)
				{
					path.push_back(currentnode->node);
					currentnode = currentnode->parent.lock();
				}
				return std::vector<Node>(path.rbegin(), path.rend());
			}
			else
			{
				// compute neighbors
				const std::vector<Node> neighbors(get_neighbors(currentnode->node));
				for (const Node &neigh : neighbors)
				{ // for each neighbor
					NodePtr nn(std::make_shared<AStarNode<Node>>());
					nn->node = neigh;
					const double newcost = currentnode->cumulCost + stepcost(currentnode->node, neigh);
					// check if present in open set
					auto openit = std::find_if(openset.begin(), openset.end(), [&neigh](const NodePtr &n){ return n->node == neigh; });
					if (openit != openset.end())
					{ // found in open set
						if ((*openit)->cumulCost <= newcost)
							continue; // previous occurrence was better
						else
						{ // update
							nn = *openit;
							//openset.erase(openit);
							std::swap(*openit, openset.back()); openset.pop_back();
						}
					}
					else
					{
						// check if present in close set
						auto closeit = std::find_if(closeset.begin(), closeset.end(), [&neigh](const NodePtr &n){ return n->node == neigh; });
						if (closeit != closeset.end())
						{ // found in close set
							if ((*closeit)->cumulCost <= newcost)
								continue; // previous pass was better
							else
							{ // need to pass on it again
								nn = *closeit;
								closeset.erase(closeit);
							}
						}
					}
					// set the new candidate node
					nn->cumulCost = newcost;
					nn->distToEnd = heuristic(nn->node, last);
					nn->totalCost = newcost + nn->distToEnd;
					nn->parent = currentnode;
					openset.push_back(std::move(nn));
				} // for each neighbor
			} // not end node
		} // while there are candidate nodes left
		throw ExceptionNotFound{"AStar(): No path found."};
	}

	/*! \brief	A node in an A* graph */
	template<typename Node> struct AStarNodeC
	{
		/*! \brief	Constructor */
		AStarNodeC():cumulCost(0), distToEnd(0), totalCost(0) { }
		Node node; /*!< The content of the node */
		double cumulCost; /*!< The cumulated cost to the node */
		double distToEnd; /*!< The estimated distance to the end of the path */
		double totalCost; /*!< The cumulated cost + the cost of the estimated distance to the end */
		std::weak_ptr<AStarNodeC<Node>> parent; /*!< Back reference to the parent node */
		struct CMP
		{
			bool operator()(const std::shared_ptr<AStarNodeC> &n1, const std::shared_ptr<AStarNodeC> &n2) const { return n1->node < n2->node; }
		};
	};

	/*! \brief A* path finding for comparable data
	 *
	 * Searches the best path between two comparable nodes with a heuristic.
	 *
	 * \param[in]	first	start node
	 * \param[in]	last	end node
	 * \param[in]	stepcost	computes the cost from a node to one of its neighbors: double stepcost(const Node&, const Node&)
	 * \param[in]	heuristic	estimates the cost from a node to another: double heuristic(const Node&, const Node&)
	 * \param[in]	get_neighbors	returns the list of neighbors to a node: std::vector<Node> get_neighbors(const Node&)
	 * \return	a vector of Nodes containing the shortest path or an empty vector if no path was found.
	 *
	 * \author Yann LEYDIER
	 * \date	June 2012
	 * \version 0.1
	 * \ingroup ai
	 */
	template<
		typename Node,
		typename StepCostFunc,
		typename DistanceEstimationFunc,
		typename NeighborFunc,
		typename std::enable_if<traits::HasLT<Node>::value, int>::type = 0
		>
	std::vector<Node> AStar(const Node &first, const Node &last, const StepCostFunc &stepcost, const DistanceEstimationFunc &heuristic, const NeighborFunc &get_neighbors)
	{
		using NodePtr = std::shared_ptr<AStarNodeC<Node>>;
		const auto comparer = typename AStarNodeC<Node>::CMP{};
		auto openset = std::set<NodePtr, typename AStarNodeC<Node>::CMP>(comparer);
		auto closeset = std::set<NodePtr, typename AStarNodeC<Node>::CMP>(comparer);
		// fist node
		auto n = std::make_shared<AStarNodeC<Node>>();
		n->node = first;
		n->distToEnd = heuristic(n->node, last);
		n->totalCost = n->distToEnd;
		openset.insert(std::move(n));
		while (!openset.empty())
		{
			// pick up the node with lowest cost
			auto minit = std::min_element(openset.begin(), openset.end(), [](const NodePtr &n1, const NodePtr &n2){ return n1->cumulCost < n2->cumulCost; });
			auto currentnode = *minit;
			openset.erase(minit);
			closeset.insert(currentnode);
			if (currentnode->node == last)
			{ // destination reached!
				auto path = std::vector<Node>{};
				// fill the path
				while (currentnode)
				{
					path.push_back(currentnode->node);
					currentnode = currentnode->parent.lock();
				}
				return std::vector<Node>(path.rbegin(), path.rend());
			}
			else
			{
				// compute neighbors
				auto neighbors = get_neighbors(currentnode->node);
				for (auto &&neigh : neighbors)
				{ // for each neighbor
					auto nn = std::make_shared<AStarNodeC<Node>>();
					nn->node = std::move(neigh);
					const double newcost = currentnode->cumulCost + stepcost(currentnode->node, nn->node);
					// check if present in open set
					auto openit = openset.find(nn);
					if (openit != openset.end())
					{ // found in open set
						if ((*openit)->cumulCost <= newcost)
							continue; // previous occurrence was better
						else
						{ // update
							nn = *openit;
							openset.erase(openit);
						}
					}
					else
					{
						// check if present in close set
						auto closeit = closeset.find(nn);
						if (closeit != closeset.end())
						{ // found in close set
							if ((*closeit)->cumulCost <= newcost)
								continue; // previous pass was better
							else
							{ // need to pass on it again
								nn = *closeit;
								closeset.erase(closeit);
							}
						}
					}
					// set the new candidate node
					nn->cumulCost = newcost;
					nn->distToEnd = heuristic(nn->node, last);
					nn->totalCost = newcost + nn->distToEnd;
					nn->parent = currentnode;
					openset.insert(std::move(nn));
				} // for each neighbor
			} // not end node
		} // while there are candidate nodes left
		throw ExceptionNotFound{"AStar(): No path found."};
	}
	
}

#endif


