#pragma once

#include"LSEngine.h"
#include"LSGraphicContext.h"
#include"Event/LSMouseEvent.h"

namespace lse {
	class LSWindow {
	public:

		LSWindow(const LSWindow&) = delete;
		LSWindow& operator=(const LSWindow&) = delete;
		virtual ~LSWindow() = default;

		static std::unique_ptr<LSWindow>Create(uint32_t width, uint32_t height, const char* title);

		virtual bool ShouldClose() = 0;
		virtual void PollEvents() = 0;
		virtual void SwapBuffer() = 0;

		virtual void* GetImplWindowPointer() const = 0;  //拿到原先的window

		virtual void GetMousePos(glm::vec2 &mousePos) const = 0;
		virtual bool IsMouseDown(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const = 0;
		virtual bool IsMouseUp(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const = 0;
		virtual bool IsKeyDown(Key key)const = 0;
		virtual bool IsKeyUp(Key key) const = 0;

	protected:
		LSWindow() = default;

	};


}




