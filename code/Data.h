#include <GL/glut.h>


#define RED  0
#define BLUE 1
#define GREEN 2
#define YELLOW 3
#define BLACK 4
#define ORANGE 5

#define USED 1
#define UNUSED 0 

typedef struct
{
	GLfloat Vertex[2];
}Point2D;

typedef struct 
{
	char clientId[50];
	int positionX;
	int positionY;
	int sensorColor;
	int sensorPosition;
}MotionSensor;

typedef struct
{

	char clientId[50];
	int positionX;
	int positionY;
	int sensorColor;
	int used;
	int	sensorPan;
	int sensorTilt;
}CameraSensor;

typedef struct
{
	char clientId[50];
	int positionX;
	int positionY;
	int ledColor;
}Led;

typedef struct
{
	char clientId[50];
	int positionX;
	int positionY;
	int color;
}Intrusion;


const Point2D Boundary[4] = {{100,100},{900,100},{900,900},{100,900}};		//field boundary coordinates

int FieldX = 64 ;		//lenght of the field in meters
int FieldY = 64 ;		//width of the field in meters 

int ResolutionX = 4;	//minimum trackable areas lenght in meters
int ResolutionY = 4;	//minimum trackable areas width in meters

int WindowX = 1000;		//lenght of the window to display graphics
int WindowY = 1000;		//width of the window to display graphics 

const float ScaleX = (Boundary[1].Vertex[0] - Boundary[0].Vertex[0]) / FieldX;			//scale in x direction
const float ScaleY = (Boundary[2].Vertex[1] - Boundary[1].Vertex[1]) / FieldY;			//scale in y direction

float IntrusionX = 0;
float IntrusionY = 0;
int IntrusionSpeed = 0;	//speed of the intruding object in kmph

int MotionSensorNumber = 0;		//number of motion sensor used 
int IntrusionNumber = 0;		//number of intrusion occureed
int CameraSensorNumber = 0;		//numbber of cameras used
int LedNumber = 0;				//number of leds used

float MotionSensorRange = 10;				//range of motion sensor
float CameraSensorRange = 15;				//range of camera 
float CameraSensorAngle = 60;			//range angle of camera sensor

int MotionNumber = 75;							//randow number to restrict motion and linearity of the motion
float DegreeScaleX = 15,DegreeScaleY = 15;		//number of pixels that will move with one degree of movement of the camera in x and y direction 


MotionSensor MotionSensorArray[50];
CameraSensor CameraSensorArray[50];
Led LedArray[300];
Intrusion IntrusionArray[300];
