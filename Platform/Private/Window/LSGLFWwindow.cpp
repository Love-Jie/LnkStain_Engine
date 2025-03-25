#include"LSGLFWwindow.h"
#include"LSLog.h"
#include"GLFW/glfw3native.h"
#include "Event/LSEventDispatcher.h"

namespace lse {
	LSGLFWwindow::LSGLFWwindow(uint32_t width, uint32_t height, const char* title) {
		if (!glfwInit()) {
			LOG_E("Failed to init glfw.");
			return;
		}
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);  //设置可见性，等做完下列操作后再显示

		mGLFWwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (!mGLFWwindow) {
			LOG_E("Failed to caeate glfw window!");
			return;
		}

		//获取显示器的信息，使生成的窗口放置在显示器中间
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		if (primaryMonitor) {
			int xPos, yPos, workWidth, workHeight;
			glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
			glfwSetWindowPos(mGLFWwindow, workWidth / 2 - width / 2, workHeight / 2 - height / 2);
		}

		glfwMakeContextCurrent(mGLFWwindow);

		SetupWindowCallBacks();

		//显示窗口
		glfwShowWindow(mGLFWwindow);
	}

	LSGLFWwindow::~LSGLFWwindow() {
		glfwDestroyWindow(mGLFWwindow);
		glfwTerminate();
		LOG_I("The application running over.");
	}

	bool LSGLFWwindow::ShouldClose() {
		return glfwWindowShouldClose(mGLFWwindow);
	}

	void LSGLFWwindow::PollEvents() {
		glfwPollEvents();
	}

	void LSGLFWwindow::SwapBuffer() {
		glfwSwapBuffers(mGLFWwindow);
	}

	void LSGLFWwindow::GetMousePos(glm::vec2 &mousePos) const {
		glfwGetCursorPos(mGLFWwindow,reinterpret_cast<double *>(&mousePos.x),reinterpret_cast<double *>(&mousePos.y));
	}

	bool LSGLFWwindow::IsMouseDown(MouseButton mouseButton) const {
		return glfwGetMouseButton(mGLFWwindow,mouseButton) == GLFW_PRESS;
	}

	bool LSGLFWwindow::IsMouseUp(MouseButton mouseButton) const {
		return glfwGetMouseButton(mGLFWwindow,mouseButton) == GLFW_RELEASE;
	}

	bool LSGLFWwindow::IsKeyDown(Key key) const {
		return glfwGetKey(mGLFWwindow,key) == GLFW_PRESS;
	}

	bool LSGLFWwindow::IsKeyUp(Key key) const {
		return glfwGetKey(mGLFWwindow,key) == GLFW_RELEASE;
	}

	void LSGLFWwindow::SetupWindowCallBacks() {
		glfwSetWindowUserPointer(mGLFWwindow,this);
		 glfwSetFramebufferSizeCallback(mGLFWwindow, [](GLFWwindow* window, int width, int height){
            auto *lsWindow = static_cast<LSGLFWwindow*>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                LSFramebufferResizeEvent fbResizeEvent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
                LSEventDispatcher::GetInstance()->Dispatch(fbResizeEvent);
            }
        });

        glfwSetWindowFocusCallback(mGLFWwindow, [](GLFWwindow* window, int focused){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                if(focused){
                    LSWindowFocusEvent windowFocusEvent{};
                    LSEventDispatcher::GetInstance()->Dispatch(windowFocusEvent);
                } else {
                    LSWindowLostFocusEvent windowLostFocusEvent{};
                    LSEventDispatcher::GetInstance()->Dispatch(windowLostFocusEvent);
                }
            }
        });

        glfwSetWindowPosCallback(mGLFWwindow, [](GLFWwindow* window, int xpos, int ypos){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                LSWindowMovedEvent windowMovedEvent{ static_cast<uint32_t>(xpos), static_cast<uint32_t>(ypos) };
                LSEventDispatcher::GetInstance()->Dispatch(windowMovedEvent);
            }
        });

        glfwSetWindowCloseCallback(mGLFWwindow, [](GLFWwindow* window){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                LSWindowCloseEvent windowCloseEvent{};
                LSEventDispatcher::GetInstance()->Dispatch(windowCloseEvent);
            }
        });

        glfwSetKeyCallback(mGLFWwindow, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                if(action == GLFW_RELEASE){
                    LSKeyReleaseEvent keyReleaseEvent{static_cast<Key>(key) };
                    LSEventDispatcher::GetInstance()->Dispatch(keyReleaseEvent);
                } else {
                    LSKeyPressEvent keyPressEvent{static_cast<Key>(key), static_cast<KeyMod>(mods), action == GLFW_REPEAT };
                    LSEventDispatcher::GetInstance()->Dispatch(keyPressEvent);
                }
            }
        });

        glfwSetMouseButtonCallback(mGLFWwindow, [](GLFWwindow* window, int button, int action, int mods){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                if(action == GLFW_PRESS){
                    LSMouseButtonPressEvent mouseButtonPressEvent{static_cast<MouseButton>(button), static_cast<KeyMod>(mods), false };
                    LSEventDispatcher::GetInstance()->Dispatch(mouseButtonPressEvent);
                }
                if(action == GLFW_RELEASE){
                    LSMouseButtonReleaseEvent mouseButtonReleaseEvent{static_cast<MouseButton>(button) };
                    LSEventDispatcher::GetInstance()->Dispatch(mouseButtonReleaseEvent);
                }
            }
        });

        glfwSetCursorPosCallback(mGLFWwindow, [](GLFWwindow* window, double xpos, double ypos){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
				LSMouseMovedEvent mouseMovedEvent{ static_cast<float>(xpos), static_cast<float>(ypos) };
                LSEventDispatcher::GetInstance()->Dispatch(mouseMovedEvent);
            }
        });

        glfwSetScrollCallback(mGLFWwindow, [](GLFWwindow* window, double xoffset, double yoffset){
            auto *lsWindow = static_cast<LSGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(lsWindow){
                LSMouseScrollEvent mouseScrollEvent{ static_cast<float>(xoffset), static_cast<float>(yoffset) };
                LSEventDispatcher::GetInstance()->Dispatch(mouseScrollEvent);
            }
        });
    }
}

