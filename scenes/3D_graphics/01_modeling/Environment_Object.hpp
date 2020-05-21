#pragma once
#include "main/scene_base/base.hpp"
#include "Environment.hpp"


struct object_model
{
	// Global parameters
	std::vector<vcl::vec3> list_position_rock1;
	std::vector<vcl::vec3> list_position_rock2;
	std::vector<vcl::vec3> tree_position;

	std::vector<vcl::mat3> rock1_rotations;	
	std::vector<vcl::mat3> rock2_rotations;
	std::vector<vcl::mat3> tree_rotations;


	int N_rocks1;
	int N_rocks2;

	
	//Rock generation
	//Functions 
	vcl::mesh create_rock1();
	void set_rock1();
	vcl::mesh create_rock2();
	void set_rock2();
	//Features
	vcl::mesh_drawable rock2;
	GLuint rock2_texture_id;	
	vcl::mesh_drawable rock1;
	GLuint rock1_texture_id;



	// Trees
	vcl::mesh create_palm_tree();
	void set_palm_tree();

	vcl::mesh create_palm_tree2();
	void set_palm_tree2();

	vcl::mesh_drawable palm_tree;
	GLuint palm_tree_texture_id;
	vcl::mesh_drawable palm_tree2;
	GLuint palm_tree2_texture_id;



	/**************/
	// Billboards //
	/**************/
	std::vector<vcl::vec3> billboard_positions;
	int N_grass;
	int N_flower1;
	int N_flower2;
	int N_flower3;

	GLuint grass_texture_id;
	GLuint flower1_texture_id;
	GLuint flower2_texture_id;
	GLuint flower3_texture_id;

	vcl::mesh create_billboard();
	void set_billboards();
	vcl::mesh_drawable billboard;
	vcl::mat3 Identity3;
	vcl::mat3 Rotation;




	// Objects initialisation
	void init_objects(int N, std::vector<vcl::vec3>& list_position, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, bool billboard,  terrain_model& env);
	void update_random_position(int N, std::vector<vcl::vec3>& list_position, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, bool billboard, terrain_model& env);
	bool acceptablePosition(vcl::vec3 pos, std::vector<vcl::vec3> list_position, float min_dist, float max_dist, float min_height, bool billboard);


	void init_rocks1(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env);
	void init_rocks2(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env);
	void init_trees(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env);
	void init_billboards(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env);

	void set_and_init_all(terrain_model& env);



	/*      DRAWING METHODS          */
	void draw_rock1(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void draw_rock2(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void draw_tree(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void draw_all(std::map<std::string, GLuint>& shaders, scene_structure& scene);  
	void draw_palm_tree2(std::map<std::string, GLuint>& shaders, scene_structure& scene);

	void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, vcl::mat3& Identity, vcl::mat3& R);
};