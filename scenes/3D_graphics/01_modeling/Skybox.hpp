#pragma once
#include "main/scene_base/base.hpp"



struct skybox_textures
{
	GLuint back;
	GLuint bottom;
	GLuint front;
	GLuint left;
	GLuint right;
	GLuint top;
};



struct skybox_model
{

	//	skybox generation
	vcl::mesh create_skybox();
	void set_skybox();

	// Mesh drawable object
	vcl::mesh_drawable skybox;

	// Textures
	skybox_textures skybox_ids;


};



