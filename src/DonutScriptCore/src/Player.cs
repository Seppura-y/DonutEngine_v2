using System;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using Donut;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Sandbox
{
    public class Player : Entity
    {
        public float speed_ = 5.0f;

        void onCreate()
        {
            Console.WriteLine($"Player.onCreate - {ID}");
        }

        void onUpdate(float ts)
        {
            Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if(Input.IsKeydown(KeyCode.W))
            {
                velocity.Y = 1.0f;
            }
            else if (Input.IsKeydown(KeyCode.S))
            {
                velocity.Y = -1.0f;
            }

            if (Input.IsKeydown(KeyCode.A))
            {
                velocity.X = -1.0f;
            }
            else if (Input.IsKeydown(KeyCode.D))
            {
                velocity.X = 1.0f;
            }

            velocity *= speed;

            Vector3 translation = Translation;
            translation += velocity * ts;
            Translation = translation;
        }
    }
}