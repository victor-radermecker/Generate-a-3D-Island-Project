#pragma once
#include "main/scene_base/base.hpp"


struct perlin_noise
{
	float scaling;
	int octave;
	float persistency;
	float height;
};


struct terrain_model
{

	//	Terrain generation
	vcl::mesh create_terrain();
	float evaluate_terrain_z(float u, float v);
	vcl::vec3 evaluate_terrain(float u, float v);
	void set_terrain();

	//Ocean Generation
	vcl::mesh create_ocean();
	vcl::vec3 evaluate_ocean(float u, float v);
	void update_ocean(vcl::mesh_drawable& ocean, vcl::buffer<vcl::vec3>& current_position, vcl::buffer<vcl::vec3>& current_normals, vcl::buffer<vcl::uint3> connectivity, float t, float tmax, perlin_noise p);
	void set_ocean();

	//Mesh objects
	vcl::mesh_drawable terrain;

	// Ocean
	vcl::mesh ocean_cpu;
	vcl::mesh_drawable ocean;
	vcl::buffer<vcl::vec3> ocean_positions;
	vcl::buffer<vcl::vec3> ocean_normals;
	vcl::buffer<vcl::uint3> ocean_connectivity;
	perlin_noise ocean_perlin;





};



