using System;
using System.Runtime.CompilerServices;

namespace Donut
{
    // in C# 
    // struct is stack allocated (value type)
    // class is heap allocated (+ GC)
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float scalar)
        {
            X = scalar;
            Y = scalar;
            Z = scalar;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }


        public static Vector3 operator +(Vector3 vector1, Vector3 vector2)
        {
            return new Vector3(vector1.X + vector2.X, vector1.Y + vector2.Y, vector1.Z + vector2.Z);
        }

        public static Vector3 operator*(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        }
    }

    public static class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void CppFunction();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void nativeLog(string text, int param);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void nativeLog_Vector(ref Vector3 param, out Vector3 result);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float entity_setTranslation(ulong entity_id, ref Vector3 translation);


        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float entity_getTranslation(ulong entity_id, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool input_isKeydown(KeyCode keycode);

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //internal extern static float entity_setTranslation(ref Vector3 translation);
    }

    public class Entity
    {
        protected Entity() { ID = 0; }

        internal Entity(ulong id)
        {
            ID = id;
        }

        public readonly ulong ID;

        public Vector3 Translation
        {
            get
            {
                InternalCalls.entity_getTranslation(ID, out Vector3 translation);
                return translation;
            }

            set
            {
                InternalCalls.entity_setTranslation(ID, ref value);
            }
        }

    }
}
