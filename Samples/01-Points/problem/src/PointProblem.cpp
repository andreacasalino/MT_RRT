/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <PointProblem.h>
#include <sampler/Box.h>
#include "Line.h"

namespace mt::sample {
    float getSteerDegree(const sample::Obstacle& boundaries) {
        float temp1 = 0.05f * (boundaries.getXMax() - boundaries.getXMin());
        float temp2 = 0.05f * (boundaries.getYMax() - boundaries.getYMin());
        if (temp1 < temp2) return temp1;
        return temp2;
    }

    sampling::SamplerPtr getSampler(const sample::Obstacle& boundaries) {
        NodeState low = { boundaries.getXMin() , boundaries.getXMin() };
        NodeState upp = { boundaries.getXMax() , boundaries.getYMax() };
        return std::make_unique<sampling::Box>(low, upp);
    }

    PointProblem::PointProblem(const sample::Obstacle& boundaries, const std::vector<sample::Obstacle>& obstacles)
        : Problem(getSampler(boundaries), 
                  std::make_unique<traj::LineManager>(getSteerDegree(boundaries), obstacles), 
                  2, 10.f) {
    }

    const std::vector<sample::Obstacle>& PointProblem::getObstacles() const {
        return static_cast<traj::LineManager*>(this->trajManager.get())->getObstacles();
    }

    sample::Obstacle PointProblem::getBoundaries() const {
        const sampling::Box* ptSam = static_cast<const sampling::Box*>(this->sampler.get());
        geometry::Point A(ptSam->getLowerLimit().front(), ptSam->getLowerLimit().back());
        geometry::Point B(ptSam->getLowerLimit().front() + ptSam->getDeltaLimit().front(), ptSam->getLowerLimit().back() + ptSam->getDeltaLimit().back());
        return sample::Obstacle(A, B);
    }

    structJSON PointProblem::getJSON() const {
        structJSON result;

        arrayJSON limits;
        auto Lim = this->getBoundaries();
        limits.addElement(Number<float>(Lim.getXMin()));
        limits.addElement(Number<float>(Lim.getYMin()));
        limits.addElement(Number<float>(Lim.getXMax()));
        limits.addElement(Number<float>(Lim.getYMax()));
        result.addElement("limits", limits);

        arrayJSON obstacles;
        const std::vector<sample::Obstacle>& obst = this->getObstacles();
        for (auto it = obst.begin(); it != obst.end(); ++it) {
            arrayJSON obstacle;
            obstacle.addElement(Number<float>(it->getXMin()));
            obstacle.addElement(Number<float>(it->getYMin()));
            obstacle.addElement(Number<float>(it->getXMax()));
            obstacle.addElement(Number<float>(it->getYMax()));
            obstacles.addElement(obstacle);
        }
        result.addElement("obstacles", obstacles);

        return result;
    }
}