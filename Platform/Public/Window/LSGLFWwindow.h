#pragma once

#include"LSWindow.h"
#include"LSGraphicContext.h"
#include"GLFW/glfw3.h"

namespace lse {
	class LSGLFWwindow : public LSWindow {
	
	public:
		LSGLFWwindow() = delete;
		LSGLFWwindow(uint32_t width, uint32_t height, const char* title);

		~LSGLFWwindow() override;

		bool ShouldClose() override;
		void PollEvents() override;
		void SwapBuffer() override;

		void* GetImplWindowPointer() const override { return mGLFWwindow; };

		void GetMousePos(glm::vec2 &mousePos) const override;
		bool IsMouseDown(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const override;
		bool IsMouseUp(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const override;
		bool IsKeyDown(Key key)const override;
		bool IsKeyUp(Key key) const override;

	private:
		void SetupWindowCallBacks();
		GLFWwindow* mGLFWwindow;

	};
}



