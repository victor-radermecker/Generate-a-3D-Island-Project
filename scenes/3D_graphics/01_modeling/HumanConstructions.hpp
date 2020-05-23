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

};