#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <glew.h>
#include <SDL_opengl.h>
#include <string>
#include <iostream>
#include "Debug.h"

// Plan:
// 1. The error occurs because you are trying to cast an SDL_Window* to a GLFWwindow*.
// 2. SDL_Window and GLFWwindow are unrelated types from different libraries and are not interchangeable.
// 3. The Window class is using SDL, not GLFW, so there is no valid way to return a GLFWwindow* from an SDL_Window*.
// 4. The correct fix is to remove the getGLFWwindow() method, as it cannot be implemented meaningfully in this context.


class Window {
private:
	int width, height;
	SDL_Window* window;
	SDL_GLContext context;

public:
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator=(const Window&) = delete;
	Window& operator=(Window&&) = delete;

	Window();
	~Window();
	bool OnCreate(std::string name_, int width_, int height_);
	void OnDestroy();
	
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	SDL_Window* getWindow() const { return window; }

private: /// internal tools OpenGl versions. 
	void setAttributes(int major_, int minor_);
	void getInstalledOpenGLInfo(int *major, int *minor);
};
#endif /// !WINDOW_H
