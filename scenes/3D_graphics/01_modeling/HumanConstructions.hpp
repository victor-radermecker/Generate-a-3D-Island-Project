#pragma once
#include "main/scene_base/base.hpp"


using namespace vcl;


struct treasure_model
{
	
	//init
	void init_all();
	void treasure_model::draw_all(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	
	vcl::hierarchy_mesh_drawable hierarchy_treasure;

	//Functions
	void create_treasure_box();
	void open_chest();
	void draw_treasure(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	mesh mesh_primitive_parallelepiped_chest(const vcl::vec3& p0, const vcl::vec3& u1, const vcl::vec3& u2, const vcl::vec3& u3, std::string part);
	void mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int);

	bool open_chest_bool = false;
	vec3 chest_position;

	//Gluint wooden texture
	GLuint wood_texture_id;
	GLuint treasure_texture_id;


	//Bridge
	vcl::mesh_drawable bridge;
	vcl::vec3 bridge_position;
	GLuint bridge_texture_id;
	void set_bridge();
	mesh create_bridge();
	void draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene);

	//Canoe
	vcl::mesh_drawable canoe;
	vcl::vec3 canoe_position;
	GLuint canoe_texture_id;
	void set_canoe();
	mesh create_canoe();
	void draw_canoe(std::map<std::string, GLuint>& shaders, scene_structure& scene);


};

