#pragma once

#include <glad/glad.h>

class GLRenderer {
public:
	static void SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	static void Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	static void EnableDepthBuffer(unsigned int func = GL_LESS) {
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(func);
	}

	static void DisableDepthBuffer() {
		glDisable(GL_DEPTH_TEST);
	}
};