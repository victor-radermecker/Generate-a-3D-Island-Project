#include "HumanConstructions.hpp"

using namespace vcl;


void treasure_model::create_treasure_box()
{
    float scale = 1;
    vec3 p0 = { 0,0,0 };

    const float L = 1.5f * scale;
    const float l = 0.65f * scale;
    const float h = 0.05f * scale;

    mesh_drawable bottom = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { l,0,0 }, { 0,L,0 }, { 0,0,h }, "bottom"));
    bottom.uniform.color = { 1,1,1 };
    hierarchy_treasure.add(bottom, "bottom");

    mesh_drawable left_panel = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { l,0,0 }, { 0,h,0 }, { 0,0,l }, "left"));
    mesh_drawable right_panel = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { l,0,0 }, { 0,h,0 }, { 0,0,l }, "right"));
    mesh_drawable back_panel = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { h,0,0 }, { 0,L,0 }, { 0,0,l - h }, "back"));
    mesh_drawable front_panel = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { h,0,0 }, { 0,L,0 }, { 0,0,l }, "front"));
    mesh_drawable top = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { l,0,0 }, { 0,L,0 }, { 0,0,h }, "top"));
    mesh_drawable top_gold = mesh_drawable(mesh_primitive_parallelepiped_chest(p0, { l - 2 * h,0,0 }, { 0,L - 2 * h,0 }, { 0,0,h }, "gold"));



    left_panel.uniform.color = { 1,1,1 };
    right_panel.uniform.color = { 1,1,1 };
    front_panel.uniform.color = { 1,1,1 };
    back_panel.uniform.color = { 1,1,1 };
    top_gold.uniform.color = { 1,1,0 };

    hierarchy_treasure.add(left_panel, "left", "bottom", vec3(0, -h / 10, 0.1 * h));
    hierarchy_treasure.add(right_panel, "right", "bottom", vec3(0, L - h * 0.9, 0.1 * h));
    hierarchy_treasure.add(back_panel, "back", "bottom", vec3(-0.1 * h, 0, h));
    hierarchy_treasure.add(front_panel, "front", "bottom", vec3(l - h * 0.9, 0, 0));
    hierarchy_treasure.add(top, "top", "back", vec3(0, 0, l - h));
    hierarchy_treasure.add(top_gold, "top_gold", "bottom", vec3(h, h, l - 4 * h));

    //mat3 const rot_top = rotation_from_axis_angle_mat3({ 0,1,0 }, -3.14f / 4);
    //hierarchy_treasure["top"].transform.rotation = rot_top;
    hierarchy_treasure["bottom"].transform.scaling = 1.5f;
    chest_position = vec3(17.f, -5.f, 6.7f);
    hierarchy_treasure["bottom"].transform.translation = chest_position ;
    mat3 rot_treasure = rotation_from_axis_angle_mat3(vec3(0, 0, 1), -1.57f / 3);
    canoe.uniform.transform.rotation = rot_treasure;
    hierarchy_treasure.update_local_to_global_coordinates();


    //texture of treasure
    treasure_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/chest/chestFull.png"));



}


void treasure_model::open_chest()
{
    //rotation of top


    if (open_chest_bool == false) {
        //opening the chest
        mat3 const rot_top = rotation_from_axis_angle_mat3({ 0,1,0 }, -3.14f / 3);
        hierarchy_treasure["top"].transform.rotation = rot_top;
        open_chest_bool = true;
    }
    else {
        //closing the chest
        mat3 const rot_top = rotation_from_axis_angle_mat3({ 0,1,0 }, 0);
        hierarchy_treasure["top"].transform.rotation = rot_top;
        open_chest_bool = false;

    }
   
    hierarchy_treasure.update_local_to_global_coordinates();

}

