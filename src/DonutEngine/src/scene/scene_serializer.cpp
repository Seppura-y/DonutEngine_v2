#include "pch.h"
#include "scene_serializer.h"

#include "entity.h"
#include "components.h"

#include "scripting/script_engine.h"
#include "uuid.h"

#include "project/project.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

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

	template<>
	struct convert<Donut::UUID>
	{
		static Node encode(const Donut::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Donut::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};

}


namespace Donut
{
#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << script_field.getValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = script_field["Data"].as<Type>();    \
		field_instance.setValue(data);                  \
		break;                                         \
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

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

	static std::string rigidbody2DComponentTypeToString(Rigidbody2DComponent::BodyType body_type)
	{
		switch (body_type)
		{
		case Rigidbody2DComponent::BodyType::Static: return "Static";
		case Rigidbody2DComponent::BodyType::Dynamic: return "Dynamic";
		case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		DN_CORE_ASSERT(false, "Unknown body type");
		return {};
	}

	static Rigidbody2DComponent::BodyType rigidbody2DComponentTypeFromString(const std::string& body_type_str)
	{
		if (body_type_str == "Static") return Rigidbody2DComponent::BodyType::Static;
		if (body_type_str == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
		if (body_type_str == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		DN_CORE_ASSERT(false, "Unknown body type");
		return Rigidbody2DComponent::BodyType::Static;
	}

	static void serializeEntity(YAML::Emitter& out, Entity entity)
	{
		DN_CORE_ASSERT(entity.hasComponent<IDComponent>(), "Error at serializeEntity : entity does not have IDComponent");
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.getUUID();

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

		if (entity.hasComponent<ScriptComponent>())
		{
			auto& script_component = entity.getComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "ClassName" << YAML::Value << script_component.class_name_;

			// Fields
			Ref<ScriptClass> entity_class = ScriptEngine::getEntityClass(script_component.class_name_);
			if (entity_class)
			{
				const auto& fields = entity_class->getFields();
				if (fields.size() > 0)
				{
					out << YAML::Key << "ScriptFields" << YAML::Value;
					auto& entity_fields = ScriptEngine::getScriptFieldMap(entity);
					out << YAML::BeginSeq;
					for (const auto& [name, field] : fields)
					{
						if (entity_fields.find(name) == entity_fields.end())
							continue;

						out << YAML::BeginMap; // ScriptField
						out << YAML::Key << "Name" << YAML::Value << name;
						out << YAML::Key << "Type" << YAML::Value << Utils::scriptFieldTypeToString(field.type_);

						out << YAML::Key << "Data" << YAML::Value;
						ScriptFieldInstance& script_field = entity_fields.at(name);

						switch (field.type_)
						{
							WRITE_SCRIPT_FIELD(Float, float);
							WRITE_SCRIPT_FIELD(Double, double);
							WRITE_SCRIPT_FIELD(Bool, bool);
							WRITE_SCRIPT_FIELD(Char, char);
							WRITE_SCRIPT_FIELD(Byte, int8_t);
							WRITE_SCRIPT_FIELD(Short, int16_t);
							WRITE_SCRIPT_FIELD(Int, int32_t);
							WRITE_SCRIPT_FIELD(Long, int64_t);
							WRITE_SCRIPT_FIELD(UByte, uint8_t);
							WRITE_SCRIPT_FIELD(UShort, uint16_t);
							WRITE_SCRIPT_FIELD(UInt, uint32_t);
							WRITE_SCRIPT_FIELD(ULong, uint64_t);
							WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
							WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
							WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
							WRITE_SCRIPT_FIELD(Entity, UUID);
						}
						out << YAML::EndMap; // ScriptFields
					}
					out << YAML::EndSeq;
				}

			}
			out << YAML::EndMap;
			
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color_;

			if (spriteRendererComponent.texture_)
			{
				out << YAML::Key << "TextureHandle" << YAML::Value << spriteRendererComponent.texture_;
			}

			out << YAML::Key << "TillingFactor" << YAML::Value << spriteRendererComponent.tiling_factor_;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.hasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.getComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.color_;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness_;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade_;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& rigidbody_2d_component = entity.getComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << rigidbody2DComponentTypeToString(rigidbody_2d_component.type_);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody_2d_component.fixed_rotation_;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& box_collider_2d_component = entity.getComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << box_collider_2d_component.offset_;
			out << YAML::Key << "Size" << YAML::Value << box_collider_2d_component.size_;
			out << YAML::Key << "Density" << YAML::Value << box_collider_2d_component.density_;
			out << YAML::Key << "Friction" << YAML::Value << box_collider_2d_component.friction_;
			out << YAML::Key << "Restitution" << YAML::Value << box_collider_2d_component.restitution_;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << box_collider_2d_component.restitution_threshold_;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& box_collider_2d_component = entity.getComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << box_collider_2d_component.offset_;
			out << YAML::Key << "Radius" << YAML::Value << box_collider_2d_component.radius_;
			out << YAML::Key << "Density" << YAML::Value << box_collider_2d_component.density_;
			out << YAML::Key << "Friction" << YAML::Value << box_collider_2d_component.friction_;
			out << YAML::Key << "Restitution" << YAML::Value << box_collider_2d_component.restitution_;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << box_collider_2d_component.restitution_threshold_;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<TextComponent>())
		{
			out << YAML::Key << "TextComponent";
			out << YAML::BeginMap;

			auto& text_component = entity.getComponent<TextComponent>();
			out << YAML::Key << "TextString" << YAML::Value << text_component.text_string_;
			out << YAML::Key << "Color" << YAML::Value << text_component.color_;
			out << YAML::Key << "Kerning" << YAML::Value << text_component.kerning_;
			out << YAML::Key << "LineSpacing" << YAML::Value << text_component.line_spacing_;

			out << YAML::EndMap;
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
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch(YAML::ParserException e)
		{
			DN_CORE_ERROR("Failed to load .yaml file '{0}'\n   {1}", filepath, e.what());
			return false;
		}

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

				Entity deserialized_entity = scene_->createEntityWithUUID(uuid, name);
				
				auto transform_component = entity["TransformComponent"];
				if (transform_component)
				{
					auto& tc = deserialized_entity.getComponent<TransformComponent>();

					tc.translation_ = transform_component["Translation"].as<glm::vec3>();
					tc.rotation_ = transform_component["Rotation"].as<glm::vec3>();
					tc.scale_ = transform_component["Scale"].as<glm::vec3>();
				}

				auto camera_component = entity["CameraComponent"];
				if (camera_component)
				{
					auto& cc = deserialized_entity.addComponent<CameraComponent>();

					auto& cameraProps = camera_component["Camera"];
					cc.camera_.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera_.setPerspectiveVerticalFov(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera_.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera_.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera_.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera_.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera_.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.is_primary_ = camera_component["Primary"].as<bool>();
					cc.is_fixed_aspect_ratio_ = camera_component["FixedAspectRatio"].as<bool>();
				}

				auto script_component = entity["ScriptComponent"];
				if (script_component)
				{
					auto& sc = deserialized_entity.addComponent<ScriptComponent>();
					sc.class_name_ = script_component["ClassName"].as<std::string>();

					auto script_fields = script_component["ScriptFields"];
					if (script_fields)
					{
						Ref<ScriptClass> entity_class = ScriptEngine::getEntityClass(sc.class_name_);

						if(entity_class)
						{
							const auto& fields = entity_class->getFields();
							auto& entityFields = ScriptEngine::getScriptFieldMap(deserialized_entity);
							for (auto script_field : script_fields)
							{
								std::string name = script_field["Name"].as<std::string>();
								std::string typeString = script_field["Type"].as<std::string>();
								ScriptFieldType type = Utils::scriptFieldTypeFromString(typeString);

								ScriptFieldInstance& field_instance = entityFields[name];

								// TODO(Yan): turn this assert into Hazelnut log warning
								DN_CORE_ASSERT(fields.find(name) != fields.end(), " ");

								if (fields.find(name) == fields.end())
									continue;

								field_instance.field_ = fields.at(name);

								switch (type)
								{
									READ_SCRIPT_FIELD(Float, float);
									READ_SCRIPT_FIELD(Double, double);
									READ_SCRIPT_FIELD(Bool, bool);
									READ_SCRIPT_FIELD(Char, char);
									READ_SCRIPT_FIELD(Byte, int8_t);
									READ_SCRIPT_FIELD(Short, int16_t);
									READ_SCRIPT_FIELD(Int, int32_t);
									READ_SCRIPT_FIELD(Long, int64_t);
									READ_SCRIPT_FIELD(UByte, uint8_t);
									READ_SCRIPT_FIELD(UShort, uint16_t);
									READ_SCRIPT_FIELD(UInt, uint32_t);
									READ_SCRIPT_FIELD(ULong, uint64_t);
									READ_SCRIPT_FIELD(Vector2, glm::vec2);
									READ_SCRIPT_FIELD(Vector3, glm::vec3);
									READ_SCRIPT_FIELD(Vector4, glm::vec4);
									READ_SCRIPT_FIELD(Entity, UUID);
								}
							}
						}
					}
				}

				auto sprite_renderer_component = entity["SpriteRendererComponent"];
				if (sprite_renderer_component)
				{
					auto& src = deserialized_entity.addComponent<SpriteRendererComponent>();
					src.color_ = sprite_renderer_component["Color"].as<glm::vec4>();

					if (sprite_renderer_component["TexturePath"])
					{
						//std::string texture_path = sprite_renderer_component["TexturePath"].as<std::string>();
						//auto path = Project::getAssetFileSystemPath(texture_path);
						//src.texture_ = Texture2D::createTexture(path.string());
					}

					if (sprite_renderer_component["TextureHandle"])
					{
						src.texture_ = sprite_renderer_component["TextureHandle"].as<AssetHandle>();
					}

					if (sprite_renderer_component["TillingFactor"])
					{
						src.tiling_factor_ = sprite_renderer_component["TillingFactor"].as<float>();
					}
				}

				auto circle_renderer_component = entity["CircleRendererComponent"];
				if (circle_renderer_component)
				{
					auto& crc = deserialized_entity.addComponent<CircleRendererComponent>();
					crc.color_ = circle_renderer_component["Color"].as<glm::vec4>();
					crc.thickness_ = circle_renderer_component["Thickness"].as<float>();
					crc.fade_ = circle_renderer_component["Fade"].as<float>();
				}

				auto rigidbody_2d_component = entity["Rigidbody2DComponent"];
				if (rigidbody_2d_component)
				{
					auto& rb2d = deserialized_entity.addComponent<Rigidbody2DComponent>();
					rb2d.type_ = rigidbody2DComponentTypeFromString(rigidbody_2d_component["BodyType"].as<std::string>());
					rb2d.fixed_rotation_ = rigidbody_2d_component["FixedRotation"].as<bool>();
				}

				auto box_collider_2d_component = entity["BoxCollider2DComponent"];
				if (box_collider_2d_component)
				{
					auto& bc2d = deserialized_entity.addComponent<BoxCollider2DComponent>();
					bc2d.offset_ = box_collider_2d_component["Offset"].as<glm::vec2>();
					bc2d.size_ = box_collider_2d_component["Size"].as<glm::vec2>();
					bc2d.density_ = box_collider_2d_component["Density"].as<float>();
					bc2d.friction_ = box_collider_2d_component["Friction"].as<float>();
					bc2d.restitution_ = box_collider_2d_component["Restitution"].as<float>();
					bc2d.restitution_threshold_ = box_collider_2d_component["RestitutionThreshold"].as<float>();
				}

				auto circle_collider_2d_component = entity["CircleCollider2DComponent"];
				if (circle_collider_2d_component)
				{
					auto& cc2d = deserialized_entity.addComponent<CircleCollider2DComponent>();
					cc2d.offset_ = circle_collider_2d_component["Offset"].as<glm::vec2>();
					cc2d.radius_ = circle_collider_2d_component["Radius"].as<float>();
					cc2d.density_ = circle_collider_2d_component["Density"].as<float>();
					cc2d.friction_ = circle_collider_2d_component["Friction"].as<float>();
					cc2d.restitution_ = circle_collider_2d_component["Restitution"].as<float>();
					cc2d.restitution_threshold_ = circle_collider_2d_component["RestitutionThreshold"].as<float>();
				}

				auto text_component = entity["TextComponent"];
				if (text_component)
				{
					auto& tc = deserialized_entity.addComponent<TextComponent>();
					tc.text_string_ = text_component["TextString"].as<std::string>();
					tc.color_ = text_component["Color"].as<glm::vec4>();
					tc.kerning_ = text_component["Kerning"].as<float>();
					tc.line_spacing_ = text_component["LineSpacing"].as<float>();

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