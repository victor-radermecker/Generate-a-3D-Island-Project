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
	rock1.uniform.transform.scaling = 0.2f;
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
	rock2.uniform.transform.scaling = 0.005f;
	rock2_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/rocks/Stone5ok17Diff.png"));

}

mesh object_model::create_palm_tree()
{
	mesh rock_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/palm/Palm_tall.obj");
	return rock_cpu;
}

void object_model::set_palm_tree()
{
	palm_tree = create_palm_tree();
	//palm_tree.uniform.transform.scaling = 0.02f;
	palm_tree_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/palm/leaf_front1.png"));
}

mesh object_model::create_billboard()
{
	mesh billboard;
	billboard.position = { {0,-1,-1}, { 0,1,0}, { 0,1,2}, {0,-1,2} };
	billboard.texture_uv = { {0,1}, {1,1}, {1,0}, {0,0} };
	billboard.connectivity = { {0,1,2}, {0,2,3} };
	return billboard;
}

void object_model::set_billboards()
{
	grass_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/billboards/grass.png"));
	flower1_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/billboards/flower1.png"));
	flower2_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/billboards/flower2.png"));
	flower3_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/billboards/flower3.png"));
	billboard = create_billboard();
	billboard.uniform.transform.scaling = 1.5f;
	billboard.uniform.shading.specular = 0;

	N_grass = 50;
	N_flower1 = 25;
	N_flower2 = 25;
	N_flower3 = 15;

	Rotation = rotation_from_axis_angle_mat3(vec3(0, 0, 1), 1.57f);
	Identity3 = mat3();
}


void object_model::init_objects(int N, std::vector<vcl::vec3>& list_position, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, bool is_billboard, terrain_model& env)
{
	update_random_position(N, list_position, min_dist, max_dist, min_height, z_offset_down, terrain_type, is_billboard, env);
}
void object_model::update_random_position(int N, std::vector<vcl::vec3>& list_position, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, bool is_billboard, terrain_model& env)
{
	//N is the total number of trees to place, i is the number currently places
	int i = 0;
	while (i < N) {
		float u = (float)rand() / (float)RAND_MAX;
		float v = (float)rand() / (float)RAND_MAX;

		vec3 pos = env.evaluate_terrain(u, v, terrain_type, vec3(0, 0, 0));
		if (acceptablePosition(pos, list_position, min_dist, max_dist, min_height, is_billboard)) {
			list_position.push_back(pos - vec3(0, 0, z_offset_down));
			++i;
		}
	}
}
bool object_model::acceptablePosition(vcl::vec3 pos, std::vector<vcl::vec3> list_position, float min_dist, float max_dist, float min_height, bool is_billboard)
{
	// Verify if the object is not to far/close to center, or too low (in the water)
	vec2 pos_xy = vec2(pos.x, pos.y);
	float dist = norm(pos_xy);
	if (dist < min_dist || dist > max_dist || pos.z < min_height)
		return false;

	if (is_billboard)
		return true;

	// Verify if it is not too close to other objects
	for (vec3 object_pos : list_position) {
		vec2 obj_xy = vec2(object_pos.x, object_pos.y);
		float dist_to_others = norm(pos_xy - obj_xy);

		if (dist_to_others < 2.0f) {
			return false;
		}
	}
	return true;
}

void object_model::init_rocks1(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env)
{
	init_objects(N, list_position_rock1, min_dist, max_dist, min_height, z_offset_down, terrain_type, false, env);

	mat3 base_rot = rotation_from_axis_angle_mat3(vec3(1, 1.2f, 0.5f), 1.5f);
	for (int i = 0; i < N; i++)
		rock1_rotations.push_back(rotation_from_axis_angle_mat3(vec3(0, 0, 1), rand_interval(0, 6.18f)) * base_rot);

}

void object_model::init_rocks2(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env)
{
	init_objects(N, list_position_rock2, min_dist, max_dist, min_height, z_offset_down, terrain_type, false, env);
	mat3 base_rot = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
	for (int i = 0; i < N; i++)
		rock2_rotations.push_back(rotation_from_axis_angle_mat3(vec3(0, 0, 1), rand_interval(0, 6.18f)) * base_rot);

}

void object_model::init_trees(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env)
{
	init_objects(N, tree_position, min_dist, max_dist, min_height, z_offset_down, terrain_type, false, env);

	mat3 base_rot = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
	for(int i = 0; i < N; i++)
		tree_rotations.push_back(rotation_from_axis_angle_mat3(vec3(0, 0, 1), rand_interval(0, 6.18f)) * base_rot);

}

