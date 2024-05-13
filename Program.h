//
// Created by mahie on 23/02/2024.
//

#ifndef PROGRAM_H
#define PROGRAM_H

#include <filesystem>
#include <glad/gl.h>

#include "Diagnose.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <stb_include.h>
#include <string>
#include <string_view>

class Program {
private:
	GLuint id;
	GLuint create_shader(const std::filesystem::path path, GLenum shader_type) {
		// Why is getting a C string so hard in c++ depending on the compiler... GCC vs MSVC
		// https://stackoverflow.com/a/52011138
		// due to this... we have to convert to string before converting again to c_str
		// why not pass it as a c_str first then... because It is a file path so I like to label it as such.
		// we also have to do this to remove const.. why can't it implicitly turn itself non-const

		char* source = stb_include_file(
			const_cast<char*>(path.string().c_str()),
			nullptr,
			const_cast<char*>(path.parent_path().string().c_str()),
			Diagnose::callback());
		Diagnose::unwrap();
		GLuint shader = glCreateShader(shader_type);
		// OpenGL does that automatically.
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		// check for shader compile errors
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			glGetShaderInfoLog(shader, 512, nullptr, Diagnose::callback());
			// Should Only be Vert or Frag Shader passed in
			std::string_view shader_type_str{
				(shader_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT" };
			std::cout << "ERROR::SHADER::" << shader_type_str
				<< "::COMPILATION_FAILED\n"
				<< Diagnose::callback() << std::endl;
		}

		free(source);
		return shader;
	}

public:
	Program(const std::filesystem::path vert, const std::filesystem::path frag) {
		auto vertex = create_shader(vert, GL_VERTEX_SHADER);
		auto fragment = create_shader(frag, GL_FRAGMENT_SHADER);
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);
		GLint linked = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &linked);
		if (!linked) {
			glGetProgramInfoLog(id, 512, nullptr, Diagnose::callback());
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				<< Diagnose::callback() << std::endl;
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	GLuint getId() const { return id; }
};

struct Attribute {
	GLuint attrib_index;
	size_t offset;
	std::pair<GLenum, GLint> type_size;
};

#endif