/**
 * Author:    Andrea Casalino
 * Created:   16.05.2019
*
* report any bug to andrecasa91@gmail.com.
 **/

#include "../src/Problem_planar_arms.h"
#include "../../src/Solve.h"
#include "../../src/Log.h"
using namespace std;
using namespace MT_RTT;

#define USE_BUBBLE
#define USE_MULTIPLE_STEER

//this kind of problem is described in Section 2.2 of the documentation
int main() {

	size_t Iterations = 5000;

	vector<float> Qo, Qf;
	unique_ptr<Node::I_Node_factory> Scene;
	float gamma = 2.f;

//read the problem from the textual file
	auto Scene_raw = get_content_of_file("problem.json");
	vector<json_parser::field> fields = json_parser::parse_JSON(Scene_raw);

#ifdef USE_BUBBLE

	auto Scene_description = new Scene_Proximity_calculator(fields);
	unique_ptr<Bubbles_free_configuration::I_Proximity_calculator> pt_temp(Scene_description);
	Scene = move(unique_ptr<Node::I_Node_factory>(new Bubbles_free_configuration( gamma, 4.712385f, -4.712385f, Scene_description->Get_Dof_tot() , pt_temp)));

#ifdef USE_MULTIPLE_STEER
	Scene->Set_Steer_iterations(3);
#endif
	Scene->Set_CostToGoConstraints_iterations(3);
#else

	auto Scene_description = new Scene_Collision_checker(fields);
	unique_ptr<Tunneled_check_collision::I_Collision_checker> pt_temp(Scene_description);
	size_t dof = Scene_description->Get_wrapped_prox()->Get_Dof_tot();
	float steer_degree = 5.f * 3.14159f / 180.f;
	Scene = move(unique_ptr<Node::I_Node_factory>(new Tunneled_check_collision(gamma, steer_degree, 4.712385f, -4.712385f, dof, pt_temp)));

#ifdef USE_MULTIPLE_STEER
	Scene->Set_Steer_iterations(6);
#endif
	Scene->Set_CostToGoConstraints_iterations(6);
#endif

	Qo = import_pose(*json_parser::get_field(fields, "Q_curr"));
	Qf = import_pose(*json_parser::get_field(fields, "Q_trgt"));

//check the behaviour of this function to understand how to use the planning algorithms
	auto Log_results = Solve_using_planners_and_strategies(Iterations, 0.1f, Scene.get(), Qo, Qf);

	Log_creator("../../src_JS/Result_template.html", "Results/Serial.html", Scene_raw, Log_results[0]);
	Log_creator("../../src_JS/Result_template.html", "Results/Parallel_query.html", Scene_raw, Log_results[1]);
	Log_creator("../../src_JS/Result_template.html", "Results/Parallel_shared.html", Scene_raw, Log_results[2]);
	Log_creator("../../src_JS/Result_template.html", "Results/Parallel_copied.html", Scene_raw, Log_results[3]);
	Log_creator("../../src_JS/Result_template.html", "Results/Parallel_multiag.html", Scene_raw, Log_results[4]);
// you can use your favorite browser to open the .html file created in the Results folder

	cout << "done\n";
	return 0;
}
