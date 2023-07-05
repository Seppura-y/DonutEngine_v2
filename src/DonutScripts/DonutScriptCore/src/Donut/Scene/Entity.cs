using System;
using System.Runtime.CompilerServices;

namespace Donut
{
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
                InternalCalls.TransformComponent_getTranslation(ID, out Vector3 result);
                return result;
            }

            set
            {
                InternalCalls.TransformComponent_setTranslation(ID, ref value);
            }
        }

        public bool hasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_hasComponent(ID, componentType);
        }

        public T getComponent<T>() where T :Component, new()
        {
            if(!hasComponent<T>())
            {
                return null;
            }

            T component = new T() { Entity = this };
            return component;
        }

    }
}
