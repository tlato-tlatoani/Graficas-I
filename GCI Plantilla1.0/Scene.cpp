#include "Scene.h"

Scene::Scene(OpenGLClass* OpenGLObject, HWND hwnd) {
	handlerWindow = hwnd;
	OpenGL = OpenGLObject;
	Camera = 0;
	LightShader = 0;
	ColorShader = 0;
	ShaderBounding = 0;
	Light = 0;
	speedAxisX = 0.0f;
	speedAxisY = 0.0f;
	speed = 0.0f;
	angulo = 0.0f;
	LoaderTexture = new TextureClass(OpenGL);
	PosX = 20.0f;
	PosY = 20.0f;
	PosZ = -30.0f;
	RotY = 315 * (M_PI / 180.0f);
	colision = false;
	volar = false;
}

Scene::~Scene() {

}

bool Scene::Initialize() {
	bool result = true;
	string resultDetail = "";

	angulo = 0.0f;
	speedAxisX = SPEED_AXIS_X;
	speedAxisY = SPEED_AXIS_Y;
	speed = SPEED_CAMERA;


	// Crea un objeto camara.
	Camera = new CameraClass;
	if (!Camera) {
		result = false;
		_RPT1(0, "Alert! Camera has not been initialized. \n", 0);
		return result;
	}
	else {
		// Asigna las coordenadas de posicion y rotacion por defecto a los arreglos delta.
		DeltaPosition->X = InitialPosition[0]; DeltaPosition->Y = InitialPosition[1]; DeltaPosition->Z = InitialPosition[2];
		DeltaRotation->X = InitialRotation[0]; DeltaRotation->Y = InitialRotation[1]; DeltaRotation->Z = InitialRotation[2];
		// Luego se los pasamos a la camara.
		Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
		Camera->SetRotation(DeltaRotation->X, DeltaRotation->Y, DeltaRotation->Z);
		// Con esto podriamos aislar siempre el punto de partida inicial de la escena para evitar teletrasportarse sin querer.
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Carga de Terrenos
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Skydome
	ShaderSkydome = new SkydomeShaderClass(OpenGL, handlerWindow, "shaders/SkydomeShader.vs", "shaders/SkydomeShader.ps");
	if (!ShaderSkydome) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader skydome.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	Skydome = new Dome("recursos/tarde.jpg", OpenGL, LoaderTexture, 500);
	if (!Skydome) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the Skydome.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Skydome->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the Skydome.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Skydome->SetIterpolationDay(false, true);

		Skydome->SetShader(ShaderSkydome);
	}

	ShaderBox = new ShaderBoundingBox(OpenGL, handlerWindow, "shaders/vertexcolor.vs", "shaders/vertexcolor.ps");
	if (!ShaderBox) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader model object.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	LightShader = new LightShaderClass(OpenGL, handlerWindow, "shaders/terrain.vs", "shaders/terrain.ps");
	if (!LightShader) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the light shader object.", "Error", MB_OK);
		_RPT1(0, "Alert! LightShader has an error on declare and not been initialized. \n", 0);
		return result;
	}

	ColorShader = new ColorShaderClass(OpenGL, handlerWindow, "shaders/color.vs", "shaders/color.ps");
	if (!ColorShader) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the color shader object.", "Error", MB_OK);
		_RPT1(0, "Alert! ColorShader has an error on declare and not been initialized. \n", 0);
		return result;
	}

	Terreno = new Land(handlerWindow, OpenGL, LoaderTexture, L"recursos/mapaAlturas.jpg", string("recursos/piso1.jpg"), string("recursos/texterr2.jpg"), string("recursos/mapaAlturas.jpg"), (float)400, (float)400);
	if (!Terreno) {
		result = false;
		_RPT1(0, "Alert! Land has not been initialized. \n", 0);
		return result;
	}
	else {
		Terreno->SetLandShader(LightShader);
	}

	Agua = new Land(handlerWindow, OpenGL, LoaderTexture, L"recursos/watermap1.jpg", "recursos/agua.jpg", "recursos/agua.jpg", "recursos/watermap1.jpg", (float)400, (float)400);
	if (!Agua) {
		result = false;
		_RPT1(0, "Alert! Land has not been initialized. \n", 0);
		return result;
	}
	else {
		Agua->SetLandShader(LightShader);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Carga de Modelos
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Shader para los modelos
	ShaderModel = new ShaderTexture(OpenGL, handlerWindow, "shaders/light.vs", "shaders/light.ps");
	if (!ShaderModel) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader model object.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	// Carga de modelos
	//--------------- Casa
	Casa = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/tienda.obj",
		"modelos/tienda0.png", "modelos/tienda1.png");
	if (!Casa) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Casa->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Casa->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Casa habitable
	Casa2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/tienda.obj",
		"modelos/tienda0.png","modelos/tienda1.png");
	if (!Casa2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Casa2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Casa2->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Torre
	Torre = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/torre.obj",
		"modelos/torre.png");
	if (!Torre) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Torre->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Torre->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Globo
	Dragon = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/globo.obj", "modelos/globo0.png", "modelos/globo1.png");
	if (!Dragon) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Dragon->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Dragon->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- globo montable
	Dragon2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/globo.obj", "modelos/globo0.png", "modelos/globo1.png");
	if (!Dragon2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Dragon2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Dragon2->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Pozo
	Pozo = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/pozo.obj", "modelos/pozo.png");
	if (!Pozo) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Pozo->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Pozo->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Carreta
	Carreta = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/carreta1.obj", "modelos/carreta.png");
	if (!Carreta) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Carreta->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Carreta->SetShaders(ShaderModel, ShaderBox);
	}
	//--------------- Vaca
	Vaca = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca->SetShaders(ShaderModel, ShaderBox);
	}
	Vaca2 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca2) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca2->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca2->SetShaders(ShaderModel, ShaderBox);
	}
	Vaca3 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca3) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca3->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca3->SetShaders(ShaderModel, ShaderBox);
	}
	Vaca4 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca4) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca4->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca4->SetShaders(ShaderModel, ShaderBox);
	}
	Vaca5 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca5) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca5->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca5->SetShaders(ShaderModel, ShaderBox);
	}
	Vaca6 = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/vaca.obj", "modelos/vaca.png", "modelos/vacaNmap.png");
	if (!Vaca6) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = Vaca6->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		Vaca6->SetShaders(ShaderModel, ShaderBox);
	}
	ArbolAhorcado = new GameObject(OpenGL, handlerWindow, LoaderTexture,
		"modelos/arbolColgado2.obj", "modelos/arbolColgado.png");
	if (!ArbolAhorcado) {
		result = false;
		MessageBoxA(handlerWindow, "Could not initialize the GameObject.", "Error", MB_OK);
		_RPT1(0, "Alert! GameObject has an error on start. \n", 0);
		return result;
	}
	else {
		result = ArbolAhorcado->Initialize();
		if (!result) {
			MessageBoxA(handlerWindow, "Could not initialize the model of Gameobject.", "Error", MB_OK);
			_RPT1(0, "Alert! GameObject has an error on initialize. \n", 0);
			return result;
		}
		ArbolAhorcado->SetShaders(ShaderModel, ShaderBox);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Carga de Billboards
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	ShaderBillboard = new BillboardShaderClass(OpenGL, handlerWindow, "shaders/billboard.vs", "shaders/billboard.ps");
	if (!ShaderBillboard) {
		result = false;
		MessageBoxA(handlerWindow, "Could not declared and initialized the shader billboard.", "Error", MB_OK);
		_RPT1(0, "Alert! ShaderModel has an error on declare and not been initialized. \n", 0);
		return result;
	}

	Arbol = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/arbol.png");
	Arbol->Initialize(16.0f, 16.0f);
	Arbol->SetShader(ShaderBillboard);

	Arbol2 = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/arbol2.png");
	Arbol2->Initialize(13.0f, 13.0f);
	Arbol2->SetShader(ShaderBillboard);

	ArbolQuemado = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/arbolquemado1.png");
	ArbolQuemado->Initialize(20.0f, 20.0f);
	ArbolQuemado->SetShader(ShaderBillboard);

	Pajaros = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/pajaro.png");
	Pajaros->Initialize(9.0f, 9.0f);
	Pajaros->SetShader(ShaderBillboard);

	Flores = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/flores1.png");
	Flores->Initialize(4.0f, 4.0f);
	Flores->SetShader(ShaderBillboard);

	Bosque = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/bosque1.png");
	Bosque->Initialize(300.0f, 120.0f);
	Bosque->SetShader(ShaderBillboard);

	Ahorcado = new Billboard(OpenGL, handlerWindow, LoaderTexture, "recursos/Billboards/ahorcado.png");
	Ahorcado->Initialize(10.0f, 10.0f);
	Ahorcado->SetShader(ShaderBillboard);

	Arbol->SetShader(ShaderBillboard);
	Arbol2->SetShader(ShaderBillboard);
	ArbolQuemado->SetShader(ShaderBillboard);
	Pajaros->SetShader(ShaderBillboard);
	Flores->SetShader(ShaderBillboard);
	Bosque->SetShader(ShaderBillboard);
	Ahorcado->SetShader(ShaderBillboard);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Carga de Colisiones
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	Limite1 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -199.0f, -10.0f, 199.0f }, BoundingBox::GLFLOAT3{ -180.0f, 150.0f, -230.0f });
	Limite1->SetShader(ShaderBox);

	Limite2 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ 199.0f, -10.0f, 199.0f }, BoundingBox::GLFLOAT3{ 194.0f, 150.0f, -230.0f });
	Limite2->SetShader(ShaderBox);

	Limite3 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -199.0f, -10.0f, -199.0f }, BoundingBox::GLFLOAT3{ 199.0f, 150.0f, -190.0f });
	Limite3->SetShader(ShaderBox);

	Limite4 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ 199.0f, -10.0f, 190.0f }, BoundingBox::GLFLOAT3{ -199.0f, 150.0f, 180.0f });
	Limite4->SetShader(ShaderBox);

	//Esquina inferior derecha a esquina superior derecha Pt.1
	Exterior1 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -132.0f, -10.0f, -90.0f }, BoundingBox::GLFLOAT3{ -135.0f, 50.0f, -81.5f });
	Exterior1->SetShader(ShaderBox);

	//Esquina inferior derecha a esquina superior derecha Pt.2
	Exterior11 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -132.0f, -10.0f, -76.0f }, BoundingBox::GLFLOAT3{ -135.0f, 50.0f, -30.0f });
	Exterior11->SetShader(ShaderBox);

	//Esquina inferior izquiera a esquina inferior derecha
	Exterior2 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -170.0f, -10.0f, -90.0f }, BoundingBox::GLFLOAT3{ -130.0f, 50.0f, -87.0f });
	Exterior2->SetShader(ShaderBox);

	//Esquina superior izquiera a esquina superior derecha
	Exterior3 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -170.0f, -10.0f, -30.0f }, BoundingBox::GLFLOAT3{ -130.0f, 50.0f, -33.0f });
	Exterior3->SetShader(ShaderBox);

	//Esquina inferior izquiera a esquina superior izquierda
	Exterior4 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ -170.0f, -10.0f, -90.0f }, BoundingBox::GLFLOAT3{ -164.0f, 50.0f, -30.0f });
	Exterior4->SetShader(ShaderBox);

	Object3d1 = new BoundingBoxRenderer(OpenGL, BoundingBox::GLFLOAT3{ 0.0f, 0.0f , 0.0f}, 
		BoundingBox::GLFLOAT3{ 3.0f, 3.0f , 3.0f });
	Object3d1->SetShader(ShaderBox);

	return result;
}

