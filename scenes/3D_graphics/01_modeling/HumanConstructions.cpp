#include "HumanConstructions.hpp"

using namespace vcl;


void treasure_model::create_treasure_box()
{
	vec3 p0 = { 0,0,0 };
	float scale = 1.0f;
	const float L = 1.5f * scale;
	const float l = 0.65f * scale;
	const float h = 0.05f * scale;

	mesh_drawable bottom = mesh_drawable(mesh_primitive_parallelepiped(p0, { l,0,0 }, { 0,L,0 }, { 0,0,h }));
	bottom.uniform.color = { 1,1,1 };
	hierarchy_treasure.add(bottom, "bottom");

	mesh_drawable left_panel = mesh_drawable(mesh_primitive_parallelepiped(p0, { l,0,0 }, { 0,h,0 }, { 0,0,l-h }));
	mesh_drawable right_panel = mesh_drawable(mesh_primitive_parallelepiped(p0, { l,0,0 }, { 0,h,0 }, { 0,0,l }));
	mesh_drawable back_panel = mesh_drawable(mesh_primitive_parallelepiped(p0, { h,0,0 }, { 0,L,0 }, { 0,0,l - h }));
	mesh_drawable front_panel = mesh_drawable(mesh_primitive_parallelepiped(p0, { h,0,0 }, { 0,L,0 }, { 0,0,l-h }));
	mesh_drawable top = mesh_drawable(mesh_primitive_parallelepiped(p0, { l,0,0 }, { 0,L,0 }, { 0,0,h }));
	mesh_drawable money = mesh_drawable(mesh_primitive_parallelepiped(p0, { l - 2 * h,0,0 }, { 0,L - 2 * h,0 }, { 0,0,h }));

	left_panel.uniform.color = { 1,1,1 };
	right_panel.uniform.color = { 1,1,1 };
	front_panel.uniform.color = { 1,1,1 };
	back_panel.uniform.color = { 1,1,1 };
	money.uniform.color = { 1,1,0 };

	hierarchy_treasure.add(left_panel, "left", "bottom");
	hierarchy_treasure.add(right_panel, "right", "bottom", vec3(0, L - h, 0));
	hierarchy_treasure.add(back_panel, "back", "bottom", { 0, 0, h });
	hierarchy_treasure.add(front_panel, "front", "bottom", vec3(l-h, 0, h));
	hierarchy_treasure.add(top, "top", "back", vec3(0, 0, l - h));
	hierarchy_treasure.add(money, "money", "bottom", vec3(h, h, l - 2 * h));

	
}


void treasure_model::open_chest()
{
	//rotation of top
	mat3 const rot_top = rotation_from_axis_angle_mat3({ 0,1,0 }, 3.14f / 4);
	hierarchy_treasure["top"].transform.rotation = rot_top;
	hierarchy_treasure.update_local_to_global_coordinates();
}


void treasure_model::draw_treasure(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, scene.texture_white);

	draw(hierarchy_treasure, scene.camera, shaders["mesh_sun"]);
}