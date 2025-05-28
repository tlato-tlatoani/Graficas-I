#include "Triangle.h"

TriangleClass::TriangleClass(OpenGLClass* OpenGL){
	this->OpenGL = OpenGL;
	this->worldmatrix = 0;
	Initialize();
}


TriangleClass::TriangleClass(const TriangleClass& other)
{
}


TriangleClass::~TriangleClass()
{
}

bool TriangleClass::Initialize()
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers();
	if (!result)
	{
		return false;
	}

	worldmatrix = new float[16] { 0.0f };
	if (!OpenGL) {
		result = false;
		_RPT1(0, "Alert! GameObject OpenGL does not defined. \n", 0);
		return result;
	}
	else
		OpenGL->BuildIdentityMatrix(worldmatrix);

	return true;
}

float* TriangleClass::GetWorldMatrix() {
	return this->worldmatrix;
}

bool TriangleClass::SetShaderTriangle(Shader* TriangleShader) {
	ShaderTriangle = TriangleShader;
	return true;
}

void TriangleClass::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	return;
}

void TriangleClass::Render(float* viewMatrix, float* projectionMatrix)
{
	ShaderTriangle->SetShader();
	ShaderTriangle->PonMatriz4x4((char*)"worldMatrix", worldmatrix);
	ShaderTriangle->PonMatriz4x4((char*)"viewMatrix", viewMatrix);
	ShaderTriangle->PonMatriz4x4((char*)"projectionMatrix", projectionMatrix);
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	return;
}

bool TriangleClass::InitializeBuffers()
{
	VertexType* vertices;
	unsigned int* indices;
	// Set the number of vertices in the vertex array.
	m_vertexCount = 3;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned int[m_indexCount];
	if (!indices)
	{
		return false;
	};
	// Load the vertex array with data.

	// Bottom left.
	vertices[0].x = -1.0f;  // Position.
	vertices[0].y = -1.0f;
	vertices[0].z = 0.0f;

	vertices[0].r = 0.0f;  // Color.
	vertices[0].g = 1.0f;
	vertices[0].b = 0.0f;

	// Top middle.
	vertices[1].x = 0.0f;  // Position.
	vertices[1].y = 1.0f;
	vertices[1].z = 0.0f;

	vertices[1].r = 0.0f;  // Color.
	vertices[1].g = 1.0f;
	vertices[1].b = 0.0f;

	// Bottom right.
	vertices[2].x = 1.0f;  // Position.
	vertices[2].y = -1.0f;
	vertices[2].z = 0.0f;

	vertices[2].r = 0.0f;  // Color.
	vertices[2].g = 1.0f;
	vertices[2].b = 0.0f;

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.
	indices[3] = 2;  // Bottom left. Se a�ade a esta practica con el fin de verlo desde al frente como desde atras
	indices[4] = 1;  // Top middle. Se a�ade a esta practica con el fin de verlo desde al frente como desde atras
	indices[5] = 0;  // Bottom right. Se a�ade a esta practica con el fin de verlo desde al frente como desde atras

	// Allocate an OpenGL vertex array object.
	OpenGL->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	OpenGL->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

	// Enable the two vertex array attributes.
	OpenGL->glEnableVertexAttribArray(0);  // Vertex position.
	OpenGL->glEnableVertexAttribArray(1);  // Vertex color.

	// Specify the location and format of the position portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

	// Specify the location and format of the color portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)NULL + (3 * sizeof(float)));

	// Generate an ID for the index buffer.
	OpenGL->glGenBuffers(1, &m_indexBufferId);

	// Bind the index buffer and load the index data into it.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
	OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	// Now that the buffers have been loaded we can release the array data.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void TriangleClass::ShutdownBuffers()
{
	// Disable the two vertex array attributes.
	OpenGL->glDisableVertexAttribArray(0);
	OpenGL->glDisableVertexAttribArray(1);

	// Release the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_vertexBufferId);

	// Release the index buffer.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_indexBufferId);

	// Release the vertex array object.
	OpenGL->glBindVertexArray(0);
	OpenGL->glDeleteVertexArrays(1, &m_vertexArrayId);

	return;
}

void TriangleClass::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	if (worldmatrix) {
		delete worldmatrix;
		worldmatrix = 0;
	}

	return;
}