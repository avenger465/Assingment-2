// Assignment 2 - Games Concepts.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;

const float kGameSpeed = 1.0f;
IModel* frog[3];
const float frogSize = 0.5;
const float frogRadius = 5.04487f * frogSize;
const float frogDiameter = frogRadius * 2;
int currentFrog = 0;


float carMinX[1];
float carMaxX[1];
float carMinZ[1];
float carMaxZ[1];

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine( kTLX );
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");
	//myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );
	IMesh* skyBoxMesh = myEngine->LoadMesh("skybox.x");
	IMesh* floorMesh = myEngine->LoadMesh("surface.x");
	IMesh* islandOneMesh = myEngine->LoadMesh("island1.x");
	IMesh* islandTwoMesh = myEngine->LoadMesh("island2.x");
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

	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 40, -60);
	myCamera->RotateX(20);


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
			frog[currentFrog]->MoveX(0.01f * kGameSpeed);
		}
		else if (myEngine->KeyHeld(Key_W))
		{
			frog[currentFrog]->MoveZ(0.01f * kGameSpeed);
		}
		else if (myEngine->KeyHeld(Key_S))
		{
			frog[currentFrog]->MoveZ(-(0.01f * kGameSpeed));
		}


		if (myEngine->KeyHit(Key_Escape))
		{
			myEngine->Stop();
		}

		/**** Update your scene each frame here ****/

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
