#include "Environment_Object.hpp"

using namespace vcl;

mesh object_model::create_rock1()
{
	mesh rock_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/rocks/Stone_01.OBJ");
	return rock_cpu;
}

void object_model::set_rock1()
{
	rock1 = create_rock1();
	rock1.uniform.transform.translation = vec3(10, 10, 10);
	rock1.uniform.transform.scaling = 0.02f;
	rock1.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 1.2f, 0.5f), 1.5f);
	rock1_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/rocks/rock2.png"));

}

mesh object_model::create_rock2()
{
	mesh rock_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/rocks/Stone5ok17.obj");
	return rock_cpu;
}

void object_model::set_rock2()
{
	rock2 = create_rock2();
	rock2.uniform.transform.translation = vec3(10, 10, 10);
	rock2.uniform.transform.scaling = 0.002f;
	rock2.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
	rock2_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/rocks/Stone5ok17Diff.png"));

}