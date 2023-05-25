#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Donut::Layer("sandbox 2d"), camera_controller_(1600.0f / 900.0f)
{

}


void Sandbox2D::onAttach()
{
	rectangle_va_ = Donut::VertexArray::create();

	float rect_vertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Donut::Ref<Donut::VertexBuffer> rectangle_vb;
	rectangle_vb.reset(Donut::VertexBuffer::create(rect_vertices, sizeof(rect_vertices)));
	rectangle_vb->setLayout({
			{ Donut::ShaderDataType::Float3, "a_Position" }
		});
	rectangle_va_->addVertexBuffer(rectangle_vb);

	uint32_t rectangle_indices[6] = { 0, 1, 2, 2, 3, 0 };
	Donut::Ref<Donut::IndexBuffer> rectangle_ib;
	rectangle_ib.reset(Donut::IndexBuffer::create(rectangle_indices, sizeof(rectangle_indices) / sizeof(uint32_t)));
	rectangle_va_->setIndexBuffer(rectangle_ib);

	rectangle_shader_ = Donut::Shader::createShader("assets/shaders/flatcolor.glsl");
}

void Sandbox2D::onDetach()
{

}

void Sandbox2D::onUpdate(Donut::Timestep ts)
{
	camera_controller_.onUpdate(ts);

	Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Donut::RenderCommand::clear();

	Donut::Renderer::beginScene(camera_controller_.getCamera());

	std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->bind();
	std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_color", rectangle_color_);

	Donut::Renderer::submit(rectangle_shader_, rectangle_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Donut::Renderer::endScene();
}

void Sandbox2D::onEvent(Donut::Event& ev)
{
	camera_controller_.onEvent(ev);
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(rectangle_color_));
	ImGui::End();
}
