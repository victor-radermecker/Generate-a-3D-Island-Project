#pragma once
#include "main/scene_base/base.hpp"
#include "Environment_Object.hpp"


struct particle_structure
{
	//Used to make the smoke out of the volcano

	vcl::vec3 p; // Position
	vcl::vec3 v; // Speed
	float scale; // scale going from 0 (dissapear) to 1 (entire)
};


struct fire_particle_structure
{
	//Used to make the lava meteorites getting out of the volcano

	vcl::vec3 p; // Position
	vcl::vec3 v; // Speed
	float scale; // scale 
	bool activated = false;
	float theta = 0;
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

	void create_particule(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void update_particles();
	void draw_particles(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void init_objects();
	void update_smoke(bool is_new_particle, float dt);
	void update_fire_particle(float dt);
	void update_fire_cloud(bool is_new_particle, float dt);



	vcl::mesh_drawable particle; // Visual representation of a particle - a sphere
	vcl::mesh_drawable fire_sphere;
	vcl::timer_event timer;
	std::list<particle_structure> particles; //storing all particles
	fire_particle_structure fire_particle;
	std::list<particle_structure> fire_particles; //storing all particles

	//sprite for smoke
	vcl::mesh_drawable sprite_smoke;
	void init_smoke(std::map<std::string, GLuint>& shaders, scene_structure& scene);

	//Fire cloud
	vcl::mesh_drawable sprite_fire;
	void init_fire(std::map<std::string, GLuint>& shaders, scene_structure& scene);


	//rock
	vcl::mesh_drawable rock;
	GLuint rock_texture_id;
	vcl::mat3 random_rotation;
	void set_rock_rotation();


	vcl::mesh create_rock();
	void set_rock();
	void lava_model::draw_rock(std::map<std::string, GLuint>& shaders, scene_structure& scene);


};



