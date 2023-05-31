#include "pch.h"
#include "particle_system.h"


#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <random>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

ParticleSystem::ParticleSystem(uint32_t particle_count) : pool_index_(particle_count - 1)
{
	particle_pool_.resize(particle_count);
}

void ParticleSystem::onUpdate(Donut::Timestep ts)
{
	for (auto& particle : particle_pool_)
	{
		if (!particle.is_active_)
			continue;

		if (particle.life_remaining_ <= 0.0f)
		{
			particle.is_active_ = false;
			continue;
		}

		particle.life_remaining_ -= ts;
		particle.position_ += particle.velocity_ * (float)ts;
		particle.rotation_ += 100.0f * ts;
	}
}

void ParticleSystem::onRender(Donut::OrthographicCamera& camera)
{
	Donut::Renderer2D::beginScene(camera);
	for (auto& particle : particle_pool_)
	{
		if (!particle.is_active_)
			continue;

		// Fade away particles
		float life = particle.life_remaining_ / particle.lifetime_;
		glm::vec4 color = glm::lerp(particle.color_end_, particle.color_begin_, life);
		//color.a = color.a * life;

		float size = glm::lerp(particle.size_end_, particle.size_begin_, life);

		glm::vec3 position_ = { particle.position_.x,particle.position_.y, 0.2f };

		Donut::Renderer2D::drawRotatedRectangle(position_, glm::vec2{ size, size }, particle.rotation_, color);
	}
	Donut::Renderer2D::endScene();
}

void ParticleSystem::emit(const ParticleProps& particle_props)
{
	Particle& particle = particle_pool_[pool_index_];
	particle.is_active_ = true;
	particle.position_ = particle_props.position_;
	particle.rotation_ = Random::Float() * 2.0f * glm::pi<float>();

	// velocity_
	particle.velocity_ = particle_props.velocity_;
	particle.velocity_.x += particle_props.velocity_variation_.x * (Random::Float() - 0.5f);
	particle.velocity_.y += particle_props.velocity_variation_.y * (Random::Float() - 0.5f);

	// Color
	particle.color_begin_ = particle_props.color_begin_;
	particle.color_end_ = particle_props.color_end_;

	particle.lifetime_ = particle_props.lifetime_;
	particle.life_remaining_ = particle_props.lifetime_;
	particle.size_begin_ = particle_props.size_begin_ + particle_props.size_variation_ * (Random::Float() - 0.5f);
	particle.size_end_ = particle_props.size_end_;

	pool_index_ = --pool_index_ % particle_pool_.size();
}