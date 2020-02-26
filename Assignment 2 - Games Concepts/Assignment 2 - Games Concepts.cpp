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

struct SfrogStructure
{
	IModel* frogModel;
};

const float kGameSpeed = 0.25f;
const int KCarAmount = 8;
const int kCarTypes = 4;
const int kFrogAmount = 3;
const int kTreeAmount = 6;
const int kTyreAmount = 12;
const int kCarType = 4;
IModel* car[KCarAmount];
//IModel* frog[kFrogAmount];
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

int frogStateIdentifier[kFrogAmount];

bool collidedWithTyre;
bool collidedWithCar = false;
bool collidedWithTree;
bool movingLeft = false;
bool movingRight = false;
bool frogOnTyre;

//information about the tyre model
const float tyreSize = 10.0f;
const float tyreSpeed = 0.25f;
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


int score = 0;

float carWidthRadius[KCarAmount];
float carLengthRadius[KCarAmount];
//float carWidthRadius = 2.79333f;
//float carLengthRadius = 5.9069f;
int carXRange = 50;
int carRotateRange = 90;

float treeWidthRadius = 3.063925;
float treeLengthRadius = 3.04428;

SfrogStructure frog[kFrogAmount];

//function prototype calls
bool CollisionWithCar(float carminx, float carmaxx, float carminz, float carmaxz, SfrogStructure frog[kFrogAmount]);
bool CollisionWithTyre(float tyreminx, float tyremaxx, float tyreminz, float tyremaxz, SfrogStructure frog[kFrogAmount]);
bool CollisionWithTree(float treeminx, float treemaxx, float treeminz, float treemaxz, SfrogStructure frog[kFrogAmount]);
void InitialGameStatesForFrogs();
void Timer(float frameTime, IFont* GameOverFont, gameStates gameplay);
void DisplayScore(IFont* GameOverFont, int score);
void CheckForCollision(frogDirections currentFrogDirection, SfrogStructure frog[kFrogAmount], IModel* dummyModel, tyreStates tyreStatesArray[kTyreAmount], float frogOldXpos, float frogOldZpos);
void CarMovement(carStates carStatesArray[KCarAmount]);
void TyreMovement(tyreStates tyreStatesArray[kTyreAmount]);
void CheckForSafeZone(IModel* frog[kFrogAmount]);
void MoveAlongTyre(int tyreCollided, SfrogStructure frog[kFrogAmount]);
void ResetCamera(I3DEngine* myEngine, ICamera* myCamera, IModel* dummyModel);
//void FrogMovement(IModel* dummyModel, frogRotation currentRotation, int rotationAmount, IModel* frog[kFrogAmount]);

float startTimer = 21;
float incrementBy = 0;

//The boundary for the car collisions
const int trafficMinX = -50;
const int trafficMaxX = 50;
const int trafficMinZ = 25;
const int trafficMaxZ = 55;

const float islandOneY = -5.0f;
const float islandOneZ = 40.0f;
const float islandTwoY = -5.0f;
const float islandTwoZ = 115.0f;
const float skyBoxY = -1000.0f;
const float floorModelY = -2.5f;

const float frogStartX = -10.0f;
const float frogStartZ = 15.0f;

const float plantStartX = -50.0f;
const float plantXMultiplier = 20.0f;
const float plantStartZ = 115.0f;

const float carStartY = -2.5f;
const float carScaleMultiplier = 10.0f;

const int textSize = 45;

const float cameraStartY = 60.0f;
const float cameraStartZ = -160.0f;
const float cameraRotationAmount = 20;
const float cameraRotationSpeed = 0.01f;
const float cameraYLimit = 15;
const float cameraZLimit = 20;

const float frogSlideMovementSpeed = 0.35f;

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

EKeyCode MoveForward = EKeyCode(192);
EKeyCode MoveBackward = EKeyCode(191);
EKeyCode Wkey = Key_W;
EKeyCode Skey = Key_S;
EKeyCode MoveLeft = Key_Z;
EKeyCode MoveRight = Key_X;
EKeyCode Pausing = Key_P;
EKeyCode Escape = Key_Escape;
EKeyCode keyForCameraReset = Key_C;
EKeyCode cameraRotationUp = Key_Up;
EKeyCode cameraRotationDown = Key_Down;

int randomCar = 0;
float frameTime;

