#pragma comment(lib, "nclgl.lib")

#include "Renderer.h"
#include "../../ext/nclgl/Window.h"

int main() 
{
	Timer timer;
	timer.Init();

	Window window("[2018/19] CSC8502 Coursework - By Hasan Ahmed", 1280, 720, false);
	if (!window.HasInitialised()) 
		return -1;

	//window.LockMouseToWindow(true);
	//window.ShowOSPointer(false);

	Renderer renderer(window, &timer);
	if (!renderer.HasInitialised()) 
		return -1;

	while (window.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) 
	{
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F1))
			renderer.ToggleStats();

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT))
			renderer.PreviousScene();

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT))
			renderer.NextScene();

		renderer.UpdateScene(window.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}