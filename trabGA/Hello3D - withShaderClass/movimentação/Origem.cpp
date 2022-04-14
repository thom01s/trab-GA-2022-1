/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "objloader.h"


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Protótipos das funções
int setupGeometry();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;


bool view1 = false, view2 = false, view3 = false, view4 = false, view5 = false, firstMouse = true, rotatex = false, rotatey = false, rotatez = false, modelx = false, modely = false, modelz = false;

glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
glm::vec3 front;

float lastX = WIDTH/2.0 , lastY = HEIGHT/2.0;
float yaw = -90.0f;
float pitch = 0.0f;

float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Compilando e buildando o programa de shader
	Shader shader = Shader("../shaders/hello.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();

	// Read our .obj file
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.
	bool res = loadOBJ("pikachu.obj", vertices, uvs, normals);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glUseProgram(shader.ID);

	cameraPos = glm::vec3(0.0, 0.0, 3.0);
	cameraFront = glm::vec3(0.0, 0.0, -1.0);
	cameraUp = glm::vec3(0.0, 1.0, 0.0);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");

	glm::mat4 view = glm::mat4(1); //matriz identidade;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1); //matriz identidade;
	GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");
	projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));

	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glEnable(GL_DEPTH_TEST);

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		model = glm::mat4(1);

		glm::mat4 view = glm::mat4(1); //matriz identidade;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		if (view1)
		{
			view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view2)
		{
			view = glm::lookAt(glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view3)
		{
			view = glm::lookAt(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view4)
		{
			view = glm::lookAt(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (view5)
		{
			view = glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (rotatex)
		{
			model = glm::rotate(model, currentFrame*glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else if (rotatey)
		{
			model = glm::rotate(model, currentFrame*glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else if (rotatez)
		{
			model = glm::rotate(model, currentFrame*glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		}

		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, FALSE, glm::value_ptr(projection));

		

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 42);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP

		glDrawArrays(GL_POINTS, 0, 42);
		glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float cameraSpeed = 0.1f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		modelx = true;
		modely = false;
		modelz = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		modelx = false;
		modely = true;
		modelz = false;
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		modelx = false;
		modely = false;
		modelz = true;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		view1 = true;
		view2 = false;
		view3 = false;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = true;
		view3 = false;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = true;
		view4 = false;
		view5 = false;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = false;
		view4 = true;
		view5 = false;
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		view1 = false;
		view2 = false;
		view3 = false;
		view4 = false;
		view5 = true;
	}

}



void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {

		//Base da pirâmide: 2 triângulos
		//x     y      z    r    g    b
		 -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		 -0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		 -0.5, -0.5,  0.5, 1.0, 1.0, 0.0,
		  0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,

		  //lado 1
		  -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		   0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		  -0.5,  0.5, -0.5, 1.0, 1.0, 0.0,

		  -0.5,  0.5, -0.5, 1.0, 1.0, 0.0,
		   0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		   0.5,  0.5, -0.5, 1.0, 1.0, 0.0,

		   //lado 2

			-0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
			-0.5,  0.5, -0.5, 1.0, 0.0, 1.0,
			-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,

			-0.5,  0.5, -0.5, 1.0, 0.0, 1.0,
			-0.5,  0.5,  0.5, 1.0, 0.0, 1.0,
			-0.5, -0.5,  0.5, 1.0, 0.0, 1.0,

			//lado 3

			0.5, -0.5, 0.5, 0.0, 1.0, 1.0,
			0.5, -0.5,-0.5, 0.0, 1.0, 1.0,
			0.5,  0.5, 0.5, 0.0, 1.0, 1.0,

			0.5,  0.5, -0.5, 0.0, 1.0, 1.0,
			0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
			0.5,  0.5,  0.5, 0.0, 1.0, 1.0,

			//lado 4 

			0.5, -0.5, 0.5, 0.0, 0.0, 1.0,
			0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
		   -0.5, -0.5, 0.5, 0.0, 0.0, 1.0,

		   -0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
			0.5,  0.5, 0.5, 0.0, 0.0, 1.0,
		   -0.5, -0.5, 0.5, 0.0, 0.0, 1.0,

		   //topo
		   -0.5, 0.5, -0.5, 1.0, 1.0, 0.0,
		   -0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
			0.5, 0.5, -0.5, 1.0, 0.0, 1.0,

		   -0.5, 0.5,  0.5, 1.0, 1.0, 0.0,
			0.5, 0.5,  0.5, 0.0, 1.0, 1.0,
			0.5, 0.5, -0.5, 1.0, 0.0, 1.0,

			//chão 
			 1.0, -0.5,  1.0, 0.3, 1.0, 0.3,
			 1.0, -0.5, -1.0, 0.3, 1.0, 0.3,
			-1.0, -0.5,  1.0, 0.3, 1.0, 0.3,

			 1.0, -0.5, -1.0, 0.3, 1.0, 0.3,
			-1.0, -0.5,  1.0, 0.3, 1.0, 0.3,
			-1.0, -0.5, -1.0, 0.3, 1.0, 0.3
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);



	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

