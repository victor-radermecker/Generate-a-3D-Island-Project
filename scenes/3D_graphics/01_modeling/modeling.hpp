#pragma once

#include "main/scene_base/base.hpp"
#include "Environment.hpp"
#include "vegetation.hpp"

#ifdef SCENE_3D_GRAPHICS

// Stores some parameters that can be set from the GUI
struct gui_scene_structure
{
    bool wireframe;
};
/*
struct perlin_noise
{
    float scaling;
    int octave;
    float persistency;
    float height;
};*/

struct scene_model : scene_base
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);

    void set_gui();

    terrain_model env;
    vegetation_model veg;


    vcl::timer_interval timer;
    gui_scene_structure gui_scene;
};

#endif


