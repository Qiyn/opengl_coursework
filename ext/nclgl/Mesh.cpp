#include "Mesh.h"

Mesh::Mesh(void)
{
	//Initialise all VBOs to 0
	for (int i = 0; i < MAX_BUFFER; ++i)
		bufferObject[i] = 0;

	//Generate VAO
	glGenVertexArrays(1, &arrayObject);
	
	//Initialise other settings, including the draw type
	numVertices = 0;
	vertices	= NULL;
	colours		= NULL;
	drawType	= GL_TRIANGLES; //An OpenGL Symbolic Constant

	texture = 0;
	textureCoords = NULL;

	indices = NULL;
	numIndices = 0;

	normals = NULL;

	tangents = NULL;
	bumpTexture = 0;
}

Mesh::~Mesh(void)
{
	//Destruct VAO and VBO using specific OpenGL delete functions
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	delete[] vertices;
	delete[] colours;

	glDeleteTextures(1, &texture);
	delete[] textureCoords;

	delete[] indices;

	delete[] normals;

	delete[] tangents;
	glDeleteTextures(1, &bumpTexture);
}

Mesh* Mesh::GenerateTriangle()
{
	Mesh* m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateQuad()
{
	Mesh* m				= new Mesh();
	m->numVertices		= 4;
	m->drawType			= GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->normals			= new Vector3[m->numVertices];
	m->tangents			= new Vector3[m->numVertices];

	m->vertices[0]		= Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1]		= Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2]		= Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3]		= Vector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		m->normals[i] = Vector3(0.0f, 0.0f, -1.0f);
		m->tangents[i] = Vector3(1.0f, 0.0f, 0.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::BufferData()
{
	//Make VAO available to GPU (All following related functions carried out on related object)
	glBindVertexArray(arrayObject);
	
	//--- Begin the Buffering process ---
	
	//Generate VBO
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	
	//Make VBO available to GPU
	//Assign VBO to currently bound VAO
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	
	//Copy vertex data into the graphics memory
	//Second parameter, defining graphics memory space
	//Third parameter, pointer to start of data to copy
	//Fourth parameter, instruction/hint on how data is expected to be used - dynamically updated or loaded once as static data
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);

	//Define how copied data is accessed, by modifying VAO
	//Second parameter, defining to OpenGL that vertex attribute has 3 float components per vertex
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	//Enable the definitions specified in above line
	glEnableVertexAttribArray(VERTEX_BUFFER);

	if (textureCoords)
	{
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	if (colours)
	{
		//Similar to vertices (above)

		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		
		//Difference being the use of Vector4
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		
		//And the use of 4 float components to represent RGBA
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	if (indices)
	{
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	if (normals)
	{
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}

	if (tangents)
	{
		glGenBuffers(1, &bufferObject[TANGENT_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TANGENT_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), tangents, GL_STATIC_DRAW);
		glVertexAttribPointer(TANGENT_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TANGENT_BUFFER);
	}

	//Unbind VAO, to prevent accidental modifications to VAO
	glBindVertexArray(0);
}

void Mesh::Draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumpTexture);

	//Make VAO available to GPU
	glBindVertexArray(arrayObject);
	
	if (bufferObject[INDEX_BUFFER])
	{
		glDrawElements(drawType, numIndices, GL_UNSIGNED_INT, 0);
	}
	else
	{
		//Draw the contents of VAO
		//First parameter, specified draw type
		//Second parameter, starting at first index (0) 
		//Third parameter, how many vertices to draw (all)
		glDrawArrays(drawType, 0, numVertices);
	}

	//Unbind VAO
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::GenerateNormals()
{
	if (!normals)
	{
		normals = new Vector3[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i] = Vector3();
	}

	if (indices)	//Generate per-vertex normals
	{
		for (GLuint i = 0; i < numIndices; i += 3)
		{
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else	//It's just a list of triangles, so generate face normals
	{
		for (GLuint i = 0; i < numVertices; i += 3)
		{
			Vector3 &a = vertices[i];
			Vector3 &b = vertices[i + 1];
			Vector3 &c = vertices[i + 2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] = normal;
			normals[i + 1] = normal;
			normals[i + 2] = normal;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		normals[i].Normalise();
	}

}

void Mesh::GenerateTangents()
{
	if (!tangents)
	{
		tangents = new Vector3[numVertices];
	}

	if (!textureCoords)
	{
		return;	//Can't use texture coords if there aren't any!
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i] = Vector3();
	}

	if (indices)
	{
		for (GLuint i = 0; i < numIndices; i += 3)
		{
			int a = indices[i];
			int b = indices[i + 1];
			int c = indices[i + 2];

			Vector3 tangent = GenerateTangent(vertices[a], vertices[b], vertices[c],
				textureCoords[a], textureCoords[b], textureCoords[c]);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;
		}
	}
	else
	{
		for (GLuint i = 0; i < numVertices; i += 3)
		{
			Vector3 tangent = GenerateTangent(vertices[i], vertices[i + 1], vertices[i + 2],
				textureCoords[i], textureCoords[i + 1], textureCoords[i + 2]);

			tangents[i] += tangent;
			tangents[i + 1] += tangent;
			tangents[i + 2] += tangent;
		}
	}

	for (GLuint i = 0; i < numVertices; ++i)
	{
		tangents[i].Normalise();
	}

}

Vector3 Mesh::GenerateTangent(const Vector3 &a, const Vector3 &b, const Vector3 &c,
	const Vector2 &ta, const Vector2 &tb, const Vector2 &tc)
{
	Vector2 coord1 = tb - ta;
	Vector2 coord2 = tc - ta;

	Vector3 vertex1 = b - a;
	Vector3 vertex2 = c - a;

	Vector3 axis = Vector3(vertex1 * coord2.y - vertex2 * coord1.y);

	float factor = 1.0f / (coord1.x * coord2.y - coord2.x * coord1.y);

	return axis * factor;
}