#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>
#include <SOIL2.h>
#include <cmath>
#include <utility>
#include <iostream>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp> 

#include "GLShader.h"
#include "GLobject.h"

int w = 0, h = 0;

GLShader * shaderwrap;
GLobject * objectwrap, *object1, *object2;

std::vector<int> VertShaders;
std::vector<int> FragShaders;
glm::mat4 Matrix_projection;

float rotateX = 0;
float rotateY = 0;
float scaleX = 1;
float scaleY = 1;

int VertShader, FragShaderLambert, FragShaderP, FragShaderBlinn, FragShaderMinnaert, FragShaderToon, FragShaderGooch, FragShaderRim;
GLuint tex1, tex2, tex;
int mode = 0;
bool use_texture = false;

glm::vec3 eye {1.3,0,0};
glm::vec4 light_position, light_ambient, light_diffuse, light_specular;
glm::vec3 light_attenuation;

float lightZ = 0;
float lightX = 2;
float lightAngle = 0;

std::vector<std::string> pathsVert = {
"shader_lab14_phong.vert"
};

std::vector<std::string> pathsFrag = {
"shader_lab14_phong.frag",
"shader_lambert.frag",
"shader_blinn.frag",
"shader_minnaert.frag",
"shader_toon.frag",
"shader_gooch.frag",
"shader_rim.frag"
};

void LoadShaders() {
	VertShader = shaderwrap->load_shader(pathsVert[0], GL_VERTEX_SHADER);
	FragShaderP = shaderwrap->load_shader(pathsFrag[0], GL_FRAGMENT_SHADER);
	FragShaderLambert = shaderwrap->load_shader(pathsFrag[1], GL_FRAGMENT_SHADER);
	FragShaderBlinn = shaderwrap->load_shader(pathsFrag[2], GL_FRAGMENT_SHADER);
	FragShaderMinnaert = shaderwrap->load_shader(pathsFrag[3], GL_FRAGMENT_SHADER);
	FragShaderToon = shaderwrap->load_shader(pathsFrag[4], GL_FRAGMENT_SHADER);
	FragShaderGooch = shaderwrap->load_shader(pathsFrag[5], GL_FRAGMENT_SHADER);
	FragShaderRim = shaderwrap->load_shader(pathsFrag[6], GL_FRAGMENT_SHADER);
}

