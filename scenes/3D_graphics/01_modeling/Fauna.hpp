#pragma once
#include "main/scene_base/base.hpp"

struct vec3t {
	vcl::vec3 p; // position
	float t;     // time
};


struct fauna_model
{
	
	vcl::hierarchy_mesh_drawable bird;
	vcl::buffer<vec3t> keyframes;

	// Timer
	vcl::timer_interval timer_fauna;

	void setup_bird();
	vcl::vec3 get_bird_position(float t, float t0, float t1, float t2, float t3, const vcl::vec3& p0, const vcl::vec3& p1, const vcl::vec3& p2, const vcl::vec3& p3, float K);
	vcl::vec3 get_bird_position(float t);
	vcl::mat3 get_bird_rotation(float t, float t0, float t1, float t2, float t3, const vcl::vec3& p0, const vcl::vec3& p1, const vcl::vec3& p2, const vcl::vec3& p3, float K);
	vcl::mat3 get_bird_rotation(float t);
	size_t index_at_value(float t);
	void set_keyframes();

	void update_bird();
	void draw_bird(std::map<std::string, GLuint>& shaders, scene_structure& scene);



	// For displaying and moving the keyframes
	vcl::mesh_drawable keyframe_visual;                    // keyframe samples
	vcl::mesh_drawable keyframe_picked;                    // showing the picked sample
	vcl::segment_drawable_immediate_mode segment_drawer;   // used to draw segments between keyframe samples
	vcl::curve_dynamic_drawable trajectory;                // Draw the trajectory of the moving point as a curve

	// Store the index of a selected sphere
	int picked_object;

	void draw_keyframes(std::map<std::string, GLuint>& shaders, scene_structure& scene);
	
	void mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int);
	void mouse_move(scene_structure& scene, GLFWwindow* window);

};