bool Scene::Render() {
	bool result = true;

	LightClass luz;
	luz.SetDiffuseColor(100.0f, 43.0f, 185.0f, 0.0f);
	luz.SetDirection(199.0f, 70.0f, 0.0f);
	float diffuseColor[4];
	float direccion[4];
	luz.GetDiffuseColor(diffuseColor);
	luz.GetDirection(direccion);

	// Limpia los buffers para iniciar la escena nueva.
	OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Definimos nuestro espacio para las 3 matrices necesarias para renderizar.
	float worldMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];

	if (volar) {
		// Configuración de los offsets
		float offsetDistance = - 10.0f; // Distancia detrás del dragón
		float offsetHeight = 5.0f;    // Altura por encima del dragón

		// Convierte el ángulo de rotación del dragón a radianes
		float radians = RotY;

		// Calcula la posición de la cámara detrás del dragón
		float cameraX = PosX - sinf(radians) * offsetDistance;
		float cameraY = PosY + offsetHeight;
		float cameraZ = PosZ - cosf(radians) * offsetDistance;

		// Establece la posición de la cámara
		Camera->SetPosition(cameraX, cameraY, cameraZ);

		// Apunta la cámara hacia la posición del dragón
		Camera->SetRotation(DeltaRotation->X, RotY * (180.0f / M_PI) + 180.0f, 0.0f); // Mira hacia adelante
	}
	else {
		// Comportamiento normal de la cámara
		Camera->SetPosition(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);
		Camera->SetRotation(DeltaRotation->X, DeltaRotation->Y, DeltaRotation->Z);
	}
	Camera->Render();


		
	// Obtiene las matrices de vista, mundo y proyeccion de opengl y el objeto de la camara.
	OpenGL->GetWorldMatrix(worldMatrix);
	Camera->GetViewMatrix(viewMatrix);
	OpenGL->GetProjectionMatrix(projectionMatrix);

	// Renderizamos el Skydome
	Skydome->CicloInterpolaciones();
	Skydome->Render(viewMatrix, projectionMatrix);

	// Renderizamos terreno
	Terreno->Render(OpenGL, worldMatrix, viewMatrix, projectionMatrix);
	Agua->Render(OpenGL, worldMatrix, viewMatrix, projectionMatrix);

	// Renderizamos limites
	Limite1->Draw(viewMatrix, projectionMatrix);
	Limite2->Draw(viewMatrix, projectionMatrix);
	Limite3->Draw(viewMatrix, projectionMatrix);
	Limite4->Draw(viewMatrix, projectionMatrix);

	// Renderizamos colisiones
	Exterior1->Draw(viewMatrix, projectionMatrix);
	Exterior11->Draw(viewMatrix, projectionMatrix);
	Exterior2->Draw(viewMatrix, projectionMatrix);
	Exterior3->Draw(viewMatrix, projectionMatrix);
	Exterior4->Draw(viewMatrix, projectionMatrix);
	Object3d1->Draw(viewMatrix, projectionMatrix);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Renderizado de modelos
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	Casa->Render(viewMatrix, projectionMatrix, false);
	Casa2->Render(viewMatrix, projectionMatrix, false);
	Torre->Render(viewMatrix, projectionMatrix, false);
	Dragon->Render(viewMatrix, projectionMatrix, false);
	Dragon2->Render(viewMatrix, projectionMatrix, false);
	Carreta->Render(viewMatrix, projectionMatrix, false);
	Pozo->Render(viewMatrix, projectionMatrix, false);
	Vaca->Render(viewMatrix, projectionMatrix, false);
	Vaca2->Render(viewMatrix, projectionMatrix, false);
	Vaca3->Render(viewMatrix, projectionMatrix, false);
	Vaca4->Render(viewMatrix, projectionMatrix, false);
	Vaca5->Render(viewMatrix, projectionMatrix, false);
	Vaca6->Render(viewMatrix, projectionMatrix, false);
	ArbolAhorcado->Render(viewMatrix, projectionMatrix, false);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Renderizado de billboards
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Renderizado de bosque
	Arbol->Render(viewMatrix, projectionMatrix, 10.0f, Terreno->Superficie(10.0f, 10.0f) - 2, 10.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 20.0f, Terreno->Superficie(20.0f, 30.0f) - 2, 30.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 50.0f, Terreno->Superficie(50.0f, 20.0f) - 2, 20.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 80.0f, Terreno->Superficie(80.0f, 10.0f) - 2, 10.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 100.0f, Terreno->Superficie(100.0f, 50.0f) - 2, 50.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 120.0f, Terreno->Superficie(120.0f, 120.0f) - 2, 120.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 140.0f, Terreno->Superficie(140.0f, 183.0f) - 2, 183.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 160.0f, Terreno->Superficie(160.0f, 40.0f) - 2, 40.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 180.0f, Terreno->Superficie(180.0f, 20.0f) - 2, 20.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 190.0f, Terreno->Superficie(190.0f, 60.0f) - 2, 60.0f, DeltaPosition->X, DeltaPosition->Z);

	Arbol->Render(viewMatrix, projectionMatrix, 15.0f, Terreno->Superficie(15.0f, 140.0f) - 2, 140.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 35.0f, Terreno->Superficie(35.0f, 150.0f) - 2, 150.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 45.0f, Terreno->Superficie(45.0f, 120.0f) - 2, 120.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 60.0f, Terreno->Superficie(60.0f, 100.0f) - 2, 100.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 70.0f, Terreno->Superficie(70.0f, 110.0f) - 2, 110.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 85.0f, Terreno->Superficie(85.0f, 160.0f) - 2, 160.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 90.0f, Terreno->Superficie(90.0f, 170.0f) - 2, 170.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 95.0f, Terreno->Superficie(95.0f, 180.0f) - 2, 180.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 50.0f, Terreno->Superficie(50.0f, 130.0f) - 2, 130.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol->Render(viewMatrix, projectionMatrix, 20.0f, Terreno->Superficie(20.0f, 140.0f) - 2, 140.0f, DeltaPosition->X, DeltaPosition->Z);


	Arbol2->Render(viewMatrix, projectionMatrix, 25.0f, Terreno->Superficie(25.0f, 25.0f) - 4, 25.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 70.0f, Terreno->Superficie(70.0f, 70.0f) - 4, 70.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 110.0f, Terreno->Superficie(110.0f, 110.0f) - 4, 110.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 150.0f, Terreno->Superficie(150.0f, 150.0f) - 4, 150.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 30.0f, Terreno->Superficie(30.0f, 90.0f) - 4, 90.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 90.0f, Terreno->Superficie(90.0f, 30.0f) - 4, 30.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 140.0f, Terreno->Superficie(140.0f, 40.0f) - 4, 40.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 180.0f, Terreno->Superficie(180.0f, 130.0f) - 4, 130.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 60.0f, Terreno->Superficie(60.0f, 150.0f) - 4, 150.0f, DeltaPosition->X, DeltaPosition->Z);
	Arbol2->Render(viewMatrix, projectionMatrix, 130.0f, Terreno->Superficie(130.0f, 70.0f) - 4, 70.0f, DeltaPosition->X, DeltaPosition->Z);

	ArbolQuemado->Render(viewMatrix, projectionMatrix, 140.0f, Terreno->Superficie(159.0f, -175.0f) - 3, -175.0f, DeltaPosition->X, DeltaPosition->Z);
	ArbolQuemado->Render(viewMatrix, projectionMatrix, 180.0f, Terreno->Superficie(180.0f, -140.0f) - 3, -140.0f, DeltaPosition->X, DeltaPosition->Z);
	ArbolQuemado->Render(viewMatrix, projectionMatrix, 190.0f, Terreno->Superficie(190.0f, -160.0f) - 3, -160.0f, DeltaPosition->X, DeltaPosition->Z);

	// Renderizado de flores
	Flores->Render(viewMatrix, projectionMatrix, 40.0f, Terreno->Superficie(40.0f, 40.0f) - 3, 40.0f, DeltaPosition->X, DeltaPosition->Z);
	Flores->Render(viewMatrix, projectionMatrix, 55.0f, Terreno->Superficie(55.0f, 30.0f) - 3, 30.0f, DeltaPosition->X, DeltaPosition->Z);
	Flores->Render(viewMatrix, projectionMatrix, 80.0f, Terreno->Superficie(80.0f, 20.0f) - 3, 20.0f, DeltaPosition->X, DeltaPosition->Z);
	Flores->Render(viewMatrix, projectionMatrix, 100.0f, Terreno->Superficie(100.0f, 70.0f) - 3, 70.0f, DeltaPosition->X, DeltaPosition->Z);
	Flores->Render(viewMatrix, projectionMatrix, 120.0f, Terreno->Superficie(120.0f, 40.0f) - 3, 40.0f, DeltaPosition->X, DeltaPosition->Z);

	// Renderizado de pájaros
	Pajaros->Render(viewMatrix, projectionMatrix, 190.0f, Terreno->Superficie(190.0f, -160.0f) + 39, -160.0f, DeltaPosition->X, DeltaPosition->Z);
	Pajaros->Render(viewMatrix, projectionMatrix, 180.0f, Terreno->Superficie(180.0f, -140.0f) + 25, -140.0f, DeltaPosition->X, DeltaPosition->Z);
	Pajaros->Render(viewMatrix, projectionMatrix, 140.0f, Terreno->Superficie(140.0f, -160.0f) + 35, -160.0f, DeltaPosition->X, DeltaPosition->Z);
	Pajaros->Render(viewMatrix, projectionMatrix, 187.0f, Terreno->Superficie(187.0f, -166.0f) + 27, -166.0f, DeltaPosition->X, DeltaPosition->Z);
	
	// Renderizado de fondo
	Bosque->Render(viewMatrix, projectionMatrix, 197.0f, Terreno->Superficie(197.0f, 0.0f) - 30.0f, 0.0f, 0.0f, 0.0f);
	Bosque->Render(viewMatrix, projectionMatrix, -197.0f, Terreno->Superficie(-197.0f, 0.0f) - 30.0f, 0.0f, 0.0f, 0.0f);
	Bosque->Render(viewMatrix, projectionMatrix, 0.0f, Terreno->Superficie(0.0f, -197.0f) - 30.0f, -197.0f, 0.0f, 0.0f);
	Bosque->Render(viewMatrix, projectionMatrix, 0.0f, Terreno->Superficie(0.0f, 197.0f) - 30.0f, 197.0f, 0.0f, 0.0f);

	Ahorcado-> Render(viewMatrix, projectionMatrix, -128.36f, 45.0f, -132.94f, DeltaPosition->X, DeltaPosition->Z);

	// Damos la instruccion de que termino la escena para que nos muestre frame a frame.
	OpenGL->EndScene();

	return result;
}

