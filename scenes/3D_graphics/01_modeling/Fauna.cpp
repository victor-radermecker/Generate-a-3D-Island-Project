#include "Fauna.hpp"
#include <random>
using namespace vcl;


std::default_random_engine generator2;
std::uniform_real_distribution<float> distrib10(0.9, 1.0);






// ------------------------------------------------------ //
 //Animation Functions 
 // ------------------------------------------------------ //




vec3 fauna_model::cardinal_interpolation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K)
{
    const float s = (t - t1) / (t2 - t1);
    const vec3 d1 = 2 * K * (p2 - p0) / (t2 - t0);
    const vec3 d2 = 2 * K * (p3 - p1) / (t3 - t1);

    return (s * s * (2 * s - 3) + 1) * p1 + s * (1 + s * (-2 + s)) * d1 + s * s * (3 - 2 * s) * p2 + s * s * (s - 1) * d2;

}

vec3 fauna_model::get_position(float t, vcl::buffer<vec3t>& keys) {
    // ********************************************* //
    // Compute interpolated position at time t       //
    // ********************************************* //
    const int idx = index_at_value(t, keys);

    // Assume a closed curve trajectory
    const size_t N = keys.size();

    // Preparation of data for the linear interpolation
    // Parameters used to compute the linear interpolation
    const float t1 = keys[idx].t; // = t_i
    const float t2 = keys[idx + 1].t; // = t_{i+1}

    const vec3& p1 = keys[idx].p; // = p_i
    const vec3& p2 = keys[idx + 1].p; // = p_{i+1}


    // Create and call a function cardinal_spline_interpolation(...) instead
    // ...
    const float t0 = keys[idx - 1].t;  // = t_i-1
    const float t3 = keys[idx + 2].t;  // = t_i+2
    const vec3& p0 = keys[idx - 1].p;    // = p_i - 1
    const vec3& p3 = keys[idx + 2].p;  // = p_{i+2}

    return cardinal_interpolation(t, t0, t1, t2, t3, p0, p1, p2, p3, 0.5f);

}


void fauna_model::init_timer(vcl::timer_interval& timer_name, vcl::buffer<vec3t>& keys)
{
    timer_name.t_min = keys[1].t;                   // first time of the keyframe
    timer_name.t_max = keys[keys.size() - 2].t;  // last time of the keyframe
    timer_name.t = timer_name.t_min;

    keyframe_visual = mesh_primitive_sphere();
    keyframe_visual.uniform.color = { 1,1,1 };
    keyframe_visual.uniform.transform.scaling = 0.05f;

    keyframe_picked = mesh_primitive_sphere();
    keyframe_picked.uniform.color = { 1,0,0 };
    keyframe_picked.uniform.transform.scaling = 0.055f;

    segment_drawer.init();

    picked_object = -1;
}




mat3 fauna_model::get_rotation(float t, float t0, float t1, float t2, float t3, const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float K)
{
    const vec3 d1 = 2 * K * (p2 - p0) / (t2 - t0);
    const vec3 d2 = 2 * K * (p3 - p1) / (t3 - t1);

    const float alpha = (t - t1) / (t2 - t1);
    vec3 d = (1 - alpha) * d1 + alpha * d2; //linear interpolation of derivative at point p

    //d /= sqrt(d.x * d.x + d.y * d.y + d.z * d.z);

    /*const mat3 R = { {1, -d.x, 0},
                     {0, -d.y, 0},
                     {0, -d.z, 1} };*/

    const mat3 R = rotation_between_vector_mat3(vec3(1, 0, 0), d);

    //const double norm_d = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
    //const double theta = acos(-d.y / norm_d);
    //const mat3 R = rotation_from_axis_angle_mat3({ -d.z, 0, d.x }, theta);
    return R;
}

