using Donut;

namespace Donut
{
    public class Input
    {
        public static bool IsKeydown(KeyCode keycode)
        {
            return InternalCalls.input_isKeydown(keycode);
        }
    }
}