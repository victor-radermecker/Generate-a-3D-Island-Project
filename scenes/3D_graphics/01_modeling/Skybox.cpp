#include "Skybox.hpp"

using namespace vcl;


/*************************************/
//   Create skybox quadrangles         //
/***************************************/
mesh create_skybox_back(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { {-size,-size, size}, { -size,-size,-size}, { -size, size,-size}, {-size, size, size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_bottom(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { { size, size,-size}, {-size, size,-size},  {-size,-size,-size}, { size,-size,-size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}


mesh create_skybox_front(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { {size,size,size}, { size,size,-size}, { size, -size,-size}, {size, -size, size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}

mesh create_skybox_left(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { {-size,size, size}, {-size, size, -size} , { size, size,-size},  { size,size,size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}


mesh create_skybox_right(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { {size,-size, size}, { size,-size,-size}, { -size, -size,-size}, {-size, -size, size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}


mesh create_skybox_top(float size)
{
    mesh bottom_cpu;
    bottom_cpu.position = { { -size, size, size}, {size, size, size}, {size,-size, size}, { -size,-size, size} };
    bottom_cpu.texture_uv = { {0,0}, {0,1}, {1,1}, {1,0} };
    bottom_cpu.connectivity = { {0,1,2}, {0,2,3} };

    return bottom_cpu;
}





void skybox_model::set_skybox()
{
    float size = 550;
    skybox.back = create_skybox_back(size);
    skybox.bottom = create_skybox_bottom(size);
    skybox.front = create_skybox_front(size);
    skybox.left = create_skybox_left(size);
    skybox.right = create_skybox_right(size);
    skybox.top = create_skybox_top(size);

    skybox_ids.back = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/back.png"));
    skybox_ids.bottom = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/bottom.png"));
    skybox_ids.front = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/front.png"));
    skybox_ids.left = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/left.png"));
    skybox_ids.right = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/right.png"));
    skybox_ids.top = create_texture_gpu(image_load_png("scenes/3D_graphics/02_texture/assets/skybox/top.png"));
}



void skybox_model::draw_skybox(std::map<std::string, GLuint>& shaders, scene_structure& scene)
{
    glDepthMask(GL_FALSE);
    vec3 camPos = scene.camera.camera_position();
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.back);
    skybox.back.uniform.transform.translation = camPos;
    draw(skybox.back, scene.camera, shaders["skybox"]);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.bottom);
    skybox.bottom.uniform.transform.translation = camPos;
    draw(skybox.bottom, scene.camera, shaders["skybox"]);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.front);
    skybox.front.uniform.transform.translation = camPos;
    draw(skybox.front, scene.camera, shaders["skybox"]);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.left);
    skybox.left.uniform.transform.translation = camPos;
    draw(skybox.left, scene.camera, shaders["skybox"]);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.right);
    skybox.right.uniform.transform.translation = camPos;
    draw(skybox.right, scene.camera, shaders["skybox"]);

    glBindTexture(GL_TEXTURE_2D, skybox_ids.top);
    skybox.top.uniform.transform.translation = camPos;
    draw(skybox.top, scene.camera, shaders["skybox"]);
    glDepthMask(GL_TRUE);
}