mat3 fauna_model::get_rotation(float t, vcl::buffer<vec3t>& keys)
{
    // ********************************************* //
    // Compute interpolated rotation at time t       //
    // ********************************************* //

    const int idx = index_at_value(t, keys);

    // Assume a closed curve trajectory
    const size_t N = keys.size();

    // Preparation of data for the linear interpolation
    // Parameters used to compute the linear interpolation
    const float t1 = keys[idx].t; // = t_i
    const float t2 = keys[idx + 1].t; // = t_{i+1}

    const vec3& p1 = keys[idx].p; // = p_i
    const vec3& p2 = keys[idx + 1].p; // = p_{i+1}


    // Create and call a function cardinal_spline_interpolation(...) instead
    // ...
    const float t0 = keys[idx - 1].t;  // = t_i-1
    const float t3 = keys[idx + 2].t;  // = t_i+2
    const vec3& p0 = keys[idx - 1].p;    // = p_i - 1
    const vec3& p3 = keys[idx + 2].p;  // = p_{i+2}

    return get_rotation(t, t0, t1, t2, t3, p0, p1, p2, p3, 0.2f);

}


size_t fauna_model::index_at_value(float t, vcl::buffer<vec3t>& keys)
{
    //Renvoie la dernière clef inférieure ou égale au temps t
    const size_t N = keys.size();
    size_t k = 0;
    while (keys[k + 1].t < t)
        ++k;
    return k;
}













// ------------------------------------------------------ //
 //Drawing bird 
 // ------------------------------------------------------ //


void fauna_model::setup_bird() {
    const float radius_body = 0.5f;
    const float radius_head = 0.25f;
    const float shoulder_length = 0.34f;
    const float shoulder_width = 0.4f;
    const float arm_length = 0.5f;
    const float arm_width = 0.1f;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_drawable(mesh_primitive_sphere(radius_body, { 0,0,0 }, 20, 20));
    body.uniform.transform.scaling_axis = { 1.2f,0.7f,0.7f };
    body.uniform.color = { 0.7f, 0.2f, 0.2f };

    // Geometry of head
    mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 20, 20));
    head.uniform.color = { 1.0f, 1.0f, 0 };

    // Geamotry of beak (cone)
    mesh beak_cpu = mesh_primitive_cone(0.12f, vec3(0, 0, 0), vec3(0.6f, 0, -0.1f), 20, 20);
    
    size_t N_beak = beak_cpu.position.size();
    beak_cpu.color.resize(N_beak);
    for (int i = 0; i < N_beak; i++)
    {
        float x = 1.0f * (float)i / (float)N_beak;
        float red = 0.5f + 0.3f * cos(x * 2.0f * 3.1415f);
        float green = 0.5f + 0.3f * cos((x + 0.3333f) * 2.0f * 3.1415f);
        float blue = 0.5f + 0.3f * cos((x + 0.6667f) * 2.0f * 3.1415f);
        beak_cpu.color[i] = vec4( red, green, blue, 1.0f);
    }

    mesh_drawable beak = beak_cpu;

    // Geometry of the eyes: black spheres
    mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.06f, { 0,0,0 }, 7, 7));
    eye.uniform.color = { 0,0.7f,1.0f };
    // Shoulder  : top of the wing, quadrangle
    mesh_drawable shoulder = mesh_primitive_quad(vec3(0, 0, 0), vec3(0, shoulder_length, 0), vec3(-shoulder_width, shoulder_length, 0), vec3(-shoulder_width, 0, 0));
    shoulder.uniform.color = { 0.05f, 0.15f, 0.05f };

    // Arm : bottom of the wing, quadrangle
    mesh_drawable arm = mesh_primitive_quad(vec3(0, 0, 0), vec3(-shoulder_width, 0, 0), vec3(- shoulder_width, arm_length, 0), vec3(-shoulder_width + arm_width, arm_length, 0));
    arm.uniform.color = { 0.8f, 0.8f, 0.8f };


    // Build the hierarchy:
    // Syntax to add element
    //   hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])
    bird.add(body, "body");

    // Set the head and the eyes, and the beak
    bird.add(head, "head", "body", radius_body * vec3(1.1f, 0, 0.5f));

    bird.add(eye, "eye_left", "head", radius_head * vec3(1 / 1.5f, 1 / 2.0f, 1 / 3.0f));
    bird.add(eye, "eye_right", "head", radius_head * vec3(1 / 1.5f, -1 / 2.0f, 1 / 3.0f));
    bird.add(beak, "beak", "head", radius_head * vec3(0.7f, 0, -0.2f));


    // Set the left part of the body arm: shoulder-elbow-arm
    bird.add(shoulder, "shoulder_left", "body", { 0.1f,radius_body * 0.6f,0}); // extremity of the spherical body
    bird.add(arm, "arm_bottom_left", "shoulder_left", { 0,shoulder_length, 0});                        // the arm start at the center of the elbow

    // Set the right part of the body arm: similar to the left part excepted a symmetry is applied along y direction for the shoulder
    bird.add(shoulder, "shoulder_right", "body", { {0.1f,-radius_body * 0.6f,0}, {1,0,0, 0,-1,0, 0,0,1}/*Symmetry*/ });
    bird.add(arm, "arm_bottom_right", "shoulder_right", { 0,shoulder_length,0 });
    

}

