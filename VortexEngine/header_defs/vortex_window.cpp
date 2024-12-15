#include "../headers/vortex_window.h"

#include <stdexcept>

namespace VortexEngine {

	VortexWindow::VortexWindow(int width, int height, std::string name) : width{ width }, height{ height }, windowName{ name } {
		initWindow();
	}

	VortexWindow::~VortexWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VortexWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void VortexWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void VortexWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vortexWindow = reinterpret_cast<VortexWindow*>(glfwGetWindowUserPointer(window));
		vortexWindow->frameBufferResized = true;
		vortexWindow->width = width;
		vortexWindow->height = height;
	}
}