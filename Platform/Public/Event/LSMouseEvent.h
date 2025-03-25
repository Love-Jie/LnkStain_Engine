#pragma once

#include "Event/LSKeyEvent.h"

namespace lse{
    enum MouseButton{
        MOUSE_BUTTON_LEFT   = 0,
        MOUSE_BUTTON_RIGHT  = 1,
        MOUSE_BUTTON_MIDDLE = 2,
    };

    static const char* LSMouseButtonToStr(MouseButton mouseButton){
        switch(mouseButton){
            ENUM_TO_STR(MOUSE_BUTTON_LEFT);
            ENUM_TO_STR(MOUSE_BUTTON_RIGHT);
            ENUM_TO_STR(MOUSE_BUTTON_MIDDLE);
        }
        return "unknow";
    };

     class LSMouseButtonPressEvent : public LSEvent{
    public:
        LSMouseButtonPressEvent(MouseButton mouseButton, KeyMod keyMod, bool repeat) : mMouseButton(mouseButton), mKeyMod(keyMod), mRepeat(repeat) {
        };

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << LSEvent::ToString();
            ss << "( mouseButton=" << LSMouseButtonToStr(mMouseButton);
            ss << ", keyMod=" << LSKeyModToStr(mKeyMod);
            ss << ", isRepeat=" << mRepeat << " )";
            return ss.str();
        }

        [[nodiscard]] bool IsShiftPressed() const { return mKeyMod & LS_MOD_SHIFT; };
        [[nodiscard]] bool IsControlPressed() const { return mKeyMod & LS_MOD_CONTROL; };
        [[nodiscard]] bool IsAltPressed() const { return mKeyMod & LS_MOD_ALT; };
        [[nodiscard]] bool IsSuperPressed() const { return mKeyMod & LS_MOD_SUPER; };
        [[nodiscard]] bool IsCapsLockPressed() const { return mKeyMod & LS_MOD_CAPS_LOCK; };    //TODO mod do not return this.
        [[nodiscard]] bool IsNumLockPressed() const { return mKeyMod & LS_MOD_NUM_LOCK; };      //TODO mod do not return this.
        [[nodiscard]] bool IsRepeat() const { return mRepeat; };

        MouseButton mMouseButton;
        KeyMod mKeyMod;
        bool mRepeat;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_PRESS);
    };

    class LSMouseButtonReleaseEvent : public LSEvent{
    public:
        LSMouseButtonReleaseEvent(MouseButton mouseButton) : mMouseButton(mouseButton){

        };
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << LSEvent::ToString();
            ss << "( mouseButton=" << LSMouseButtonToStr(mMouseButton) << " )";
            return ss.str();
        }

        MouseButton mMouseButton;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_RELEASE);
    };

    class LSMouseMovedEvent : public LSEvent{
    public:
        LSMouseMovedEvent(float xPos, float yPos) : mXPos(xPos), mYPos(yPos) {

        }
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << LSEvent::ToString();
            ss << "( xPos=" << mXPos;
            ss << ", yPos=" << mYPos << " )";
            return ss.str();
        }

        float mXPos;
        float mYPos;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_MOVED);
    };

    class LSMouseScrollEvent : public LSEvent{
    public:
        LSMouseScrollEvent(float xOffset, float yOffset) : mXOffset(xOffset), mYOffset(yOffset) {};
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << LSEvent::ToString();
            ss << "( xOffset=" << mXOffset;
            ss << ", yOffset=" << mYOffset << " )";
            return ss.str();
        }

        float mXOffset;
        float mYOffset;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_SCROLLED);
    };
}