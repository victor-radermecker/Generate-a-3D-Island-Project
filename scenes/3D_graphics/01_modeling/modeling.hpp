#pragma once

#include "main/scene_base/base.hpp"
#include "Environment.hpp"
#include "Environment_Object.hpp"
#include "Skybox.hpp"
#include "Lava.hpp"
#include "Fauna.hpp"

#ifdef SCENE_3D_GRAPHICS

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe = false;
    bool ocean = false;
    bool trees = false;
    bool skybox = false;
    bool billboards = false;
    bool terrain = true;
    bool particles = true;
    bool fauna = true;
    bool keyframes = false;
};


struct scene_model : scene_base
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();

    // Ocean + terrain
    terrain_model env;
    object_model objects;
    lava_model lava;
    skybox_model skybox;
    fauna_model fauna;




    vcl::timer_interval timer;
    gui_scene_structure gui_scene;
    gui_scene_structure ocean;


    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    void mouse_move(scene_structure& scene, GLFWwindow* window);


};

#endif


