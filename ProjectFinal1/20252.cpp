/*---------------------------------------------------------*/
/* ----------------   Proyecto  --------------------------*/
/*-----------------    2025-2   ---------------------------*/
/*------------- Alumno: Cuevas Gonz�lez Dalia                   ---------------*/
/*------------- No. Cuenta: 316113919 ---------------*/


#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL3/SDL.h>
//#include <SDL3/SDL_mixer.h>
#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Audio
Mix_Music* gBackgroundMusic = nullptr;

//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la pr�ctica 6
unsigned int generateTextures(char*, bool, bool);	// De la pr�ctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f;

//Texture
unsigned int	t_smile,
t_toalla,
t_unam,
t_white,
t_ladrillos;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);

// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulaci�n y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir n�mero en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{

	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}


//bool initAudio() {
//	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
//		std::cerr <<"SDL_mixer no pudo inicializarse! SDL_mixer ERROR: " << Mix_GetError() << std::endl;
//		return false;
//	}
//	return true;
//}
//
//bool loadAndPlayMusic(const std::string& musicPath) {
//	// Cargar m�sica
//	gBackgroundMusic = Mix_LoadMUS(musicPath.c_str());
//	if (gBackgroundMusic == nullptr) {
//		std::cerr << "No se pudo cargar la m�sica de fondo! SDL_mixer Error: " << Mix_GetError() << std::endl;
//		return false;
//	}
//
//	// Reproducir m�sica en bucle infinito (-1)
//	if (Mix_PlayMusic(gBackgroundMusic, -1) == -1) {
//		std::cerr << "No se pudo reproducir la m�sica de fondo! SDL_mixer Error: " << Mix_GetError() << std::endl;
//		return false;
//	}
//
//	return true;
//}
//
//// Funci�n para manejar controles b�sicos de m�sica (puedes llamarla en tu bucle de eventos)
//void handleMusicControls(SDL_Event& e) {
//	if (e.type == SDL_KEYDOWN) {
//		switch (e.key.keysym.sym) {
//		case SDLK_UP:
//			// Aumentar volumen
//		{
//			int volume = Mix_VolumeMusic(-1) + 8;
//			if (volume > MIX_MAX_VOLUME) volume = MIX_MAX_VOLUME;
//			Mix_VolumeMusic(volume);
//		}
//		break;
//
//		case SDLK_DOWN:
//			// Disminuir volumen
//		{
//			int volume = Mix_VolumeMusic(-1) - 8;
//			if (volume < 0) volume = 0;
//			Mix_VolumeMusic(volume);
//		}
//		break;
//
//		case SDLK_p:
//			// Pausar/reanudar m�sica
//			if (Mix_PausedMusic()) {
//				Mix_ResumeMusic();
//			}
//			else {
//				Mix_PauseMusic();
//			}
//			break;
//
//		case SDLK_m:
//			// Silenciar/activar m�sica
//			static int savedVolume = MIX_MAX_VOLUME;
//			if (Mix_VolumeMusic(-1) > 0) {
//				savedVolume = Mix_VolumeMusic(-1);
//				Mix_VolumeMusic(0);
//			}
//			else {
//				Mix_VolumeMusic(savedVolume);
//			}
//			break;
//		}
//	}
//}
//
//// Funci�n para liberar recursos de m�sica (ll�mala antes de SDL_Quit)
//void closeAudio() {
//	// Liberar recursos de m�sica
//	Mix_FreeMusic(gBackgroundMusic);
//	gBackgroundMusic = nullptr;
//	Mix_CloseAudio();
//}

