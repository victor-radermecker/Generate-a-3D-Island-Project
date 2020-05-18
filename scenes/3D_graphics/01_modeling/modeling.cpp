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
void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& )
{
    // Create and initialise terrain surface
    env.set_terrain();
    // Create and initialise ocean
    env.set_ocean();

    //create first palm tree
    objects.set_and_init_all(env);

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
    /*     DISPLAY ENV OBJECTS     */
    /******************************/
    objects.draw_all(shaders, scene);


    /**********************************/
    //  Update and display ocean        //
    /*********************************/
    env.update_ocean(env.ocean, env.ocean_positions, env.ocean_normals, env.ocean_connectivity, t, timer.t_max, env.ocean_perlin);
    env.draw_ocean(shaders, scene);


    /**********************************************************/
    /********************* DISPLAY TERRAIN ********************/
    /**********************************************************/
    
    env.draw_terrain(shaders, scene);

    /**************************************/
    /*         DISPLAY SKYBOX            */
    /*************************************/

    skybox.draw_skybox(shaders, scene);
    
    /******************************/
    /*      BILLBOARDS            */
    /******************************/

    objects.draw_billboards(shaders, scene, objects.Identity3, objects.Rotation);
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}



#endif

