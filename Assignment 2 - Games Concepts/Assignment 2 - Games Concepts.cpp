// Assignment 2 - Games Concepts.cpp: A program using the TL-Engine

#include <sstream>
#include <TL-Engine.h> // TL-Engine include file and namespace
#include <cmath>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <iostream>
using namespace tle;
using namespace std;

const float kGameSpeed = 0.25f;
const int KCarAmount = 8;
const int kCarTypes = 4;
const int kFrogAmount = 3;
const int kTreeAmount = 6;
const int kTyreAmount = 12;
IModel* car[KCarAmount];
IModel* frog[kFrogAmount];
IModel* tyres[kTyreAmount];
const float frogSize = 0.5;
const float frogRadius = 5.04487f * frogSize;
const float frogDiameter = frogRadius * 2;
int currentFrog = 0;

int frogOne = 0, frogTwo = 1, frogThree = 2;

float CountDown;

float frogMovementOnTyreSpeed;

enum frogState {waiting, crossing, safe, dead};
enum frogDirections {MovingLeft, MovingRight, NotOnTyre};
enum gameStates {playing, paused, over};
enum carStates {CarGoingLeft, CarGoingRight, LeftDownSlope, RightDownSlope, LeftUpSlope, RightUpSlope};
enum tyreStates {TyreGoingLeft, TyreGoingRight};
enum frogMovement {Forward, Backward, Left, Right, Reset};
enum frogRotation {RotateRight, RotateLeft};

float frogStateIdentifier[kFrogAmount];

bool collidedWithTyre;
bool collidedWithCar = false;
bool collidedWithTree;
bool movingLeft = false;
bool movingRight = false;
bool frogOnTyre;

//information about the tyre model
const float tyreSize = 10.0f;
const float tyreSSpeed = 0.25f;
const float tyreRadius = 0.45f * tyreSize;
const float tyreDiameter = tyreRadius * 2;

//boundary boxes for the tyres
int numberOfCollisions = 0;

IModel* trees[kTreeAmount];

float tyreMinX[kTyreAmount];
float tyreMaxX[kTyreAmount];
float tyreMinZ[kTyreAmount];
float tyreMaxZ[kTyreAmount];

float x;

float carMinX[KCarAmount];
float carMaxX[KCarAmount];
float carMinZ[KCarAmount];
float carMaxZ[KCarAmount];
float carDirection[KCarAmount] = {50, -50, 50, -50};
float carLaneSpeeds[KCarAmount];
float tyreLaneSpeeds[kTyreAmount];

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
float treeLengthRadius = 3.04428;

//function prototype calls
bool CollisionWithCar(float carminx, float carmaxx, float carminz, float carmaxz);
bool CollisionWithTyre(float tyreminx, float tyremaxx, float tyreminz, float tyremaxz);
bool CollisionWithTree(float treeminx, float treemaxx, float treeminz, float treemaxz);
void InitialGameStatesForFrogs();
void Timer(float frameTime, IFont* GameOverFont, gameStates gameplay);
void DisplayScore(IFont* GameOverFont, int score);
void CheckForCollision(frogDirections currentFrogDirection);
void CarMovement(carStates carStatesArray[KCarAmount]);
void TyreMovement(tyreStates tyreStatesArray[kTyreAmount]);
void CheckForSafeZone();
void MoveAlongTyre(int tyreCollided);
void ResetCamera(I3DEngine* myEngine, ICamera* myCamera, IModel* dummyModel);
void FrogMovement(IModel* dummyModel, frogRotation currentRotation, int rotationAmount);

float startTimer = 21;
float incrementBy = 0;

//The boundary for the car collisions
const int trafficMinX = -50;
const int trafficMaxX = 50;
const int trafficMinZ = 25;
const int trafficMaxZ = 55;


int frogDesiredXLocation = -10;
int frogDesiredZlocation = 15;

