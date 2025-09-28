#pragma once

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

typedef struct _Texture2DProperties {
	const char *imagePath;
	unsigned int wrapMode_S = GL_REPEAT;
	unsigned int wrapMode_T = GL_REPEAT;
	unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
	unsigned int magFilter = GL_LINEAR;
	bool flipVertically = true;
	float tileX = 1.0f;
	float tileY = 1.0f;
} Texture2DProperties;

class GameTexture2D {
public:

	float tileX = 1.0f;
	float tileY = 1.0f;

	GameTexture2D() = default;
	~GameTexture2D() {
		if (textureID == 0 || !loaded) {
			return;
		}
		glDeleteTextures(1, &textureID);

		stbi_image_free(data);
	}

	bool Load(Texture2DProperties *properties) {
		glGenTextures(1, &textureID);
		if (textureID == 0) {
			return false;
		}

		stbi_set_flip_vertically_on_load(properties->flipVertically);

		data = stbi_load(properties->imagePath, &width, &height, &nrChannels, 0);
		if (data == NULL) {
			glDeleteTextures(1, &textureID);
			textureID = 0;
			printf("Couldn't load the texture with the name \'%s\'.\n", properties->imagePath);
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties->wrapMode_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties->wrapMode_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties->minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties->magFilter);

		GLenum format = 0;
		GLenum internalFormat = 0;

		switch (nrChannels) {
			case 1:
				format = GL_RED;
				internalFormat = GL_R8;
				break;
			case 2:
				format = GL_RG;
				internalFormat = GL_RG8;
				break;
			case 3:
				format = GL_RGB;
				internalFormat = GL_RGB8;
				break;
			case 4:
				format = GL_RGBA;
				internalFormat = GL_RGBA8;
				break;
			default:
				printf("Unsupported number of channels: %d\n", nrChannels);
				stbi_image_free(data);
				glDeleteTextures(1, &textureID);
				textureID = 0;
				return false;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		tileX = properties->tileX;
		tileY = properties->tileY;

		loaded = true;
		return true;
	}

	void Activate(int unit = 0) {
		if (textureID == 0 || !loaded) {
			return;
		}

		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	bool IsLoaded() {
		return loaded;
	}

private:
	bool loaded = false;
	unsigned int textureID = 0;

	int width, height, nrChannels;
	unsigned char *data = NULL;
};