// Assignment 2 - Games Concepts.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

const float kGameSpeed = 0.5f;
IModel* frog[3];
IModel* car[4];
IModel* tyres[4];
const float frogSize = 0.5;
const float frogRadius = 5.04487f * frogSize;
const float frogDiameter = frogRadius * 2;
int currentFrog = 0;

enum frogState {waiting, crossing, safe, dead};
enum gameStates {playing, paused, over};

bool collidedWithCar = false;

const int carAmount = 4;
//float carMinX[carAmount];
//float carMaxX[carAmount];
//float carMinZ[carAmount];
//float carMaxZ[carAmount];

float carMinX;
float carMaxX;
float carMinZ;
float carMaxZ;

//float carWidthRadius[carAmount];
//float carLengthRadius[carAmount];
float carWidthRadius = 2.79333f;
float carLengthRadius = 5.9069f;
int carXRange = 50;
int carRotateRange = 90;


bool CollisionWithCar();
int GameDecisions(gameStates gameplay);
bool CollisionWithTyre();

float tyreRadius = 0.45f * 10;



float vehicleLane2Speed = 0.01f * kGameSpeed;
float vehicleLane1Speed = 1.0f / 2.0f * vehicleLane2Speed;
float vehicleLane3Speed = 1.5 * vehicleLane2Speed;
float vehicleLane4Speed = 2 * vehicleLane2Speed;
void main()
{
	gameStates game = playing;
	//frogState frog[currentFrog] = crossing;
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	string carModels[4] = {"truck/truck.bmp", "transit/transit.bmp", "rover/rover.bmp", "pickUp/pickUp.bmp"};
	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");
	//myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );
	IMesh* skyBoxMesh = myEngine->LoadMesh("skybox.x");
	IMesh* floorMesh = myEngine->LoadMesh("surface.x");
	IMesh* islandOneMesh = myEngine->LoadMesh("island1.x");
	IMesh* islandTwoMesh = myEngine->LoadMesh("island2.x");
	IMesh* dummy = myEngine->LoadMesh("Dummy.x");
	IModel* dummyModel = dummy->CreateModel(0,0,0);
	IModel* islandTwoModel = islandTwoMesh->CreateModel(0, -5, 115);
	IModel* islandOneModel = islandOneMesh->CreateModel(0,-5,40);
	IModel* skyBoxModel = skyBoxMesh->CreateModel(0,-1000,0);
	IModel* floorModel = floorMesh->CreateModel(0,-2.5,0);

	IMesh* frogMesh = myEngine->LoadMesh("frog.x");
	for (int i = 0; i < 3; ++i)
	{
		frog[i] = frogMesh->CreateModel(-10 + (i*frogDiameter), 0.0f, 15);
		frog[i]->Scale(frogSize);
	}
	dummyModel->AttachToParent(frog[currentFrog]);
	IMesh* carMesh = myEngine->LoadMesh("rover.x");
	for (int i = 0; i < 4; ++i)
	{
		car[i] = carMesh->CreateModel(0 + (-(carXRange)), 0.0f, 25 + (i * 10));

		car[i]->RotateY(carRotateRange);
		car[i]->SetSkin(carModels[i]);
		//carLengthRadius[i] = 5.9069;
		//carWidthRadius[i] = 2.79333;
		carXRange *= -1;
		carRotateRange *= -1;

	}

	IMesh* tyreMesh = myEngine->LoadMesh("tyre.x");
	for (int i = 0; i < 4; ++i)
	{
		tyres[i] = tyreMesh->CreateModel(0 + (-(carXRange)),-2.5,75 + (i * 10));
		tyres[i]->Scale(10);
		carXRange *= -1;
	}

	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 60, -160);
	myCamera->RotateX(20);
	myCamera->AttachToParent(dummyModel);
	dummyModel->AttachToParent(frog[currentFrog]);


	/**** Set up your scene here ****/


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		tyres[0]->MoveX(vehicleLane4Speed);
		tyres[1]->MoveX(-vehicleLane4Speed);
		tyres[2]->MoveX(vehicleLane4Speed);
		tyres[3]->MoveX(-vehicleLane4Speed);
		for (int j = 0; j < 4; ++j)
		{
			if (tyres[j]->GetX() <= -45)
			{
				tyres[j]->MoveX(vehicleLane4Speed);
			}
			else if (tyres[j]->GetX() >= 45)
			{
				tyres[j]->MoveX(-vehicleLane4Speed);
			}
		}
		if (collidedWithCar)
		{
			frog[currentFrog]->SetSkin("frog_red.jpg");
			frog[currentFrog]->Scale(0.5);
			currentFrog++;
			collidedWithCar = !collidedWithCar;
			if (currentFrog > 2) myEngine->Stop();
		}

		GameDecisions(game);

		if (myEngine->KeyHit(Key_P))
		{
			if (game == paused)
			{
				game = playing;
			}
			else if (game == playing)
			{
				game = paused;
			}

		}
		if (myEngine->KeyHeld(Key_Z))
		{
			frog[currentFrog]->MoveX(-(0.01f * kGameSpeed));
		}
		else if (myEngine->KeyHeld(Key_X))
		{
			//frog[currentFrog]->SetLocalY(90);
			frog[currentFrog]->MoveX(0.01f * kGameSpeed);
		}
		else if (myEngine->KeyHeld(EKeyCode(192)) || myEngine->KeyHeld(Key_W))
		{
			frog[currentFrog]->MoveZ(0.01f * kGameSpeed);
		}
		else if (myEngine->KeyHeld(EKeyCode(191)) || myEngine->KeyHeld(Key_S))
		{
			frog[currentFrog]->MoveZ(-(0.01f * kGameSpeed));
		}


		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}

		if (myEngine->KeyHeld(Key_Up))
		{
			if (myCamera->GetZ() >= (frog[currentFrog]->GetZ() - 20))
			{

			}
			else
			{
				myCamera->MoveZ(0.01f);
				myCamera->MoveY(0.01f);
				myCamera->RotateX(0.01f);
			}
		}
		else if (myEngine->KeyHeld(Key_Down)) 
		{
			if (myCamera->GetY() <= 15)
			{

			}
			else
			{
				myCamera->MoveZ(-0.01f);
				myCamera->MoveY(-0.01f);
				myCamera->RotateX(-0.01f);
			}
		}

		if (frog[currentFrog]->GetZ() >=  65&& frog[currentFrog]->GetZ() <= 115)
		{
			frog[currentFrog]->SetY(15);
		}
		else
		{
			frog[currentFrog]->SetY(0);
		}
		/**** Update your scene each frame here ****/

		//if (frog[currentFrog]->GetZ() >= 20 && frog[currentFrog]->GetZ() <= 60)
		//{

		//for (int i = 0; i < carAmount; i++)
		//{
			//carMinX = car[i]->GetX() - carLengthRadius - frogRadius;
			//carMaxX = car[i]->GetX() + carLengthRadius + frogRadius;
			//carMinZ = car[i]->GetZ() - carWidthRadius - frogRadius;
			//carMaxZ = car[i]->GetZ() + carWidthRadius + frogRadius;
			//carMinX[i] = car[i]->GetX() - carLengthRadius[i] - frogRadius;
			//carMaxX[i] = car[i]->GetX() + carLengthRadius[i] + frogRadius;
			//carMinZ[i] = car[i]->GetZ() - carWidthRadius[i] - frogRadius;
			//carMaxZ[i] = car[i]->GetZ() + carWidthRadius[i] + frogRadius;

			//collidedWithCar = CollisionWithCar();
		//}
		//else 
		//{
		//	frog[currentFrog]->SetY(0);
		//}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

bool CollisionWithCar() 
{
	if (frog[currentFrog]->GetX() > carMinX && frog[currentFrog]->GetX() < carMaxX &&
		frog[currentFrog]->GetZ() > carMinZ && frog[currentFrog]->GetZ() < carMaxZ)
	{
		return true;
	}
}
int GameDecisions(gameStates gameplay)
{
	if (gameplay == playing)
	{
		car[0]->MoveX(vehicleLane1Speed);
		if (car[0]->GetX() >= 55)
		{
			car[0]->MoveX(-100);
		}
		car[1]->MoveX(-vehicleLane2Speed);
		if (car[1]->GetX() <= -55)
		{
			car[1]->MoveX(100);
		}
		car[2]->MoveX(vehicleLane3Speed);
		if (car[2]->GetX() >= 55)
		{
			car[2]->MoveX(-100);
		}
		car[3]->MoveX(-vehicleLane4Speed);
		if (car[3]->GetX() <= -55)
		{
			car[3]->MoveX(100);
		}
		return 1;
	}
	else if (gameplay == paused)
	{
		return 1;
	}
	else if (gameplay == over)
	{
		return 1;
	}
}
bool CollisionWithTyre()
{



	return true;
}
