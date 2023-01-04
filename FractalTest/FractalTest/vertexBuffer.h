#pragma once
#include <cstdint>

#include "defines.h"

struct VertexBuffer {
	VertexBuffer(void* data, uint32_t numVertices);
	virtual ~VertexBuffer();

	void bind();
	void unbind();

private:
	GLuint bufferId;
	GLuint vao;
};

