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

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
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
        internal extern static float nativeLog_VectorDot(ref Vector3 param);
    }

    public class Entity
    {
        
        public Entity()
        {
            Console.WriteLine("Main constructor");
            //InternalCalls.CppFunction();
            Log("sdfasdf", 1234);

            Vector3 pos = new Vector3(1, 2, 3);
            Vector3 result = Log(pos);
            Console.WriteLine($"{result.X}, {result.Y}, {result.Z}");
            Console.WriteLine("{0}", InternalCalls.nativeLog_VectorDot(ref pos));
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }

        public void PrintInt(int value)
        {
            Console.WriteLine($"C# says: {value}");
        }

        public void PrintInts(int value1, int value2)
        {
            Console.WriteLine($"C# says: {value1} and {value2}");
        }

        public void PrintCustomMessage(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }

        private void Log(string text, int parameter)
        {
            InternalCalls.nativeLog(text, parameter);
        }

        //private void Log(Vector3 parameter)
        //{
        //    nativeLog_Vector(ref parameter);
        //}

        private Vector3 Log(Vector3 parameter)
        {
            InternalCalls.nativeLog_Vector(ref parameter, out Vector3 result);
            return result;

            //Vector3 result;
            //nativeLog_Vector_Ret(ref parameter, out result);
            //return result;
        }

        

    }
}
