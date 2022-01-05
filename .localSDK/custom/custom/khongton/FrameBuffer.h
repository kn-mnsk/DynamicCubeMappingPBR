#pragma once
#ifndef _FrameBuffer_H_
#define _FrameBuffer_H_


//#ifndef __glew_h__
#define GLEW_BUILD
#include <GL/glew.h>
//#endif

#include<custom/khongton/MatrixStack.h> 
#include <iostream>
#include <memory>

class FrameBuffer {
public:
	FrameBuffer();
	~FrameBuffer();
	void init();
	void checkFBO();
	void bindFBO(GLuint handle, float width, float height);
	GLuint getFBOid() const { return fboID; }
	GLuint getTextureID() const { return textureID; }
	void drawToSide(std::shared_ptr<MatrixStack> &MV, std::shared_ptr<MatrixStack> &P, int side);
private:
	GLuint textureID;
	GLuint fboID;
	GLuint renderID;
};

#endif