void fauna_model::set_bird_keyframes()
{
    // Il faut au moins 6 points pour que ce l'interpolation soit possible
    // Et les 3 derniers doivent être idetntiques aux 3 premiers (pour un boucle)
    // Le premier temps doit être inférieur à timer.t_min
  

std::vector<vec3> keyframe_position = { {54.9148f,19.3474f,30.7046f}, {67.6399f,28.4372f,26.3839f},
                                            {85,32,26}, {93.1389f,43.5856f,19.121f},
                                            {99,67,16}, {96.9577f,78.806f,13.7881f},
                                            {89,87,11}, {74.1603f,90.9598f,12.175f},
                                            {56,83,14}, {43.4345f,73.589f,15.1772f},
                                            {30,65,18}, {24.2062f,55.0778f,18.3315f},
                                            {18.1149f,46.7241f,17.1391f}, {8.3415f,39.813f,19.8772f},
                                            {-3.68978f,36.298f,17.9547f}, {-12.1354f,39.8758f,20.0135f},
                                            {-27,52,19}, {-32.7706f,62.4676f,16.8557f},
                                            {-36.1918f,73.1443f,17.2897f}, {-41.2002f,80.0028f,15.1059f},
                                            {-52,86,15}, {-68.4563f,87.3021f,14.0391f},
                                            {-82.674f,80.5606f,17.3077f}, {-89.0487f,66.976f,23.5047f},
                                            {-92.9506f,48.6087f,24.3035f}, {-80.1503f,29.1942f,28.3507f},
                                            {-64.5063f,18.6017f,22.3111f}, {-56.831f,11.274f,23.3999f},
                                            {-54.8933f,-1.06999f,21.4609f}, {-62.3239f,-10.2664f,23.6312f},
                                            {-71.6844f,-18.4844f,23.4017f}, {-84.1772f,-26.3277f,21.2737f},
                                            {-93.8271f,-35.1776f,20.2459f}, {-98.5213f,-46.4066f,24.4245f},
                                            {-99.7266f,-65.1846f,23.3187f}, {-86.5131f,-80.8987f,29.804f},
                                            {-69.6836f,-87.2802f,30.3084f}, {-45.2748f,-74.1915f,28.4198f},
                                            {-34.2948f,-55.3886f,27.3166f}, {-35.87f,-36.8296f,21.2248f},
                                            {-36.8738f,-23.1538f,13.5402f}, {-31.7009f,-14.1697f,15.473f},
                                            {-24,-1,15}, {-22.3796f,12.1491f,15.9118f}, {-4,19,16},
                                            {1.32445f,19.9717f,13.07f}, {15.4436f,16.2788f,7.21621f},
                                            {30.4114f,8.86765f,8.88205f}, {37.6316f,-4.10415f,9.25065f},
                                            {63.8006f,-10.0096f,12.3249f}, {80.8627f,-28.3386f,14.7858f},
                                            {78.1944f,-52.456f,16.0434f}, {52.1547f,-63.3542f,17.4368f},
                                            {41.9989f,-53.5907f,19.907f}, {35.1962f,-41.0269f,25.7137f},
                                            {33.8145f,-25.0471f,28.1503f}, {35.8901f,-12.8365f,31.3032f},
                                            {49.0166f,0.106949f,30.4218f}, {54.9148f,19.3474f,30.7046f},
                                            {67.6399f,28.4372f,26.3839f}, {85,32,26} 
};

    const float speed = 8.0f;
    float t = 0;
    for (size_t i = 0; i < keyframe_position.size(); i++)
    {
        vec3 p = keyframe_position[i];
        bird_keyframes.push_back({ keyframe_position[i], t });
        if (i != keyframe_position.size() - 1)
            t += norm(p - keyframe_position[i + 1]) / speed;
    }

    init_timer(timer_fauna_bird, bird_keyframes);

}



