// Assignment 2 - Games Concepts.cpp: A program using the TL-Engine

#include <sstream>
#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
using namespace std;

const float kGameSpeed = 0.75f;
IModel* frog[3];
const int KCarAmount = 4;
const int kFrogAmount = 3;
const int kTreeAmount = 6;
const int kTyreAmount = 4;
IModel* car[KCarAmount];
const float frogSize = 0.5;
const float frogRadius = 5.04487f * frogSize;
const float frogDiameter = frogRadius * 2;
int currentFrog = 0;


float frogMovementOnTyreSpeed;

enum frogState {waiting, crossing, safe, dead};
enum frogDirections {MovingLeft, MovingRight, NotOnTyre};
enum gameStates {playing, paused, over};


float frogStateIdentifier[kFrogAmount];

bool collidedWithCar = false;
bool collidedWithTree;
bool frogOnTyre;

//information about the tyre model
const float tyreSize = 10.0f;
const float tyreSSpeed = 0.25f;
const float tyreRadius = 0.45f * tyreSize;
const float tyreDiameter = tyreRadius * 2;
IModel* tyres[4];

//boundary boxes for the tyres
int numberOfCollisions = 0;

IModel* trees[kTreeAmount];

float tyreMinX[kTyreAmount];
float tyreMaxX[kTyreAmount];
float tyreMinZ[kTyreAmount];
float tyreMaxZ[kTyreAmount];

float carMinX[KCarAmount];
float carMaxX[KCarAmount];
float carMinZ[KCarAmount];
float carMaxZ[KCarAmount];

float treeMinX[kTreeAmount];
float treeMaxX[kTreeAmount];
float treeMinZ[kTreeAmount];
float treeMaxZ[kTreeAmount];

float frogOldXpos;
float frogOldZpos;

//boundary box for the cars;
//float carMinX;
//float carMaxX;
//float carMinZ;
//float carMaxZ;

int score = 0;

float carWidthRadius[KCarAmount];
float carLengthRadius[KCarAmount];
//float carWidthRadius = 2.79333f;
//float carLengthRadius = 5.9069f;
int carXRange = 50;
int carRotateRange = 90;

float treeWidthRadius = 3.063925;
float treeLengthRadius = 0.5;//3.04428;

//function prototype calls
bool CollisionWithCar(float carminx, float carmaxx, float carminz, float carmaxz);
int GameDecisions(gameStates gameplay);
bool CollisionWithTyre(float tyreminx, float tyremaxx, float tyreminz, float tyremaxz);
bool CollisionWithTree(float treeminx, float treemaxx, float treeminz, float treemaxz);
void InitialGameStatesForFrogs();

//The boundary for the car collisions
const int trafficMinX = -50;
const int trafficMaxX = 50;
const int trafficMinZ = 25;
const int trafficMaxZ = 55;

//tyre speed declarations
float tyreSpeedLane1 = 0.02f * kGameSpeed;
float tyreSpeedLane2 = 0.025f * kGameSpeed;
float tyreSpeedLane3 = 0.03f * kGameSpeed;
float tyreSpeedLane4 = 0.035f * kGameSpeed;

EKeyCode MoveForward = EKeyCode(192);
EKeyCode MoveBackward = EKeyCode(191);
EKeyCode MoveLeft = Key_Z;
EKeyCode MoveRight = Key_X;
EKeyCode Pausing = Key_P;
EKeyCode Escape = Key_Escape;

