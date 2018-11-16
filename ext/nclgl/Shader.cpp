#include "Shader.h"

Shader::Shader(string vFile, string fFile, string gFile)
{
	//Generate new shader program object name
	program = glCreateProgram();
	
	//Generate the shader objects from file for respective type
	objects[SHADER_VERTEX] = GenerateShader(vFile, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fFile, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!gFile.empty())
	{
		objects[SHADER_GEOMETRY] = GenerateShader(gFile, GL_GEOMETRY_SHADER);
		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	//Attach shader objects to shader program
	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);
	
	//Pass vertex and colour data to shaders
	SetDefaultAttributes();
}

Shader::~Shader(void)
{
	for (int i = 0; i < 3; ++i)
	{
		//Detach shader object from shader program
		glDetachShader(program, objects[i]);
		//Delete shader object
		glDeleteShader(objects[i]);
	}
	//Delete shader program
	glDeleteProgram(program);
}

GLuint Shader::GenerateShader(string from, GLenum type)
{
	cout << "Compiling Shader..." << endl;

	//Contains the shader object source code
	string load;
	if (!LoadShaderFile(from, load))
	{
		cout << "Compiling failed!" << endl;
		loadFailed = true;
		return 0;
	}

	//Create an OpenGL shader object
	GLuint shader = glCreateShader(type);

	const char* chars = load.c_str();
	glShaderSource(shader, 1, &chars, NULL);	//Store the shader object source
	glCompileShader(shader);	//Compile the shader object source

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);	//See whether compilation was successful, if not print why not 

	if(status == GL_FALSE)
	{
		cout << "Compiling failed!" << endl;
		char error[512];
		glGetInfoLogARB(shader, sizeof(error), NULL, error);
		cout << error;
		loadFailed = true;
		return 0;
	}

	cout << "Compiling success!\n\n";
	loadFailed = false;
	return shader;
}

bool Shader::LoadShaderFile(string from, string &into)
{
	ifstream file;
	string temp;

	cout << "Loading shader text from " << from << "\n\n";

	file.open(from.c_str());
	if (!file.is_open())
	{
		cout << "File does not exist!\n";
		return false;
	}

	while (!file.eof())
	{
		getline(file, temp);
		into += temp + "\n";
	}

	file.close();
	cout << into << "\n\n";
	cout << "Loaded shader text!\n\n";
	return true;
}

bool Shader::LinkProgram()
{
	if (loadFailed)
		return false;

	//Link the shader objects into a shader executable
	glLinkProgram(program);

	GLenum error = glGetError();

	//Check status of linking
	GLint code;
	glGetProgramiv(program, GL_LINK_STATUS, &code);
	return code == GL_TRUE ? true : false;
}

void Shader::SetDefaultAttributes()
{
	//Bind vertex attribute to input variables in shader as parameters
	glBindAttribLocation(program, VERTEX_BUFFER, "position");
	glBindAttribLocation(program, COLOUR_BUFFER, "colour");
	glBindAttribLocation(program, NORMAL_BUFFER, "normal");
	glBindAttribLocation(program, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(program, TEXTURE_BUFFER, "texCoord");
}