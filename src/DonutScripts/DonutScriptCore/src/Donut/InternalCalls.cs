using System;
using System.Runtime.CompilerServices;

namespace Donut
{
    public static class InternalCalls
    {
        #region Entity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_hasComponent(ulong entity_id, Type component_type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_findEntityByName(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object getScriptInstance(ulong enttity_id);
        #endregion


        #region TransformComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_getTranslation(ulong entity_id, out Vector3 translation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_setTranslation(ulong entity_id, ref Vector3 translation);
        #endregion


        #region Rigidbody2DComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_applyLinearImpulse(ulong entity_id, ref Vector2 impulse, ref Vector2 world_pos, ref bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_applyLinearImpulseToCenter(ulong entity_id, ref Vector2 impulse, ref bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_getLinearVelocity(ulong entity_id, out Vector2 linear_velocity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static Rigidbody2DComponent.BodyType Rigidbody2DComponent_getType(ulong entity_id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_setType(ulong entity_id, Rigidbody2DComponent.BodyType type);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_isKeydown(KeyCode keycode);
        #endregion


        #region TextComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string TextComponent_getText(ulong entity_id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_setText(ulong entity_id, string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_getColor(ulong entity_id, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_setColor(ulong entity_id, ref Vector4 color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_getKerning(ulong entity_id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_setKerning(ulong entity_id, float kerning);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_getLineSpacing(ulong entity_id);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_setLineSpacing(ulong entity_id, float line_spacing);
        #endregion
    }
}