//tyre speed declarations
float tyreSpeedLane1 = 0.02f * kGameSpeed;
float tyreSpeedLane2 = 0.025f * kGameSpeed;
float tyreSpeedLane3 = 0.03f * kGameSpeed;
float tyreSpeedLane4 = 0.035f * kGameSpeed;

int lane1Start = 0;
int lane2Start = 2;
int lane3Start = 4;
int lane4Start = 6;

int rightRotateAmount = 0;
int leftRotateAmount = 0;


EKeyCode MoveForward = EKeyCode(192);
EKeyCode MoveBackward = EKeyCode(191);
EKeyCode MoveLeft = Key_Z;
EKeyCode MoveRight = Key_X;
EKeyCode Pausing = Key_P;
EKeyCode Escape = Key_Escape;
EKeyCode keyForCameraReset = Key_C;

int randomCar = 0;




float tyreZPos[kTyreAmount] = {75, 75,  75, 85, 85, 85, 95, 95, 95, 105, 105, 105};
float tyreXPos[kTyreAmount] = {-60, -68, -76, 60, 68, 76, -60, -68, -76, 60, 68, 76};
float xPos[KCarAmount] = { -50.0f,   50.0f, -50.0f, 50.0f, -50.0f,   50.0f, -50.0f, 50.0f };
float zPos[KCarAmount] = { 25.0f, 35.0f, 45.0f, 55.0f, 25.0f, 35.0f, 45.0f, 55.0f };


float vehicleLane2Speed = 0.05f * kGameSpeed;
float vehicleLane1Speed = (1.0f / 2.0f) * vehicleLane2Speed;
float vehicleLane3Speed = 1.5 * vehicleLane2Speed;
float vehicleLane4Speed = 2 * vehicleLane2Speed;
struct SfrogStructure
{

};

