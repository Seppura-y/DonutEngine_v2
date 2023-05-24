#include "pch.h"
#include "renderer/renderer.h"
#include "platform/opengl/opengl_shader.h"

namespace Donut
{
    Renderer::SceneData* Renderer::scene_data_ = new SceneData();

    void Renderer::init()
    {
        RenderCommand::init();
    }

    void Renderer::beginScene(OrthographicCamera& camera)
    {
        scene_data_->view_projection_matrix_ = camera.getViewProjectionMatrix();
    }

    void Renderer::endScene()
    {

    }

    void Renderer::submit(const Donut::Ref<Shader>& shader, const Donut::Ref<VertexArray>& va)
    {
        shader->bind();
        std::dynamic_pointer_cast<Donut::OpenGLShader>(shader)->uploadUniformMat4fv("u_viewProjectionMatrix", scene_data_->view_projection_matrix_);

        va->bind();
        RenderCommand::drawIndices(va);
    }

    void Renderer::submit(const Donut::Ref<Shader>& shader, const Donut::Ref<VertexArray>& va, const glm::mat4& transform)
    {
        shader->bind();
        std::dynamic_pointer_cast<Donut::OpenGLShader>(shader)->uploadUniformMat4fv("u_viewProjectionMatrix", scene_data_->view_projection_matrix_);
        std::dynamic_pointer_cast<Donut::OpenGLShader>(shader)->uploadUniformMat4fv("u_transformMatrix", transform);
        va->bind();
        RenderCommand::drawIndices(va);
    }
}