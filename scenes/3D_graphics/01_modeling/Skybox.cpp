#include "Skybox.hpp"
#include <algorithm>
#include <iostream> 
#include <stdlib.h>    
#include <ctime>

using namespace vcl;


/*************************************/
//   Create skybox quadrangles         //
/***************************************/
mesh create_skybox_back()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {-100,-100, 100}, { -100,-100,-100}, { -100, 100,-100}, {-100, 100, 100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_bottom()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {-100,-100,-100}, { 100,-100,-100}, { 100, 100,-100}, {-100, 100,-100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}


mesh create_skybox_front()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {100,100,100}, { 100,100,-100}, { 100, -100,-100}, {100, -100, 100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_left()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {100,-100, 100}, { 100,-100,-100}, { -100, -100,-100}, {-100, -100, 100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_right()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {-100,100, 100}, { 100,100,100}, { 100, 100,-100}, {-100, 100, -100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_top()
{
    mesh bottom_cpu;
    bottom_cpu.position = { {100,-100, 100}, { -100,-100, 100}, { -100, 100, 100}, {100, 100, 100} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}






mesh skybox_model::create_skybox()
{
    mesh skybox_cpu;
    return skybox_cpu;
}



void skybox_model::set_skybox()
{
    skybox = create_skybox();
    //ocean_texture_id = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/ocean_texture.png"));
    //ocean.uniform.color = { 0.2f,0.5f,0.9f };
    //ocean.uniform.transform.translation = { 0,0,1.0f };

    // Illumination parameters
    //ocean.uniform.shading.ambiant = 1.0f;
    //ocean.uniform.shading.diffuse = 0.3f;
    //ocean.uniform.shading.specular = 0.8f;
    //ocean.uniform.shading.specular_exponent = 512;


    //ocean_perlin.height = 0.05f;
    //ocean_perlin.octave = 10;
    //ocean_perlin.persistency = 0.55f;
    //ocean_perlin.scaling = 2.0f;
}




