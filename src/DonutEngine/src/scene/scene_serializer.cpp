#include "pch.h"
#include "scene_serializer.h"

#include "entity.h"
#include "components.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}


namespace Donut
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void serializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "12323443545";

		if (entity.hasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.getComponent<TagComponent>().tag_;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& tc = entity.getComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.translation_;
			out << YAML::Key << "Rotation" << YAML::Value << tc.rotation_;
			out << YAML::Key << "Scale" << YAML::Value << tc.scale_;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.hasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.getComponent<CameraComponent>();
			auto& camera = cameraComponent.camera_;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.getProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.getPerspectiveVerticalFov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.getOrthographicFarClip();
			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.is_primary_;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.is_fixed_aspect_ratio_;

			out << YAML::EndMap; // CameraComponent
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color_;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		out << YAML::EndMap;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: scene_(scene)
	{

	}

	void SceneSerializer::serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		scene_->registry_.each([&](auto entity_id)
		{
			Entity entity = { entity_id, scene_.get() };
			if (!entity)
			{
				return;
			}
			serializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::serializeRuntime(const std::string& filepath)
	{
		// Not implemented
		DN_CORE_ASSERT(false,"");
	}

	bool SceneSerializer::deserialize(const std::string& filepath)
	{
		std::ifstream ifs(filepath);
		std::stringstream str_stream;
		str_stream << ifs.rdbuf();

		YAML::Node data = YAML::Load(str_stream.str());
		if (!data["Scene"])
		{
			return false;
		}

		std::string scene_name = data["Scene"].as<std::string>();
		DN_CORE_TRACE("Deserializing scene '{0}'", scene_name);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
				{
					name = tag_component["Tag"].as<std::string>();
				}

				DN_CORE_TRACE("Deserializing entity with ID = {0}, name = {1}", uuid, name);

				Entity deserialized_entity = scene_->createEntity(name);
				
				auto transform_component = entity["TransformComponent"];
				if (transform_component)
				{
					auto& tc = deserialized_entity.getComponent<TransformComponent>();

					tc.translation_ = transform_component["Translation"].as<glm::vec3>();
					tc.rotation_ = transform_component["Rotation"].as<glm::vec3>();
					tc.scale_ = transform_component["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserialized_entity.addComponent<CameraComponent>();

					auto& cameraProps = cameraComponent["Camera"];
					cc.camera_.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera_.setPerspectiveVerticalFov(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera_.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera_.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera_.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera_.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera_.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.is_primary_ = cameraComponent["Primary"].as<bool>();
					cc.is_fixed_aspect_ratio_ = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserialized_entity.addComponent<SpriteRendererComponent>();
					src.color_ = spriteRendererComponent["Color"].as<glm::vec4>();
				}
			}
		}
		return true;
	}

	bool SceneSerializer::deserializeRuntime(const std::string& filepath)
	{
		DN_CORE_ASSERT(false, "");
		return false;
	}
}