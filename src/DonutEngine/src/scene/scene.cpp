#include "pch.h"
#include "scene.h"
#include "components.h"
#include "scriptable_entity.h"

#include "renderer/renderer_2d.h"

#include "renderer/editor_camera.h"

#include "scene/entity.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

#include <glm/glm.hpp>

namespace Donut
{
	static b2BodyType rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType body_type)
	{
		switch (body_type)
		{
			case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		DN_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	template<typename Component>
	static void copyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& entt_map)
	{
		auto view = src.view<Component>();
		for (auto entity : view)
		{
			UUID uuid = src.get<IDComponent>(entity).id_;
			DN_CORE_ASSERT(entt_map.find(uuid) != entt_map.end(), "");
			entt::entity dst_entt = entt_map.at(uuid);

			auto& component = src.get<Component>(entity);
			dst.emplace_or_replace<Component>(dst_entt, component);
		}
	}

	template<typename Component>
	static void copyComponentIfExists(Entity dst, Entity src)
	{
		if (src.hasComponent<Component>())
		{
			dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
		}
	}

	Scene::Scene()
	{
#if ENTT_TEST_CODE
		struct MeshComponent
		{
			//MeshComponent() = default;
			//MeshComponent(const MeshComponent&) = default;

			int a;
		};


		entt::entity entity = registry_.create();
		registry_.emplace<TransformComponent>(entity, glm::mat4(1.0f));
		
		// 这个entity中是否存在TransformComponent
		if (registry_.all_of<TransformComponent>(entity))
		{
			TransformComponent& transform = registry_.get<TransformComponent>(entity);
		}

		// 获取所有存在TransformComponent的entity
		auto view = registry_.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = registry_.get<TransformComponent>(entity);
		}

		// 
		auto group = registry_.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[trans, mesh] = group.get<TransformComponent, MeshComponent>(entity);
		}
