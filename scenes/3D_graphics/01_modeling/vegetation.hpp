#pragma once
#include "main/scene_base/base.hpp"
#include "Environment.hpp"

struct vegetation_model
{

	void vegetation_model::init_forest(int N, std::vector<vcl::vec3>& list_position, float min_dist, float z_offset_down, std::string type);
	void vegetation_model::update_random_position(int N, std::vector<vcl::vec3>& list_position, float min_dist, float z_offset_down, std::string type);
	bool vegetation_model::acceptablePosition(vcl::vec3 pos, std::vector<vcl::vec3> list_position, float min_dist);



	//Rock generation
	std::vector<vcl::vec3> tree_position;
	terrain_model env;

	vcl::mesh_drawable palm_tree;
	vcl::mesh create_palm_tree();
	void set_palm_tree();
	GLuint palm_tree_texture_id;





};