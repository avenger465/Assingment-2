// Assignment 2 - Games Concepts.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

const float kGameSpeed = 1.0f;
IModel* frog[3];
IModel* car[4];
const float frogSize = 0.5;
const float frogRadius = 5.04487f * frogSize;
const float frogDiameter = frogRadius * 2;
int currentFrog = 0;

enum frogState {waiting, crossing, safe, dead};
enum gameStates {playing, paused, over};


float carMinX[1];
float carMaxX[1];
float carMinZ[1];
float carMaxZ[1];
int carXRange = 50;
int carRotateRange = 90;

bool CollisionWithCar(int a);
float vehicleLane2Speed = 0.01f * kGameSpeed;
float vehicleLane1Speed = 1.0f / 2.0f * vehicleLane2Speed;
float vehicleLane3Speed = 1.5 * vehicleLane2Speed;
float vehicleLane4Speed = 2 * vehicleLane1Speed;
void main()
{
	gameStates game = playing;
	//frogState frog[currentFrog] = crossing;
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	string carModels[4] = {"truck.bmp", "transit.bmp", "rover.bmp", "pickUp.bmp"};
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
	IModel* floorModel = floorMesh->CreateModel(0,-5,0);

	IMesh* frogMesh = myEngine->LoadMesh("frog.x");
	for (int i = 0; i < 3; i++)
	{
		frog[i] = frogMesh->CreateModel(-10 + (i*frogDiameter), 0.0f, 15);
		frog[i]->Scale(frogSize);
	}
	dummyModel->AttachToParent(frog[currentFrog]);
	IMesh* carMesh = myEngine->LoadMesh("rover.x");
	for (int i = 0; i < 4; i++)
	{
		car[i] = carMesh->CreateModel(0 + (-(carXRange)), 0.0f, 25 + (i * 10));

		car[i]->RotateY(carRotateRange);
		car[i]->SetSkin(carModels[i]);
		carXRange *= -1;
		carRotateRange *= -1;

	}

	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 40, -60);
	myCamera->RotateX(20);
	//myCamera->AttachToParent(dummyModel);


	/**** Set up your scene here ****/


	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

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
			if (myCamera->GetZ() >= (frog[currentFrog]->GetZ() - 15))
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
		/**** Update your scene each frame here ****/

		if (frog[currentFrog]->GetZ() >= 20 && frog[currentFrog]->GetZ() <=110)
		{
			frog[currentFrog]->SetY(10);
		}
		else 
		{
			frog[currentFrog]->SetY(0);
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}

bool CollisionWithCar(int a) 
{
	if (frog[currentFrog]->GetX() > carMinX[a] && frog[currentFrog]->GetX() < carMaxX[a] &&
		frog[currentFrog]->GetZ() > carMinZ[a] && frog[currentFrog]->GetZ() < carMaxZ[a])
	{
		return true;
	}
}
