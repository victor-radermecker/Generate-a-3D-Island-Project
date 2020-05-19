#pragma once
#include "main/scene_base/base.hpp"
#include "Environment_Object.hpp"


struct particle_structure
{
	vcl::vec3 p; // Position
	vcl::vec3 v; // Speed
	float scale; // scale going from 0 (dissapear) to 1 (entire)
};




struct lava_model
{
	// Lava functions
	GLuint lava_texture_id;
	vcl::mesh create_lava();
	vcl::vec3 evaluate_lava(float u, float v);
	void update_lava(vcl::mesh_drawable& ocean, vcl::buffer<vcl::vec3>& current_position, vcl::buffer<vcl::vec3>& current_normals, vcl::buffer<vcl::uint3> connectivity, float t, float tmax, perlin_noise p);
	void lava_model::draw_lava(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void set_lava();

	// Lava features
	vcl::mesh lava_cpu;
	vcl::mesh_drawable lava;
	vcl::buffer<vcl::vec3> lava_positions;
	vcl::buffer<vcl::vec3> lava_normals;
	vcl::buffer<vcl::uint3> lava_connectivity;
	perlin_noise lava_perlin;

	
	
	//particle functions

	void create_particule();
	void update_particles();
	void draw_particles(std::map<std::string, GLuint>& shaders, scene_structure& scene);


	vcl::mesh_drawable particle; // Visual representation of a particle - a sphere
	vcl::mesh_drawable particle_fire;
	vcl::timer_event timer;
	std::list<particle_structure> particles; //storing all particles
	std::list<particle_structure> particles_fire; //storing all particles

};