void fauna_model::update_bird()
{
    timer_fauna_bird.update();
    const float t = timer_fauna_bird.t;

    // Rotate it in the right direction
    mat3 const R_body = get_rotation(t, bird_keyframes);
    bird["body"].transform.rotation = R_body;

    // The body oscillate along the z direction and moves 
    vec3 const p = get_position(t, bird_keyframes);
    bird["body"].transform.translation = p + vec3(0, 0, 0.05f * (1 + std::sin(2 * 3.14f * t)));
    // Rotation of the shoulder around the y axis
    mat3 const R_shoulder = rotation_from_axis_angle_mat3({ 1,0, 0}, std::sin(4 * 3.14f * (t - 0.4f)));
    // Rotation of the arm around the y axis (delayed with respect to the shoulder)
    mat3 const R_arm = rotation_from_axis_angle_mat3({ 1,0,0 }, std::sin(4 * 3.14f * (t - 0.6f)));
    // Symmetry in the x-direction between the left/right parts
    mat3 const Symmetry = { 1,0,0, 0,-1,0, 0,0,1 };

    // Set the rotation to the elements in the hierarchy
    bird["shoulder_left"].transform.rotation = R_shoulder;
    bird["arm_bottom_left"].transform.rotation = R_arm;

    bird["shoulder_right"].transform.rotation = Symmetry * R_shoulder; // apply the symmetry
    bird["arm_bottom_right"].transform.rotation = R_arm; //note that the symmetry is already applied by the parent element

    bird.update_local_to_global_coordinates();
}

void fauna_model::draw_bird(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glUseProgram(shaders["mesh_sun"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    draw(bird, scene.camera, shaders["mesh_sun"]);
}

void fauna_model::draw_keyframes(std::map<std::string, GLuint>& shaders, scene_structure& scene, vcl::buffer<vec3t>& keys, vec3 color)
{

    // Draw sphere at each keyframe position
    size_t N = keys.size();
    for (size_t k = 0; k < N; ++k)
    {
        const vec3& p_keyframe = keys[k].p;
        keyframe_visual.uniform.transform.translation = p_keyframe;
        draw(keyframe_visual, scene.camera, shaders["mesh"]);
    }
    
    // Draw selected sphere in red
    if (picked_object != -1)
    {
        const vec3& p_keyframe = keys[picked_object].p;
        keyframe_picked.uniform.transform.translation = p_keyframe;
        draw(keyframe_picked, scene.camera, shaders["mesh"]);
    }
    // Draw segments between each keyframe
    for (size_t k = 0; k < keys.size() - 1; ++k)
    {
        const vec3& pa = keys[k].p;
        const vec3& pb = keys[k + 1].p;
        segment_drawer.uniform_parameter.p1 = pa;
        segment_drawer.uniform_parameter.p2 = pb;
        segment_drawer.uniform_parameter.color = color;
        segment_drawer.draw(shaders["segment_im"], scene.camera);
    }
}

void fauna_model::mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int)
{
    // Mouse click is used to select a position of the control polygon
    // ******************************************************************** //

    // Cursor coordinates
    const vec2 cursor = glfw_cursor_coordinates_window(window);

    // Check that the mouse is clicked (drag and drop)
    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);

    // Check if shift key is pressed
    if (mouse_click_left && key_shift)
    {
        // Create the 3D ray passing by the selected point on the screen
        const ray r = picking_ray(scene.camera, cursor);

        // Check if this ray intersects a position (represented by a sphere)
        //  Loop over all positions and get the intersected position (the closest one in case of multiple intersection)
        const size_t N = bird_keyframes.size();
        picked_object = -1;
        float distance_min = 0.0f;
        for (size_t k = 0; k < N; ++k)
        {
            const vec3 c = bird_keyframes[k].p;
            const picking_info info = ray_intersect_sphere(r, c, 0.1f);

            if (info.picking_valid) // the ray intersects a sphere
            {
                const float distance = norm(info.intersection - r.p); // get the closest intersection
                if (picked_object == -1 || distance < distance_min) {
                    distance_min = distance;
                    picked_object = k;
                    std::cout << "you cliked on the point!" << std::endl;
                }
            }
        }
    }
    if (mouse_click_left) {
        const ray r = picking_ray(scene.camera, cursor);
        //Trying to interact with the shark
        const float t = timer_fauna_shark.t;
        const vec3 shark_p = get_position(t, shark_keyframes);
        const picking_info info_shark = ray_intersect_sphere(r, shark_p, 1.0f);
        if (info_shark.picking_valid) // the ray intersects a sphere
        {
            std::cout << "You clicked on the shark !" << std::endl;
        }
    }

}

