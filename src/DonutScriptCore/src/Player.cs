using System;
using System.Runtime.CompilerServices;

using Donut;

namespace Sandbox
{
    public class Player : Entity
    {
        public float speed_ = 5.0f;

        void onCreate()
        {
            Console.WriteLine("Player.onCreate");
        }

        void onUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");
        }
    }
}