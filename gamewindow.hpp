#pragma once

#include "glrenderer.hpp"
#include <GLFW/glfw3.h>

#include <stdlib.h>

typedef struct _WindowProperties {
	const char *title;
	int width;
	int height;
	bool resizeable = true;
	bool vsync = true;
} WindowProperties;

class GameWindow {
public:
	double deltaTime = 0;
	int fpsCount = 0;

	GameWindow() {
		if (glfwInit() == GLFW_FALSE) {
			exit(EXIT_FAILURE);
		}
	}

	~GameWindow() {
		glfwTerminate();
	}

	void Create(WindowProperties *properties) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_RESIZABLE, properties->resizeable);
		resizeable = properties->resizeable;

		windowPtr = glfwCreateWindow(properties->width, properties->height, properties->title, NULL, NULL);
		if (windowPtr == NULL) {
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(windowPtr);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			exit(EXIT_FAILURE);
		}

		windowWidth = properties->width;
		windowHeight = properties->height;

		glViewport(0, 0, windowWidth, windowHeight);

		if (!properties->vsync) {
			glfwSwapInterval(0);
		}

		glfwSetWindowUserPointer(windowPtr, this);

		glfwSetKeyCallback(windowPtr, KeyCallback);
		glfwSetMouseButtonCallback(windowPtr, ButtonCallback);
		glfwSetCursorPosCallback(windowPtr, CursorCallback);
		glfwSetScrollCallback(windowPtr, ScrollCallback);
		glfwSetFramebufferSizeCallback(windowPtr, FramebufferSizeCallback);
	}

	int GetWidth() { 
		return windowWidth; 
	}

	int GetHeight() { 
		return windowHeight; 
	}

	double GetAspect() {
		return (double)windowWidth / windowHeight;
	}

	void SetTitle(const char *title) {
		glfwSetWindowTitle(windowPtr, title);
	}

	void DisableVsync() {
		glfwSwapInterval(0);
	}

	void EnableVsync() {
		glfwSwapInterval(1);
	}

	void DisableCursor() {
		glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void EnableCursor() {
		glfwSetInputMode(windowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void ResetCursorPos() {
		glfwSetCursorPos(windowPtr, 0, 0);
	}

	void SetShouldClose(bool close) {
		glfwSetWindowShouldClose(windowPtr, close);
	}

	bool ShouldClose() {
		return glfwWindowShouldClose(windowPtr);
	}

	void Start() {
		for (int i = 0; i < GLFW_KEY_LAST; i++) {
			if (keyMap[i] == 0 || keyMap[i] == 2) {
				continue;
			}

			if (keyMap[i] == 1) {
				keyMap[i] = 2;
			}

			if (keyMap[i] == 3) {
				keyMap[i] = 0;
			}
		}

		for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
			if (buttonMap[i] == 0 || buttonMap[i] == 2) {
				continue;
			}

			if (buttonMap[i] == 1) {
				buttonMap[i] = 2;
			}

			if (buttonMap[i] == 3) {
				buttonMap[i] = 0;
			}
		}

		mouseDeltaX = 0;
		mouseDeltaY = 0;

		scrollX = 0;
		scrollY = 0;

		glfwPollEvents();

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		if (currentTime - lastFpsTime >= 1.0f) {
			fpsCount = fpsCounter;
			fpsCounter = 0;
			lastFpsTime = currentTime;
		}
		else {
			fpsCounter++;
		}
	}

	void End() {
		glfwSwapBuffers(windowPtr);
	}

	bool GetKey(int key) {
		return keyMap[key] == 1 || keyMap[key] == 2;
	}

	bool GetKeyHold(int key) {
		return keyMap[key] == 2;
	}

	bool GetKeyDown(int key) {
		return keyMap[key] == 1;
	}

	bool GetKeyUp(int key) {
		return keyMap[key] == 3;
	}

	bool GetButton(int button) {
		return buttonMap[button] == 1 || buttonMap[button] == 2;
	}

	bool GetButtonHold(int button) {
		return buttonMap[button] == 2;
	}

	bool GetButtonDown(int button) {
		return buttonMap[button] == 1;
	}

	bool GetButtonUp(int button) {
		return buttonMap[button] == 3;
	}

	double GetMouseX() {
		return mouseX;
	}

	double GetMouseY() {
		return mouseY;
	}

	double GetMouseDeltaX() {
		return mouseDeltaX;
	}

	double GetMouseDeltaY() {
		return mouseDeltaY;
	}

	double GetScrollX() {
		return scrollX;
	}

	double GetScrollY() {
		return scrollY;
	}

private:
	GLFWwindow *windowPtr = NULL;
	int windowWidth = 0;
	int windowHeight = 0;

	bool resizeable = false;

	double lastFrameTime = 0;
	double lastFpsTime = 0;
	int fpsCounter = 0;

	int keyMap[GLFW_KEY_LAST] = {}; /* 0 = none, 1 = just pressed, 2 = holding, 3 = just released */
	int buttonMap[GLFW_MOUSE_BUTTON_LAST] = {}; /* 0 = none, 1 = just pressed, 2 = holding, 3 = just released */

	double mouseX = 0;
	double mouseY = 0;

	double mouseDeltaX = 0;
	double mouseDeltaY = 0;

	double scrollX = 0;
	double scrollY = 0;

	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		GameWindow *gameWindow = static_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		if (gameWindow == NULL) {
			return;
		}

		if (action == GLFW_PRESS && gameWindow->keyMap[key] != 2) {
			gameWindow->keyMap[key] = 1;
		}
		else if (action == GLFW_RELEASE && gameWindow->keyMap[key] != 0) {
			gameWindow->keyMap[key] = 3;
		}
	}
	static void ButtonCallback(GLFWwindow *window, int button, int action, int mods) {
		GameWindow *gameWindow = static_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		if (gameWindow == NULL) {
			return;
		}

		if (action == GLFW_PRESS && gameWindow->buttonMap[button] != 2) {
			gameWindow->buttonMap[button] = 1;
		}
		else if (action == GLFW_RELEASE && gameWindow->buttonMap[button] != 0) {
			gameWindow->buttonMap[button] = 3;
		}
	}
	static void CursorCallback(GLFWwindow *window, double xpos, double ypos) {
		GameWindow *gameWindow = static_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		if (gameWindow == NULL) {
			return;
		}

		gameWindow->mouseDeltaX = xpos - gameWindow->mouseX;
		gameWindow->mouseDeltaY = ypos - gameWindow->mouseY;

		gameWindow->mouseX = xpos;
		gameWindow->mouseY = ypos;
	}
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
		GameWindow *gameWindow = static_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		if (gameWindow == NULL) {
			return;
		}

		gameWindow->scrollX = xoffset;
		gameWindow->scrollY = yoffset;
	}
	static void FramebufferSizeCallback(GLFWwindow *window, int w, int h) {
		GameWindow *gameWindow = static_cast<GameWindow *>(glfwGetWindowUserPointer(window));
		if (gameWindow == NULL || gameWindow->resizeable == false) {
			return;
		}

		gameWindow->windowWidth = w;
		gameWindow->windowHeight = h;

		glViewport(0, 0, w, h);
	}
};