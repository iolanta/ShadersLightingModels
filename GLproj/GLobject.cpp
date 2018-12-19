
#include "OBJ_Loader.h"
#include "GLobject.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLobject::GLobject(std::string path, glm::vec3 clr)
{
	objl::Loader modelloader;
	modelloader.LoadFile(path);
	count_indexes = modelloader.LoadedIndices.size();
	count_vertex = modelloader.LoadedVertices.size();

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);
	
	//		   XYZW					Normal XYZ		 TexCoords	UV					Color RGB		
	auto len = sizeof(GLfloat) * 4 + sizeof(GLfloat) * 3 + sizeof(GLfloat) * 2 + sizeof(GLfloat) * 3;
	CoordOffset = 0;
	NormalOffset = sizeof(GLfloat)*4;
	TexCoordOffset = NormalOffset + sizeof(GLfloat) * 3;
	ColorOffset = TexCoordOffset + sizeof(GLfloat) * 2;
	stride = len;

	glBindVertexArray(VAO);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	glBufferData(GL_ARRAY_BUFFER, len*modelloader.LoadedVertices.size(), NULL, GL_STATIC_DRAW);
	GLfloat * mapped_data = (GLfloat *)glMapBufferRange(GL_ARRAY_BUFFER, 0, len*modelloader.LoadedVertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	int ind = 0;
	for (size_t i = 0; i < count_vertex; ++i)
	{
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.X;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.Y;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Position.Z;
		mapped_data[ind++] = 1;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.X;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.Y;
		mapped_data[ind++] = modelloader.LoadedVertices[i].Normal.Z;
		mapped_data[ind++] = modelloader.LoadedVertices[i].TextureCoordinate.X;
		mapped_data[ind++] = modelloader.LoadedVertices[i].TextureCoordinate.Y;
		mapped_data[ind++] = clr.r;
		mapped_data[ind++] = clr.g;
		mapped_data[ind++] = clr.b;
		
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	
	indeces = new GLushort[count_indexes];
	for (size_t i = 0; i < count_indexes; i++)
	{
		indeces[i] = modelloader.LoadedIndices[i];
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_indexes * sizeof(GLushort), indeces, GL_STATIC_DRAW);
	delete indeces;
	glBindVertexArray(0);

	auto mat =  modelloader.LoadedMeshes[0].MeshMaterial;
	material_ambient = {mat.Ka.X,mat.Ka.Y, mat.Ka.Z,1};
	material_diffuse = { mat.Kd.X,mat.Kd.Y, mat.Kd.Z,1 };
	material_specular = { mat.Ks.X,mat.Ks.Y, mat.Ks.Z,1 };
	material_emission = { 0,0,0,1 };
	material_shininess = mat.Ns;


}

bool GLobject::BindAttributesToShader(GLShader & shaderobject)
{
	glBindVertexArray(VAO);
	GLint CoordAttrib = shaderobject.getAttributeLocation("coord");
	GLint NormAttrib = shaderobject.getAttributeLocation("norm");

	GLint TexCoordAttrib = shaderobject.getAttributeLocation("texcoord");
	GLint ColorAttrib = shaderobject.getAttributeLocation("color");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (CoordAttrib == -1 || NormAttrib == -1 || TexCoordAttrib == -1 || ColorAttrib == -1)
	{
		//glBindVertexArray(0);
		//return false;
	}
	if (CoordAttrib != -1) {
		glEnableVertexAttribArray(CoordAttrib);
		glVertexAttribPointer(CoordAttrib, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
	}
	if (NormAttrib != -1) {
		glEnableVertexAttribArray(NormAttrib);
		glVertexAttribPointer(NormAttrib, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(GLfloat)*4));
	}
	if (TexCoordAttrib != -1) {
		glEnableVertexAttribArray(TexCoordAttrib);
		glVertexAttribPointer(TexCoordAttrib, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(GLfloat)* 7));
	}
	if (ColorAttrib != -1){
		glEnableVertexAttribArray(ColorAttrib);
		glVertexAttribPointer(ColorAttrib, 3, GL_FLOAT, GL_TRUE, stride, BUFFER_OFFSET(sizeof(GLfloat)* 9));
	}

	glBindVertexArray(0);
	return true;
}

void GLobject::drawObject()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDrawElements(GL_TRIANGLES, count_indexes, GL_UNSIGNED_SHORT,NULL);
	glBindVertexArray(0);
}


GLobject::~GLobject()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
}
