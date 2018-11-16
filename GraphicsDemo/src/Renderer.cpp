#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();

	currentShader = new Shader(SHADERDIR"basicVertex.glsl", SHADERDIR"colourFragment.glsl");

	if (!currentShader->LinkProgram())
		return;

	init = true;
}

Renderer::~Renderer(void)
{
	delete triangle;
}

void Renderer::RenderScene()
{
	//Clear screen ready for draw calls
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Use the shader executable that was created
	glUseProgram(currentShader->GetProgram());

	//Draw contents of VAO
	triangle->Draw();

	//Unbind the shader executable after drawing (good practice)
	glUseProgram(0);

	//Swap to second buffer
	SwapBuffers();
}