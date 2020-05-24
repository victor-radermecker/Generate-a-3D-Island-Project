#pragma once
#include "main/scene_base/base.hpp"


using namespace vcl;


struct treasure_model
{
	vcl::hierarchy_mesh_drawable hierarchy_treasure;

	//Functions
	void create_treasure_box();
	void open_chest();
	void draw_treasure(std::map<std::string, GLuint>& shaders, scene_structure& scene);

	//Gluint wooden texture
	GLuint wood_texture_id;


	//Bridge
	vcl::mesh_drawable bridge;
	GLuint bridge_texture_id;
	void set_bridge();
	mesh create_bridge();
	void draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene);




};

