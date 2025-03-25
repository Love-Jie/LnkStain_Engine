#include"LSWindow.h"
#include"LSGLFWwindow.h"

namespace lse {
	std::unique_ptr<LSWindow>LSWindow::Create(uint32_t width, uint32_t height, const char* title)
	{
#ifdef LS_ENGINE_PLATFORM_WIN32
		return std::make_unique<LSGLFWwindow>(width, height, title);
#elif LS_ENGINE_PLATFORM_MACOS
		return std::make_unique<LSGLFWwindow>(width, height, title);
#elif LS_ENGINE_PLATFORM_LINUX
		return std::make_unique<LSGLFWwindow>(width, height, title);
#endif
		return nullptr;
	}



}