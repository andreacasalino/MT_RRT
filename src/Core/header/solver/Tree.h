/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef MT_RRT_TREE_H
#define MT_RRT_TREE_H

#include <problem/Problem.h>
#include <list>
#include <utility>

namespace mt::solver::tree {
	typedef std::list<NodePtr> Nodes;

    class Tree {
    public:
        virtual	~Tree() = default;

		Tree(const Tree&) = delete;
		Tree& operator=(const Tree&) = delete;

		/** \brief Tries to extend the tree toward a random configuration that is internally sampled.
		\detials In case an extensio was possible, the node added to the tree is returned. Otherwise, NULL is returned
		* @param[out] return the node added to the tree as a consequence of the extension (NULL is returned in case the extension was not possible).
		*/
		virtual const Node* extendRandom() = 0;

		/** \brief An extension toward the passed target node is tried.
		\details In case the extension succeeds, a new node with the steered configuration, Section 1.2 of the documentation,  is
		automatically inserted to the list of nodes contained in this tree and returned. On the opposite,
		when the extension was not possible a NULL value is returned.
		* @param[in] target the target node toward which the extension must be tried
		* @param[out] return the node added to the tree as a consequence of the extension (NULL is returned in case the extension was not possible).
		*/
		// todo spiegare bene comportamento
		virtual std::pair<const Node*, bool> extendDeterministic(const NodeState& target) = 0;

		virtual const Nodes& getNodes() const = 0;

		virtual problem::Problem& getProblem() = 0;

	protected:
		Tree() = default;
    };

    typedef std::unique_ptr<Tree> TreePtr;
}

#endif