void animate(void) 
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	//Veh�culo
	if (animacion)
	{
		movAuto_x += 3.0f;
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,	//V0
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,	//V3
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,	//V7

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,	//V3
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, -0.5f, 0.5f,		0.0f, 1.0f,	//V0

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,	//V2

		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,	//V4
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V7
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	1.0f, 1.0f,	//V3
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		-0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V0
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,	//V2
		0.5f, -0.5f, 0.5f,		1.0f, 1.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL no pudo inicializarse!" << std::endl;
		return -1;
	}

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pratica X 2025-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}






	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);



	//if (!initAudio()) {
	//	std::cerr << "No se pudo inicializar el audio. El programa continuar� sin sonido." << std::endl;
	//	// El programa puede continuar sin audio
	//}
	//else {
	//	// Cargar y reproducir m�sica de fondo (reemplaza con la ruta a tu archivo de m�sica)
	//	if (!loadAndPlayMusic("audio/Test Drive (From How To Train Your Dragon Music From The Motion Picture)-yt.savetube.me.mp3")) {
	//		std::cerr << "No se pudo cargar la m�sica. El programa continuar� sin m�sica de fondo." << std::endl;
	//	}
	//}

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 

	vector<std::string> faces{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	Model piso("resources/objects/piso/piso.obj");
	Model carro("resources/objects/lambo/carroceria.obj");
	Model llanta("resources/objects/lambo/Wheel.obj");
	Model casaVieja("resources/objects/casa/OldHouse.obj");
	//Model cubo("resources/objects/cubo/cube02.obj");
	Model casaDoll("resources/objects/casa/DollHouse.obj");
	Model laptop("resources/objects/Obj1/iMAC.obj");
	Model picosd0("resources/objects/Dragon0/picosd0.obj");
	Model carad0("resources/objects/Dragon0/cabezad0.obj");
	Model patasd0("resources/objects/Dragon0/patasd0.obj");
	Model torsod0("resources/objects/Dragon0/torsod0.obj");

	Model aladerd1("resources/objects/Dragon1/aladerd1.obj");
	Model alaizqd1("resources/objects/Dragon1/alaizqd1.obj");
	Model bocad1("resources/objects/Dragon1/bocad1.obj");
	Model dientesd1("resources/objects/Dragon1/dientesd1.obj");
	Model garrasd1("resources/objects/Dragon1/garrasd1.obj");
	Model lenguad1("resources/objects/Dragon1/lenguad1.obj");
	Model ojosd1("resources/objects/Dragon1/ojosd1.obj");
	Model patad1("resources/objects/Dragon1/patad1.obj");
	Model torsod1("resources/objects/Dragon1/torsod1.obj");
	Model garrasd11("resources/objects/Dragon1/garrasd11.obj");

	Model aladerd2("resources/objects/Dragon2/aladerd2.obj");
	Model alaizqd2("resources/objects/Dragon2/alaizqderd2.obj");
	Model bocad2("resources/objects/Dragon2/bocad2.obj");
	Model dientesd2("resources/objects/Dragon2/dientesd2.obj");
	Model garrascomd2("resources/objects/Dragon2/garrascomd2.obj");
	Model garrasd2("resources/objects/Dragon2/garrasd2.obj");
	Model lenguad2("resources/objects/Dragon2/lenguad2.obj");
	Model ojosd2("resources/objects/Dragon2/ojosd2.obj");
	Model pataderd2("resources/objects/Dragon2/pataderd2.obj");
	Model torsod2("resources/objects/Dragon2/torsod2.obj");

	Model salon1("resources/objects/salongrafica/salongrafica.obj");
	Model salon2("resources/objects/saloncisco/saloncisco2.obj");
	//Model salon1a("resources/objects/salongrafica/sillasmesas.obj");

	//Model pataizqd0("resources/objects/Dragon0/Pataizq0.obj");
	//Model pataderad0("resources/objects/Dragon0/Pataderad0.obj");
	//Model pataizqad0("resources/objects/Dragon0/PAtaizqa0.obj");

	ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	animacionPersonaje.initShaders(animShader.ID);


	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection
	glm::mat4 cuerpod0 = glm::mat4(1.0f);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection);
		staticShader.setVec3("dirLight.ambient", ambientColor);
		staticShader.setVec3("dirLight.diffuse", diffuseColor);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));

		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(0.0f, 20.0f, 10.0f));
		staticShader.setVec3("spotLight[0].direction", glm::vec3(0.0f, -1.0f, 0.0f));
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(60.0f)));
		staticShader.setFloat("spotLight[0].constant", 1.0f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", ambientColor);
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------




		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar c�digo aqu�
		modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[0]);
		//Colocar c�digo aqu�
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		/***   Segundo objeto  **/
		/*
		glBindVertexArray(VAO[1]);
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_unam);
		glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		*/
		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//casaDoll.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		//piso.Draw(staticShader);

		//modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		//modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		//staticShader.setMat4("model", modelOp);
		//staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//casaVieja.Draw(staticShader);

		//---------------------------------------------------------------------------------------------------------------------------
		//Modelos agregados para la estructuracion del proyecto



		//---------------------------------------------------------------------------------------------------------------------------
		//Modelos de personajes secundario: dragon0 
		//---------------------------------------------------------------------------------------------------------------------------
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 29.0f, 25.8f));  //50 s 57 s 115 s
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		picosd0.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 24.1f, 0.0f)); //Al parecer si funcionan como x,y,z?  50 s   39 s  20 s
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		carad0.Draw(staticShader);

		modelOp = glm::translate(cuerpod0, glm::vec3(0.0f, -1.61f, -20.46f));  //50 s  39.7s  58.2 s
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		patasd0.Draw(staticShader);

		cuerpod0 = modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.3f, 28.8f)); //50 s 45s  127s
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		torsod0.Draw(staticShader);


		//---------------------------------------------------------------------------------------------------------------------------
        //Modelos de personajes secundario: dragon1 
        //---------------------------------------------------------------------------------------------------------------------------
		//Model aladerd1("resources/objects/Dragon1/aladerd1.obj");
		//Model alaizqd1("resources/objects/Dragon1/alaizqd1.obj");
		//Model bocad1("resources/objects/Dragon1/bocad1.obj");
		//Model dientesd1("resources/objects/Dragon1/dientesd1.obj");
		//Model garrasd1("resources/objects/Dragon1/garrasd1.obj");
		//Model lenguad1("resources/objects/Dragon1/lenguad1.obj");
		//Model ojosd1("resources/objects/Dragon1/ojosd1.obj");
		//Model patad1("resources/objects/Dragon1/patad1.obj");
		//Model torsod1("resources/objects/Dragon1/torsod1.obj");

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 10.0f, 40.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		patad1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(41.0f, 10.0f, 41.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		aladerd1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(43.0f, 10.0f, 43.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		alaizqd1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(45.0f, 10.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		torsod1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 20.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		bocad1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 25.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		dientesd1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 27.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		garrasd1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 30.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		lenguad1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 33.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		ojosd1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 36.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		garrasd11.Draw(staticShader);


		//---------------------------------------------------------------------------------------------------------------------------
		//Modelos de personajes secundario: dragon2 
		//---------------------------------------------------------------------------------------------------------------------------
		//Model aladerd2("resources/objects/Dragon2/aladerd2.obj");
		//Model alaizqd2("resources/objects/Dragon2/alaizqderd2.obj");
		//Model bocad2("resources/objects/Dragon2/bocad2.obj");
		//Model dientesd2("resources/objects/Dragon2/dientesd2.obj");
		//Model garrascomd2("resources/objects/Dragon2/garrascomd2.obj");
		//Model garrasd2("resources/objects/Dragon2/garrasd2.obj");
		//Model lenguad2("resources/objects/Dragon2/lenguad2.obj");
		//Model ojosd2("resources/objects/Dragon2/ojosd2.obj");
		//Model pataderd2("resources/objects/Dragon2/pataderd2.obj");
		//Model torsod2("resources/objects/Dragon2/torsod2.obj");

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 40.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		aladerd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 10.0f, 41.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		alaizqd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 10.0f, 43.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		bocad2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 10.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		dientesd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 20.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		garrascomd2.Draw(staticShader); 

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 25.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		garrasd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 27.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		lenguad2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 30.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		ojosd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 33.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		pataderd2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 36.0f, 45.0f));
		modelOp = glm::scale(modelOp, glm::vec3(20.0f));
		staticShader.setMat4("model", modelOp);
		torsod2.Draw(staticShader);

	/*	modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(6.5f, 36.47f, 122.649f));
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		pataizqd0.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 35.72f, -92.39f));
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		pataderad0.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(61.7f, 65.3f, 69.2f));
		modelOp = glm::scale(modelOp, glm::vec3(0.4f));
		staticShader.setMat4("model", modelOp);
		pataizqad0.Draw(staticShader);*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		//modelOp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movAuto_x, -1.0f, movAuto_z - 15.0f));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setMat4("model", modelOp);
		//carro.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Izq delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Der delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Der trasera

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		//llanta.Draw(staticShader);	//Izq trase


		modelOp = glm::translate(tmp, glm::vec3(5.5f, 8.5f, 10.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		salon1.Draw(staticShader);	

		modelOp = glm::translate(tmp, glm::vec3(5.5f, 8.5f, 20.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		salon2.Draw(staticShader);
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		// -------------------------------------------------------------------------------------------------------------------------
		// Just in case
		// -------------------------------------------------------------------------------------------------------------------------
		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		torso.Draw(staticShader);

		//Pierna Der
		modelOp = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::rotate(modelOp, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaDer.Draw(staticShader);

		//Pie Der
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);

		//Pierna Izq
		modelOp = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(-0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoDer.Draw(staticShader);

		//Brazo izquierdo
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoIzq.Draw(staticShader);

		//Cabeza
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", modelOp);
		cabeza.Draw(staticShader);*/

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//Mix_FreeMusic(backgroundMusic);
	//Mix_FreeChunk(soundEffect);
	//Mix_CloseAudio();
	//SDL_QuitSubSystem(SDL_INIT_AUDIO);
	//skybox.Terminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

	//if (key == GLFW_KEY_M && action == GLFW_PRESS) {
	//	if (Mix_PlayingMusic()) {
	//		if (Mix_PausedMusic()) {
	//			Mix_ResumeMusic();
	//		}
	//		else {
	//			Mix_PauseMusic();
	//		}
	//	}
	//}

	//if (key == GLFW_KEY_N && action == GLFW_PRESS) {
	//	Mix_HaltMusic();
	//	isMusicPlaying = false;
	//}

	//if (key == GLFW_KEY_O && action == GLFW_PRESS) {
	//	playSoundEffect();
	//}

	//bool quit = false;
	//SDL_Event e;

	//while (!quit) {
	//	// Manejo de eventos
	//	while (SDL_PollEvent(&e)) {
	//		if (e.type == SDL_QUIT) {
	//			quit = true;
	//		}

	//		// A�ade esto para controlar la m�sica (opcional)
	//		handleMusicControls(e);
	//	}

	//}

	//// Antes de salir, libera los recursos de audio (a�ade esto antes de SDL_Quit)
	//closeAudio();
	//return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}