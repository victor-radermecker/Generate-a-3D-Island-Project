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
	rock2.uniform.transform.scaling = 0.002f;
	rock2.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
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
	palm_tree.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
	palm_tree_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/palm/leaf_front1.png"));
}




void object_model::init_objects(int N, std::vector<vcl::vec3>& list_position, float min_dist, float z_offset_down, std::string terrain_type)
{
	update_random_position(N, list_position, min_dist, z_offset_down, terrain_type);
}
void object_model::update_random_position(int N, std::vector<vcl::vec3>& list_position, float min_dist, float z_offset_down, std::string terrain_type)
{
	//N is the number of trees
	while (list_position.size() < N) {
		float u = ((double)rand() / (RAND_MAX));
		float v = ((double)rand() / (RAND_MAX));
		vec3 pos = env.evaluate_terrain(u, v, terrain_type, vec3(0, 0, 0));
		if (acceptablePosition(pos, list_position, min_dist)) {
			list_position.push_back(pos - vec3(0, 0, z_offset_down));
		}
	}
}
bool object_model::acceptablePosition(vcl::vec3 pos, std::vector<vcl::vec3> list_position, float min_dist)
{
	for (vec3 tree : list_position) {
		float dist = 0;
		float radius = 0;
		for (int i = 0; i < 2; i++) {
			dist += (pos[i] - tree[i]) * (pos[i] - tree[i]);
			radius += pos[i] * pos[i];
			std::cout << radius << std::endl;
		}
		dist = pow(dist, 0.5);
		if (dist < min_dist || radius < 4.0f) {
			std::cout << "NO" << std::endl;
			return false;
		}
	}
	return true;
}


void object_model::init_rocks1(int N, float min_dist, float z_offset_down, std::string terrain_type)
{
	init_objects(N, list_position_rock1, min_dist, z_offset_down, terrain_type);
}

void object_model::init_rocks2(int N, float min_dist, float z_offset_down, std::string terrain_type)
{
	init_objects(N, list_position_rock2, min_dist, z_offset_down, terrain_type);
}

void object_model::init_trees(int N, float min_dist, float z_offset_down, std::string terrain_type)
{
	init_objects(N, tree_position, min_dist, z_offset_down, terrain_type);
}

void object_model::draw_rock1(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, rock1_texture_id);

	for (int i = 0; i < list_position_rock1.size(); i++) {
		vec3 pos = list_position_rock1[i];
		rock1.uniform.transform.translation = pos;
		draw(rock1, scene.camera, shaders["mesh_sun"]);
	}

}
void object_model::draw_rock2(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, rock2_texture_id);

	for (int i = 0; i < list_position_rock2.size(); i++) {
		vec3 pos = list_position_rock2[i];
		rock2.uniform.transform.translation = pos;
		draw(rock2, scene.camera, shaders["mesh_sun"]);
	}
}
void object_model::draw_tree(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, palm_tree_texture_id);


	for (int i = 0; i < tree_position.size(); i++) {
		vec3 pos = tree_position[i];
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
