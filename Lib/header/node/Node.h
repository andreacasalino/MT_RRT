/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
 *
 * report any bug to andrecasa91@gmail.com.
 **/
 
#ifndef MT_RRT_NODE_H
#define MT_RRT_NODE_H

#include <vector>

namespace mt::node {
	/** \brief Used internally by a tree (see Tree.h) for representing a state  x \in \underline{\mathcal{X}}, Section 1.2.1 of the documentation.
	*/
	class Node {
	public:
		virtual ~Node() = default;

		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;
		Node(Node&&) = delete;
		Node& operator=(Node&&) = delete;

		/** \brief Computes the cost to go from the root to this node.
		* @param[out] result the cost to go to compute.
		*/
		float													cost2Root() const;
		/** \brief Similar to Node::Cost_to_root(float* result), but throwing an exception when the length of the path that must be followed to reach the root
		is higher than I_max.
		\details This method is called by Tree_star to detect the existance of loop in the tree. It is never used when _REW_DEBUG (check Tree.h) is not activated
		*/
		float													cost2Root(const size_t& I_max) const;

		/** \brief Computes the cost to go from the father of this node to this node.
		* @param[out] return the cost to go to return.
		*/
		inline const float&									    getCostFromFather() const { return this->costFromFather; };

		/** \brief Returns the state represented by this node.
		* @param[out] return the address of the first value of the array representing the state.
		*/
		inline const std::vector<float>&						getState() const { return this->state; };
		
		/** \brief Returns the father of this node.
		\details Each node has a single father and can be the father of many nodes. 
		* @param[out] return the father node of this node.
		*/
		inline Node*											getFather() const { return this->father; };

		/** \brief Connect this node to the new one passed as input.
		\details Each node has a single father and can be the father of many nodes. 
		* @param[in] new_father the node to assume as new father
		* @param[in] cost_from_father the cost to go from the new father
		*/
		void													setFather(Node* new_father, const float& cost_from_father);

	protected:
		Node(Node* father, const float& cost, const std::vector<float>& state);
		
		Node(const std::vector<float>& state) : Node(nullptr, 0.f, state) {};
		
	// data
		std::vector<float>				state;
		Node*							father;
		float							costFromFather;
	};
}

#endif