bool Scene::Update(InputClass* input, float deltaTime) {
	bool result = true;
	float rotationSpeed = 3.0f;
	this->input = input;
	this->deltaTime = deltaTime;
	result = ManageCommands();
	if (!result) {
		result = false;
		_RPT1(0, "Alert! Error on key. \n", 0);
		return result;
	}
	if (DeltaPosition->X > 198 && DeltaPosition->X < 199) {
		DeltaPosition->X = 197;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (DeltaPosition->Z > 198 && DeltaPosition->Z < 199) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = 197;
	}
	if (DeltaPosition->X < -198 && DeltaPosition->X > -199) {
		DeltaPosition->X = -197;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (DeltaPosition->Z < -198 && DeltaPosition->Z > -199) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = -197;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Transformaciones de modelos
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Renderizado de casa 1
	float* matrixcasa1 = Casa->GetWorldMatrix();
	float rotationMatrixCasa[16];
	float translationMatrixCasa[16];
	float scaleMatrixCasa[16];
	float tempMatrixCasa[16];

	OpenGL->BuildIdentityMatrix(matrixcasa1);

	OpenGL->BuildIdentityMatrix(rotationMatrixCasa);
	OpenGL->MatrixRotationY(rotationMatrixCasa, 315.0f * (M_PI / 180.0f));

	OpenGL->BuildIdentityMatrix(translationMatrixCasa);
	OpenGL->MatrixTranslation(translationMatrixCasa, 170.0f, Terreno->Superficie(170.0f, 140.0f) - 3.0f, 140.0f);
	
	OpenGL->BuildIdentityMatrix(scaleMatrixCasa);
	OpenGL->MatrixScale(scaleMatrixCasa, 0.999f, 0.999f, 0.999f);
	// Aplica las transformaciones en orden: Escala -> Rotación -> Traslación
	OpenGL->MatrixMultiply(tempMatrixCasa, scaleMatrixCasa, rotationMatrixCasa);
	OpenGL->MatrixMultiply(matrixcasa1, tempMatrixCasa, translationMatrixCasa);

	// Renderizado de casa 2 habitable
	float* matrixcasa2 = Casa2->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixcasa2, -150.0f, Terreno->Superficie(160.0f, 160.0f), -60.0f);
	OpenGL->MatrixScale(matrixcasa2,15.0f, 15.0f, 15.0f);

	// Renderizado de torre
	float* matrixtorre = Torre->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixtorre, 160.0f, Terreno->Superficie(160.0f, 160.0f), 160.0f);
	OpenGL->MatrixScale(matrixtorre, 1.0f, 1.0f, 1.0f);

	float* matrixColision = Object3d1->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixColision, DeltaPosition->X-1.5, DeltaPosition->Y-1.5, DeltaPosition->Z-1.5);

	float* matrixdragon = Dragon->GetWorldMatrix();
	float rotationMatrix[16];
	float translationMatrix[16];
	float scaleMatrix[16];
	float tempMatrix[16];

	float* matrixdragon2 = Dragon2->GetWorldMatrix();
	float rotationMatrix2[16];
	float translationMatrix2[16];
	float scaleMatrix2[16];
	float tempMatrix2[16];

	// Inicializa la matriz del dragón como identidad
	OpenGL->BuildIdentityMatrix(matrixdragon);

	// Configura las matrices de transformación individuales
	OpenGL->BuildIdentityMatrix(rotationMatrix);
	OpenGL->MatrixRotationY(rotationMatrix, 135.0f * (M_PI / 180.0f)); // Rotación en Y

	OpenGL->BuildIdentityMatrix(translationMatrix);
	OpenGL->MatrixTranslation(translationMatrix, 160.0f, Terreno->Superficie(160.0f, -160.0f), -160.0f); // Traslación

	OpenGL->BuildIdentityMatrix(scaleMatrix);
	OpenGL->MatrixScale(scaleMatrix, 15.0f, 15.0f, 15.0f); // Escala

	// Aplica las transformaciones en orden: Escala -> Rotación -> Traslación
	OpenGL->MatrixMultiply(tempMatrix, scaleMatrix, rotationMatrix);
	OpenGL->MatrixMultiply(matrixdragon, tempMatrix, translationMatrix); 

	//VEHICULO----------------------------------------------------------

	if (volar) {
		// Inicializa la matriz del dragón montable como identidad
		OpenGL->BuildIdentityMatrix(matrixdragon2);

		// Configura las matrices de transformación individuales
		OpenGL->BuildIdentityMatrix(rotationMatrix2);
		OpenGL->MatrixRotationY(rotationMatrix2, RotY); // Rotación en Y

		OpenGL->BuildIdentityMatrix(translationMatrix2);
		OpenGL->MatrixTranslation(translationMatrix2, PosX, PosY-1, PosZ); // Traslación

		OpenGL->BuildIdentityMatrix(scaleMatrix2);
		OpenGL->MatrixScale(scaleMatrix2, 4.0f, 4.0f, 4.0f); // Escala

		// Aplica las transformaciones en orden: Escala -> Rotación -> Traslación
		OpenGL->MatrixMultiply(tempMatrix2, scaleMatrix2, rotationMatrix2); // Primero escala y rotación
		OpenGL->MatrixMultiply(matrixdragon2, tempMatrix2, translationMatrix2); // Luego traslación
	}
	else {
		PosX = 20.0f;
		PosY = 10.0f;
		PosZ = -30.0f;
		RotY = 315 * (M_PI / 180.0f);

		// Inicializa la matriz del dragón montable como identidad
		OpenGL->BuildIdentityMatrix(matrixdragon2);

		// Configura las matrices de transformación individuales
		OpenGL->BuildIdentityMatrix(rotationMatrix2);
		OpenGL->MatrixRotationY(rotationMatrix2, RotY); // Rotación en Y

		OpenGL->BuildIdentityMatrix(translationMatrix2);
		OpenGL->MatrixTranslation(translationMatrix2, PosX, PosY+4, PosZ); // Traslación

		OpenGL->BuildIdentityMatrix(scaleMatrix2);
		OpenGL->MatrixScale(scaleMatrix2, 4.0f, 4.0f, 4.0f); // Escala

		// Aplica las transformaciones en orden: Escala -> Rotación -> Traslación
		OpenGL->MatrixMultiply(tempMatrix2, scaleMatrix2, rotationMatrix2); // Primero escala y rotación
		OpenGL->MatrixMultiply(matrixdragon2, tempMatrix2, translationMatrix2); // Luego traslación
	}

	// Objetos varios ----------------------------------------------
	
	// Renderizado de Carreta
	float* matrixcarreta = Carreta->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixcarreta, -150.0f, Terreno->Superficie(-150.0f, 30.0f), 30.0f);
	OpenGL->MatrixScale(matrixcarreta, 4.0f, 4.0f, 4.0f);

	// Renderizado de Pozo
	float* matrixpozo = Pozo->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixpozo, -100.0f, Terreno->Superficie(-100.0f, 160.0f), 160.0f);
	OpenGL->MatrixScale(matrixpozo, 2.0f, 2.0f, 2.0f);

	// Renderizado de Vacas

