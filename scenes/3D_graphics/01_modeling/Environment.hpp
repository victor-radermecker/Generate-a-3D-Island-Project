#pragma once
#include "main/scene_base/base.hpp"


struct perlin_noise
{
	float scaling;
	int octave;
	float persistency;
	float height;
};

struct terrain_textures
{
	GLuint sand_id; // Sand texture id
	GLuint grass_id; // Grass texture id
	GLuint rock_id; // Rock texture id
};

struct island_param
{
	std::vector<vcl::vec2> u0;
	std::vector<float> h;
	std::vector<float> sigma;
	int N;

	void set_island_parameters(std::vector<vcl::vec2>& u0, std::vector<float>& h, std::vector<float>& sigma, int& N);

};


struct terrain_model
{

	//	Terrain generation
	vcl::mesh create_terrain(std::string type, vcl::vec3 p0);
	float evaluate_terrain_z(float u, float v);
	vcl::vec3 evaluate_terrain(float u, float v, std::string type, vcl::vec3 p0);
	void set_terrain();

	//Ocean Generation
	GLuint ocean_texture_id;
	vcl::mesh create_ocean();
	vcl::vec3 evaluate_ocean(float u, float v);
	void update_ocean(vcl::mesh_drawable& ocean, vcl::buffer<vcl::vec3>& current_position, vcl::buffer<vcl::vec3>& current_normals, vcl::buffer<vcl::uint3> connectivity, float t, float tmax, perlin_noise p);
	void set_ocean();

	// Ocean
	vcl::mesh ocean_cpu;
	vcl::mesh_drawable ocean;
	vcl::buffer<vcl::vec3> ocean_positions;
	vcl::buffer<vcl::vec3> ocean_normals;
	vcl::buffer<vcl::uint3> ocean_connectivity;
	perlin_noise ocean_perlin;



	//volcano
	float evaluate_terrain_z_volcano(float u, float v);
	vcl::vec3 terrain_model::evaluate_terrain_volcano(float u, float v);

	//sand
	vcl::vec3 terrain_model::evaluate_terrain_sand(float u, float v);
	float evaluate_terrain_z_sand(float u, float v);


	//Mesh objects
	std::vector<vcl::mesh_drawable> terrain;

	//Small islands parameters
	island_param islands;

	// Terrain textures
	terrain_textures texture_ids;

	// Display methods
	void draw_terrain(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void draw_ocean(std::map<std::string, GLuint>& shaders, scene_structure& scene);





};