const int frogIncrement = 10;
const int pauseTime = 2000;
int waitTime = 0;


float tyreZPos[kTyreAmount] = {75, 75,  75, 85, 85, 85, 95, 95, 95, 105, 105, 105};
float tyreXPos[kTyreAmount] = {-60, -68, -76, 60, 68, 76, -60, -68, -76, 60, 68, 76};
float xPos[KCarAmount] = { -50.0f,   50.0f, -50.0f, 50.0f, -50.0f,   50.0f, -50.0f, 50.0f };
float zPos[KCarAmount] = { 25.0f, 35.0f, 45.0f, 55.0f, 25.0f, 35.0f, 45.0f, 55.0f };


float vehicleLane2Speed = 0.05f * kGameSpeed;
float vehicleLane1Speed = (1.0f / 2.0f) * vehicleLane2Speed;
float vehicleLane3Speed = 1.5 * vehicleLane2Speed;
float vehicleLane4Speed = 2 * vehicleLane2Speed;

float TransitLengthRadius = 6.1216f;
float TransitWidthRadius = 2.82654;
float roverLengthRadius = 5.9069;
float roverWidthRadius = 2.79333;
float pickUpLengthRadius = 6.14739;
float pickUpWidthRadius = 2.58098;
float rangeRoverLengthRadius = 5.53402;
float rangeRoverWidthRadius = 2.68256;


const float frogDeadScale = 0.5;
const int frogLimit = 2;
string frogDeadSkin = "frog_red.jpg";
string frogSafeSkin = "frog_blue.jpg";

const int gameOverX = 600;
const int gameOverY = 250;
const int timerX = 1125;
const int timerY = 0;

int timerInitialValue = 21;

const float carMovementXLimit = 60;
const float carMovementSlopeXLimit = 75;
const float carMovementSpeedThroughSlope = 0.01f;
const float carMovementRotationThroughSlope = 0.1f;

const float tyreMovementXLimit = 50;
const float additionalTyreFirstX = 57;
const float additionalTyreSecondX = 55;
const float additionalTyreY = 2.5f;

const float minimumSafeZoneX = -50.0f;
const float maximumSafeZoneX = 50.0f;
const float minimumSafeZoneZ = 112.5f;
const float maximumSafeZoneZ = 117.5f;

