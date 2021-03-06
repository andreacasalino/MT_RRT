/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
*
* report any bug to andrecasa91@gmail.com.
 **/

#include "../Header/Problem_path_basic.h"
#include <random>
#include <float.h>
using namespace std;

#define MIN_S_ACCEPT_BUBBLE 0.01f


namespace MT_RTT
{

	Manipulator_path_handler::Manipulator_path_handler(const float& Gamma, const Array& Q_max, const Array& Q_min, const float& steer_degree) :
		Equispaced_Node_factory(Q_max.size(), Gamma, steer_degree, true), Max_Q_vals(Q_max), Min_Q_vals(Q_min), Delta_Q_vals(Q_max) {

		if(Q_max.size() != Q_min.size()) throw 0;

		for (size_t k = 0; k < Q_min.size(); ++k) this->Delta_Q_vals[k] -= this->Min_Q_vals[k];
		for (size_t k = 0; k < this->Get_State_size(); ++k) {
			if (this->Delta_Q_vals[k] <= 0.f) throw 1;
		}

	}
	
	void	Manipulator_path_handler::Random_node(float* random_state) {

		auto S = this->Get_State_size();
		for (size_t k = 0; k < S; ++k)
			random_state[k] = this->Min_Q_vals[k] + this->Delta_Q_vals[k] * (float)this->rand() / (float)RAND_MAX;

	}



	Tunneled_check_collision::Tunneled_check_collision(const float& Gamma, const float& steer_degree, const Array& Q_max, const Array& Q_min, unique_ptr<I_Collision_checker>& coll_checker):
		Manipulator_path_handler( Gamma, Q_max, Q_min, steer_degree), Collision_checker(move(coll_checker)) {}

	Tunneled_check_collision::Tunneled_check_collision(const float& Gamma, const float& steer_degree, const float& q_max, const float& q_min, const size_t& dof, std::unique_ptr<I_Collision_checker>& coll_checker) :
		Manipulator_path_handler(Gamma, Array(q_max, dof), Array(q_min, dof), steer_degree), Collision_checker(move(coll_checker)) {};

	Tunneled_check_collision::Tunneled_check_collision(Tunneled_check_collision& o) :
		Manipulator_path_handler(o.Get_Gamma(), o.Get_max(), o.Get_min(), o.Get_Steer_degree()), Collision_checker(move(o.Collision_checker->copy_checker())) {};

	bool Tunneled_check_collision::Check_reached_in_cache(){
		
		const float* reached = this->last_computed_traj->Get_state_current();
		return this->Collision_checker->Collision_present(reached);
		 
	}




	Bubbles_free_configuration::I_Proximity_calculator::I_Proximity_calculator(const std::vector<size_t>& Dof) : Robot_distance_pairs(nullptr) {

		if (Dof.empty()) throw 0;

		this->Robots_info.reserve(Dof.size());
		for (size_t k = 0; k < Dof.size(); ++k)  this->Robots_info.emplace_back(Dof[k]);
		if(Dof.size() > 1) this->Robot_distance_pairs = new Array(0.f , (Dof.size() * (Dof.size() - 1)) / 2);

	}

	Bubbles_free_configuration::Bubbles_free_configuration(const float& Gamma, const Array& Q_max, const Array& Q_min, unique_ptr<I_Proximity_calculator>& prox_calc) :
		Manipulator_path_handler(Gamma, Q_max, Q_min, 1.f), Proximity_calculator(move(prox_calc)) {};

	Bubbles_free_configuration::Bubbles_free_configuration(const float& Gamma, const float& q_max, const float& q_min, const size_t& dof, std::unique_ptr<I_Proximity_calculator>& prox_calc):
		Manipulator_path_handler(Gamma, Array(q_max, dof), Array(q_min, dof), 1.f), Proximity_calculator(move(prox_calc)) {};

	Bubbles_free_configuration::Bubbles_free_configuration(Bubbles_free_configuration& o):
		Manipulator_path_handler(o.Get_Gamma(), o.Get_max(), o.Get_min(), 1.f), Proximity_calculator(move(o.Proximity_calculator->copy_calculator())) {}

	bool    Bubbles_free_configuration::Check_reached_in_cache(){

		return (static_cast<bubble_trajectory*>(this->last_computed_traj)->Get_s_advance() < MIN_S_ACCEPT_BUBBLE);

	}	

	Bubbles_free_configuration::bubble_trajectory::bubble_trajectory(const float* start, const float* end, Bubbles_free_configuration* caller) : 
	I_trajectory(start, end, caller) { };

	bool Bubbles_free_configuration::bubble_trajectory::Advance(){

		size_t S = this->Caller->Get_State_size();
		Bubbles_free_configuration* proxier = static_cast<Bubbles_free_configuration*>(this->Caller);

		if(this->Cursor_along_traj == nullptr){
			this->Cursor_previous = new float[S];
			Array::Array_copy( this->Cursor_previous, this->Start, S);

			this->Cursor_along_traj = new float[S];
		}
		else {
			float* temp = this->Cursor_previous;
			this->Cursor_previous = this->Cursor_along_traj;
			this->Cursor_along_traj = temp;
		}
		this->s_advance = 1.f;
		float s_att;
		size_t p=0;

		proxier->Proximity_calculator->Recompute_Proximity_Info(this->Cursor_previous);

		const std::vector<I_Proximity_calculator::single_robot_prox>& single_info = proxier->Proximity_calculator->Get_single_info();

		size_t K, R = single_info.size(), r;
		Array dot_radii_deltaQ(0.f, R);
		for(r=0; r<R; ++r){
			K = single_info[r].Radii.size();
			for(size_t k=0; k<K; ++k){
			 	dot_radii_deltaQ[r] += abs(this->Cursor_previous[p] - this->End[p]) * single_info[r].Radii[k];
				++p;
			}
		}

		for(r=0; r<R; ++r){
			s_att = single_info[r].Distance_to_fixed_obstacles / dot_radii_deltaQ[r];
			if(s_att < this->s_advance) this->s_advance = s_att;
		}
		
		const Array*	 info_pairs = proxier->Get_proxier()->Get_distances_pairs();
		if(info_pairs != nullptr){
			size_t r2;
			p = 0;
			for(r=0; r<(R-1); ++r){
				for(r2 = r+1; r2 < R; ++r2){
					s_att = (*info_pairs)[p] / (dot_radii_deltaQ[r] + dot_radii_deltaQ[r2]);
					if(s_att < this->s_advance) this->s_advance = s_att;
					++p;
				}
			}
		}

		if(this->s_advance == 1.f){
			Array::Array_copy(this->Cursor_along_traj , this->End, S);
		}
		else{
			this->s_advance = abs(this->s_advance);
			for(p=0; p<S; ++p) this->Cursor_along_traj[p] = this->s_advance * (this->End[p]  - this->Cursor_previous[p]) + this->Cursor_previous[p];
		}
		this->Cumulated_cost = linear_trajectory::Euclidean_distance(this->Start , this->Cursor_along_traj, this->Caller->Get_State_size());

		return (this->s_advance < 1.f);

	}

	float Bubbles_free_configuration::bubble_trajectory::Cost_to_go(){

		return linear_trajectory::Euclidean_distance(this->Start , this->End, this->Caller->Get_State_size());

	}

}