void main()
{
	gameStates game = playing;
	frogDirections currentFrogDirection = NotOnTyre;
	carStates carStatesArray[KCarAmount];
	tyreStates tyreStatesArray[kTyreAmount];
	frogMovement currentFrogMovement = Reset;
	frogRotation currentFrogRotation;


	//frogState frog[currentFrog] = crossing;
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);


	myEngine->StartWindowed();
	IFont* GameOverFont = myEngine->LoadFont("Comic Sans MS", 45);

	string carModels[4] = { "truck.bmp", "transit.bmp", "rover.bmp", "pickUp.bmp" };
	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media");
	//myEngine->AddMediaFolder( "C:\\ProgramData\\TL-Engine\\Media" );

	//mesh and model declarations for the game
	IMesh* skyBoxMesh = myEngine->LoadMesh("skybox.x");
	IMesh* floorMesh = myEngine->LoadMesh("surface.x");
	IMesh* islandOneMesh = myEngine->LoadMesh("island1.x");
	IMesh* islandTwoMesh = myEngine->LoadMesh("island2.x");
	IMesh* dummy = myEngine->LoadMesh("Dummy.x");

	srand(myEngine->Timer() * 100000);

	IMesh* carMeshs[kCarTypes] = { myEngine->LoadMesh("transit.x") ,myEngine->LoadMesh("rover.x") ,myEngine->LoadMesh("pickUp.x") ,myEngine->LoadMesh("rangeRover.x") };



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
	for (int i = 0; i < KCarAmount; ++i)
	{
		randomCar = rand() % 4;
		switch (i)
		{
		case 0:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingRight;
			carLaneSpeeds[i] = vehicleLane1Speed;
			break;
		case 1:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingLeft;
			carLaneSpeeds[i] = vehicleLane2Speed;
			break;
		case 2:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingRight;
			carLaneSpeeds[i] = vehicleLane3Speed;
			break;
		case 3:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingLeft;
			carLaneSpeeds[i] = vehicleLane4Speed;
			break;
		case 4:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = RightDownSlope;
			carLaneSpeeds[i] = vehicleLane1Speed;
			break;
		case 5:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = LeftDownSlope;
			carLaneSpeeds[i] = vehicleLane2Speed;
			break;
		case 6:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = RightDownSlope;
			carLaneSpeeds[i] = vehicleLane3Speed;
			break;
		case 7:
			car[i] = carMeshs[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = LeftDownSlope;
			carLaneSpeeds[i] = vehicleLane4Speed;
			break;
		}
		if (randomCar == 0)
		{
			carLengthRadius[i] = 6.1216;
			carWidthRadius[i] = 2.82654;
		}
		else if (randomCar == 1)
		{
			carLengthRadius[i] = 5.9069;
			carWidthRadius[i] = 2.79333;
		}
		else if (randomCar == 2)
		{
			carLengthRadius[i] = 6.14739;
			carWidthRadius[i] = 2.58098;
		}
		else if (randomCar == 3)
		{
			carLengthRadius[i] = 5.53402;
			carWidthRadius[i] = 2.68256;
		}
		//(0 + (-(carXRange)), 0.0f, 25 + (i * 10));

		car[i]->RotateY(carRotateRange);
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
	for (int i = 0; i < kTyreAmount; ++i)
	{
		if (i <= 2)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], -2.5, tyreZPos[i]);
			tyres[i]->Scale(10);
			tyreStatesArray[i] = TyreGoingRight;
			tyreLaneSpeeds[i] = tyreSpeedLane1;
		}
		else if (i > 2 && i <= 5)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], -2.5, tyreZPos[i]);
			tyres[i]->Scale(10);
			tyreStatesArray[i] = TyreGoingLeft;
			tyreLaneSpeeds[i] = tyreSpeedLane2;
		}
		else if (i > 5 && i <= 8)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], -2.5, tyreZPos[i]);
			tyres[i]->Scale(10);
			tyreStatesArray[i] = TyreGoingRight;
			tyreLaneSpeeds[i] = tyreSpeedLane3;
		}
		else if (i > 8 && i <= 11)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], -2.5, tyreZPos[i]);
			tyres[i]->Scale(10);
			tyreStatesArray[i] = TyreGoingLeft;
			tyreLaneSpeeds[i] = tyreSpeedLane4;
		}
		// 75 + (i * 10));
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
		ResetCamera(myEngine, myCamera, dummyModel);
		float frameTime = myEngine->Timer();



		//for (int i = 0; i < KCarAmount; ++i)
		carLaneSpeeds[1] = vehicleLane2Speed = 0.05f * kGameSpeed * frameTime;
		if (game == playing)
		{
			Timer(frameTime, GameOverFont, game);
			DisplayScore(GameOverFont, 0);
	
			if (frogStateIdentifier[frogOne] == dead && frogStateIdentifier[frogTwo] == dead && frogStateIdentifier[frogThree] == dead)
			{
				game = over;
			}


			if (currentFrogMovement == Forward)
			{
				frog[currentFrog]->MoveZ(0.035f);
				if (frog[currentFrog]->GetZ() >= frogDesiredZlocation)
				{
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Backward)
			{
				frog[currentFrog]->MoveZ(-0.035f);
				if (frog[currentFrog]->GetZ() <= frogDesiredZlocation)
				{
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Left)
			{
				frog[currentFrog]->MoveX(-0.035f);
				if (frog[currentFrog]->GetX() <= frogDesiredXLocation)
				{
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Right)
			{
				frog[currentFrog]->MoveX(0.035f);
				if (frog[currentFrog]->GetX() >= frogDesiredXLocation)
				{
					currentFrogMovement = Reset;
				}
			}

			CarMovement(carStatesArray);
			TyreMovement(tyreStatesArray);

			CheckForCollision(currentFrogDirection);

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
				//CheckForSafeZone();


				if (movingLeft)
				{
					float z = 0;

					frog[currentFrog]->ResetOrientation();
					dummyModel->RotateLocalY(90);
					for (int i = 0; i < 900; i++)
						frog[currentFrog]->RotateLocalY(-90);
					frog[currentFrog]->MoveX(-10);
					movingLeft = !movingLeft;
				}
				else if (movingRight)
				{
					frog[currentFrog]->ResetOrientation();
					dummyModel->RotateLocalY(-90);
					frog[currentFrog]->RotateY(90);
					frog[currentFrog]->MoveX(10);
					movingRight = !movingRight;
				}

				if (myEngine->KeyHit(MoveLeft))
				{
					//leftRotateAmount -= 1;
					//currentFrogRotation = RotateLeft;
					//FrogMovement(dummyModel, currentFrogRotation, leftRotateAmount);
					frogDesiredXLocation -= 10;
					currentFrogMovement = Left;

				}
				else if (myEngine->KeyHit(MoveRight))
				{
					//dummyModel->ResetOrientation();
					//movingRight = !movingRight;
					
					//rightRotateAmount += 1;
					//currentFrogRotation = RotateRight;
					//FrogMovement(dummyModel, currentFrogRotation, rightRotateAmount);
					frogDesiredXLocation += 10;
					currentFrogMovement = Right;
				}
				else if (myEngine->KeyHit(MoveForward) || myEngine->KeyHit(Key_W))
				{

					frogDesiredZlocation += 10;
					currentFrogMovement = Forward;
				}
				else if (myEngine->KeyHit(MoveBackward) || myEngine->KeyHit(Key_S))
				{
					frogDesiredZlocation -= 10;
					currentFrogMovement = Backward;
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
						incrementBy += 0.01f;
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
						incrementBy -= 0.01f;
						dummyModel->RotateLocalX(-0.01f);
						/*myCamera->MoveZ(-0.01f);
						myCamera->MoveY(-0.01f);
						myCamera->RotateX(-0.01f);*/
					}
				}

				

			}


			else if (frogStateIdentifier[currentFrog] == dead)

			{
				if ((frog[currentFrog]->GetZ() >= 117.5 || frog[currentFrog]->GetZ() <= 7.5 || frog[currentFrog]->GetX() <= -52.5 || frog[currentFrog]->GetX() >= 52.5) || (frog[currentFrog]->GetZ() >= 75 && frog[currentFrog]->GetZ() <= 105 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50))
				{
					frogDesiredXLocation = -10;
					frogDesiredZlocation = 15;
					frog[currentFrog]->SetSkin("frog_red.jpg");
					frog[currentFrog]->Scale(0.5);
					dummyModel->DetachFromParent();
					for (int i = 0; i < 10; ++i)
					{
						frog[currentFrog]->MoveY(-0.1f);
					}
					frogStateIdentifier[currentFrog] = dead;
					currentFrog++;
					if (currentFrog > 2)
					{
						game = over;
					}
					else
					{
						dummyModel->AttachToParent(frog[currentFrog]);
						frogStateIdentifier[currentFrog] = crossing;
					}
				}
				else if (frog[currentFrog]->GetZ() <= 65 && frog[currentFrog]->GetZ() >= 15 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50)
				{
					frogDesiredXLocation = -10;
					frogDesiredZlocation = 15;
					frog[currentFrog]->SetSkin("frog_red.jpg");
					frog[currentFrog]->Scale(0.5);
					dummyModel->DetachFromParent();
					frogStateIdentifier[currentFrog] = dead;
					currentFrog++;
					if (currentFrog > 2)
					{
						game = over;
					}
					else
					{
						dummyModel->AttachToParent(frog[currentFrog]);
						frogStateIdentifier[currentFrog] = crossing;
					}
				}

			}
			else if (frogStateIdentifier[currentFrog] == safe)
			{
				frogDesiredXLocation = -10;
				frogDesiredZlocation = 15;
				frog[currentFrog]->SetSkin("frog_blue.jpg");
				dummyModel->DetachFromParent();
				currentFrog++;
				if (currentFrog > 2)
				{
					game = over;
				}
				else
				{
					dummyModel->AttachToParent(frog[currentFrog]);
					frogStateIdentifier[currentFrog] = crossing;
				}
			}

		}
		else if (game == paused)
		{
			DisplayScore(GameOverFont, 0);
			Timer(frameTime, GameOverFont, game);
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

void Timer(float frameTime, IFont* GameOverFont, gameStates gameplay)
{
	float timer;
	stringstream time;
	float countUp = 0;
	if (gameplay == playing)
	{
		startTimer -= frameTime;
		timer = floor(startTimer);
		x = floor(startTimer);
		countUp += 1;
	}
	else if (gameplay == paused)
	{
		timer = x;
	}

	time << "Time: " << timer;
	GameOverFont->Draw(time.str(), 1125, 0);
	time.str("");
	if (timer < 0)
	{
		startTimer = 21;
		//frogStateIdentifier[currentFrog] = dead;
	}
}

void DisplayScore(IFont* GameOverFont, int score)
{
	stringstream scoreOut;
	scoreOut << "Score: " << score;
	GameOverFont->Draw(scoreOut.str(), 0, 0);
	scoreOut.str("");
}

void CarMovement(carStates carStatesArray[KCarAmount])
{
	for (int i = 0; i < KCarAmount; ++i)
	{
		if (carStatesArray[i] == CarGoingLeft)
		{
			//car[i]->ResetOrientation();
			if (car[i]->GetX() <= -60) 
			{
				carStatesArray[i] = LeftDownSlope;
			}
			else
			{
				car[i]->MoveX(-carLaneSpeeds[i]);
			}
		}
		else if (carStatesArray[i] == CarGoingRight)
		{
			//car[i]->ResetOrientation()
			if (car[i]->GetX() >= 60) {
				carStatesArray[i] = RightDownSlope;
			}
			else
			{
				car[i]->MoveX(carLaneSpeeds[i]);
			}
		}
		else if (carStatesArray[i] == LeftDownSlope)
		{
			if (car[i]->GetX() <= -75 || car[i]->GetY() <= -5)
			{
				carStatesArray[i] = LeftUpSlope;
			}
			else
			{
				car[i]->RotateZ(0.1f);
				car[i]->MoveX(-0.01f);
				car[i]->MoveY(-0.01f);
			}
		}
		else if (carStatesArray[i] == RightDownSlope)
		{
			if (car[i]->GetX() >= 75 || car[i]->GetY() <= -5)
			{
				carStatesArray[i] = RightUpSlope;
			}
			else
			{
				car[i]->RotateZ(-0.1);
				car[i]->MoveX(0.01f);
				car[i]->MoveY(-0.01f);
			}
		}
		else if (carStatesArray[i] == LeftUpSlope)
		{
			car[i]->SetX(75);
			if(car[i]->GetY() >= 0)
			{
				carStatesArray[i] = CarGoingLeft;
			}
			else
			{
				car[i]->RotateZ(-0.1f);
				car[i]->MoveX(-0.01f);
				car[i]->MoveY(0.01f);
			}

		}
		else if (carStatesArray[i] == RightUpSlope)
		{
			car[i]->SetX(-75);
			if (car[i]->GetY() >= 0)
			{
				carStatesArray[i] = CarGoingRight;
			}
			else
			{
				car[i]->RotateZ(0.1f);
				car[i]->MoveX(0.01f);
				car[i]->MoveY(0.01f);
			}
		}
	}
}

void TyreMovement(tyreStates tyreStatesArray[kTyreAmount])
{

	for (int i = 0; i < kTyreAmount; ++i)
	{
		if (tyreStatesArray[i] == TyreGoingLeft)
		{
			if (tyres[i]->GetX() <= -50)
			{
				tyreStatesArray[i] = TyreGoingRight;
			}
			else  if (tyres[i]->GetX() <= 57 && tyres[i]->GetX() >= 55)
			{
				tyres[i]->SetY(-2.5);
				tyres[i]->MoveX(-tyreLaneSpeeds[i]);
			}
			else
			{
				tyres[i]->MoveX(-tyreLaneSpeeds[i]);
			}
		}
		else if (tyreStatesArray[i] == TyreGoingRight)
		{
			if (tyres[i]->GetX() >= 50)
			{
				tyreStatesArray[i] = TyreGoingLeft;
			}
			else  if (tyres[i]->GetX() >= -57 && tyres[i]->GetX() <= -55)
			{
				tyres[i]->SetY(-2.5);
				tyres[i]->MoveX(tyreLaneSpeeds[i]);
			}
			else
			{
				tyres[i]->MoveX(tyreLaneSpeeds[i]);
			}
		}
	}
}

void CheckForCollision(frogDirections currentFrogDirection)
{
	frogOldXpos = frog[currentFrog]->GetX();
	frogOldZpos = frog[currentFrog]->GetZ();
	if (frog[currentFrog]->GetZ() >= 25 && frog[currentFrog]->GetZ() <= 55 && frog[currentFrog]->GetX() >= -50 && frog[currentFrog]->GetX() <= 50)
	{

		for (int i = 0; i < KCarAmount; i++)
		{
			carMinX[i] = car[i]->GetX() - carLengthRadius[i] - frogRadius;
			carMaxX[i] = car[i]->GetX() + carLengthRadius[i] + frogRadius;
			carMinZ[i] = car[i]->GetZ() - carWidthRadius[i] - frogRadius;
			carMaxZ[i] = car[i]->GetZ() + carWidthRadius[i] + frogRadius;

			if (CollisionWithCar(carMinX[i], carMaxX[i], carMinZ[i], carMaxZ[i]))
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
			if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i]))
			{
				frog[currentFrog]->SetPosition(tyres[i]->GetX(), 0.0f, tyres[i]->GetZ());
				//MoveAlongTyre(i);
			}
			else
			{
				frogStateIdentifier[currentFrog] = dead;
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
				score = 10;
				//collidedWithCar = true;
			}
		}
	}
	else if (frog[currentFrog]->GetZ() >= 117.5 || frog[currentFrog]->GetZ() <= 7.5 || frog[currentFrog]->GetX() <= -52.5 || frog[currentFrog]->GetX() >= 52.5)
	{
		//currentFrogDirection = NotOnTyre;
		frogStateIdentifier[currentFrog] = dead;
	}
}

