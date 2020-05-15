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
    veg.set_palm_tree();
    veg.init_forest(10, veg.tree_position, 4.0f, 0.05f, "volcano");


    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 10.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);

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

    /**********************************/
    //        Display ocean           //
    /*********************************/
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, env.ocean_texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    env.update_ocean(env.ocean, env.ocean_positions, env.ocean_normals, env.ocean_connectivity, t, timer.t_max, env.ocean_perlin);
    draw(env.ocean, scene.camera, shaders["mesh_sun"]);
    draw(veg.palm_tree, scene.camera, shaders["terrain1"]);

    for (int i = 0; i < veg.tree_position.size();i++) {
        vec3 pos = veg.tree_position[i];
        veg.palm_tree.uniform.transform.translation = pos;
        draw(veg.palm_tree, scene.camera, shaders["terrain1"]);
    }


    /**********************************************************/
    /********************* DISPLAY TERRAIN ********************/
    /**********************************************************/
    
    glUseProgram(shaders["terrain"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, env.texture_ids.sand_id);
    uniform(shaders["terrain"], "sand_sampler", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, env.texture_ids.grass_id);
    uniform(shaders["terrain"], "grass_sampler", 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, env.texture_ids.rock_id);
    uniform(shaders["terrain"], "rock_sampler", 2); 
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


    draw(env.terrain[0], scene.camera, shaders["terrain"]);


    glUseProgram(shaders["terrain1"]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, env.texture_ids.sand_id);
    uniform(shaders["terrain1"], "sand_sampler", 0); opengl_debug();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, env.texture_ids.grass_id);
    uniform(shaders["terrain1"], "grass_sampler", 1); opengl_debug();

<<<<<<< HEAD
=======
    
    draw(env.terrain[1], scene.camera, shaders["terrain1"]);
>>>>>>> b84978c574fe675c42ed9af5b9f579922e30cbbf
    
    draw(env.terrain[1], scene.camera, shaders["terrain1"]);

    /**************************************/
    /*         DISPLAY SKYBOX            */
    /*************************************/

    skybox.draw_skybox(shaders, scene);
    
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe);
    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}



#endif

