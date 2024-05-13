//
// Created by mahie on 05/03/2024.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include <stb_image.h>
#include <string>

class Texture {
private:
	GLuint id;

public:
	enum class TextureType { DIFFUSE, SPECULAR, NORMAL };
	TextureType type;

	Texture(const std::string& path, GLenum format,
		TextureType tex_type) : type(tex_type) {
		int width, height, numColChannel;
		stbi_uc* bytes = stbi_load(path.c_str(), &width, &height, &numColChannel, 0);
		glCreateTextures(GL_TEXTURE_2D, 1, &id);

		glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureStorage2D(id, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(id, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE,
			bytes);
		// RGB for jpeg, RGBA for png
		glGenerateTextureMipmap(id);
		stbi_image_free(bytes);
	};

	virtual ~Texture() { glDeleteTextures(1, &id); };
	void bind(GLuint unit) const { glBindTextureUnit(unit, id); };

	GLuint getId() const { return id; }
};

#endif