#endif
	}

	Scene::~Scene()
	{

	}

	void Scene::onUpdateRuntime(Timestep ts)
	{
		// Update scripts
		{
			registry_.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
				{
					if (!nsc.instance_)
					{
						nsc.instance_ = nsc.instantiateScript();
						nsc.instance_->entity_ = Entity{ entity, this };
						nsc.instance_->onCreate();
					}

					nsc.instance_->onUpdate(ts);
				});
		}

		// Update physics
		{
			const int32_t velocity_iterations = 6;
			const int32_t position_iterations = 2;
			physics_world_->Step(ts, velocity_iterations, position_iterations);

			auto view = registry_.view<Rigidbody2DComponent>();
			for (auto it : view)
			{
				Entity entity = { it, this };
				auto& transform = entity.getComponent<TransformComponent>();
				auto& rigidbody_2d = entity.getComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rigidbody_2d.runtime_body_;
				const auto& position = body->GetPosition();
				transform.translation_.x = position.x;
				transform.translation_.y = position.y;
				transform.rotation_.z = body->GetAngle();
			}
		}

		// Update Renderer
		Camera* main_camera = nullptr;
		glm::mat4 camera_transform;
		{
			auto group = registry_.view<TransformComponent, CameraComponent>();
			for (auto entity : group)
			{
				auto [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);

				if (camera.is_primary_)
				{
					main_camera = &camera.camera_;
					camera_transform = transform.getTransform();
					break;
				}
			}
		}

		if (main_camera)
		{
			Renderer2D::beginScene(main_camera->getProjection(), camera_transform);

			auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				// return a tuple that consturct with the references value, and the tuple return as value
				// so we don't need to use '&' to receive the return value.
				// because the API already did that.
				//auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				//Renderer2D::drawRectangle(transform.getTransform(), sprite.color_);
				Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::beginScene(camera);

		auto group = registry_.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			// return a tuple that consturct with the references value, and the tuple return as value
			// so we don't need to use '&' to receive the return value.
			// because the API already did that.
			//auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			//Renderer2D::drawRectangle(transform.getTransform(), sprite.color_);
			Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
		}

		Renderer2D::endScene();
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		viewport_width_ = width;
		viewport_height_ = height;

		// resize our non fixed aspect ratio camera
		auto view = registry_.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& camera_component = view.get<CameraComponent>(entity);
			if (!camera_component.is_fixed_aspect_ratio_)
			{
				camera_component.camera_.setViewportSize(width, height);
			}
		}
	}

	Entity& Scene::createEntity(const std::string& tag)
	{
		return createEntityWithUUID(UUID(), tag);
	}

	Entity& Scene::createEntityWithUUID(UUID id, const std::string& tag)
	{
		Entity entity = { registry_.create(), this };
		entity.addComponent<IDComponent>(id);
		entity.addComponent<TransformComponent>();

		auto& t = entity.addComponent<TagComponent>(tag);
		t.tag_ = tag.empty() ? "entity" : tag;
		return entity;
	}

	entt::entity Scene::createEntity()
	{
		return registry_.create();
	}

	void Scene::destroyEntity(Entity entity)
	{
		registry_.destroy(entity);
	}

	Entity Scene::getPrimaryCameraEntity()
	{
		auto view = registry_.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.is_primary_)
			{
				return Entity{ entity, this };
			}
		}

		return {};
	}

	void Scene::onRuntimeStart()
	{
		physics_world_ = new b2World({ 0.0f, -9.8f });

		auto view = registry_.view<Rigidbody2DComponent>();
		for (auto it : view)
		{
			Entity entity = { it, this };
			auto& transform = entity.getComponent<TransformComponent>();
			auto& rigidbody_2d = entity.getComponent<Rigidbody2DComponent>();

			b2BodyDef body_def;
			body_def.type = rigidbody2DTypeToBox2DBody(rigidbody_2d.type_);
			body_def.position.Set(transform.translation_.x, transform.translation_.y);
			body_def.angle = transform.rotation_.z;

			b2Body* body = physics_world_->CreateBody(&body_def);
			body->SetFixedRotation(rigidbody_2d.fixed_rotation_);
			rigidbody_2d.runtime_body_ = body;
			if (entity.hasComponent<BoxCollider2DComponent>())
			{
				auto& box_collider_2d = entity.getComponent<BoxCollider2DComponent>();

				b2PolygonShape box_shape;
				box_shape.SetAsBox(box_collider_2d.size_.x * transform.scale_.x, box_collider_2d.size_.y * transform.scale_.y);

				b2FixtureDef fixture_def;
				fixture_def.shape = &box_shape;
				fixture_def.density = box_collider_2d.density_;
				fixture_def.friction = box_collider_2d.friction_;
				fixture_def.restitution = box_collider_2d.restitution_;
				fixture_def.restitutionThreshold = box_collider_2d.restitution_threshold_;
				body->CreateFixture(&fixture_def);
			}
		}
	}

	void Scene::onRuntimeStop()
	{
		delete physics_world_;
		physics_world_ = nullptr;
	}

	Ref<Scene> Scene::copyScene(Ref<Scene> other)
	{
		Ref<Scene> new_scene = createRef<Scene>();

		new_scene->viewport_width_ = other->viewport_width_;
		new_scene->viewport_height_ = other->viewport_height_;

		auto& src_scene_registry = other->registry_;
		auto& dst_scene_registry = new_scene->registry_;
		std::unordered_map<UUID, entt::entity> entt_map;

		// create entities in new scene
		auto id_view = src_scene_registry.view<IDComponent>();
		for (auto entity : id_view)
		{
			UUID uuid = src_scene_registry.get<IDComponent>(entity).id_;
			const auto& name = src_scene_registry.get<TagComponent>(entity).tag_;
			Entity new_entity = new_scene->createEntityWithUUID(uuid, name);
			entt_map[uuid] = (entt::entity)new_entity;
		}

		// copy components
		copyComponent<TransformComponent>(dst_scene_registry, src_scene_registry, entt_map);
		copyComponent<SpriteRendererComponent>(dst_scene_registry, src_scene_registry, entt_map);
		copyComponent<CameraComponent>(dst_scene_registry, src_scene_registry, entt_map);
		copyComponent<NativeScriptComponent>(dst_scene_registry, src_scene_registry, entt_map);
		copyComponent<Rigidbody2DComponent>(dst_scene_registry, src_scene_registry, entt_map);
		copyComponent<BoxCollider2DComponent>(dst_scene_registry, src_scene_registry, entt_map);
		
		return new_scene;
	}

	void Scene::duplicateEntity(Entity entity)
	{
		std::string name = entity.getName();
		Entity new_entity = createEntity(name);

		copyComponentIfExists<TransformComponent>(new_entity, entity);
		copyComponentIfExists<SpriteRendererComponent>(new_entity, entity);
		copyComponentIfExists<NativeScriptComponent>(new_entity, entity);
		copyComponentIfExists<CameraComponent>(new_entity, entity);
		copyComponentIfExists<Rigidbody2DComponent>(new_entity, entity);
		copyComponentIfExists<BoxCollider2DComponent>(new_entity, entity);
	}

	template<typename T>
	void Scene::onComponentAdded(Entity entity, T& component)
	{
		// you never suppose to not have a specializaion for this function
		static_assert(false);
	}

	template<>
	void Scene::onComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		//component.camera_.setViewportSize(viewport_width_, viewport_height_);
		if (viewport_width_ > 0 && viewport_height_ > 0)
		{
			component.camera_.setViewportSize(viewport_width_, viewport_height_);
		}
	}

	template<>
	void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

}