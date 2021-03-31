#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "glm/glm.hpp"
#include <array>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 700

#define numVAO 2
#define numVBO 2

using namespace std;


float x , y = 0.0f;
float rectangleY = 0.0f, rectangleX = 0.0f;
float SpeedX = 0.002, SpeedY = 0.007f;
GLfloat radius;


float rectangleCoordinates[] = {
	-0.25f,  0.01f, 0.0f,	
	-0.25f, -0.01f, 0.0f,	
	 0.25f, -0.01f, 0.0f,	
	 0.25f,  0.01f, 0.0f
};
unsigned int RectangleIndices[] = {   // rectangel is formed by two triangles
	0, 1, 2,	
	0, 2, 3		
};


std::vector<glm::vec3> CircleVertices;

GLuint vbo[numVBO];
GLuint vao[numVAO];
GLuint EBO;

GLuint renderingProgram;

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}


GLuint generateShaderProgram() {

	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string fragShaderStr = readShaderSource("fragmentShader.glsl");
	string vertShaderStr = readShaderSource("vertexShader.glsl");

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}


	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}


void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
	int numberOfVertices = numberOfSides + 2;
		GLfloat twicePi = 2.0f * M_PI;

		GLfloat* circleVerticesX = new GLfloat[numberOfVertices];
		GLfloat* circleVerticesY = new GLfloat[numberOfVertices];
		GLfloat* circleVerticesZ = new GLfloat[numberOfVertices];

		circleVerticesX[0] = x;
		circleVerticesY[0] = y;
		circleVerticesZ[0] = z;

		for (int i = 1; i < numberOfVertices; i++)
		{
			circleVerticesX[i] = x + (radius * cos(i * twicePi / numberOfSides));
			circleVerticesY[i] = y + (radius * sin(i * twicePi / numberOfSides));
			circleVerticesZ[i] = z;
			CircleVertices.push_back(glm::vec3(circleVerticesX[i], circleVerticesY[i], circleVerticesZ[i]));
		}
	
}

void init(GLFWwindow* window) {
	renderingProgram = generateShaderProgram();

	drawCircle(0.0f, 0.0f, 0.0f, 0.1, 35);

	glGenBuffers(2, vbo);
	glGenVertexArrays(2, vao);
	glGenBuffers(1, &EBO);

	//Bind rectangle rectangle data
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleCoordinates), rectangleCoordinates, GL_STATIC_DRAW);

	//Indices of the Rectangle
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RectangleIndices), RectangleIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);


	//The circle
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, CircleVertices.size() * sizeof(glm::vec3), CircleVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void display(GLFWwindow* window, double currentTime) {


	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	//Action upon pressing keys
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (rectangleY < 1.0f - 0.02f)	rectangleY += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (rectangleY > -1.0f + 0.02f) rectangleY -= 0.01f;
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (rectangleX < 1.0f - 0.02f)	rectangleX += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (rectangleX > -1.0f + 0.02f) rectangleX -= 0.01f;
	}

	// Animation control, compute the Location for the next refresh
	x += SpeedX;
	y += SpeedY;

	// check if the circle trespassed the edges

	GLfloat Xmax = 1.0f - radius;
	GLfloat Ymax = 1.0f - radius;
	GLfloat Xmin = radius - 1.0f;
	GLfloat Ymin = radius - 1.0f;


	if (x > Xmax) {

		x = Xmax;
		SpeedX = -SpeedX;
	}

	else if (x < Xmin) {

		x = Xmin;
		SpeedX = -SpeedX;
	}
	if (y > Ymax) {

		y = Ymax;
		SpeedY = -SpeedY;
	}
	else if (y < Ymin) {

		y = Ymin;
		SpeedY = -SpeedY;
	}


	



	GLuint offsetLocX = glGetUniformLocation(renderingProgram, "offsetX");
	GLuint offsetLocY = glGetUniformLocation(renderingProgram, "offsetY");

	//color
	GLuint Color = glGetUniformLocation(renderingProgram, "v_Color");
	glUniform4f (Color, 1.0f, 0.0f, 0.0f, 1.0f);

	//Specify  Objects Values for rectangle  
	glProgramUniform1f(renderingProgram, offsetLocX, rectangleX);
	glProgramUniform1f(renderingProgram, offsetLocY, rectangleY);

	glBindVertexArray(vao[0]);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glProgramUniform1f(renderingProgram, offsetLocX, x);
	glProgramUniform1f(renderingProgram, offsetLocY, y);


	//Apply changes if the circle intercepts the line.
	if ((y - radius <= (rectangleY + 0.01f) && y + radius >= (rectangleY - 0.01f)) && (x - radius <= (rectangleX + 0.25f) && x + radius >= (rectangleX - 0.25f))) {
		glUniform4f(Color, 1.0f, 0.0f, 0.0f, 1.0f);
	}
	else {
		glUniform4f(Color, 0.0f, 1.0f, 0.0f, 1.0f);
	}

	glBindVertexArray(0);

	glBindVertexArray(vao[1]);

	// Pixels Parameters 
	glLineWidth(5.0f);
	glPointSize(25.0f);
	glDrawArrays(GL_LINE_LOOP, 0, CircleVertices.size());




}

int main(void) {

	if (!glfwInit()) { exit(EXIT_FAILURE); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "First Assignment", NULL, NULL);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glDeleteVertexArrays(2, vao);
	glDeleteBuffers(2, vbo);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(renderingProgram);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}