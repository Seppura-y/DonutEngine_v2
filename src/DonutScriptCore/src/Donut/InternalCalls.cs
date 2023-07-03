using System;
using System.Runtime.CompilerServices;

namespace Donut
{
    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_hasComponent(ulong entity_id, Type component_type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_getTranslation(ulong entity_id, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_setTranslation(ulong entity_id, ref Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_applyLinearImpulse(ulong entity_id, ref Vector2 impulse, ref Vector2 world_pos, ref bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_applyLinearImpulseToCenter(ulong entity_id, ref Vector2 impulse, ref bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_isKeydown(KeyCode keycode);
    }
}