using System;

namespace Donut
{
    // in C# 
    // struct is stack allocated (value type)
    // class is heap allocated (+ GC)
    public struct Vector2
    {
        public float X, Y;

        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar)
        {
            X = scalar;
            Y = scalar;
        }

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }


        public static Vector2 operator +(Vector2 vector_a, Vector2 vector_b)
        {
            return new Vector2(vector_a.X + vector_b.X, vector_a.Y + vector_b.Y);
        }

        public static Vector2 operator *(Vector2 vector, float scalar)
        {
            return new Vector2(vector.X * scalar, vector.Y * scalar);
        }

        public float lengthSquared()
        {
            return X * X + Y * Y;
        }

        public float length()
        {
            return (float)Math.Sqrt(lengthSquared());
        }
    }
}