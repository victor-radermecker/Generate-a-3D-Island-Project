#include <cmath>
#include <iostream>

#include "modeling.hpp"


#ifdef SCENE_3D_GRAPHICS

// Add vcl namespace within the current one - Allows to use function from vcl library without explicitely preceeding their name with vcl::
using namespace vcl;



/*
float evaluate_terrain_z(float u, float v);
vec3 evaluate_terrain(float u, float v);
mesh create_terrain();
*/

//mesh create_ocean();
//void update_ocean(mesh_drawable& ocean, buffer<vec3>& ocean_positions, buffer<vec3>& current_normals, buffer<uint3> connectivity, float t, float tmax, perlin_noise p);



/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders , scene_structure& scene, gui_structure& gui)
{

    // Create fauna
    fauna.setup_bird();
    fauna.set_bird_keyframes();

    //shark
    fauna.set_shark();
    fauna.set_shark_keyframes();
    
    // Create and initialise terrain surface
    env.set_terrain();
    // Create and initialise ocean
    env.set_ocean();

    // Create treasure
    treasure.create_treasure_box();
    treasure.set_bridge();
    treasure.set_canoe();

    //create first palm tree
    objects.set_and_init_all(env);

    //setting lava in volcano
    lava.set_lava();
    lava.create_particule(shaders, scene);

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    // Setup gui
    gui.show_frame_camera = false;
    gui.show_frame_worldspace = false;


    // Setup skybox
    skybox.set_skybox();

    // Timer parameters
    timer.t_max = 120.0f;
    timer.scale = 1.0f;

}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    const float t = timer.t;

    //Update shaders
    update_shaders(t, shaders);


    set_gui();
    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
    glPolygonOffset(1.0, 1.0);



    //scene.gluPerspective(45.0f, (GLfloat)500 / (GLfloat)500, 0.5f, 3000000.0f);
    // Draw treasure
    //treasure.open_chest();
    treasure.draw_treasure(shaders, scene);
    treasure.draw_bridge(shaders, scene);
    treasure.draw_canoe(shaders, scene);


    /********************************/
    /*         DISPLAY FAUNA       */
    /******************************/

    if (gui_scene.fauna) {
        fauna.update_bird();
        fauna.update_shark();


        //INTERACTION WITH SHARK/BIRD
        if (fauna.bird_clicked)
        {
            scene.camera.translation = -fauna.bird["body"].transform.translation;
            scene.camera.orientation = fauna.bird["body"].transform.rotation * mat3(0, 0, -1, 1, 0, 0, 0, 1, 0);
        }
        else if (fauna.shark_clicked)
        {
            scene.camera.translation = -fauna.shark.uniform.transform.translation - vec3(0, 0, 3.0f);
            scene.camera.orientation = fauna.shark.uniform.transform.rotation * mat3(0, 0, 1, 0, 1, 0, 1, 0, 0);
        }

        fauna.draw_bird(shaders, scene);
        fauna.draw_shark(shaders, scene);
        if (gui_scene.keyframes) {
            fauna.draw_keyframes(shaders, scene, fauna.bird_keyframes, { 1,1,1 });
            fauna.draw_keyframes(shaders, scene, fauna.shark_keyframes, { 1,0,1 });
        }
    }
    /********************************/
    /*     DISPLAY ENV OBJECTS     */
    /******************************/
    if (gui_scene.trees) {
        objects.draw_all(shaders, scene);
    }


    /**********************************/
    //  Update and display ocean        //
    /*********************************/
    if (gui_scene.ocean) {
        env.update_ocean(env.ocean, env.ocean_positions, env.ocean_normals, env.ocean_connectivity, env.ocean_perlin);
        env.draw_ocean(shaders, scene);
    }


    /**********************************************************/
    /********************* DISPLAY TERRAIN ********************/
    /**********************************************************/
    if (gui_scene.terrain) {
        env.draw_terrain(shaders, scene);
    }

    /**************************************/
/*         DISPLAY SKYBOX            */
/*************************************/
    if (gui_scene.skybox) {
        skybox.draw_skybox(shaders, scene);
    }


    /**************************************/
    /*         DISPLAY PARTICLES          */
    /*************************************/
    if (gui_scene.particles) {
        lava.update_lava(lava.lava, lava.lava_positions, lava.lava_normals, lava.lava_connectivity, t, timer.t_max, lava.lava_perlin);
        lava.draw_lava(shaders, scene);
        lava.update_particles();
        lava.draw_particles(shaders, scene);

    }

    
    /******************************/
    /*      BILLBOARDS            */
    /******************************/
    
    if (gui_scene.billboards) {
        objects.draw_billboards(shaders, scene, objects.Identity3, objects.Rotation);
    }

 }

void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int x, int y, int z)
{
    if(gui_scene.fauna)
        fauna.mouse_click(scene, window, x, y, z);
    treasure.mouse_click(scene, window, x, y, z);
}

void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    if(gui_scene.fauna)
        fauna.mouse_move(scene, window);
}