void treasure_model::draw_treasure(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glUseProgram(shaders["mesh_sun"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treasure_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    draw(hierarchy_treasure, scene.camera, shaders["mesh_sun"]);
}


mesh treasure_model::mesh_primitive_parallelepiped_chest(const vec3& p0, const vec3& u1, const vec3& u2, const vec3& u3, std::string part)
{

    const vec3 p000 = p0;
    const vec3 p100 = p0 + u1;
    const vec3 p010 = p0 + u2;
    const vec3 p001 = p0 + u3;
    const vec3 p110 = p0 + u1 + u2;
    const vec3 p101 = p0 + u1 + u3;
    const vec3 p011 = p0 + u2 + u3;
    const vec3 p111 = p0 + u1 + u2 + u3;

    mesh shape;
    shape.position = { p000, p100, p110, p010,  //bottom
                      p100, p101, p111, p110,    //front 
                      p110, p111, p011, p010,      //right
                      p000, p001, p011, p010,      //back
                      p000, p001, p101, p100,      //left
                      p101, p001, p011, p111 };    //top


    const vec3 n1 = normalize(u1);
    const vec3 n2 = normalize(u2);
    const vec3 n3 = normalize(u3);

    shape.normal = { -n3, -n3, -n3, -n3,
                    n1, n1, n1, n1,
                    n2, n2, n2, n2,
                    -n1, -n1, -n1, -n1,
                    -n2, -n2, -n2, -n2,
                    n3, n3, n3, n3 };

    shape.connectivity = { {0,1,2}, {0,2,3},
                          {4,5,6}, {4,6,7},
                          {8,9,10}, {8,10,11},
                          {12,13,14}, {12,14,15},
                          {16,17,18}, {16,18,19},
                          {20,21,22}, {20,22,23} };

    float w = 512; //width
    float h = 405;  //height

    std::vector<vec2> bottom = { {23 / w,100 / h}, {23 / w,195 / h}, {213 / w,195 / h} , {213 / w,100 / h} };
    std::vector<vec2> top = { {23 / w,100 / h}, {23 / w,200 / h}, {213 / w,200 / h} , {213 / w,100 / h} };  //top, bottom, back are the same
    std::vector<vec2> back = { {23 / w,100 / h}, {23 / w,200 / h}, {213 / w,200 / h} , {213 / w,100 / h} };  //top, bottom, back are the same
    std::vector<vec2> front = { {23 / w,205 / h}, {23 / w,295 / h}, {213 / w,295 / h} , {213 / w,205 / h} };  //front
    std::vector<vec2> left = { {285 / w,295 / h}, {285 / w,385 / h}, {382 / w,385 / h} , {382 / w,295 / h} };  //left
    std::vector<vec2> right = { {285 / w,295 / h}, {285 / w,385 / h}, {382 / w,385 / h} , {382 / w,295 / h} };  //right and left are the same

    std::vector<vec2> top_gold = { {320 / w,140 / h}, {320 / w,235 / h}, {450 / w,235 / h} , {450 / w,140 / h} };  //top, bottom, back are the same
    std::vector<vec2> metal_long = { {30 / w,97 / h}, {30 / w,103 / h}, {214 / w,103 / h} , {214 / w,97 / h} };  //long side metal bar
    std::vector<vec2> metal_short = { {285 / w,295 / h}, {285 / w,300 / h}, {383 / w,300 / h} , {383 / w,295 / h} };  //short side metal bar


    if (part == "gold") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         front[0], front[1], front[2], front[3],
                         right[0], right[1], right[2], right[3],
                         back[0], back[1], back[2], back[3],
                         left[0], left[1], left[2], left[3],
                         top_gold[0], top_gold[1], top_gold[2], top_gold[3] };
    }
    else if (part == "back") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         back[1], back[0], back[3], back[2],
                         right[0], right[1], right[2], right[3],
                         back[0], back[1], back[2], back[3],
                         left[0], left[1], left[2], left[3],
                         top[0], top[1], top[2], top[3] };
    }
    else if (part == "top") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         metal_long[1], metal_long[0], metal_long[3], metal_long[2],
                         metal_short[0], metal_short[1], metal_short[2], metal_short[3],
                         metal_long[0], metal_long[1], metal_long[2], metal_long[3],
                         metal_short[0], metal_short[1], metal_short[2], metal_short[3],
                         top[0], top[1], top[2], top[3] };
    }
    else if (part == "bottom") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         metal_long[1], metal_long[0], metal_long[3], metal_long[2],
                         metal_short[0], metal_short[1], metal_short[2], metal_short[3],
                         metal_long[0], metal_long[1], metal_long[2], metal_long[3],
                         metal_short[0], metal_short[1], metal_short[2], metal_short[3],
                         top[0], top[1], top[2], top[3] };
    }
    else if (part == "left" || part == "right") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         front[1], front[0], front[3], front[2],
                         right[0], right[1], right[2], right[3],
                         back[0], back[1], back[2], back[3],
                         left[0], left[1], left[2], left[3],
                         metal_short[0], metal_short[1], metal_short[2], metal_short[3] };
    }
    else if (part == "front") {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         front[1], front[0], front[3], front[2],
                         right[0], right[1], right[2], right[3],
                         back[0], back[1], back[2], back[3],
                         left[0], left[1], left[2], left[3],
                         metal_long[0], metal_long[1], metal_long[2], metal_long[3] };
    }

    else {
        shape.texture_uv = { bottom[0], bottom[1], bottom[2], bottom[3],
                         front[1], front[0], front[3], front[2],
                         right[0], right[1], right[2], right[3],
                         back[0], back[1], back[2], back[3],
                         left[0], left[1], left[2], left[3],
                         top[0], top[1], top[2], top[3] };
    }



    /*shape.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0},        //bottom
                            {0,0}, {0,1}, {1,1}, {1,0} ,        //front
                            {0,0}, {0,1}, {1,1}, {1,0} ,        //right
                            {0,0}, {0,1}, {1,1}, {1,0} ,        //back
                            {0,0}, {0,1}, {1,1}, {1,0} ,        //left
                            {0,0}, {0,1}, {1,1}, {1,0} };       //top
                            */

    return shape;
}

