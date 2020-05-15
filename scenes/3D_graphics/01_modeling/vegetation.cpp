#include "vegetation.hpp"

using namespace vcl;

mesh vegetation_model::create_palm_tree()
{
	mesh rock_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/palm/Palm_tall.obj");
	return rock_cpu;
}

void vegetation_model::set_palm_tree()
{
	palm_tree = create_palm_tree();
	palm_tree.uniform.transform.translation = vec3(10, 10, 10);
	//palm_tree.uniform.transform.scaling = 0.02f;
	palm_tree.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.5f);
	palm_tree_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/rocks/rock2.png"));
}

void vegetation_model::init_forest(int N, std::vector<vec3>& list_position, float min_dist, float z_offset_down, std::string type)
{
    update_random_position(N, list_position, min_dist, z_offset_down, type);
}

void vegetation_model::update_random_position(int N, std::vector<vec3>& list_position, float min_dist, float z_offset_down, std::string type)
{
    //N is the number of trees
    while (list_position.size() < N) {
        float u = ((double)rand() / (RAND_MAX));
        float v = ((double)rand() / (RAND_MAX));
        vec3 pos = env.evaluate_terrain(u, v, type, vec3(0,0,0));
        if (acceptablePosition(pos, list_position, min_dist)) {
            list_position.push_back(pos - vec3(0, 0, z_offset_down));
        }
    }
}

bool vegetation_model::acceptablePosition(vec3 pos, std::vector<vec3> list_position, float min_dist)
{
    for (vec3 tree : list_position) {
        float dist = 0;
        float radius = 0;
        for (int i = 0;i < 3;i++) {
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


