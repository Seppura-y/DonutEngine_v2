#include "pch.h"
#include "renderer/renderer.h"

namespace Donut
{
    Renderer::SceneData* Renderer::scene_data_ = new SceneData();
    void Renderer::beginScene(OrthographicCamera& camera)
    {
        scene_data_->view_projection_matrix_ = camera.getViewProjectionMatrix();
    }

    void Renderer::endScene()
    {

    }

    void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& va)
    {
        shader->bind();
        shader->uploadUniformMat4v("u_viewProjectionMatrix", scene_data_->view_projection_matrix_);

        va->bind();
        RenderCommand::drawIndices(va);
    }

    void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& va, const glm::mat4& transform)
    {
        shader->bind();
        shader->uploadUniformMat4v("u_viewProjectionMatrix", scene_data_->view_projection_matrix_);
        shader->uploadUniformMat4v("u_transformMatrix", transform);
        va->bind();
        RenderCommand::drawIndices(va);
    }
}