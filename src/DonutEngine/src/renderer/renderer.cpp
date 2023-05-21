#include "pch.h"
#include "renderer/renderer.h"

namespace Donut
{
    void Renderer::beginScene()
    {

    }

    void Renderer::endScene()
    {

    }

    void Renderer::submit(const std::shared_ptr<VertexArray>& va)
    {
        va->bind();
        RenderCommand::drawIndices(va);
    }
}