#pragma once

#include <stdlib.h>
#include <string>
#include <unordered_map>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/file.hpp"

typedef struct _ShaderProperties {
	const char *vertexPath;
	const char *fragmentPath;
} ShaderProperties;

class GameShader {
public:

	GameShader() = default;
	~GameShader() {
		if (programID == 0 || !loaded) {
			return;
		}
		glDeleteProgram(programID);
	}

	bool Load(ShaderProperties *properties) {
		std::string vertexSrcStr, fragmentSrcStr;
		vertexSrcStr = ReadFileContent(properties->vertexPath);
		fragmentSrcStr = ReadFileContent(properties->fragmentPath);

		const char *vertexSrc, *fragmentSrc;
		vertexSrc = vertexSrcStr.c_str();
		fragmentSrc = fragmentSrcStr.c_str();

		unsigned int vertexID, fragmentID;
		vertexID = glCreateShader(GL_VERTEX_SHADER);
		if (vertexID == 0) {
			return false;
		}

		fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
		if (fragmentID == 0) {
			glDeleteShader(vertexID);
			return false;
		}

		glShaderSource(vertexID, 1, &vertexSrc, NULL);
		glShaderSource(fragmentID, 1, &fragmentSrc, NULL);

		int success = 0;
		int logSize = 0;
		char *logBuffer = NULL;

		glCompileShader(vertexID);
		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
		if (success == 0) {
			glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &logSize);
			if (logSize > 0) {
				logBuffer = (char *)malloc(logSize * sizeof(char));
				if (logBuffer == NULL) {
					printf("Couldn't malloc %d amount of bytes.\n", logSize);
					glDeleteShader(vertexID);
					return false;
				}

				glGetShaderInfoLog(vertexID, logSize, &logSize, logBuffer);

				if (logSize > 0) {
					printf("An error has occured while compiling the vertex shader, error log: {\n%s\n}\n", logBuffer);
				}
			}
			free(logBuffer);
			glDeleteShader(vertexID);
			return false;
		}

		glCompileShader(fragmentID);
		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
		if (success == 0) {
			glDeleteShader(vertexID);

			glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &logSize);
			if (logSize > 0) {
				logBuffer = (char *)malloc(logSize * sizeof(char));
				if (logBuffer == NULL) {
					printf("Couldn't malloc %d amount of bytes.\n", logSize);
					glDeleteShader(fragmentID);
					return false;
				}

				glGetShaderInfoLog(fragmentID, logSize, &logSize, logBuffer);

				if (logSize > 0) {
					printf("An error has occured while compiling the fragment shader, error log: {\n%s\n}\n", logBuffer);
				}
			}
			free(logBuffer);
			glDeleteShader(fragmentID);
			return false;
		}

		programID = glCreateProgram();
		if (programID == 0) {
			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);
			return false;
		}

		glAttachShader(programID, vertexID);
		glAttachShader(programID, fragmentID);

		glLinkProgram(programID);
		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (success == 0) {
			glDeleteShader(vertexID);
			glDeleteShader(fragmentID);

			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logSize);
			if (logSize > 0) {
				logBuffer = (char *)malloc(logSize * sizeof(char));
				if (logBuffer == NULL) {
					printf("Couldn't malloc %d amount of bytes.\n", logSize);
					glDeleteProgram(programID);
					return false;
				}

				glGetProgramInfoLog(programID, logSize, &logSize, logBuffer);

				if (logSize > 0) {
					printf("An error has occured while linking the program, error log: {\n%s\n}\n", logBuffer);
				}
			}
			free(logBuffer);
			glDeleteProgram(programID);
			return false;
		}

		glDetachShader(programID, vertexID);
		glDetachShader(programID, fragmentID);

		loaded = true;
		return true;
	}

	void Activate() {
		if (programID == 0 || !loaded) {
			return;
		}

		glUseProgram(programID);
	}

	bool IsLoaded() {
		return loaded;
	}

	void SetInt(const std::string &name, int value) {
		glUniform1i(GetLocation(name), value);
	}

	void SetUnsignedInt(const std::string &name, unsigned int value) {
		glUniform1ui(GetLocation(name), value);
	}

	void SetFloat(const std::string &name, float value) {
		glUniform1f(GetLocation(name), value);
	}

	void SetVec2(const std::string &name, const glm::vec2 &value) {
		glUniform2fv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void SetVec3(const std::string &name, const glm::vec3 &value) {
		glUniform3fv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void SetVec4(const std::string &name, const glm::vec4 &value) {
		glUniform4fv(GetLocation(name), 1, glm::value_ptr(value));
	}

	void SetMat4(const std::string &name, const glm::mat4 &value) {
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

private:
	bool loaded = false;
	unsigned int programID = 0;

	std::unordered_map<std::string, unsigned int> uniformLocationCache;

	int GetLocation(const std::string &name) {
		if (programID == 0 || !loaded) {
			return -1;
		}

		auto cache_it = uniformLocationCache.find(name);
		if (cache_it == uniformLocationCache.end()) {
			int uniformLocation = glGetUniformLocation(programID, name.c_str());
			if (uniformLocation == -1) {
				printf("Couldn't find a uniform with the name \'%s\'.\n", name.c_str());
			}
			uniformLocationCache.insert(std::make_pair(name, uniformLocation));
			return uniformLocation;
		}

		return cache_it->second;
	}
};
