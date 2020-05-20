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
void scene_model::setup_data(std::map<std::string,GLuint>& shaders , scene_structure& scene, gui_structure& )
{
    // Create and initialise terrain surface
    env.set_terrain();
    // Create and initialise ocean
    env.set_ocean();

    //create first palm tree
    objects.set_and_init_all(env);

    //setting lava in volcano
    lava.set_lava();
    lava.create_particule(shaders, scene);

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

    // Setup skybox
    skybox.set_skybox();

    // Timer parameters
    timer.t_max = 10.0f;
    timer.scale = 1.0f;
}



/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    const float t = timer.t;
    set_gui();
    glEnable( GL_POLYGON_OFFSET_FILL ); // avoids z-fighting when displaying wireframe
    glPolygonOffset(1.0, 1.0);



    /********************************/
    /*         DISPLAY FAUNA       */
    /******************************/
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
        env.update_ocean(env.ocean, env.ocean_positions, env.ocean_normals, env.ocean_connectivity, t, timer.t_max, env.ocean_perlin);
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
        //lava.update_particles();
        //lava.draw_particles(shaders, scene);
        //lava.update_lava(lava.lava, lava.lava_positions, lava.lava_normals, lava.lava_connectivity, t, timer.t_max, lava.lava_perlin);
        //lava.draw_lava(shaders, scene);

    }

    
    /******************************/
    /*      BILLBOARDS            */
    /******************************/
    
    if (gui_scene.billboards) {
        objects.draw_billboards(shaders, scene, objects.Identity3, objects.Rotation);
    }
    
 }
/*
void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int x, int y, int z)
{
    fauna.mouse_click(scene, window, x, y, z);
}

void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    fauna.mouse_move(scene, window);
}*/



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
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

    /*
    if (ImGui::Button("Print Keyframe"))
    {
        std::cout << "keyframe_position={";
        for (size_t k = 0; k < fauna.keyframes.size(); ++k)
        {
            const vec3& p = fauna.keyframes[k].p;
            std::cout << "{" << p.x << "f," << p.y << "f," << p.z << "f}";
            if (k < fauna.keyframes.size() - 1)
                std::cout << ", ";
        }
        std::cout << "}" << std::endl;
    }*/

}



#endif

