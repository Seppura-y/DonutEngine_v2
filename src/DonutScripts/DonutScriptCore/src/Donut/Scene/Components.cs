

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
        public enum BodyType { Static = 0, Dynamic, Kinematic }
        public Vector2 LinearVelocity
        {
            get
            {
                InternalCalls.Rigidbody2DComponent_getLinearVelocity(Entity.ID, out Vector2 velocity);
                return velocity;
            }
        }

        public BodyType type
        {
            get => InternalCalls.Rigidbody2DComponent_getType(Entity.ID);
            set => InternalCalls.Rigidbody2DComponent_setType(Entity.ID, value);
        }

        public void applyLinearImpulse(Vector2 impulse, Vector2 world_pos, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_applyLinearImpulse(Entity.ID, ref impulse, ref world_pos, ref wake);
        }

        public void applyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_applyLinearImpulseToCenter(Entity.ID, ref impulse, ref wake);
        }


    }

    public class TextComponent : Component
    {
        public string Text
        {
            get => InternalCalls.TextComponent_getText(Entity.ID);
            set => InternalCalls.TextComponent_setText(Entity.ID, value);
        }

        public Vector4 Color
        {
            get
            {
                InternalCalls.TextComponent_getColor(Entity.ID, out Vector4 color);
                return color;
            }

            set
            {
                InternalCalls.TextComponent_setColor(Entity.ID, ref value);
            }
        }

        public float Kerning
        {
            get => InternalCalls.TextComponent_getKerning(Entity.ID);
            set => InternalCalls.TextComponent_setKerning(Entity.ID, value);
        }


        public float LineSpacing
        {
            get => InternalCalls.TextComponent_getLineSpacing(Entity.ID);
            set => InternalCalls.TextComponent_setLineSpacing(Entity.ID, value);
        }
    }
}