#ifndef _Scene_
#define _Scene_

#include "openglclass.h"
#include "cameraclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "Land.h"
#include "inputclass.h"
#include "KeyCode.h"
#include "VectorRR.h"
#include "Model.h"
#include "Triangle.h"
#include "colorshaderclass.h"
#include "ShaderBoundingBox.h"
#include "GameObject.h"
#include "Shader.h"
#include "TextureShader.h"
#include "TextureLoaderForOpenGL.h"
#include "Dome.h"
#include "ShaderSkyDome.h"
#include "Billboard.h"
#include "ShaderBillboard.h"
#include "BoundingBoxRenderer.h"
#include <string>

// GLOBALS //
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float SPEED_AXIS_Y = 0.05f;
const float SPEED_AXIS_X = 0.05f;
const float SPEED_CAMERA = 0.10f;
const float InitialPosition[3] = { 0.0f, 11.0f, -10.0f };
const float InitialRotation[3] = { 0.0f, 0.0f, 0.0f };


using std::vector;
using namespace std;

class Scene {
private:
	//Estructuras
	struct informacionDColision
	{
		float Position[3];
		float Address[3];
		float Speed[3];
	};
	struct Vector {
		float X;
		float Y;
		float Z;
	};
	
	// Variables de movimiento y rotacion del personaje
	Vector* DeltaPosition = new Vector();
	Vector* DeltaRotation = new Vector();
	Vector* LastDeltaPosition = new Vector();

	//Dependencias Externas
	HWND handlerWindow = NULL;

	//Motor Grafico
	OpenGLClass* OpenGL;

	//Elementos Primarios
	CameraClass* Camera;
	Land* Terreno;
	Land* Agua;
	Dome* Skydome;
	informacionDColision* prevColision;
	TriangleClass* Triangulo;

	//Shaders
	LightShaderClass* LightShader;
	ColorShaderClass* ColorShader;
	ShaderBoundingBox* ShaderBounding;
	ShaderTexture* ShaderModel;
	SkydomeShaderClass* ShaderSkydome;
	BillboardShaderClass* ShaderBillboard;
	ShaderBoundingBox* ShaderBox;
	BillboardShaderClass* ShaderBill;
	
	//Lights
	LightClass* Light;

	//Input
	KeyBoard KeyCode;
	InputClass* input;
	float deltaTime;

	TextureClass* LoaderTexture;
	//Objetos
	GameObject* Casa;
	GameObject* Casa2;
	GameObject* Torre;
	GameObject* Dragon;
	GameObject* Dragon2;
	GameObject* DragonFalso;
	GameObject* Castillo;
	GameObject* Pozo;
	GameObject* Carreta;
	GameObject* Vaca;
	GameObject* Vaca2;
	GameObject* Vaca3;
	GameObject* Vaca4;
	GameObject* Vaca5;
	GameObject* Vaca6;
	GameObject* ArbolAhorcado;

	//Billboards
	Billboard* Arbol;
	Billboard* Arbol2;
	Billboard* ArbolQuemado;
	Billboard* Arbusto;
	Billboard* Flores;
	Billboard* Pajaros;
	Billboard* Bosque;
	Billboard* Ahorcado;

	//Colisiones casa habitable
	BoundingBoxRenderer* Exterior1;
	BoundingBoxRenderer* Exterior11;
	BoundingBoxRenderer* Exterior2;
	BoundingBoxRenderer* Exterior3;
	BoundingBoxRenderer* Exterior4;
	BoundingBoxRenderer* Object3d1;

	//Limites del mapa
	BoundingBoxRenderer* Limite1;
	BoundingBoxRenderer* Limite2;
	BoundingBoxRenderer* Limite3;
	BoundingBoxRenderer* Limite4;

	//Variables de Movimiento (Traslacion)
	float speedAxisX;
	float speedAxisY;
	float speedTAxisX;
	float speedTAxisZ;
	float speed;

	//Variables de Rotacion
	float angulo;
	float scale;
	bool Reset;
	bool volar;
	float PosX;
	float PosY;
	float PosZ;
	float RotX;
	float RotY;
	float RotZ;
	float sensY;
	float profundidadZ;
	float profundidadY;
	bool colision;
	//Variables de representacion (Color)
	float color[3];

	//Camara
	float camaraX;
	float camaraY;
	float camaraZ;

	//Banderas de habilitacion
	bool RenderObject3d = true;
	bool RenderBilboard = true;
	

public:
	Scene(OpenGLClass* OpenGLObject, HWND hwnd);
	~Scene();

	bool Initialize();
	// Funcion cuya informacion de teclado o gamepad NO se le transfiere
	bool Render();
	bool Update(InputClass* input, float deltaTime);
	bool ManageCommands();
	bool Shutdown();

protected:


};

#endif //_Scene_