void Init(void)
{
	glClearColor(0, 0, 0, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void load_textures() {
	tex1 =  SOIL_load_OGL_texture("cat_diff.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
	tex2 = SOIL_load_OGL_texture("teapot.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y);
}

void Set_cam() {
	Matrix_projection = glm::perspective(80.0f, (float)w / h, 0.01f, 200.0f);
	glm::vec3 center = { 0,0,0 };
	glm::vec3 up = { 0,0,-1 };

	Matrix_projection *= glm::lookAt(eye, center, up);
}

void Reshape(int x, int y)
{
	if (y == 0 || x == 0) return;

	w = x;
	h = y;
	glViewport(0, 0, w, h);

	Set_cam();
}


void set_light() {
	light_position = {0,0,2,1};
	light_ambient = { 0,0,0,1 };
	light_diffuse = { 1,1,1,1 };
	light_specular = { 0.7,0.7,0,1 };
	light_attenuation = {0,0,0};
}

void Update(void) {
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(shaderwrap->ShaderProgram);

	glm::vec4  lp = { lightX,0,0,1 };
	glm::mat4  m = glm::translate(glm::vec3(0, 0, lightZ));
	m = glm::rotate(m,lightAngle, glm::vec3(0, 0, 1));
	lp = m * lp;

	// vert shader part, never changes
	glm::mat4 transform_model = glm::rotate(rotateY, glm::vec3(0, 0, 1));
	transform_model = glm::rotate(transform_model, rotateX, glm::vec3( 1,0,0 ));
	
	shaderwrap->setUniformmat4("transform_model",false, transform_model);	
	shaderwrap->setUniformmat4("transform_viewProjection", false, Matrix_projection);
	shaderwrap->setUniformfv3("transform_viewPosition", eye);

	glm::mat3 transform_normal = glm::inverseTranspose(glm::mat3(transform_model));
	shaderwrap->setUniformmat3("transform_normal", false, transform_normal);

	shaderwrap->setUniformfv4("light_position", lp);
	shaderwrap->setUniformfv4("light_ambient", light_ambient);
	shaderwrap->setUniformfv4("light_diffuse", light_diffuse);
	shaderwrap->setUniformfv4("light_specular", light_specular);

	shaderwrap->setUniformfv3("light_attenuation", light_attenuation);

	shaderwrap->setUniformfv4("material_ambient", objectwrap->material_ambient);
	shaderwrap->setUniformfv4("material_diffuse", objectwrap->material_diffuse);
	shaderwrap->setUniformfv4("material_specular", objectwrap->material_specular);
	shaderwrap->setUniformfv4("material_emission", objectwrap->material_emission);
	shaderwrap->setUniform1f("material_shininess", objectwrap->material_shininess);

	shaderwrap->setUniform1b("use_texture", use_texture);

	shaderwrap->setUniform1s("material_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	switch (mode)
	{
	case 2: 
		shaderwrap->setUniform1f("k", 0.7f);
		break;
	case 4:
		shaderwrap->setUniformfv4("warmColor", glm::vec4(0.9, 0, 0.45, 1));
		shaderwrap->setUniformfv4("coolColor", glm::vec4(0, 0.7, 0.7, 1));
		shaderwrap->setUniform1f("diffuseCool", 0.45f);
		shaderwrap->setUniform1f("diffuseWarm", 0.2f);
		break;
	case 5:
		shaderwrap->setUniform1f("rimPower", 5);
		shaderwrap->setUniform1f("bias", 0.3f);
		shaderwrap->setUniformfv4("rimColor", glm::vec4(1, 0.6, 0, 1));
	default:
		break;
	}
	objectwrap->drawObject();

	glUseProgram(0);
	
	glFlush();
	glutSwapBuffers();

}

void keyboard(unsigned char key, int x, int y)	
{
	switch (key)
	{
	case 'w':
		rotateX += 0.1;
		break;
	case 's':
		rotateX -= 0.1;
		break;
	case 'a':
		rotateY -= 0.1;
		break;
	case 'd':
		rotateY += 0.1;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void next_mode() {
	mode = (mode + 1) % 6;
	switch (mode)
	{
	case 0: // Lambert
		shaderwrap->linkProgram(VertShader, FragShaderLambert);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	case 1: // Blinn
		shaderwrap->linkProgram(VertShader, FragShaderBlinn);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	case 2: // Minnaert
		shaderwrap->linkProgram(VertShader, FragShaderMinnaert);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	case 3: // Toon
		shaderwrap->linkProgram(VertShader, FragShaderToon);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	case 4: // Gooch
		shaderwrap->linkProgram(VertShader, FragShaderGooch);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	case 5: // Rim
		shaderwrap->linkProgram(VertShader, FragShaderRim);
		objectwrap->BindAttributesToShader(*shaderwrap);
		break;
	default:
		break;
	}

}

void next_object() {
	if (objectwrap == object1) {
		objectwrap = object2;
		tex = tex2;
		eye = { 0.3 ,0,0 };
	}
	else {
		objectwrap = object1;
		tex = tex1;
		eye = { 1.3 ,0,0 };
	}
	Set_cam();
	objectwrap->BindAttributesToShader(*shaderwrap);
}

void specialKeys(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP:
		lightZ += 0.1;
		break;
	case GLUT_KEY_DOWN:
		lightZ -= 0.1;
		break;
	case GLUT_KEY_LEFT:
		lightAngle -= 0.1;
		break;
	case GLUT_KEY_RIGHT:
		lightAngle += 0.1;
		break;
	case GLUT_KEY_F1:
		next_mode();
		break;
	case GLUT_KEY_F2:
		use_texture = !use_texture;
		break;
	case GLUT_KEY_F3:
		next_object();
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);

	glutCreateWindow("OpenGL");
	glEnable(GL_DEPTH_TEST);
	
	glutDisplayFunc(Update);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	GLenum err = glewInit();
	if (GLEW_OK != err)
		std::cout << glewGetErrorString(err) << std::endl;

	Init();

	shaderwrap = new GLShader();
	
	load_textures();
	LoadShaders();
	mode = -1;
	object1 = new GLobject("cat.obj");
	object2 = new GLobject("teapot.obj");
	objectwrap = object1;
	tex = tex1;
	next_mode();
	shaderwrap->checkOpenGLerror();
	set_light();
	glutMainLoop();

	return 0;         
}