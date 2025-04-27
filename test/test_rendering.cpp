#include <gtest/gtest.h>

#include "rendering/context_manager.h"
#include "rendering/model.h"
#include "rendering/shader_program.h"
#include "rendering/texture.h"

TEST(RenderingTest, Texture)
{
    auto context_manager = rendering::ContextManager("Headless context", 1200, 900, true);

    auto texture = rendering::Texture("A-Wing_Diff.png", rendering::Texture::Type::TEXTURE);
    ASSERT_NE(texture.texture_id, 0);
    ASSERT_EQ(texture.width, 2048);
    ASSERT_EQ(texture.height, 2048);

    auto cubemap_texture =
        rendering::Texture("skybox/lightblue/512", rendering::Texture::Type::CUBEMAP);
    ASSERT_NE(cubemap_texture.texture_id, 0);
    ASSERT_EQ(cubemap_texture.width, 512);
    ASSERT_EQ(cubemap_texture.height, 512);

    auto texture_array = rendering::Texture("sprites/storm_trooper_walking_se",
                                            rendering::Texture::Type::TEXTURE_ARRAY);
    ASSERT_NE(texture_array.texture_id, 0);
    ASSERT_EQ(texture_array.width, 512);
    ASSERT_EQ(texture_array.height, 512);
    ASSERT_EQ(texture_array.num_layers, 40);

    auto texture2 = std::move(texture);
    auto cubemap_texture2 = std::move(cubemap_texture);
    auto texture_array2 = std::move(texture_array);
}

TEST(RenderingTest, Model)
{
    auto context_manager = rendering::ContextManager("Headless context", 1200, 900, true);

    auto model = rendering::Model("awing.obj", rendering::AutoLoadTexture);
    ASSERT_EQ(model.meshes.size(), 2);
    ASSERT_NE(model.meshes[0].vao, 0);
    ASSERT_NE(model.meshes[0].texture, nullptr);
    ASSERT_EQ(model.meshes[0].diffuse_texname, "A-Wing_Diff.png");
    ASSERT_NE(model.meshes[1].vao, 0);
    ASSERT_EQ(model.meshes[1].texture, nullptr);

    auto model_no_texture = rendering::Model("awing.obj", rendering::DontLoadTexture);
    ASSERT_EQ(model_no_texture.meshes.size(), 2);
    ASSERT_NE(model_no_texture.meshes[0].vao, 0);
    ASSERT_EQ(model_no_texture.meshes[0].texture, nullptr);
    ASSERT_NE(model_no_texture.meshes[1].vao, 0);
    ASSERT_EQ(model_no_texture.meshes[1].texture, nullptr);

    auto model2 = std::move(model);
}

TEST(RenderingTest, ShaderProgram)
{
    auto context_manager = rendering::ContextManager("Headless context", 1200, 900, true);

    auto program = rendering::ShaderProgram("model", "model.vert", "model.frag");
    ASSERT_NE(program.program_id, 0);

    auto program2 = std::move(program);
}
