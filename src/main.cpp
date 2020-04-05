#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <vector>
#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "math3d.h"
using namespace std;
// данные матрицы
typedef float Matrix4[16];
static int cursorPos[2] = { 0,0 }, rotateDelta[2] = { 0,0 }, moveDelta[3] = { 0,0,0 };
// пременные для хранения идентификаторов шейдерной программы и текстуры
static GLuint shaderProgram = 0, colorTexture = 0, heightTexture = 0;

// для хранения двух углов поворота куба
static float  cubeRotation[3] = {0.0f, 0.0f, 0.0f};

// матрицы преобразования
static Camera* mainCamera;
// индексы полученный из шейдерной программы
static GLint colorTextureLocation = -1, modelViewProjectionMatrixLocation = -1;

// для хранения VAO и VBO связанных с кубом
static GLuint cubeVBO[4], cubeVAO;

static vector<vec3> cubePositions;

static vector<vec3> cubeNormals;

// текстурные координаты куба
static vector<vec2>  cubeTexcoords;
float SizeMap = 100,HeightMap = 6;
int w, h;
// индексы вершин куба в порядке поротив часовой стрелки
static vector<uint32_t> cubeIndices;
void CreateCube(float s = 1.0)
{

	cubePositions = {
	{-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
	{ s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
	{-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
	{ s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
	{-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
	{ s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
	};

	cubeNormals = {
		{ 0, 0, 1}, { 0, 0, 1}, { 0, 0, 1}, { 0, 0, 1}, // front
		{ 0, 0,-1}, { 0, 0,-1}, { 0, 0,-1}, { 0, 0,-1}, // back
		{ 0, 1, 0}, { 0, 1, 0}, { 0, 1, 0}, { 0, 1, 0}, // top
		{ 0,-1, 0}, { 0,-1, 0}, { 0,-1, 0}, { 0,-1, 0}, // bottom
		{-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, // left
		{ 1, 0, 0}, { 1, 0, 0}, { 1, 0, 0}, { 1, 0, 0}  // right
	};

	// текстурные координаты куба
	cubeTexcoords = {
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
		{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
	};

	// индексы вершин куба в порядке поротив часовой стрелки
	cubeIndices = {
		 0, 3, 1,  1, 3, 2, // front
		 4, 7, 5,  5, 7, 6, // back
		 8,11, 9,  9,11,10, // top
		12,15,13, 13,15,14, // bottom
		16,19,17, 17,19,18, // left
		20,23,21, 21,23,22  // right
	};
}
void MeshCreatePolyPlane(const vec3 position, float size, float height)
{
	const int n = 25;
	float scale = 1. / n;
	for (int i = 0; i <= n; i++)
		for (int j = 0; j <= n; j++)
		{
			float x = j * size * scale;
			float z = i * size * scale;
			float y = 0;// (rand() % n)* height / n;
			cubePositions.push_back({ x + position.x,y + position.y,z + position.z });
			cubeNormals.push_back({ 0,1,0 });
			cubeTexcoords.push_back({ j * scale, i * scale });
		}
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			uint32_t a = i * (n + 1) + j;
			uint32_t b = a + 1;
			uint32_t d = (i + 1) * (n + 1) + j;
			uint32_t c = d + 1;
			cubeIndices.push_back(a); cubeIndices.push_back(d); cubeIndices.push_back(c);
			cubeIndices.push_back(a); cubeIndices.push_back(c); cubeIndices.push_back(b);

		}

}

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//CreateCube();
	MeshCreatePolyPlane({ 0,0,0 }, SizeMap, HeightMap);
	// создадим и загрузим текстуру
	colorTexture = TextureCreateFromTGA("data/metal.tga");
	heightTexture = TextureCreateFromTGA("data/noise.tga");

	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	if (!colorTexture)
		return false;

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, heightTexture);

	// делаем активным текстурный юнит 0 и назначаем на него текстуру
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// создадим и загрузим шейдерную программу
	shaderProgram = ShaderProgramCreateFromFile("data/lesson", ST_VERTEX | ST_FRAGMENT | ST_TESSEVAL | ST_TESSCONTROL);

	if (!shaderProgram)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgram))
		return false;

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// создадим перспективную матрицу
	const float aspectRatio = (float)window->width / (float)window->height;
	mainCamera = new Camera(vec3(.0f, 5.0f, -10.0f), vec3_zero);
	mainCamera->CameraPerspective(45.0f, aspectRatio, 0.5f, 10000.0f);


	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 0
	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "heightmap")) != -1)
		glUniform1i(colorTextureLocation, 1);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &cubeVAO);

	// сделаем VAO активным
	glBindVertexArray(cubeVAO);

	// создадим 3 VBO для данных куба - координаты вершин, текстурные координат и индексный буфер
	glGenBuffers(4, cubeVBO);

	{
		// начинаем работу с буфером для координат вершин куба
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[0]);
		// поместим в буфер координаты вершин куба
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * (3 * sizeof(float)),
			cubePositions.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	}

	{
		// начинаем работу с буфером для текстурных координат куба
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[1]);
		// поместим в буфер текстурные координаты куба
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * (2 * sizeof(float)),
			cubeTexcoords.data(), GL_STATIC_DRAW);

		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(1);
	}

	{
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * (3 * sizeof(float)),
			cubeNormals.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(2);
	}

	// начинаем работу с индексным буфером
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVBO[3]);
	// поместим в буфер индексы вершин куба
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndices.size() * sizeof(uint32_t),
		cubeIndices.data(), GL_STATIC_DRAW);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	glDeleteBuffers(3, cubeVBO);

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &cubeVAO);

	// удаляем шейдерную программу
	ShaderProgramDestroy(shaderProgram);

	// удаляем текстуру
	TextureDestroy(colorTexture);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// делаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	
	mainCamera->CameraSetup(shaderProgram, mat4_identity);
	// выводим на экран все что относится к VAO
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_PATCHES, cubeIndices.size(), GL_UNSIGNED_INT, NULL);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

