using Donut;

namespace Donut
{
    public class Input
    {
        public static bool IsKeydown(KeyCode keycode)
        {
            return InternalCalls.Input_isKeydown(keycode);
        }
    }
}