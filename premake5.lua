include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "DonutEngine"
	architecture "x86_64"
	startproject "DonutEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- -- Include directories relative to root folder (solution directory)
-- IncludeDir = {}
-- IncludeDir["GLFW"] = "%{wks.location}/DonutEngine/dependencies/GLFW/include"
-- IncludeDir["Glad"] = "%{wks.location}/DonutEngine/dependencies/Glad/include"
-- IncludeDir["ImGui"] = "%{wks.location}/DonutEngine/dependencies/imgui"
-- IncludeDir["glm"] = "%{wks.location}/DonutEngine/dependencies/glm"
-- IncludeDir["stb_image"] = "%{wks.location}/DonutEngine/dependencies/stb_image"
-- IncludeDir["entt"] = "%{wks.location}/DonutEngine/dependencies/entt/include"
-- IncludeDir["yaml_cpp"] = "%{wks.location}/DonutEngine/dependencies/yaml-cpp/include"
-- IncludeDir["ImGuizmo"] = "%{wks.location}/DonutEngine/dependencies/ImGuizmo"


group "Dependencies"
	include "vendor/premake"
	include "DonutEngine/dependencies/glfw"
	include "DonutEngine/dependencies/glad"
	include "DonutEngine/dependencies/imgui"
group ""

include "DonutEngine"
include "DonutEditor"