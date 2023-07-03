using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Donut;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent Transform;

        void onCreate()
        {
            Console.WriteLine($"Player.onCreate - {ID}");

            Transform = getComponent<TransformComponent>();

            bool has_transform_component = hasComponent<TransformComponent>();
            Console.WriteLine("has transform {0}", has_transform_component);
        }

        void onUpdate(float ts)
        {
            //Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = 1.0f;
            Vector3 velocity = Vector3.Zero;

            if(Input.IsKeydown(KeyCode.W))
            {
                velocity.Y = 1.0f;
                Console.WriteLine("key w pressed");
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

            Vector3 translation = Transform.Translation;
            translation += velocity * ts;
            Transform.Translation = translation;
        }
    }
}