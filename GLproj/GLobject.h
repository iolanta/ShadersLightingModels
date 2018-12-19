#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>
#include <SOIL2.h>
#include <string>
#include "GLShader.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class GLobject
{
	GLushort * indeces;

public:
	GLuint VBO, VAO, IBO;
	GLsizei stride, CoordOffset, NormalOffset, TexCoordOffset, ColorOffset;
	
	glm::vec4 material_ambient, material_diffuse, material_specular, material_emission;
	GLfloat material_shininess;


	int count_vertex;
	int count_indexes;
	GLobject(std::string path, glm::vec3 clr = { 1,1,1 });


	bool BindAttributesToShader(GLShader& shaderobject);

	void drawObject();

	~GLobject();
};