void fauna_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{

    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);
    if (mouse_click_left && key_shift && picked_object != -1)
    {
        // Translate the selected object to the new pointed mouse position within the camera plane
        // ************************************************************************************** //

        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = { M(0,2),M(1,2),M(2,2) };

        // Compute intersection between current ray and the plane orthogonal to the view direction and passing by the selected object
        const vec2 cursor = glfw_cursor_coordinates_window(window);
        const ray r = picking_ray(scene.camera, cursor);
        vec3& p0 = bird_keyframes[picked_object].p;
        const picking_info info = ray_intersect_plane(r, n, p0);

        // translate the position
        p0 = info.intersection;

        // If the selected point is a double point, move the other one too
        size_t N = bird_keyframes.size();
        if (picked_object == 0 && N > 2)
            bird_keyframes[N - 3].p = info.intersection;
        else if (picked_object == 1 && N > 1)
            bird_keyframes[N - 2].p = info.intersection;
        else if (picked_object == 2 && N > 0)
            bird_keyframes[N - 1].p = info.intersection;
        else if (N > 2 && picked_object == N - 2)
            bird_keyframes[0].p = info.intersection;
        else if (N > 1 && picked_object == N - 1)
            bird_keyframes[1].p = info.intersection;
        else if (N > 0 && picked_object == N)
            bird_keyframes[0].p = info.intersection;
    }
}




// ------------------------------------------------------ //
//Drawing shark
// ------------------------------------------------------ //



mesh fauna_model::create_shark()
{
    mesh shark = mesh_load_file_obj("scenes/3D_graphics/02_texture/assets/shark/shark.obj");
    return shark;
}

void fauna_model::set_shark()
{
    shark = create_shark();
    shark.uniform.transform.scaling = 2.5f;
    //shark.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(1, 0, 0), 1.57f);
    //shark.uniform.transform.rotation = rotation_from_axis_angle_mat3(vec3(0, 0, 1), 1.57f);
    //shark_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/asserts/shark/Sharktexture002.png"));
}

