#pragma once
#include "main/scene_base/base.hpp"

struct vec3t {
	vcl::vec3 p; // position
	float t;     // time
};


struct fauna_model
{

	//init all animals
	void init_fauna();

	// Animation Functions
	void init_timer(vcl::timer_interval& timer_name, vcl::buffer<vec3t>& keys);
	vcl::vec3 cardinal_interpolation(float t, float t0, float t1, float t2, float t3, const vcl::vec3& p0, const vcl::vec3& p1, const vcl::vec3& p2, const vcl::vec3& p3, float K);
	vcl::vec3 get_position(float t, vcl::buffer<vec3t>& keys);
	vcl::mat3 get_rotation(float t, float t0, float t1, float t2, float t3, const vcl::vec3& p0, const vcl::vec3& p1, const vcl::vec3& p2, const vcl::vec3& p3, float K);
	vcl::mat3 get_rotation(float t, vcl::buffer<vec3t>& keys);
	size_t index_at_value(float t, vcl::buffer<vec3t>& keys);

	//Drawing trajectory  (displaying and moving the keyframes)
	void draw_keyframes(std::map<std::string, GLuint>& shaders, scene_structure& scene, vcl::buffer<vec3t>& keys, vcl::vec3 color);
	vcl::mesh_drawable keyframe_visual;                    // keyframe samples
	vcl::mesh_drawable keyframe_picked;                    // showing the picked sample
	vcl::segment_drawable_immediate_mode segment_drawer;   // used to draw segments between keyframe samples
	int picked_object;									   // Getting an element of the trajectory



	/*        ANIMALS        */

	// Bird Features
	vcl::timer_interval timer_fauna_bird;
	vcl::hierarchy_mesh_drawable bird;
	vcl::buffer<vec3t> bird_keyframes;

	// Bird Functions 
	void setup_bird();
	void set_bird_keyframes();
	void update_bird();
	void draw_bird(std::map<std::string, GLuint>& shaders, scene_structure& scene);
							   
	// Shark Features
	vcl::timer_interval timer_fauna_shark;
	vcl::mesh_drawable shark;
	GLuint shark_texture_id;
	vcl::buffer<vec3t> shark_keyframes;

	// Shark functions
	vcl::mesh create_shark();
	void set_shark();
	void set_shark_keyframes();
	void draw_shark(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	void update_shark();

	//User interation
	void mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int);
	void mouse_move(scene_structure& scene, GLFWwindow* window);
	bool bird_clicked;
	bool shark_clicked;

};