#pragma comment(lib, "nclgl.lib")

#include "Renderer.h"
#include "../../ext/nclgl/window.h"

int main() 
{
	Window window("CSC8502 Coursework 18/19 - By Hasan Ahmed", 1280, 720, false);
	if (!window.HasInitialised()) 
		return -1;

	Renderer renderer(window);
	if (!renderer.HasInitialised()) 
		return -1;

	while (window.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) 
	{
		renderer.RenderScene();
	}

	return 0;
}