//vehicle speed declarations
float vehicleLane2Speed = 0.01f * kGameSpeed;
float vehicleLane1Speed = 1.0f / 2.0f * vehicleLane2Speed;
float vehicleLane3Speed = 1.5 * vehicleLane2Speed;
float vehicleLane4Speed = 2 * vehicleLane2Speed;
void main()
{
	gameStates game = playing;
	frogDirections currentFrogDirection = NotOnTyre;
	//frogState frog[currentFrog] = crossing;
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);


	myEngine->StartWindowed();
	IFont* GameOverFont = myEngine->LoadFont("Comic Sans MS", 45);

	string carModels[4] = { "truck/truck.bmp", "transit/transit.bmp", "rover/rover.bmp", "pickUp/pickUp.bmp" };
	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");
	//myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );

	//mesh and model declarations for the game
	IMesh* skyBoxMesh = myEngine->LoadMesh("skybox.x");
	IMesh* floorMesh = myEngine->LoadMesh("surface.x");
	IMesh* islandOneMesh = myEngine->LoadMesh("island1.x");
	IMesh* islandTwoMesh = myEngine->LoadMesh("island2.x");
	IMesh* dummy = myEngine->LoadMesh("Dummy.x");
	IModel* dummyModel = dummy->CreateModel(0, 0, 0);
	IModel* islandTwoModel = islandTwoMesh->CreateModel(0, -5, 115);
	IModel* islandOneModel = islandOneMesh->CreateModel(0, -5, 40);
	IModel* skyBoxModel = skyBoxMesh->CreateModel(0, -1000, 0);
	IModel* floorModel = floorMesh->CreateModel(0, -2.5, 0);

	//creating the array of frogs being used
	IMesh* frogMesh = myEngine->LoadMesh("frog.x");
	for (int i = 0; i < kFrogAmount; ++i)
	{
		frog[i] = frogMesh->CreateModel(-10 + (i*frogDiameter), 0.0f, 15);
		frog[i]->Scale(frogSize);
	}
	dummyModel->AttachToParent(frog[currentFrog]);

	//Creating the array of cars being used
	IMesh* carMesh = myEngine->LoadMesh("rover.x");
	for (int i = 0; i < KCarAmount; ++i)
	{
		car[i] = carMesh->CreateModel(0 + (-(carXRange)), 0.0f, 25 + (i * 10));

		car[i]->RotateY(carRotateRange);
		car[i]->SetSkin(carModels[i]);
		carLengthRadius[i] = 5.9069;
		carWidthRadius[i] = 2.79333;
		carXRange *= -1;
		carRotateRange *= -1;

	}

	IMesh* plantMesh = myEngine->LoadMesh("plant.x");
	for (int i = 0; i < kTreeAmount; ++i)
	{
		trees[i] = plantMesh->CreateModel(-50 + (i * 20),0,115);

	}

	//Creating the array of tyres being used
	IMesh* tyreMesh = myEngine->LoadMesh("tyre.x");
	for (int i = 0; i < 4; ++i)
	{
		tyres[i] = tyreMesh->CreateModel(0 + (-(carXRange)), -2.5, 75 + (i * 10));
		tyres[i]->Scale(10);
		carXRange *= -1;
	}

	//Creating the camera and assigning it to the dummy model and the current frog
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, 60, -160);
	myCamera->RotateX(20);
	myCamera->AttachToParent(dummyModel);
	dummyModel->AttachToParent(frog[currentFrog]);

	InitialGameStatesForFrogs();
	//Main code for the running of the game
	myEngine->Timer();
	while (myEngine->IsRunning())
	{
		myEngine->DrawScene();
		float frameTime = myEngine->Timer();
		if (game == playing)
		{

			stringstream time;
			time << "Time: " << frameTime;
			GameOverFont->Draw(time.str(),1000,0);
			time.str("");

			stringstream scoreOut;
			scoreOut << "Score: " << score;
			GameOverFont->Draw(scoreOut.str(), 0, 0);
			scoreOut.str("");
			tyres[0]->MoveX(tyreSpeedLane1);
			if (tyres[0]->GetX() >= 50 || tyres[0]->GetX() <= -50)
			{
				tyreSpeedLane1 *= -1;
			}
			tyres[1]->MoveX(-tyreSpeedLane2);
			if (tyres[1]->GetX() <= -50 || tyres[1]->GetX() >= 50)
			{
				tyreSpeedLane2 *= -1;
			}
			tyres[2]->MoveX(tyreSpeedLane3);
			if (tyres[2]->GetX() >= 50 || tyres[2]->GetX() <= -50)
			{
				tyreSpeedLane3 *= -1;
			}
			tyres[3]->MoveX(-tyreSpeedLane4);
			if (tyres[3]->GetX() <= -50 || tyres[3]->GetX() >= 50)
			{
				tyreSpeedLane4 *= -1;
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


			if (currentFrogDirection == NotOnTyre)
			{

			}
			else
			{
				if (currentFrogDirection == MovingLeft)
				{
					frog[currentFrog]->MoveX(-frogMovementOnTyreSpeed);
					if (frog[currentFrog]->GetX() <= -50)
					{
						currentFrogDirection = MovingRight;
					}
				}
				else if (currentFrogDirection == MovingRight)
				{
					frog[currentFrog]->MoveX(frogMovementOnTyreSpeed);
					if (frog[currentFrog]->GetX() >= 50)
					{
						currentFrogDirection = MovingLeft;
					}
				}
			}

			//for (int j = 0; j < 4; ++j)
			//{
			//	if (tyres[j]->GetX() <= -45)
			//	{
			//		tyres[j]->MoveX(vehicleLane4Speed);
			//	}
			//	else if (tyres[j]->GetX() >= 45)
			//	{
			//		tyres[j]->MoveX(-vehicleLane4Speed);
			//	}
			//}
			if (collidedWithCar)
			{
				frog[currentFrog]->SetSkin("frog_red.jpg");
				frog[currentFrog]->Scale(0.5);
				frogStateIdentifier[currentFrog] = dead;
				dummyModel->DetachFromParent();
				currentFrog++;
				frogStateIdentifier[currentFrog] = crossing;
				dummyModel->AttachToParent(frog[currentFrog]);
				collidedWithCar = !collidedWithCar;
				if (currentFrog > 2) game = over;
			}


			//GameDecisions(game);

			if (myEngine->KeyHit(Pausing))
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
			if (frogStateIdentifier[currentFrog] == crossing)
			{
				frogOldXpos = frog[currentFrog]->GetX();
				frogOldZpos = frog[currentFrog]->GetZ();

				if (myEngine->KeyHeld(MoveLeft))
				{
					//frog[currentFrog]->MoveX(-10.0f);
					frog[currentFrog]->MoveX(-(0.01f * kGameSpeed));
				}
				else if (myEngine->KeyHeld(MoveRight))
				{
					//frog[currentFrog]->SetLocalY(90);
					//frog[currentFrog]->MoveX(10.0f);
					frog[currentFrog]->MoveX(0.01f * kGameSpeed);
				}
				else if (myEngine->KeyHit(MoveForward) || myEngine->KeyHit(Key_W))
				{
					frog[currentFrog]->MoveZ(10.0f);
					//frog[currentFrog]->MoveZ(0.01f * kGameSpeed);
				}
				else if (myEngine->KeyHit(MoveBackward) || myEngine->KeyHit(Key_S))
				{
					frog[currentFrog]->MoveZ(-10.0f);
					//frog[currentFrog]->MoveZ(-(0.01f * kGameSpeed));
				}

				if (myEngine->KeyHit(Escape))
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
						dummyModel->RotateLocalX(0.01f);
						/*myCamera->MoveZ(0.01f);
						myCamera->MoveY(0.01f);
						myCamera->RotateX(0.01f);*/
					}
				}
				else if (myEngine->KeyHeld(Key_Down))
				{
					if (myCamera->GetY() <= 15)
					{

					}
					else
					{
						dummyModel->RotateLocalX(-0.01f);
						/*myCamera->MoveZ(-0.01f);
						myCamera->MoveY(-0.01f);
						myCamera->RotateX(-0.01f);*/
					}
				}

			}


			else if (frogStateIdentifier[currentFrog] == dead && ((frog[currentFrog]->GetZ() >= 117.5 || frog[currentFrog]->GetZ() <= 7.5 || frog[currentFrog]->GetX() <= -52.5 || frog[currentFrog]->GetX() >= 52.5) || (frog[currentFrog]->GetZ() >= 75 && frog[currentFrog]->GetZ() <= 105 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50)))

			{
				if (currentFrog > 2) game = over;
				frog[currentFrog]->SetSkin("frog_red.jpg");
				frog[currentFrog]->Scale(0.5);
				dummyModel->DetachFromParent();
				for (int i = 0; i <10; ++i)
				{
					frog[currentFrog]->MoveY(-0.1f);
				}
				frogStateIdentifier[currentFrog] = dead;
				currentFrog++;
				dummyModel->AttachToParent(frog[currentFrog]);
				frogStateIdentifier[currentFrog] = crossing;
			}
			else if (frogStateIdentifier[currentFrog] == dead)
			{
				if (currentFrog > 2) game = over;
				frog[currentFrog]->SetSkin("frog_red.jpg");
				frog[currentFrog]->Scale(0.5);
				dummyModel->DetachFromParent();
				frogStateIdentifier[currentFrog] = dead;
				currentFrog++;
				dummyModel->AttachToParent(frog[currentFrog]);
				frogStateIdentifier[currentFrog] = crossing;
			}
			/*else if (frogStateIdentifier[0] == dead && frogStateIdentifier[1] == dead && frogStateIdentifier[0] == dead)
			{
				game = over;
			}*/
			

			/*if (frog[currentFrog]->GetZ() >= 65 && frog[currentFrog]->GetZ() <= 115)
			{
				for (int i = 0; i < 4; ++i)
				{
					tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
					tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
					tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
					tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;

					frogOnTyre = CollisionWithTyre(i);
					switch (frogOnTyre)
					{
					case false:
						frog[currentFrog]->MoveY(-5);
						currentFrog++;
						dummyModel->AttachToParent(frog[currentFrog]);
						break;
					case true:
						frog[currentFrog]->AttachToParent(tyres[i]);
						break;
					}
				}*/
				//frog[currentFrog]->SetY(15);
			//}
			//else
			//{
			//	//frog[currentFrog]->SetY(0);
			//}
			/**** Update your scene each frame here ****/

			if (frog[currentFrog]->GetZ() >= 25 && frog[currentFrog]->GetZ() <= 55 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50)
			{

				for (int i = 0; i < KCarAmount; i++)
				{
					carMinX[i] = car[i]->GetX() - carLengthRadius[i] - frogRadius;
					carMaxX[i] = car[i]->GetX() + carLengthRadius[i] + frogRadius;
					carMinZ[i] = car[i]->GetZ() - carWidthRadius[i] - frogRadius;
					carMaxZ[i] = car[i]->GetZ() + carWidthRadius[i] + frogRadius;

					if (collidedWithCar = CollisionWithCar(carMinX[i], carMaxX[i], carMinZ[i], carMaxZ[i]))
					{
						frogStateIdentifier[currentFrog] = dead;
						//collidedWithCar = true;
					}
				}
			}

			else if (frog[currentFrog]->GetZ() >= 75 && frog[currentFrog]->GetZ() <= 105 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50)
			{
				for (int i = 0; i < kFrogAmount; ++i)
				{
					tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
					tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
					tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
					tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
					if (frogOnTyre = CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i]))
					{
						switch (i)
						{
						case 0:
							currentFrogDirection = MovingRight;
							frogMovementOnTyreSpeed = tyreSpeedLane1;
							break;
						case 1:
							currentFrogDirection = MovingLeft;
							frogMovementOnTyreSpeed = tyreSpeedLane2;
							break;
						case 2:
							currentFrogDirection = MovingRight;
							frogMovementOnTyreSpeed = tyreSpeedLane3;
							break;
						case 3:
							currentFrogDirection = MovingLeft;
							frogMovementOnTyreSpeed = tyreSpeedLane4;
							break;
						}




						//frog[currentFrog]->AttachToParent(tyres[i]);
						//frog[currentFrog]->SetX(tyres[i]->GetX());
						//frog[currentFrog]->SetY(tyres[i]->GetY());
					}
				}
			}
			else if (frog[currentFrog]->GetZ() >= 110 && frog[currentFrog]->GetZ() <= 120 && frog[currentFrog]->GetX() >= -52.5 && frog[currentFrog]->GetX() <= 52.5)
			{
				for (int i = 0; i < kTreeAmount; i++)
				{
					treeMinX[i] = trees[i]->GetX() - treeLengthRadius - frogRadius;
					treeMaxX[i] = trees[i]->GetX() + treeLengthRadius + frogRadius;
					treeMinZ[i] = trees[i]->GetZ() - treeWidthRadius - frogRadius;
					treeMaxZ[i] = trees[i]->GetZ() + treeWidthRadius + frogRadius;

					if (collidedWithTree = CollisionWithTree(treeMinX[i], treeMaxX[i], treeMinZ[i], treeMaxZ[i]))
					{
						frog[currentFrog]->SetX(frogOldXpos);
						frog[currentFrog]->SetZ(frogOldZpos);
						score == 10;
						//collidedWithCar = true;
					}
				}
			}
			else if (frog[currentFrog]->GetZ() >= 117.5 || frog[currentFrog]->GetZ() <= 7.5 || frog[currentFrog]->GetX() <= -52.5 || frog[currentFrog]->GetX() >= 52.5)
			{
				currentFrogDirection = NotOnTyre;
				frogStateIdentifier[currentFrog] = dead;
				//frog[currentFrog]->MoveY(-0.01f);
				//frog[currentFrog]->DetachFromParent();
				//collidedWithCar = true;
			}
		}
		else if (game == paused)
		{
			if (myEngine->KeyHit(Pausing))
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
			if (myEngine->KeyHit(Escape))
			{
				myEngine->Stop();
			}
		}
		else if (game == over)
		{
			GameOverFont->Draw("Game Over", 600, 250);
			if (myEngine->KeyHit(Escape))
			{
				myEngine->Stop();
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();

}

bool CollisionWithCar(float carminx, float carmaxx, float carminz, float carmaxz) 
{
	float currentFrogX = frog[currentFrog]->GetX();
	float currentFrogZ = frog[currentFrog]->GetZ();
	if (currentFrogX > carminx && currentFrogX < carmaxx &&
		currentFrogZ > carminz && currentFrogZ < carmaxz)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void InitialGameStatesForFrogs()
{
	frogStateIdentifier[0] = crossing;
	frogStateIdentifier[1] = waiting;
	frogStateIdentifier[2] = waiting;
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
bool CollisionWithTyre(float tyreminx, float tyremaxx, float tyreminz, float tyremaxz)
{
	float currentFrogX = frog[currentFrog]->GetX();
	float currentFrogZ = frog[currentFrog]->GetZ();
	if (currentFrogX > tyreminx && currentFrogX < tyremaxx &&
		currentFrogZ > tyreminz && currentFrogZ < tyremaxz)
	{
		numberOfCollisions++;
		return true;
	}
	else
	{
		return false;
	}
	/*else if (i == 4 - 1 && numberOfCollisions == 0)
	{
		return false;
	}
	return frogOnTyre;


	return true;*/
}


bool CollisionWithTree(float treeminx, float treemaxx, float treeminz, float treemaxz)
{
	float currentFrogX = frog[currentFrog]->GetX();
	float currentFrogZ = frog[currentFrog]->GetZ();
	if (currentFrogX > treeminx && currentFrogX < treemaxx &&
		currentFrogZ > treeminz && currentFrogZ < treemaxz)
	{
		return true;
	}
	else 
	{
		return false;
	}
}