// Vaca 1
	float* matrixvaca1 = Vaca->GetWorldMatrix();
	float rotationMatrixVaca1[16];
	float translationMatrixVaca1[16];
	float scaleMatrixVaca1[16];
	float tempMatrixVaca1[16];

	OpenGL->BuildIdentityMatrix(matrixvaca1);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca1);
	OpenGL->MatrixRotationY(rotationMatrixVaca1, 45.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca1);
	OpenGL->MatrixTranslation(translationMatrixVaca1, -180.0f, Terreno->Superficie(-180.0f, 0.0f), 0.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca1);
	OpenGL->MatrixScale(scaleMatrixVaca1, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca1, scaleMatrixVaca1, rotationMatrixVaca1);
	OpenGL->MatrixMultiply(matrixvaca1, tempMatrixVaca1, translationMatrixVaca1);

	// Vaca 2
	float* matrixvaca2 = Vaca2->GetWorldMatrix();
	float rotationMatrixVaca2[16];
	float translationMatrixVaca2[16];
	float scaleMatrixVaca2[16];
	float tempMatrixVaca2[16];

	OpenGL->BuildIdentityMatrix(matrixvaca2);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca2);
	OpenGL->MatrixRotationY(rotationMatrixVaca2, 90.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca2);
	OpenGL->MatrixTranslation(translationMatrixVaca2, -120.0f, Terreno->Superficie(-120.0f, 40.0f), 40.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca2);
	OpenGL->MatrixScale(scaleMatrixVaca2, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca2, scaleMatrixVaca2, rotationMatrixVaca2);
	OpenGL->MatrixMultiply(matrixvaca2, tempMatrixVaca2, translationMatrixVaca2);

	// Vaca 3
	float* matrixvaca3 = Vaca3->GetWorldMatrix();
	float rotationMatrixVaca3[16];
	float translationMatrixVaca3[16];
	float scaleMatrixVaca3[16];
	float tempMatrixVaca3[16];

	OpenGL->BuildIdentityMatrix(matrixvaca3);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca3);
	OpenGL->MatrixRotationY(rotationMatrixVaca3, 135.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca3);
	OpenGL->MatrixTranslation(translationMatrixVaca3, -60.0f, Terreno->Superficie(-60.0f, 80.0f), 80.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca3);
	OpenGL->MatrixScale(scaleMatrixVaca3, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca3, scaleMatrixVaca3, rotationMatrixVaca3);
	OpenGL->MatrixMultiply(matrixvaca3, tempMatrixVaca3, translationMatrixVaca3);

	// Vaca 4
	float* matrixvaca4 = Vaca4->GetWorldMatrix();
	float rotationMatrixVaca4[16];
	float translationMatrixVaca4[16];
	float scaleMatrixVaca4[16];
	float tempMatrixVaca4[16];

	OpenGL->BuildIdentityMatrix(matrixvaca4);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca4);
	OpenGL->MatrixRotationY(rotationMatrixVaca4, 180.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca4);
	OpenGL->MatrixTranslation(translationMatrixVaca4, -20.0f, Terreno->Superficie(-20.0f, 120.0f), 120.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca4);
	OpenGL->MatrixScale(scaleMatrixVaca4, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca4, scaleMatrixVaca4, rotationMatrixVaca4);
	OpenGL->MatrixMultiply(matrixvaca4, tempMatrixVaca4, translationMatrixVaca4);

	// Vaca 5
	float* matrixvaca5 = Vaca5->GetWorldMatrix();
	float rotationMatrixVaca5[16];
	float translationMatrixVaca5[16];
	float scaleMatrixVaca5[16];
	float tempMatrixVaca5[16];

	OpenGL->BuildIdentityMatrix(matrixvaca5);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca5);
	OpenGL->MatrixRotationY(rotationMatrixVaca5, 225.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca5);
	OpenGL->MatrixTranslation(translationMatrixVaca5, -90.0f, Terreno->Superficie(-90.0f, 60.0f), 60.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca5);
	OpenGL->MatrixScale(scaleMatrixVaca5, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca5, scaleMatrixVaca5, rotationMatrixVaca5);
	OpenGL->MatrixMultiply(matrixvaca5, tempMatrixVaca5, translationMatrixVaca5);

	// Vaca 6
	float* matrixvaca6 = Vaca6->GetWorldMatrix();
	float rotationMatrixVaca6[16];
	float translationMatrixVaca6[16];
	float scaleMatrixVaca6[16];
	float tempMatrixVaca6[16];

	OpenGL->BuildIdentityMatrix(matrixvaca6);
	OpenGL->BuildIdentityMatrix(rotationMatrixVaca6);
	OpenGL->MatrixRotationY(rotationMatrixVaca6, 270.0f * (M_PI / 180.0f)); // Rotación
	OpenGL->BuildIdentityMatrix(translationMatrixVaca6);
	OpenGL->MatrixTranslation(translationMatrixVaca6, -140.0f, Terreno->Superficie(-140.0f, 30.0f), 30.0f); // Posición
	OpenGL->BuildIdentityMatrix(scaleMatrixVaca6);
	OpenGL->MatrixScale(scaleMatrixVaca6, 3.0f, 3.0f, 3.0f); // Escala
	OpenGL->MatrixMultiply(tempMatrixVaca6, scaleMatrixVaca6, rotationMatrixVaca6);
	OpenGL->MatrixMultiply(matrixvaca6, tempMatrixVaca6, translationMatrixVaca6);

	// Arbol ahorcado
	float* matrixahorcado = ArbolAhorcado->GetWorldMatrix();
	OpenGL->MatrixTranslation(matrixahorcado, -150.0f, Terreno->Superficie(-150.0f, -150.0f), -150.0f);
	OpenGL->MatrixScale(matrixahorcado, 10.0f, 10.0f, 10.0f);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Colisiones
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Con limite del mapa
	if (Limite1->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Te estas alejando demasiado!", L"Limites del mapa", MB_OK);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Limite2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Te estas alejando demasiado!", L"Limites del mapa", MB_OK);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Limite3->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Te estas alejando demasiado!", L"Limites del mapa", MB_OK);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Limite4->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Te estas alejando demasiado!", L"Limites del mapa", MB_OK);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	//Con objetos
	if (Torre->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Descubriste una torre antigua!", L"Objeto!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Dragon->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Un dragon protege la zona!", L"Cuidado!", MB_OK | MB_ICONWARNING);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	//Vehiculo dragon
	if (Dragon2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		float* matrixVehiculo = Dragon2->GetWorldMatrix();
		PosX = DeltaPosition->X+1;
		PosY = DeltaPosition->Y - 1.0f;
		PosZ = DeltaPosition->Z + 3.0f;
		RotY = (DeltaRotation->Y + 180.0f) * (M_PI / 180.0f);
		colision = true;
		volar = true;
		Render();
	}
	//Con casa habitable
	if (Exterior1->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Exterior11->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Exterior2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Exterior3->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Exterior4->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	// Con casa normal
	if (Casa->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	// Con pozo
	if (Pozo->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	// Con carreta
	if (Carreta->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	// Con Vaca
	if (Vaca->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Vaca2->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Vaca3->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Vaca4->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Vaca5->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	if (Vaca6->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste una manada de vacas!", L"Animales!", MB_OK | MB_ICONINFORMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}
	// Con arbol del ahorcado
	if (ArbolAhorcado->GetBoxCollision(DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z)) {
		MessageBox(handlerWindow, L"Encontraste el arbol del ahorcado!", L"Cuidado!", MB_OK | MB_ICONEXCLAMATION);
		DeltaPosition->X = LastDeltaPosition->X;
		DeltaPosition->Y = LastDeltaPosition->Y;
		DeltaPosition->Z = LastDeltaPosition->Z;
	}

	return result;
}

bool Scene::ManageCommands() {
	bool result = true;

	//Guardar la posición de la Cámara antes de actualizarla
	LastDeltaPosition->X = DeltaPosition->X;
	LastDeltaPosition->Y = DeltaPosition->Y;
	LastDeltaPosition->Z = DeltaPosition->Z;

	float sensitivity = 0.1f;
	float vr[2]{ 0,0 };
	float radians = DeltaRotation->Y * 0.0174532925f;

	if ((input->GetKey(KeyCode.W) || input->GetKey(KeyCode.Up))) {
		vr[0] += sinf(radians) * speed * deltaTime;
		vr[1] += cosf(radians) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.S) || input->GetKey(KeyCode.Down))) {
		vr[0] -= sinf(radians) * speed * deltaTime;
		vr[1] -= cosf(radians) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.A) || input->GetKey(KeyCode.Left))) {
		vr[0] -= sinf(radians + (M_PI * 0.5f)) * speed * deltaTime;
		vr[1] -= cosf(radians + (M_PI * 0.5f)) * speed * deltaTime;
	}
	if ((input->GetKey(KeyCode.D) || input->GetKey(KeyCode.Right))) {
		vr[0] += sinf(radians + (M_PI * 0.5f)) * speed * deltaTime;
		vr[1] += cosf(radians + (M_PI * 0.5f)) * speed * deltaTime;
	}

	if (volar) {
		if (input->GetKey(KeyCode.E)) {
			DeltaPosition->Y += speed * deltaTime;
		}
		if (input->GetKey(KeyCode.Q)) {
			DeltaPosition->Y -= speed * deltaTime;
		}
		if (input->GetKey(KeyCode.R)) {
			volar = false;
		}
		// Limitar la altura máxima
		if (DeltaPosition->Y > 100.0f) {
			DeltaPosition->Y = 100.0f;
		}
		// Limitar para no pasar por debajo del terreno
		float terrenoAltura = Terreno->Superficie(DeltaPosition->X, DeltaPosition->Z);
		if (DeltaPosition->Y < terrenoAltura) {
			DeltaPosition->Y = terrenoAltura;
		}
	}

	if (input->GetRightAxisY() || input->GetRightAxisX()) {
		float angleX = input->GetRightAxisY() * speedAxisX * deltaTime;
		float angleY = input->GetRightAxisX() * speedAxisY * deltaTime;
		DeltaRotation->X -= angleX;
		DeltaRotation->Y += angleY;
	}

	if (input->GetKey(KeyCode.I)) {
		DeltaRotation->X -= speedAxisX * deltaTime;
	}
	if (input->GetKey(KeyCode.K)) {
		DeltaRotation->X += speedAxisX * deltaTime;
	}
	if (input->GetKey(KeyCode.J)) {
		DeltaRotation->Y -= speedAxisY * deltaTime;
	}
	if (input->GetKey(KeyCode.L)) {
		DeltaRotation->Y += speedAxisY * deltaTime;
	}
	if (input->GetKey(KeyCode.F1)) {
		char buffer[128];
		sprintf_s(buffer, "Coordenadas actuales:\nX: %.2f\nY: %.2f\nZ: %.2f",
			DeltaPosition->X, DeltaPosition->Y, DeltaPosition->Z);

		MessageBoxA(handlerWindow, buffer, "Coordenadas", MB_OK);
	}

	if (volar) {
		DeltaPosition->Y = DeltaPosition->Y;
	}
	else {
		DeltaPosition->Y = Terreno->Superficie(DeltaPosition->X, DeltaPosition->Z) + 3; // Ajuste al terreno
	}

	DeltaPosition->X += vr[0];
	DeltaPosition->Z += vr[1];

	// Captura del movimiento del ratón
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(handlerWindow, &mousePos);  // Convierte a coordenadas relativas a la ventana

	// Obtener las dimensiones de la ventana
	RECT windowRect;
	GetClientRect(handlerWindow, &windowRect);

	// Calcular el centro de la ventana
	int windowCenterX = (windowRect.right - windowRect.left) / 2;
	int windowCenterY = (windowRect.bottom - windowRect.top) / 2;

	// Convertir a coordenadas de pantalla (para poder fijar el cursor en el centro)
	POINT windowCenter;
	windowCenter.x = windowCenterX;
	windowCenter.y = windowCenterY;
	ClientToScreen(handlerWindow, &windowCenter);

	// Calcular el delta del movimiento del ratón desde el centro de la ventana
	float mouseDeltaX = (mousePos.x - windowCenterX) * sensitivity;
	float mouseDeltaY = (mousePos.y - windowCenterY) * sensitivity;

	// Si se ha movido el ratón, actualizar la rotación de la cámara
	if (mouseDeltaX != 0 || mouseDeltaY != 0) {
		DeltaRotation->Y += mouseDeltaX;  // Rotación horizontal
		DeltaRotation->X += mouseDeltaY;  // Rotación vertical

		// Limitar la rotación vertical
		if (DeltaRotation->X > 90.0f) {
			DeltaRotation->X = 90.0f;
		}
		if (DeltaRotation->X < -90.0f) {
			DeltaRotation->X = -90.0f;
		}
	}

	// Centrar el cursor nuevamente
	SetCursorPos(windowCenter.x, windowCenter.y);

	return result;
}

bool Scene::Shutdown() {
	bool result = true;

	if (Camera)
	{
		delete Camera;
		Camera = 0;
	}

	if (LightShader)
	{
		LightShader->Shutdown();
		delete LightShader;
		LightShader = 0;
	}

	if (ShaderSkydome)
	{
		ShaderSkydome->Shutdown();
		delete ShaderSkydome;
		ShaderSkydome = 0;
	}

	if (Skydome)
	{
		Skydome->Shutdown();
		delete Skydome;
		Skydome = 0;
	}

	if (ShaderBounding)
	{
		ShaderBounding->Shutdown();
		delete ShaderBounding;
		ShaderBounding = 0;
	}

	if (ShaderBox)
	{
		ShaderBox->Shutdown();
		delete ShaderBox;
		ShaderBox = 0;
	}

	if (ColorShader)
	{
		ColorShader->Shutdown();
		delete ColorShader;
		ColorShader = 0;
	}

	if (Terreno)
	{
		Terreno->Shutdown();
		delete Terreno;
		Terreno = 0;
	}

	if (Agua)
	{
		Agua->Shutdown();
		delete Agua;
		Agua = 0;
	}

	if (Casa) {
		Casa->Shutdown();
		delete Casa;
		Casa = 0;
	}

	if (Casa2) {
		Casa2->Shutdown();
		delete Casa2;
		Casa2 = 0;
	}

	if (Torre) {
		Torre->Shutdown();
		delete Torre;
		Torre = 0;
	}

	if (Dragon) {
		Dragon->Shutdown();
		delete Dragon;
		Dragon = 0;
	}

	if (Dragon2) {
		Dragon2->Shutdown();
		delete Dragon;
		Dragon2 = 0;
	}

	if (Arbol) {
		Arbol->Shutdown();
		delete Arbol;
		Arbol = 0;
	}

	if (Arbol2) {
		Arbol2->Shutdown();
		delete Arbol2;
		Arbol2 = 0;
	}

	if (ArbolQuemado) {
		ArbolQuemado->Shutdown();
		delete ArbolQuemado;
		ArbolQuemado = 0;
	}

	if (Arbusto) {
		Arbusto->Shutdown();
		delete Arbusto;
		Arbusto = 0;
	}

	if (Pajaros) {
		Pajaros->Shutdown();
		delete Pajaros;
		Pajaros = 0;
	}

	if (Flores) {
		Flores->Shutdown();
		delete Flores;
		Flores = 0;
	}

	if (Bosque) {
		Bosque->Shutdown();
		delete Bosque;
		Bosque = 0;
	}

	if (ArbolAhorcado) {
		ArbolAhorcado->Shutdown();
		delete ArbolAhorcado;
		ArbolAhorcado = 0;
	}

	if (Ahorcado) {
		Ahorcado->Shutdown();
		delete Ahorcado;
		Ahorcado = 0;
	}

	return result;
}