void CheckForSafeZone()
{
	if (frog[currentFrog]->GetX() >= 50 && frog[currentFrog]->GetX() <= 50 && frog[currentFrog]->GetZ() <= 117.5 && frog[currentFrog]->GetZ() >= 112.5)
	{
		frogStateIdentifier[currentFrog] = safe;

	}
}
void MoveAlongTyre(int tyreCollided)
{
	frog[currentFrog]->SetPosition(tyres[tyreCollided]->GetX(), 0.0f, tyres[tyreCollided]->GetZ());
}

void ResetCamera(I3DEngine* myEngine, ICamera* myCamera, IModel* dummyModel)
{
	if (myEngine->KeyHit(keyForCameraReset))
	{
		dummyModel->RotateLocalX(-(incrementBy));
		incrementBy = 0;
		//myCamera->DetachFromParent();
		////myCamera->ResetOrientation();
		//myCamera->SetPosition(0, 60, -160);
		//myCamera->RotateX(20);
		//myCamera->AttachToParent(dummyModel);
	}
}

void FrogMovement(IModel* dummyModel, frogRotation currentFrogRotation, int rotationAmount)
{
	if (currentFrogRotation == RotateRight)
	{
		if (rightRotateAmount == 1)
		{
			frog[currentFrog]->RotateLocalY(90);
			dummyModel->RotateLocalY(-90);
		}
		if (rightRotateAmount == 0)
		{
			frog[currentFrog]->RotateLocalY(-90);
			dummyModel->RotateLocalY(90);
		}
	}
	else if (currentFrogRotation == RotateLeft)
	{
		if (leftRotateAmount == -1)
		{
			frog[currentFrog]->RotateLocalY(-90);
			dummyModel->RotateLocalY(90);
		}
		if (leftRotateAmount == 0)
		{
			frog[currentFrog]->RotateLocalY(90);
			dummyModel->RotateLocalY(-90);
		}
	}
}