void object_model::init_billboards(int N, float min_dist, float max_dist, float min_height, float z_offset_down, std::string terrain_type, terrain_model& env)
{
	init_objects(N, billboard_positions, min_dist, max_dist, min_height, z_offset_down, terrain_type, true, env);

}

void object_model::set_and_init_all(terrain_model& env)
{
	set_palm_tree();
	set_rock1();
	set_rock2();
	set_billboards();

	init_trees(20, 13.0f, 30.0f, 2.0f, 1.5f, "volcano", env);
	init_rocks1(1, 10.0f, 20.0f, 2.0f, 1.0f, "volcano", env);
	init_rocks2(2, 10.0f, 20.0f, 2.0f, 1.0f, "volcano", env);

	init_trees(5, 0.0f, 300.0f, 6.0f, 2.0f, "sand", env);
	init_rocks1(2, 0.0f, 300.0f, 6.0f, 1.5f, "sand", env);
	init_rocks2(3, 0.0f, 300.0f, 6.0f, 1.5f, "sand", env);
	
	init_billboards(N_grass + N_flower1 + N_flower2 + N_flower3, 0.0f, 300.0f, 10.0f, 0.5f, "sand", env);
}

void object_model::draw_rock1(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock1_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	for (int i = 0; i < list_position_rock1.size(); i++) {
		vec3 pos = list_position_rock1[i];
		rock1.uniform.transform.rotation = rock1_rotations[i];

		rock1.uniform.transform.translation = pos;
		draw(rock1, scene.camera, shaders["mesh_sun"]);
	}

}
void object_model::draw_rock2(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, rock2_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	for (int i = 0; i < list_position_rock2.size(); i++) {
		vec3 pos = list_position_rock2[i];
		rock2.uniform.transform.rotation = rock2_rotations[i];
		rock2.uniform.transform.translation = pos;
		draw(rock2, scene.camera, shaders["mesh_sun"]);
	}
}
void object_model::draw_tree(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, palm_tree_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	for (int i = 0; i < tree_position.size(); i++) {
		vec3 pos = tree_position[i];
		palm_tree.uniform.transform.rotation = tree_rotations[i];
		palm_tree.uniform.transform.translation = pos;
		draw(palm_tree, scene.camera, shaders["mesh_sun"]);
	}
}
void object_model::draw_all(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	draw_rock1(shaders, scene);
	draw_rock2(shaders, scene);
	draw_tree(shaders, scene);
}


void object_model::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, mat3& Identity, mat3& R)
{
	// Prepare for displaying transparent elements
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	// Draw grass billboards
	glBindTexture(GL_TEXTURE_2D, grass_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	for (int i = 0; i < N_grass; i++)
	{
		billboard.uniform.transform.translation = billboard_positions[i];
		billboard.uniform.transform.rotation = Identity;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
		billboard.uniform.transform.rotation = R;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
	}

	// Draw flower1 billboards
	glBindTexture(GL_TEXTURE_2D, flower1_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (int i = N_grass; i < N_grass + N_flower1; i++)
	{
		billboard.uniform.transform.translation = billboard_positions[i];
		billboard.uniform.transform.rotation = Identity;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
		billboard.uniform.transform.rotation = R;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
	}

	// Draw flower2 billboards
	glBindTexture(GL_TEXTURE_2D, flower2_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	for (int i = N_grass + N_flower1; i < N_grass + N_flower1 + N_flower2; i++)
	{
		billboard.uniform.transform.translation = billboard_positions[i];
		billboard.uniform.transform.rotation = Identity;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
		billboard.uniform.transform.rotation = R;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
	}

	// Draw flower3 billboards
	glBindTexture(GL_TEXTURE_2D, flower3_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	billboard.uniform.transform.scaling = 1.0f;
	for (int i = N_grass + N_flower1 + N_flower2; i < N_grass + N_flower1 + N_flower2 + N_flower3; i++)
	{
		billboard.uniform.transform.translation = billboard_positions[i];
		billboard.uniform.transform.rotation = Identity;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
		billboard.uniform.transform.rotation = R;
		draw(billboard, scene.camera, shaders["mesh_sun"]);
	}
	billboard.uniform.transform.scaling = 1.5f;



	glDepthMask(true);

}