void main()
{

	gameStates game = playing;
	frogDirections currentFrogDirection = NotOnTyre;
	carStates carStatesArray[KCarAmount];
	tyreStates tyreStatesArray[kTyreAmount];
	frogMovement currentFrogMovement = Reset;


	//frogState frog[currentFrog] = crossing;
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);


	myEngine->StartWindowed();
	IFont* GameOverFont = myEngine->LoadFont("Comic Sans MS", textSize);

	string carModels[kCarType] = { "truck.bmp", "transit.bmp", "rover.bmp", "pickUp.bmp" };
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

	IMesh* carMesh[kCarTypes] = { myEngine->LoadMesh("transit.x") ,myEngine->LoadMesh("rover.x") ,myEngine->LoadMesh("pickUp.x") ,myEngine->LoadMesh("rangeRover.x") };


	IModel* dummyModel = dummy->CreateModel(0, 0, 0);
	IModel* islandTwoModel = islandTwoMesh->CreateModel(0, islandTwoY, islandTwoZ);
	IModel* islandOneModel = islandOneMesh->CreateModel(0, islandOneY, islandOneZ);
	IModel* skyBoxModel = skyBoxMesh->CreateModel(0, skyBoxY, 0);
	IModel* floorModel = floorMesh->CreateModel(0, floorModelY, 0);


	//creating the array of frogs being used
	IMesh* frogMesh = myEngine->LoadMesh("frog.x");
	for (int i = 0; i < kFrogAmount; ++i)
	{
		frog[i].frogModel = frogMesh->CreateModel(frogStartX + (i*frogDiameter), 0.0f, frogStartZ);
		frog[i].frogModel->Scale(frogSize);
	}
	dummyModel->AttachToParent(frog[currentFrog].frogModel);

	//Creating the array of cars being used
	for (int i = 0; i < KCarAmount; ++i)
	{
		randomCar = rand() % kCarType;
		switch (i)
		{
		case 0:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingRight;
			carLaneSpeeds[i] = vehicleLane1Speed;
			break;
		case 1:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingLeft;
			carLaneSpeeds[i] = vehicleLane2Speed;
			break;
		case 2:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingRight;
			carLaneSpeeds[i] = vehicleLane3Speed;
			break;
		case 3:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = CarGoingLeft;
			carLaneSpeeds[i] = vehicleLane4Speed;
			break;
		case 4:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = RightDownSlope;
			carLaneSpeeds[i] = vehicleLane1Speed;
			break;
		case 5:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = LeftDownSlope;
			carLaneSpeeds[i] = vehicleLane2Speed;
			break;
		case 6:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = RightDownSlope;
			carLaneSpeeds[i] = vehicleLane3Speed;
			break;
		case 7:
			car[i] = carMesh[randomCar]->CreateModel(xPos[i], 0.0f, zPos[i]);
			carStatesArray[i] = LeftDownSlope;
			carLaneSpeeds[i] = vehicleLane4Speed;
			break;
		}
		if (randomCar == 0)
		{
			carLengthRadius[i] = TransitLengthRadius;
			carWidthRadius[i] = TransitWidthRadius;
		}
		else if (randomCar == 1)
		{
			carLengthRadius[i] = roverLengthRadius;
			carWidthRadius[i] = roverWidthRadius;
		}
		else if (randomCar == 2)
		{
			carLengthRadius[i] = pickUpLengthRadius;
			carWidthRadius[i] = pickUpWidthRadius;
		}
		else if (randomCar == 3)
		{
			carLengthRadius[i] = rangeRoverLengthRadius;
			carWidthRadius[i] = rangeRoverWidthRadius;
		}

		car[i]->RotateY(carRotateRange);
		carXRange *= -1;
		carRotateRange *= -1;
		
	}


	IMesh* plantMesh = myEngine->LoadMesh("plant.x");
	for (int i = 0; i < kTreeAmount; ++i)
	{
		trees[i] = plantMesh->CreateModel(plantStartX + (i * plantXMultiplier), 0.0f, plantStartZ);

	}

	//Creating the array of tyres being used
	IMesh* tyreMesh = myEngine->LoadMesh("tyre.x");
	for (int i = 0; i < kTyreAmount; ++i)
	{
		if (i <= 2)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], carStartY, tyreZPos[i]);
			tyres[i]->Scale(carScaleMultiplier);
			tyreStatesArray[i] = TyreGoingRight;
			tyreLaneSpeeds[i] = tyreSpeedLane1;
		}
		else if (i > 2 && i <= 5)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], carStartY, tyreZPos[i]);
			tyres[i]->Scale(carScaleMultiplier);
			tyreStatesArray[i] = TyreGoingLeft;
			tyreLaneSpeeds[i] = tyreSpeedLane2;
		}
		else if (i > 5 && i <= 8)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], carStartY, tyreZPos[i]);
			tyres[i]->Scale(carScaleMultiplier);
			tyreStatesArray[i] = TyreGoingRight;
			tyreLaneSpeeds[i] = tyreSpeedLane3;
		}
		else if (i > 8 && i <= 11)
		{
			tyres[i] = tyreMesh->CreateModel(tyreXPos[i], carStartY, tyreZPos[i]);
			tyres[i]->Scale(carScaleMultiplier);
			tyreStatesArray[i] = TyreGoingLeft;
			tyreLaneSpeeds[i] = tyreSpeedLane4;
		}
		carXRange *= -1;
	}

	//Creating the camera and assigning it to the dummy model and the current frog
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0, cameraStartY, cameraStartZ);
	myCamera->RotateX(cameraRotationAmount);
	myCamera->AttachToParent(dummyModel);
	dummyModel->AttachToParent(frog[currentFrog].frogModel);

	InitialGameStatesForFrogs();

	//Main code for the running of the game
	while (myEngine->IsRunning())
	{
		myEngine->DrawScene();
		ResetCamera(myEngine, myCamera, dummyModel);
		frameTime = myEngine->Timer();

		vehicleLane1Speed *= frameTime;
		vehicleLane2Speed *= frameTime;
		vehicleLane3Speed *= frameTime;
		vehicleLane4Speed *= frameTime;

		if (game == playing)
		{
			frogOldXpos = frog[currentFrog].frogModel->GetX();
			frogOldZpos = frog[currentFrog].frogModel->GetZ();
			Timer(frameTime, GameOverFont, game);
			DisplayScore(GameOverFont, score);
	
			if (frogStateIdentifier[frogOne] == dead && frogStateIdentifier[frogTwo] == dead && frogStateIdentifier[frogThree] == dead)
			{
				game = over;
			}


			if (currentFrogMovement == Forward)
			{
				frog[currentFrog].frogModel->MoveZ(frogSlideMovementSpeed);
				if (frog[currentFrog].frogModel->GetZ() >= frogDesiredZlocation)
				{
					score += 10;
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Backward)
			{
				frog[currentFrog].frogModel->MoveZ(-frogSlideMovementSpeed);
				if (frog[currentFrog].frogModel->GetZ() <= frogDesiredZlocation)
				{
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Left)
			{
				frog[currentFrog].frogModel->MoveX(-frogSlideMovementSpeed);
				if (frog[currentFrog].frogModel->GetX() <= frogDesiredXLocation)
				{
					currentFrogMovement = Reset;
				}
			}
			else if (currentFrogMovement == Right)
			{
				frog[currentFrog].frogModel->MoveX(frogSlideMovementSpeed);
				if (frog[currentFrog].frogModel->GetX() >= frogDesiredXLocation)
				{
					currentFrogMovement = Reset;
				}
			}
			CheckForCollision(currentFrogDirection, frog, dummyModel, tyreStatesArray, frogOldXpos, frogOldZpos);

			CarMovement(carStatesArray);
			TyreMovement(tyreStatesArray);

			if (currentFrogDirection == NotOnTyre)
			{

			}
			else
			{
				if (currentFrogDirection == MovingLeft)
				{
					frog[currentFrog].frogModel->MoveX(-frogMovementOnTyreSpeed);
					if (frog[currentFrog].frogModel->GetX() <= -50)
					{
						currentFrogDirection = MovingRight;
					}
				}
				else if (currentFrogDirection == MovingRight)
				{
					frog[currentFrog].frogModel->MoveX(frogMovementOnTyreSpeed);
					if (frog[currentFrog].frogModel->GetX() >= 50)
					{
						currentFrogDirection = MovingLeft;
					}
				}
			}


			if (collidedWithCar)
			{
				frog[currentFrog].frogModel->SetSkin(frogDeadSkin);
				frog[currentFrog].frogModel->Scale(frogDeadScale);
				frogStateIdentifier[currentFrog] = dead;
				dummyModel->DetachFromParent();
				currentFrog++;
				frogStateIdentifier[currentFrog] = crossing;
				dummyModel->AttachToParent(frog[currentFrog].frogModel);
				collidedWithCar = !collidedWithCar;
				if (currentFrog > frogLimit) game = over;
			}

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

				if (myEngine->KeyHit(MoveLeft))
				{
					frogDesiredXLocation -= frogIncrement;
					currentFrogMovement = Left;

				}
				else if (myEngine->KeyHit(MoveRight))
				{
					frogDesiredXLocation += frogIncrement;
					currentFrogMovement = Right;
				}
				else if (myEngine->KeyHit(MoveForward) || myEngine->KeyHit(Wkey))
				{

					frogDesiredZlocation += frogIncrement;
					currentFrogMovement = Forward;
				}
				else if (myEngine->KeyHit(MoveBackward) || myEngine->KeyHit(Skey))
				{
					frogDesiredZlocation -= frogIncrement;
					currentFrogMovement = Backward;
				}

				if (myEngine->KeyHit(Escape))
				{
					myEngine->Stop();
				}

				if (myEngine->KeyHeld(cameraRotationUp))
				{
					if (myCamera->GetZ() >= (frog[currentFrog].frogModel->GetZ() - cameraZLimit))
					{

					}
					else
					{
						incrementBy += cameraRotationSpeed;
						dummyModel->RotateLocalX(cameraRotationSpeed);
					}
				}
				else if (myEngine->KeyHeld(cameraRotationDown))
				{
					if (myCamera->GetY() <= cameraYLimit)
					{

					}
					else
					{
						incrementBy -= cameraRotationSpeed;
						dummyModel->RotateLocalX(-cameraRotationSpeed);
					}
				}

				

			}


			else if (frogStateIdentifier[currentFrog] == dead)

			{
				if ((frog[currentFrog].frogModel->GetZ() >= 117.5 || frog[currentFrog].frogModel->GetZ() <= 7.5 || frog[currentFrog].frogModel->GetX() <= -52.5 || frog[currentFrog].frogModel->GetX() >= 52.5) || (frog[currentFrog].frogModel->GetZ() >= 75 && frog[currentFrog].frogModel->GetZ() <= 105 && frog[currentFrog].frogModel->GetX() >= -50 && frog[currentFrog].frogModel->GetX() <= 50))
				{
					frogDesiredXLocation = -frogIncrement;
					frogDesiredZlocation = 15;
					frog[currentFrog].frogModel->SetSkin(frogDeadSkin);
					frog[currentFrog].frogModel->Scale(frogDeadScale);
					dummyModel->DetachFromParent();
					frogStateIdentifier[currentFrog] = dead;
					currentFrog++;
					if (currentFrog > frogLimit)
					{
						game = over;
					}
					else
					{
						dummyModel->AttachToParent(frog[currentFrog].frogModel);
						frogStateIdentifier[currentFrog] = crossing;
					}
				}
				else if (frog[currentFrog].frogModel->GetZ() <= 65 && frog[currentFrog].frogModel->GetZ() >= 15 && frog[currentFrog].frogModel->GetX() >= -50 && frog[currentFrog].frogModel->GetX() <= 50)
				{
					frogDesiredXLocation = -10;
					frogDesiredZlocation = 15;
					frog[currentFrog].frogModel->SetSkin(frogDeadSkin);
					frog[currentFrog].frogModel->Scale(frogDeadScale);
					dummyModel->DetachFromParent();
					frogStateIdentifier[currentFrog] = dead;
					currentFrog++;
					if (currentFrog > frogLimit)
					{
						game = over;
					}
					else
					{
						dummyModel->AttachToParent(frog[currentFrog].frogModel);
						frogStateIdentifier[currentFrog] = crossing;
					}
				}

			}
			else if (frogStateIdentifier[currentFrog] == safe)
			{
				frogDesiredXLocation = -10;
				frogDesiredZlocation = 15;
				frog[currentFrog].frogModel->SetSkin(frogSafeSkin);
				dummyModel->DetachFromParent();
				currentFrog++;
				if (currentFrog > frogLimit)
				{
					game = over;
				}
				else
				{
					dummyModel->AttachToParent(frog[currentFrog].frogModel);
					frogStateIdentifier[currentFrog] = crossing;
				}
			}

		}
		else if (game == paused)
		{
			DisplayScore(GameOverFont, score);
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
			GameOverFont->Draw("Game Over", gameOverX, gameOverY);
			if (myEngine->KeyHit(Escape))
			{
				myEngine->Stop();
			}
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();

}

bool CollisionWithCar(float carminx, float carmaxx, float carminz, float carmaxz, SfrogStructure frog[kFrogAmount])
{
	float currentFrogX = frog[currentFrog].frogModel->GetX();
	float currentFrogZ = frog[currentFrog].frogModel->GetZ();
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
bool CollisionWithTyre(float tyreminx, float tyremaxx, float tyreminz, float tyremaxz, SfrogStructure frog[kFrogAmount])
{
	float currentFrogX = frog[currentFrog].frogModel->GetX();
	float currentFrogZ = frog[currentFrog].frogModel->GetZ();
	if (currentFrogX > tyreminx && currentFrogX < tyremaxx &&
		currentFrogZ < tyremaxz || currentFrogZ > tyreminz)
	{
		numberOfCollisions++;
		return true;
	}
	else
	{
		return false;
	}
}


bool CollisionWithTree(float treeminx, float treemaxx, float treeminz, float treemaxz, SfrogStructure frog[kFrogAmount])
{
	float currentFrogX = frog[currentFrog].frogModel->GetX();
	float currentFrogZ = frog[currentFrog].frogModel->GetZ();
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
	GameOverFont->Draw(time.str(), timerX, timerY);
	time.str("");
	if (timer < 0)
	{
		startTimer = timerInitialValue;
		frogStateIdentifier[currentFrog] = dead;
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
			if (car[i]->GetX() <= -carMovementXLimit)
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
			if (car[i]->GetX() >= carMovementXLimit) {
				carStatesArray[i] = RightDownSlope;
			}
			else
			{
				car[i]->MoveX(carLaneSpeeds[i]);
			}
		}
		else if (carStatesArray[i] == LeftDownSlope)
		{
			if (car[i]->GetX() <= -carMovementSlopeXLimit && waitTime >= pauseTime)
			{
				waitTime = 0;
				carStatesArray[i] = LeftUpSlope;
			}
			else
			{
				waitTime += 1;
				car[i]->RotateZ(carMovementRotationThroughSlope);
				car[i]->MoveX(-carMovementSpeedThroughSlope);
				car[i]->MoveY(-carMovementSpeedThroughSlope);
			}
		}
		else if (carStatesArray[i] == RightDownSlope)
		{
			//waitTime += 1
			if ((car[i]->GetX() >= carMovementSlopeXLimit || car[i]->GetY() <= -5) && waitTime >= pauseTime)
			{
				waitTime = 0;
				carStatesArray[i] = RightUpSlope;
			}
			else
			{
				waitTime += 1;
				car[i]->RotateZ(-carMovementRotationThroughSlope);
				car[i]->MoveX(carMovementSpeedThroughSlope);
				car[i]->MoveY(-carMovementSpeedThroughSlope);
			}
		}
		else if (carStatesArray[i] == LeftUpSlope)
		{
			car[i]->SetX(carMovementSlopeXLimit);
			if(car[i]->GetY() >= 0)
			{
				carStatesArray[i] = CarGoingLeft;
			}
			else
			{
				car[i]->RotateZ(-carMovementRotationThroughSlope);
				car[i]->MoveX(-carMovementSpeedThroughSlope);
				car[i]->MoveY(carMovementSpeedThroughSlope);
			}

		}
		else if (carStatesArray[i] == RightUpSlope)
		{
			car[i]->SetX(-carMovementSlopeXLimit);
			if (car[i]->GetY() >= 0)
			{
				carStatesArray[i] = CarGoingRight;
			}
			else
			{
				car[i]->RotateZ(carMovementRotationThroughSlope);
				car[i]->MoveX(carMovementSpeedThroughSlope);
				car[i]->MoveY(carMovementSpeedThroughSlope);
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
			if (tyres[i]->GetX() <= -tyreMovementXLimit)
			{
				tyreStatesArray[i] = TyreGoingRight;
			}
			else  if (tyres[i]->GetX() <= additionalTyreFirstX && tyres[i]->GetX() >= additionalTyreSecondX)
			{
				tyres[i]->SetY(-additionalTyreY);
				tyres[i]->MoveX(-tyreLaneSpeeds[i]);
			}
			else
			{
				tyres[i]->MoveX(-tyreLaneSpeeds[i]);
			}
		}
		else if (tyreStatesArray[i] == TyreGoingRight)
		{
			if (tyres[i]->GetX() >= tyreMovementXLimit)
			{
				tyreStatesArray[i] = TyreGoingLeft;
			}
			else  if (tyres[i]->GetX() >= -additionalTyreFirstX && tyres[i]->GetX() <= -additionalTyreSecondX)
			{
				tyres[i]->SetY(-additionalTyreY);
				tyres[i]->MoveX(tyreLaneSpeeds[i]);
			}
			else
			{
				tyres[i]->MoveX(tyreLaneSpeeds[i]);
			}
		}
	}
}

void CheckForCollision(frogDirections currentFrogDirection, SfrogStructure frog[kFrogAmount], IModel* dummyModel, tyreStates tyreStatesArray[kTyreAmount], float frogOldXpos, float frogOldZpos)//IModel* frog[kFrogAmount])
{
	if (frog[currentFrog].frogModel->GetZ() >= 20 && frog[currentFrog].frogModel->GetZ() <= 60 && frog[currentFrog].frogModel->GetX() >= -50 && frog[currentFrog].frogModel->GetX() <= 50)
	{

		for (int i = 0; i < KCarAmount; i++)
		{
			carMinX[i] = car[i]->GetX() - carLengthRadius[i] - frogRadius;
			carMaxX[i] = car[i]->GetX() + carLengthRadius[i] + frogRadius;
			carMinZ[i] = car[i]->GetZ() - carWidthRadius[i] - frogRadius;
			carMaxZ[i] = car[i]->GetZ() + carWidthRadius[i] + frogRadius;

			if (CollisionWithCar(carMinX[i], carMaxX[i], carMinZ[i], carMaxZ[i], frog))
			{
				frogStateIdentifier[currentFrog] = dead;
				//collidedWithCar = true;
			}
		}
	}
	else if (frog[currentFrog].frogModel->GetZ() >= 70 && frog[currentFrog].frogModel->GetZ() <= 110 && frog[currentFrog].frogModel->GetX() >= -50 && frog[currentFrog].frogModel->GetX() <= 50)
	{
		if (frog[currentFrog].frogModel->GetZ() >= 70 && frog[currentFrog].frogModel->GetZ() <= 79)
		{
			for (int i = 0; i < 3; i++)
			{
				tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
				tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
				tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
				tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
				if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i], frog))
				{
					frogMovementOnTyreSpeed = tyreLaneSpeeds[i];
					if (tyreStatesArray[i] == TyreGoingLeft)
					{
						currentFrogDirection = MovingLeft;
						break;
					}
					else if (tyreStatesArray[i] == TyreGoingRight)
					{
						currentFrogDirection = MovingRight;
						break;
					}
					break;
				}
				else
				{
					frog[currentFrog].frogModel->SetY(10);
				}
				break;
			}
			
		}
		else if (frog[currentFrog].frogModel->GetZ() >= 80 && frog[currentFrog].frogModel->GetZ() <= 89)
		{
			for (int i = 3; i < 6; i++)
			{
				tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
				tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
				tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
				tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
				if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i], frog))
				{
					frogMovementOnTyreSpeed = tyreLaneSpeeds[i];
					if (tyreStatesArray[i] == TyreGoingLeft)
					{
						currentFrogDirection = MovingLeft;
						break;
					}
					else if (tyreStatesArray[i] == TyreGoingRight)
					{
						currentFrogDirection = MovingRight;
						break;
					}
					break;
				}
				else
				{
					frog[currentFrog].frogModel->SetZ(65);

				}
			}
		}
		else if (frog[currentFrog].frogModel->GetZ() >= 90 && frog[currentFrog].frogModel->GetZ() <= 99)
		{
			for (int i = 6; i < 9; i++)
			{
				tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
				tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
				tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
				tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
				if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i], frog))
				{
					frogMovementOnTyreSpeed = tyreLaneSpeeds[i];
					if (tyreStatesArray[i] == TyreGoingLeft)
					{
						currentFrogDirection = MovingLeft;
						break;
					}
					else if (tyreStatesArray[i] == TyreGoingRight)
					{
						currentFrogDirection = MovingRight;
						break;
					}
					break;
				}
				else
				{
					frog[currentFrog].frogModel->SetZ(65);
				}
			}
		}
		else if (frog[currentFrog].frogModel->GetZ() >= 100 && frog[currentFrog].frogModel->GetZ() <= 109)
		{
			for (int i = 9; i < 12; i++)
			{
				tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
				tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
				tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
				tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
				if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i], frog))
				{
					frogMovementOnTyreSpeed = tyreLaneSpeeds[i];
					if (tyreStatesArray[i] == TyreGoingLeft)
					{
						currentFrogDirection = MovingLeft;
						break;
					}
					else if (tyreStatesArray[i] == TyreGoingRight)
					{
						currentFrogDirection = MovingRight;
						break;
					}
					break;
				}
				else
				{
					frog[currentFrog].frogModel->SetZ(65);
				}
			}
		}
		//for (int i = 0; i < kFrogAmount; ++i)
		//{
		//	//tyreMinX[i] = tyres[i]->GetX() - tyreRadius;
		//	//tyreMaxX[i] = tyres[i]->GetX() + tyreRadius;
		//	//tyreMinZ[i] = tyres[i]->GetZ() - tyreRadius;
		//	//tyreMaxZ[i] = tyres[i]->GetZ() + tyreRadius;
		//	if (CollisionWithTyre(tyreMinX[i], tyreMaxX[i], tyreMinZ[i], tyreMaxZ[i], frog ))
		//	{
		//		frogMovementOnTyreSpeed = tyreLaneSpeeds[i];
		//		if (tyreStatesArray[i] == TyreGoingLeft)
		//		{
		//			currentFrogDirection = MovingLeft;
		//		}
		//		else if (tyreStatesArray[i] == TyreGoingRight)
		//		{
		//			currentFrogDirection = MovingRight;
		//		}

		//		if (currentFrogDirection == MovingLeft)
		//		{
		//			frog[currentFrog].frogModel->MoveX(-frogMovementOnTyreSpeed);
		//			if (frog[currentFrog].frogModel->GetX() <= -50)
		//			{
		//				currentFrogDirection = MovingRight;
		//			}
		//		}
		//		else if (currentFrogDirection == MovingRight)
		//		{
		//			frog[currentFrog].frogModel->MoveX(frogMovementOnTyreSpeed);
		//			if (frog[currentFrog].frogModel->GetX() >= 50)
		//			{
		//				currentFrogDirection = MovingLeft;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		frogStateIdentifier[currentFrog] = dead;
		//	}
		//}
	}
	else if (frog[currentFrog].frogModel->GetZ() >= 110 && frog[currentFrog].frogModel->GetZ() <= 120 && frog[currentFrog].frogModel->GetX() >= -52.5 && frog[currentFrog].frogModel->GetX() <= 52.5)
	{
		for (int i = 0; i < kTreeAmount; i++)
		{
			treeMinX[i] = trees[i]->GetX() - treeLengthRadius - frogRadius;
			treeMaxX[i] = trees[i]->GetX() + treeLengthRadius + frogRadius;
			treeMinZ[i] = trees[i]->GetZ() - treeWidthRadius - frogRadius;
			treeMaxZ[i] = trees[i]->GetZ() + treeWidthRadius + frogRadius;

			if (collidedWithTree = CollisionWithTree(treeMinX[i], treeMaxX[i], treeMinZ[i], treeMaxZ[i], frog))
			{
				frog[currentFrog].frogModel->SetX(frogOldXpos);
				frog[currentFrog].frogModel->SetZ(frogOldZpos);
				score = 10;
				//collidedWithCar = true;
			}
		}
	}
	else if (frog[currentFrog].frogModel->GetZ() >= 117.5 || frog[currentFrog].frogModel->GetZ() <= 12.5 || frog[currentFrog].frogModel->GetX() <= -53 || frog[currentFrog].frogModel->GetX() >= 53)
	{
		//currentFrogDirection = NotOnTyre;
		frog[currentFrog].frogModel->SetX(frogOldXpos);
		frog[currentFrog].frogModel->SetZ(frogOldZpos);
	}
}

