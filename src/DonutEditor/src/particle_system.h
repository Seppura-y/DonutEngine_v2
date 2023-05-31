#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "donut.h"

struct ParticleProps
{
	glm::vec2 position_;
	glm::vec2 velocity_, velocity_variation_;
	glm::vec4 color_begin_, color_end_;
	float size_begin_, size_end_, size_variation_;
	float lifetime_ = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t particle_count);

	void onUpdate(Donut::Timestep ts);
	void onRender(Donut::OrthographicCamera& camera);

	void emit(const ParticleProps& particleProps);
private:
	struct Particle
	{
		glm::vec2 position_;
		glm::vec2 velocity_;
		glm::vec4 color_begin_, color_end_;
		float rotation_ = 0.0f;
		float size_begin_, size_end_;

		float lifetime_ = 1.0f;
		float life_remaining_ = 0.0f;

		bool is_active_ = false;
	};
	std::vector<Particle> particle_pool_;
	uint32_t pool_index_;
};

#endif