void scene_model::update_shaders(float t, std::map<std::string, GLuint>& shaders)
{
    float ambiant;
    float diffuse;
    float time_normalized = t / 120.0;
    vec3 light = vec3(-800.0f * cos(3.14f / 60.0f * t), 300.0f, 200.0f * cos(3.14f / 60.0f * t));

    vec3 sun_color;
    if (time_normalized < 0.25) //sun rise, 6:00 -> 12:00
    {
        sun_color = vec3(0.9 - 4 * 0.1 * time_normalized, 0.7, 0.5 + 4 * 0.1 * time_normalized);
        ambiant = 0.5 + 4 * 0.4 * time_normalized;
        diffuse = 0.3 + 4 * 0.5 * time_normalized;

    }
    else if (time_normalized < 0.4) // 12:00 -> 15:30
    {
        sun_color = vec3(0.8, 0.7, 0.6);
        ambiant = 0.9;
        diffuse = 0.8;
    }
    else if (time_normalized < 0.6) // 15:30 -> 19h30
    {
        sun_color = vec3(0.8 - 5 * 0.1 * (time_normalized - 0.4), 0.7 - 5 * 0.2 * (time_normalized - 0.4), 0.6 - 5 * 0.1 * (time_normalized - 0.4));
        ambiant = 0.9 - 5 * 0.3 * (time_normalized - 0.4);
        diffuse = 0.8 - 5 * 0.2 * (time_normalized - 0.4);
    }
    else if (time_normalized < 0.75) // 19:30 -> 00:00
    {
        sun_color = vec3(0.7 - (1.0 / 0.15) * 0.5 * (time_normalized - 0.6), 0.5 - (1.0 / 0.15) * 0.3 * (time_normalized - 0.6), 0.5);
        ambiant = 0.6 - (1.0 / 0.15) * 0.2 * (time_normalized - 0.6);
        diffuse = 0.6 - (1.0 / 0.15) * 0.4 * (time_normalized - 0.6);

    }
    else if (time_normalized < 0.9) // 19:30 -> 3:30
    {
        sun_color = vec3(0.2, 0.2, 0.5);
        ambiant = 0.4;
        diffuse = 0.2;

    }

    else //3:30 -> 6:00
    {
        sun_color = vec3(0.2 + 10 * 0.7 * (time_normalized - 0.9), 0.2 + 10 * 0.5 * (time_normalized - 0.9), 0.5);
        ambiant = 0.4 + 10 * 0.1 * (time_normalized - 0.9);
        diffuse = 0.2 + 10 * 0.1 * (time_normalized - 0.9);
    }

    glUseProgram(shaders["terrain"]);
    uniform(shaders["terrain"], "light", light);
    uniform(shaders["terrain"], "sun_color", sun_color);
    uniform(shaders["terrain"], "ambiant_sun", ambiant);
    uniform(shaders["terrain"], "diffuse_sun", diffuse);

    glUseProgram(shaders["terrain1"]);
    uniform(shaders["terrain1"], "light", light);
    uniform(shaders["terrain1"], "sun_color", sun_color);
    uniform(shaders["terrain1"], "ambiant_sun", ambiant);
    uniform(shaders["terrain1"], "diffuse_sun", diffuse);

    glUseProgram(shaders["mesh_sun"]);
    uniform(shaders["mesh_sun"], "light", light);
    uniform(shaders["mesh_sun"], "sun_color", sun_color);
    uniform(shaders["mesh_sun"], "ambiant_sun", ambiant);
    uniform(shaders["mesh_sun"], "diffuse_sun", diffuse);




    /* FOR SKYBOX */
    vec3 color;
    if (time_normalized < 0.25)
    {
        color = vec3(0.9 - 4 * 0.1 * time_normalized, 0.7 + 4 * 0.1 * time_normalized, 0.6 + 4 * 0.2 * time_normalized);
        ambiant = 0.3 + 4 * 0.5 * time_normalized;

    }
    else if (time_normalized < 0.5)
    {
        color = vec3(0.8 - 4 * 0.1 * (time_normalized - 0.25), 0.8 - 4 * 0.2 * (time_normalized - 0.25), 0.8 - 4 * 0.2 * (time_normalized - 0.25));
        ambiant = 0.8 - 4 * 0.5 * (time_normalized - 0.25);
    }
    else if (time_normalized < 0.75)
    {
        color = vec3(0.7 - 4 * 0.5 * (time_normalized - 0.5), 0.6 - 4 * 0.4 * (time_normalized - 0.5), 0.6);
        ambiant = 0.3 - 4 * 0.2 * (time_normalized - 0.5);

    }
    else
    {
        color = vec3(0.2 + 4 * 0.7 * (time_normalized - 0.75), 0.2 + 4 * 0.5 * (time_normalized - 0.75), 0.6);
        ambiant = 0.1 + 4 * 0.2 * (time_normalized - 0.75);
    }

    glUseProgram(shaders["skybox"]);
    uniform(shaders["skybox"], "color_sun", color);
    uniform(shaders["skybox"], "ambiant_sun", ambiant);

}

void scene_model::set_gui()
{
    ImGui::Checkbox("Terrain", &gui_scene.terrain);
    ImGui::Checkbox("Ocean", &gui_scene.ocean);
    ImGui::Checkbox("Trees", &gui_scene.trees);
    ImGui::Checkbox("Skybox", &gui_scene.skybox);
    ImGui::Checkbox("Billboards", &gui_scene.billboards);
    ImGui::Checkbox("Particles", &gui_scene.particles);
    ImGui::Checkbox("Fauna", &gui_scene.fauna);
    ImGui::Checkbox("Keyframes", &gui_scene.keyframes);


    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.0f, 5.0f);


}



#endif