void fauna_model::draw_shark(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe
    glBindTexture(GL_TEXTURE_2D, shark_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // avoids sampling artifacts
    //shark.uniform.color = { 1.f,1.f,1.f };
    draw(shark, scene.camera, shaders["mesh"]);
}



void fauna_model::set_shark_keyframes()
{
    std::vector<vec3> keyframe_position = { {54.9148f,19.3474f,30.7046f}, {67.6399f,28.4372f,26.3839f},
                                            {85,32,26}, {93.1389f,43.5856f,19.121f},
                                            {99,67,16}, {96.9577f,78.806f,13.7881f},
                                            {89,87,11}, {74.1603f,90.9598f,12.175f},
                                            {56,83,14}, {43.4345f,73.589f,15.1772f},
                                            {30,65,18}, {24.2062f,55.0778f,18.3315f},
                                            {18.1149f,46.7241f,17.1391f}, {8.3415f,39.813f,19.8772f},
                                            {-3.68978f,36.298f,17.9547f}, {-12.1354f,39.8758f,20.0135f},
                                            {-27,52,19}, {-32.7706f,62.4676f,16.8557f},
                                            {-36.1918f,73.1443f,17.2897f}, {-41.2002f,80.0028f,15.1059f},
                                            {-52,86,15}, {-68.4563f,87.3021f,14.0391f},
                                            {-82.674f,80.5606f,17.3077f}, {-89.0487f,66.976f,23.5047f},
                                            {-92.9506f,48.6087f,24.3035f}, {-80.1503f,29.1942f,28.3507f},
                                            {-64.5063f,18.6017f,22.3111f}, {-56.831f,11.274f,23.3999f},
                                            {-54.8933f,-1.06999f,21.4609f}, {-62.3239f,-10.2664f,23.6312f},
                                            {-71.6844f,-18.4844f,23.4017f}, {-84.1772f,-26.3277f,21.2737f},
                                            {-93.8271f,-35.1776f,20.2459f}, {-98.5213f,-46.4066f,24.4245f},
                                            {-99.7266f,-65.1846f,23.3187f}, {-86.5131f,-80.8987f,29.804f},
                                            {-69.6836f,-87.2802f,30.3084f}, {-45.2748f,-74.1915f,28.4198f},
                                            {-34.2948f,-55.3886f,27.3166f}, {-35.87f,-36.8296f,21.2248f},
                                            {-36.8738f,-23.1538f,13.5402f}, {-31.7009f,-14.1697f,15.473f},
                                            {-24,-1,15}, {-22.3796f,12.1491f,15.9118f}, {-4,19,16},
                                            {1.32445f,19.9717f,13.07f}, {15.4436f,16.2788f,7.21621f},
                                            {30.4114f,8.86765f,8.88205f}, {37.6316f,-4.10415f,9.25065f},
                                            {63.8006f,-10.0096f,12.3249f}, {80.8627f,-28.3386f,14.7858f},
                                            {78.1944f,-52.456f,16.0434f}, {52.1547f,-63.3542f,17.4368f},
                                            {41.9989f,-53.5907f,19.907f}, {35.1962f,-41.0269f,25.7137f},
                                            {33.8145f,-25.0471f,28.1503f}, {35.8901f,-12.8365f,31.3032f},
                                            {49.0166f,0.106949f,30.4218f}, {54.9148f,19.3474f,30.7046f},
                                            {67.6399f,28.4372f,26.3839f}, {85,32,26}
    };

    const float speed = 8.0f;
    float t = 0;
    for (size_t i = 0; i < keyframe_position.size(); i++)
    {
        vec3 p = keyframe_position[i];
        float randt = distrib10(generator2);
        shark_keyframes.push_back({ randt* keyframe_position[i], t });
        if (i != keyframe_position.size() - 1)
            t += norm(p - keyframe_position[i + 1]) / speed;
    }
    init_timer(timer_fauna_shark, bird_keyframes);
}

void fauna_model::update_shark()
{
    timer_fauna_shark.update();
    const float t = timer_fauna_shark.t;

    // Rotate it in the right direction
    mat3 R_body1 = rotation_from_axis_angle_mat3(vec3(0, 0, 1), 2*1.57f);
    mat3 const R_body2 = get_rotation(t, shark_keyframes);
    shark.uniform.transform.rotation = R_body1 * R_body2;

    // The body oscillate along the z direction and moves 
    vec3 const p = get_position(t, shark_keyframes);
    shark.uniform.transform.translation = p + vec3(0, 0, 0.05f * (1 + std::sin(2 * 3.14f * t)));
}