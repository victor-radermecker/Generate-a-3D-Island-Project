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

struct skybox_drawable
{
	vcl::mesh_drawable back;
	vcl::mesh_drawable bottom;
	vcl::mesh_drawable front;
	vcl::mesh_drawable left;
	vcl::mesh_drawable right;
	vcl::mesh_drawable top;
};


struct skybox_model
{

	//	skybox generation
	void set_skybox();

	// Mesh drawable object
	skybox_drawable skybox;

	// Textures
	skybox_textures skybox_ids;

	// Draw skybox
	void draw_skybox(std::map<std::string, GLuint>& shaders, scene_structure& scene);

};



