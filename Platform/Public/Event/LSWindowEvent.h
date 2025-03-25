#pragma once

#include"Event/LSEvent.h"

namespace lse{
    class LSFramebufferResizeEvent : public LSEvent{
    public:
        LSFramebufferResizeEvent(uint32_t width, uint32_t height) : mWidth(width), mHeight(height){};

        [[nodiscard]]  std::string ToString() const override{
            return LSEvent::ToString() + "( width=" + std::to_string(mWidth) + ", height=" + std::to_string(mHeight) + " )";

        }

        uint32_t mWidth,mHeight;
        EVENT_CLASS_TYPE(EVENT_TYPE_FRAME_BUFFER_RESIZE);
    };

    class LSWindowFocusEvent : public LSEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_FOCUS);
    };

    class LSWindowLostFocusEvent : public LSEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_LOST_FOCUS);
    };

    class LSWindowMovedEvent : public LSEvent{
    public:
        LSWindowMovedEvent(uint32_t xPos, uint32_t yPos) : mXPos(xPos), mYPos(yPos) {};
        [[nodiscard]] std::string ToString() const override {
            return LSEvent::ToString() + "( xPos=" + std::to_string(mXPos) + ", yPos=" + std::to_string(mYPos) + " )";
        }

        uint32_t mXPos;
        uint32_t mYPos;
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_MOVED);
    };

    class LSWindowCloseEvent : public LSEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_CLOSE);
    };


}