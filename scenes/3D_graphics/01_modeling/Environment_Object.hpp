#pragma once
#include "main/scene_base/base.hpp"

struct object_model
{

	//Rock generation
	vcl::mesh_drawable rock1;
	vcl::mesh create_rock1();
	void set_rock1();
	GLuint rock1_texture_id;

	vcl::mesh_drawable rock2;
	vcl::mesh create_rock2();
	void set_rock2();
	GLuint rock2_texture_id;




};