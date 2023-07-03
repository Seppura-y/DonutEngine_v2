

namespace Donut
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_getTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }

            set
            {
                InternalCalls.TransformComponent_setTranslation(Entity.ID, ref value);
            }
        }
    }

    public class Rigidbody2DComponent : Component
    {
        public void applyLinearImpulse(Vector2 impulse, Vector2 world_pos, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_applyLinearImpulse(Entity.ID, ref impulse, ref world_pos, ref wake);
        }

        public void applyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_applyLinearImpulseToCenter(Entity.ID, ref impulse, ref wake);
        }
    }
}