float Height(vec3 pos)
{
	return 0;
	//static float* pixels=new float[h * w * 4];
	vec2 p = vec2(pos.x, pos.z) / SizeMap* 0.05;
	p.x= clamp(p.x * w, 0, w - 1);
	p.y = clamp(p.y * h, 0, h - 1);
	int x = p.x;
	int y = p.y;
	float dx = p.x - x;
	float dy = p.y - y;
	float pixels[2 * 2 * 40] = { 0 };
	glBindBuffer(GL_TEXTURE_BUFFER, heightTexture);
	glReadPixels(x, y, 2, 2, GL_RGBA, GL_FLOAT, pixels);
	float A = pixels[1], B = pixels[5], C = pixels[9], D = pixels[13];
	return ((A * (1 - dx) + B * dx) * (1 - dy) + 
		(C * (1 - dx) + D * dx) * dy)  * HeightMap;
	
}
vec3 deltaPos;
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага
	
	vec3 rot = vec3(rotateDelta[1], -rotateDelta[0], 0.0f) * deltaTime * 200;
	float speed = 20;
	mainCamera->transform.Rotate(rot);
	deltaPos += vec3(-moveDelta[0], moveDelta[2], -moveDelta[1]) * deltaTime * speed;
	mainCamera->transform.position = deltaPos;
	mainCamera->transform.position.y += Height(deltaPos);

	rotateDelta[0] = rotateDelta[1] = 0;
	moveDelta[0] = moveDelta[1] = moveDelta[2] = 0;
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow &window)
{
	ASSERT(window);
	static bool captured = false;
	int32_t xCenter = window.width / 2, yCenter = window.height / 2;
	if (InputIsButtonClick(1))
	{
		ShowCursor(captured);
		captured = !captured;
		InputSetCursorPos(xCenter, yCenter);
	}
	if (captured)
	{
		float speed = 10;
		moveDelta[0] = speed * ((int)InputIsKeyDown('D') - (int)InputIsKeyDown('A'));
		moveDelta[1] = speed * ((int)InputIsKeyDown('S') - (int)InputIsKeyDown('W'));
		moveDelta[2] = speed * ((int)InputIsKeyDown('E') - (int)InputIsKeyDown('C'));
		InputGetCursorPos(cursorPos, cursorPos + 1);
		rotateDelta[0] += cursorPos[0] - xCenter;
		rotateDelta[1] += cursorPos[1] - yCenter;
		InputSetCursorPos(xCenter, yCenter);
	}
	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
		GLWindowSetSize(window.width, window.height, !window.fullScreen);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("CG.log");

	if (!GLWindowCreate("CG", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
