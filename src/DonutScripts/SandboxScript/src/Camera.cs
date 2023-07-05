using Donut;
using System;

namespace Sandbox
{
    public class Camera : Entity
    {
        void onUpdate(float ts)
        {
            float speed = 10.0f;
            Vector3 velocity = Vector3.Zero;

            if(Input.IsKeydown(KeyCode.Up))
            {
                velocity.Y = 1.0f;
                Console.WriteLine("key w pressed");
            }
            else if (Input.IsKeydown(KeyCode.Down))
            {
                velocity.Y = -1.0f;
            }

            if (Input.IsKeydown(KeyCode.Left))
            {
                velocity.X = -1.0f;
            }
            else if (Input.IsKeydown(KeyCode.Right))
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