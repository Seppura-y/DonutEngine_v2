#include "pch.h"
#include "script_glue.h"

#include <mono/metadata/object.h>

namespace Donut
{
#define DN_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Donut.InternalCalls::" #Name, Name)

	static void nativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void nativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		DN_CORE_WARN("Value: {0}", (*parameter).x);
		*outResult = glm::normalize(*parameter);
	}

	static float nativeLog_VectorDot(glm::vec3* parameter)
	{
		DN_CORE_WARN("Value: {0}", (*parameter).x);
		return glm::dot(*parameter, *parameter);
	}

	void ScriptGlue::registerFunctions()
	{
		DN_ADD_INTERNAL_CALL(nativeLog);
		DN_ADD_INTERNAL_CALL(nativeLog_Vector);
		DN_ADD_INTERNAL_CALL(nativeLog_VectorDot);
	}
}