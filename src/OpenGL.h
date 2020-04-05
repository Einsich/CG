#pragma once
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include <GL/gl.h>
#include "GL/glext.h"
#include "GL/wglext.h"
#include <iostream>
#include <fstream>

#include "common.h"

// глобальная перменная для хранения ошибки OpenGL
extern GLenum g_OpenGLError;

// получть адрес функции из драйвера
#define OPENGL_GET_PROC(p,n) \
	n = (p)wglGetProcAddress(#n); \
	if (NULL == n) \
	{ \
		LOG_ERROR("Loading extension '%s' fail (%d)\n", #n, GetLastError()); \
		return false; \
	}

// проверка на ошибки OpenGL
#define OPENGL_CHECK_FOR_ERRORS() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
		LOG_ERROR("OpenGL error 0x%X\n", (unsigned)g_OpenGLError);
#define GL_CHECK_ERRORS ThrowExceptionOnGLError(__LINE__, __FILE__);
#define GL_CHECK(_call) do{_call; GL_CHECK_ERRORS;}while(0);
static void ThrowExceptionOnGLError(int line, const char* file)
{

	static char errMsg[512];

	// Check opengl for error occurrence, get error code
	GLenum gl_error = glGetError();

	if (gl_error == GL_NO_ERROR)
		return;

	switch (gl_error)
	{
	case GL_INVALID_ENUM:
		std::cerr << "GL_INVALID_ENUM file " << file << " line " << line << std::endl;
		break;

	case GL_INVALID_VALUE:
		std::cerr << "GL_INVALID_VALUE file " << file << " line " << line << std::endl;
		break;

	case GL_INVALID_OPERATION:
		std::cerr << "GL_INVALID_OPERATION file " << file << " line " << line << std::endl;
		break;

	case GL_STACK_OVERFLOW:
		std::cerr << "GL_STACK_OVERFLOW file " << file << " line " << line << std::endl;
		break;

	case GL_STACK_UNDERFLOW:
		std::cerr << "GL_STACK_UNDERFLOW file " << file << " line " << line << std::endl;
		break;

	case GL_OUT_OF_MEMORY:
		std::cerr << "GL_OUT_OF_MEMORY file " << file << " line " << line << std::endl;
		break;

	case GL_NO_ERROR:
		break;

	default:
		std::cerr << "Unknown error @ file " << file << " line " << line << std::endl;
		break;
	}

	if (gl_error != GL_NO_ERROR)
		throw std::runtime_error(errMsg);
}
// инициализация необходимых расширений OpenGL
bool OpenGLInitExtensions();

// вывод в лог различной информации по OpenGL, версия, константы и т.п.
void OpenGLPrintDebugInfo();

// расширения OpenGL
// Texture
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
// VAO
extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
// VBO
extern PFNGLGENBUFFERSPROC    glGenBuffers;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLBINDBUFFERPROC    glBindBuffer;
extern PFNGLBUFFERDATAPROC    glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLMAPBUFFERPROC     glMapBuffer;
extern PFNGLUNMAPBUFFERPROC   glUnmapBuffer;
// Shaders
extern PFNGLCREATEPROGRAMPROC     glCreateProgram;
extern PFNGLDELETEPROGRAMPROC     glDeleteProgram;
extern PFNGLLINKPROGRAMPROC       glLinkProgram;
extern PFNGLVALIDATEPROGRAMPROC   glValidateProgram;
extern PFNGLUSEPROGRAMPROC        glUseProgram;
extern PFNGLGETPROGRAMIVPROC      glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLCREATESHADERPROC      glCreateShader;
extern PFNGLDELETESHADERPROC      glDeleteShader;
extern PFNGLSHADERSOURCEPROC      glShaderSource;
extern PFNGLCOMPILESHADERPROC     glCompileShader;
extern PFNGLATTACHSHADERPROC      glAttachShader;
extern PFNGLDETACHSHADERPROC      glDetachShader;
extern PFNGLGETSHADERIVPROC       glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
// Attributes
extern PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
// Uniforms
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
extern PFNGLUNIFORM1IPROC          glUniform1i;
extern PFNGLUNIFORM1FVPROC         glUniform1fv;
extern PFNGLUNIFORM3FVPROC         glUniform3fv;
extern PFNGLUNIFORM4FVPROC         glUniform4fv;
// FBO
extern PFNGLBINDFRAMEBUFFERPROC        glBindFramebuffer;
extern PFNGLDELETEFRAMEBUFFERSPROC     glDeleteFramebuffers;
extern PFNGLGENFRAMEBUFFERSPROC        glGenFramebuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
extern PFNGLFRAMEBUFFERTEXTUREPROC     glFramebufferTexture;

