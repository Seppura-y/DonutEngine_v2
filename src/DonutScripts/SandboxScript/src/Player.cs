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
        private Rigidbody2DComponent Rigidbody;

        public float Speed;
        public float Time = 0.0f;

        void onCreate()
        {
            Console.WriteLine($"Player.onCreate - {ID}");

            Transform = getComponent<TransformComponent>();
            Rigidbody = getComponent<Rigidbody2DComponent>();

            bool has_transform_component = hasComponent<TransformComponent>();
            Console.WriteLine("has transform {0}", has_transform_component);
        }

        void onUpdate(float ts)
        {
            //Console.WriteLine($"Player.OnUpdate: {ts}");
            Time += ts;
            float speed = Speed;
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

            if (Input.IsKeydown(KeyCode.Space))
            {
                velocity.X = -1.0f;
            }

            Entity camera_entity = findEntityByName("Camera");
            if(camera_entity != null) 
            {
                Camera camera = camera_entity.As<Camera>();

                if(Input.IsKeydown(KeyCode.Q)) 
                {
                    camera.DistanceFromPlayer += speed * 2.0f * ts;
                }
                else if(Input.IsKeydown(KeyCode.E)) 
                {
                    camera.DistanceFromPlayer -= speed * 2.0f * ts;
                }
            }

            velocity *= speed * ts;

            Rigidbody.applyLinearImpulse(velocity.XY, true);

            //Vector3 translation = Transform.Translation;
            //translation += velocity * ts;
            //Transform.Translation = translation;
        }
    }
}