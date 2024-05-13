#ifndef BUFFER_H
#define BUFFER_H

#include <glad/gl.h>
#include <span>

template<typename T> class Buffer {
private:
	GLuint id;
public:	
	Buffer(const std::span<T> data) {
		glCreateBuffers(1, &id);
		glNamedBufferStorage(id, data.size_bytes(), &data.front(), GL_DYNAMIC_STORAGE_BIT);
	}
	GLuint getId() const {
		return id;
	}
};

#endif