void CheckForSafeZone(IModel* frog[kFrogAmount])
{
	if (frog[currentFrog]->GetX() >= minimumSafeZoneX && frog[currentFrog]->GetX() <= maximumSafeZoneX && frog[currentFrog]->GetZ() <= minimumSafeZoneZ && frog[currentFrog]->GetZ() >= maximumSafeZoneZ)
	{
		frogStateIdentifier[currentFrog] = safe;

	}
}
void MoveAlongTyre(int tyreCollided, SfrogStructure frog[kFrogAmount])
{
	frog[currentFrog].frogModel->SetPosition(tyres[tyreCollided]->GetX(), 0.0f, tyres[tyreCollided]->GetZ());
}

void ResetCamera(I3DEngine* myEngine, ICamera* myCamera, IModel* dummyModel)
{
	if (myEngine->KeyHit(keyForCameraReset))
	{
		dummyModel->RotateLocalX(-(incrementBy));
		incrementBy = 0;
	}
}

//void FrogMovement(IModel* dummyModel, frogRotation currentFrogRotation, int rotationAmount, IModel* frog[kFrogAmount])
//{
//	if (currentFrogRotation == RotateRight)
//	{
//		if (rightRotateAmount == 1)
//		{
//			frog[currentFrog]->RotateLocalY(90);
//			dummyModel->RotateLocalY(-90);
//		}
//		if (rightRotateAmount == 0)
//		{
//			frog[currentFrog]->RotateLocalY(-90);
//			dummyModel->RotateLocalY(90);
//		}
//	}
//	else if (currentFrogRotation == RotateLeft)
//	{
//		if (leftRotateAmount == -1)
//		{
//			frog[currentFrog]->RotateLocalY(-90);
//			dummyModel->RotateLocalY(90);
//		}
//		if (leftRotateAmount == 0)
//		{
//			frog[currentFrog]->RotateLocalY(90);
//			dummyModel->RotateLocalY(-90);
//		}
//	}
//}