//interacting with the treasure chest

void treasure_model::mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int)
{

    // Cursor coordinates
    const vec2 cursor = glfw_cursor_coordinates_window(window);

    // Check that the mouse is clicked (drag and drop)
    const bool mouse_click_left = glfw_mouse_pressed_left(window);


    /*   INTERACTING WITH CHEST   */
    if (mouse_click_left) {
        const ray r = picking_ray(scene.camera, cursor);
        const vec3 chest_p = chest_position;
        const picking_info info_chest = ray_intersect_sphere(r, chest_p, 1.5f);
        if (info_chest.picking_valid) // the ray intersects a sphere
        {
            open_chest();
        }
    }
}




// Create the bridge

void treasure_model::set_bridge()
{
	bridge = create_bridge();
	bridge.uniform.transform.scaling_axis = { 0.035f, 0.035f, 0.05f };
	bridge_position = vec3(-60.f, -9.1f, 3.5f);
	bridge_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/bridge/textures/VARANDE_AO.png"));
}


mesh treasure_model::create_bridge()
{
	mesh bridge_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/bridge/BRIDGE_LOW.obj");
	return bridge_cpu;
}


void treasure_model::draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bridge_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	mat3 rot_bridge1 = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.57f);

	bridge.uniform.transform.rotation = rot_bridge1;
	bridge.uniform.transform.translation = bridge_position; // set the position of bridge to 'bridge' position
	draw(bridge, scene.camera, shaders["mesh_sun"]);
}

// Create the canoe

void treasure_model::set_canoe()
{
	canoe = create_canoe();
	canoe.uniform.transform.scaling = 1.f;
	canoe_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/canoe/wood.png"));
}


mesh treasure_model::create_canoe()
{
	mesh canoe_cpu = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/canoe/Canoe.obj");
	return canoe_cpu;
}


void treasure_model::draw_canoe(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
	glUseProgram(shaders["mesh_sun"]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, canoe_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	canoe_position = vec3(27.f, -10.1f, 3.5f);
	mat3 rot_canoe = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.57f);
	mat3 rot_canoe2 = rotation_from_axis_angle_mat3(vec3(0, 0, 1), -1.57f/3.5);
	canoe.uniform.transform.rotation = rot_canoe * rot_canoe2;
	canoe.uniform.transform.translation = canoe_position; 
	draw(canoe, scene.camera, shaders["mesh_sun"]);
}

// init all

void treasure_model::init_all()
{
    create_treasure_box();
    set_bridge();
    set_canoe();
}

void treasure_model::draw_all(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    draw_treasure(shaders, scene);
    draw_bridge(shaders, scene);
    draw_canoe(shaders, scene);
}