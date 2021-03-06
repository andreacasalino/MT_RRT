/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
*
* report any bug to andrecasa91@gmail.com.
 **/
 
#pragma once
#ifndef __MT_RRT_SIMPLIFIER_H__
#define __MT_RRT_SIMPLIFIER_H__

#include "Tree.h"
#include <vector>

namespace MT_RTT
{

    /** \brief A simplifier object implementing step F of the pipeline presented in Section 1.3 of the documentation.
    */
    class I_Simplifier{
    public:
        virtual ~I_Simplifier(){};

        virtual void operator()(std::list<Array>* wayps, Node::I_Node_factory* problem) const = 0;
    protected:
        I_Simplifier(){};
    };


    class Neutral_Simplifier : public I_Simplifier {
    public:
        virtual void operator()(std::list<Array>* wayps, Node::I_Node_factory* problem) const {};
    };


    class Brute_force_Simplifier : public I_Simplifier {
    public:
        virtual void operator()(std::list<Array>* wayps, Node::I_Node_factory* problem) const;
    };


    class Greedy_Simplifier : public I_Simplifier {
    public:
        virtual void operator()(std::list<Array>* wayps, Node::I_Node_factory* problem) const;
    };

}

#endif