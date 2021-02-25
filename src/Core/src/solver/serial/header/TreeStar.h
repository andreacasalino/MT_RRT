/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef MT_RRT_TREE_STAR_H
#define MT_RRT_TREE_STAR_H

#include <TreeConcrete.h>

namespace mt::solver::tree {
	class TreeStar : public TreeConcrete {
	public:
		TreeStar(problem::Problem& problem, NodePtr root);

		const Node* extendRandom() override;

		std::pair<const Node*, bool> extendDeterministic(const NodeState& target) override;
	};
}

#endif