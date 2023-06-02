#ifndef INPUT_H
#define INPUT_H

#include "core/core.h"

namespace Donut
{
    class DONUT_API Input
    {
    public:
        virtual ~Input() = default;
        inline static bool isKeyPressed(int keycode) { return s_instance_->isKeyPressedImpl(keycode); }
        inline static bool isMouseButtonPressed(int button) { return s_instance_->isMouseButtonPressedImpl(button); }
        inline static float getMouseX() { return s_instance_->getMouseXImpl(); }
        inline static float getMouseY() { return s_instance_->getMouseYImpl(); }
        inline static std::pair<float, float> getMousePosition() { return s_instance_->getMousePositionImpl(); }
        
    protected:
        virtual bool isKeyPressedImpl(int keycode) = 0;
        virtual bool isMouseButtonPressedImpl(int button) = 0;
        virtual float getMouseXImpl() = 0;
        virtual float getMouseYImpl() = 0;
        virtual std::pair<float, float> getMousePositionImpl() = 0;
    protected:
        static Input* s_instance_;
    };
}
#endif