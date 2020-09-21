#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glut.h>
#include "Pipe.h"
#include <math.h>
#include <string.h>
#include "Data.h"

using namespace std;

struct MessagePacket G;

int ShowBeam = 1;
int ShowRange = 0;



void pipeInitialize()
{
	N = 1;
	strcpy(PipeNames[0],"GRAPIPE");
	strcpy(PipePermission[0],"R");

	openPipes();
}

void pipeFunction(int x)
{
	int bytesRead;
	int temp;

	bytesRead = recieveMessagePacket("GRAPIPE",&G);
	if(bytesRead > 0)
	{

		if(strcmp(G.Command,"Exit") == 0)
		{
			printf("\nScale\t X:%f\tY:%f\n",ScaleX,ScaleY);
			printf("Number of Motion Sensor:%d\tNumber of Camera Sensor:%d\tNumber of Led:%d\n\n",MotionSensorNumber,CameraSensorNumber,LedNumber);
			closePipe();
			exit(0);
		}
		
		/*************************************** EDIT HERE TO ADD MORE COMMANDS************************/

		if(strcmp(G.Command, "MOTIONNUMBER") == 0)
		{
			sscanf (G.Payload,"%d",&temp);
			MotionSensorNumber = temp;
		}

		if(strcmp(G.Command, "CAMERANUMBER") == 0)
		{
			sscanf (G.Payload,"%d",&temp);
			CameraSensorNumber = temp;
		}

		if(strcmp(G.Command, "LEDNUMBER") == 0)
		{
			sscanf (G.Payload,"%d",&temp);
			LedNumber = temp;
		}

		if(strcmp(G.Command, "INTRUSIONNUMBER") == 0)
		{
			sscanf (G.Payload,"%d",&temp);
			IntrusionNumber = temp;
		}
		
		if(strcmp(G.Command, "INTRUSIONSPEED") == 0)
		{
			sscanf (G.Payload,"%d",&temp);
			IntrusionSpeed = temp;
		}

		if(strcmp(G.Command, "MOTIONSENSOR") == 0)
		{
			MotionSensor Y;
			int temp;
			sscanf (G.Payload,"%d %s %d %d %d %d",&temp,&Y.clientId,&Y.positionX,&Y.positionY,&Y.sensorColor,&Y.sensorPosition);
			memcpy(&MotionSensorArray[temp],&Y,sizeof(MotionSensor));
		}

		if(strcmp(G.Command, "CAMERASENSOR") == 0)
		{
			CameraSensor Y;
			int temp;
			sscanf (G.Payload,"%d %s %d %d %d %d %d %d",&temp,&Y.clientId,&Y.positionX,&Y.positionY,&Y.sensorColor,&Y.used,&Y.sensorPan,&Y.sensorTilt);
			memcpy(&CameraSensorArray[temp],&Y,sizeof(CameraSensor));
		}

		if(strcmp(G.Command, "LED") == 0)
		{
			Led Y;
			int temp;
			sscanf (G.Payload,"%d %s %d %d %d",&temp,&Y.clientId,&Y.positionX,&Y.positionY,&Y.ledColor);
			memcpy(&LedArray[temp],&Y,sizeof(Led));
		}
		
		if(strcmp(G.Command, "INTRUSION") == 0)
		{
			Intrusion Y;
			int temp;
			sscanf (G.Payload,"%d %s %d %d %d",&temp,&Y.clientId,&Y.positionX,&Y.positionY,&Y.color);
			memcpy(&IntrusionArray[temp],&Y,sizeof(Intrusion));
			
			IntrusionX = Y.positionX;
			IntrusionY = Y.positionY;
		}




















		/*************************************** DO NOT EDIT HERE ***********************************/	
		glutPostRedisplay();
	}
	
	glutTimerFunc(50,pipeFunction,1);
}

void init() 
{
  	glClearColor(1.0, 1.0, 1.0, 1.0);
	gluOrtho2D(0,WindowX,0,WindowY);
}

void setColor(int code)
{
	if(code == RED)
		glColor3f(1,0,0);

	if(code == BLUE)
		glColor3f(0,0,1);

	if(code == GREEN)
		glColor3f(0,1,0);

	if(code == BLACK)
		glColor3f(0,0,0);

	if(code == YELLOW)
		glColor3f(1,1,0);

	if(code == ORANGE)
		glColor3f(1,0.27,0);

}

void drawSquare(Point2D v1,Point2D v2,Point2D v3,Point2D v4)
{

	glBegin(GL_LINE_LOOP);
		glVertex2fv(v1.Vertex);
		glVertex2fv(v2.Vertex);
		glVertex2fv(v3.Vertex);
		glVertex2fv(v4.Vertex);
	glEnd();

}

void printText(int x,int y,char* string,int size)
{
	int i;
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(x,y);
	
	if(size == 12)
		for(i=0;i<strlen(string);i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,string[i]);
			
	if(size == 18)
		for(i=0;i<strlen(string);i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,string[i]);
	 		
	if(size == 24)
		for(i=0;i<strlen(string);i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,string[i]);
		
}



void drawCameraSensor(CameraSensor *x)
{
	float h = x->positionX;
	float k = x->positionY;
	float p = x->sensorPan;

		glEnable(GL_BLEND);
	  	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if(ShowRange == 1)
	{
		//DRAW THE RANGE

		//CIRCLE
		/*float radius = MotionSensorRange * ScaleX;
		const float step = M_PI / 16;         
		glBegin( GL_TRIANGLE_FAN );
		    glColor4f( 1,0,0.6,0.2);
		    glVertex2f(h, k);
		    for ( float angle = 0.0f; angle < ( 2.0f * M_PI ); angle += step )
		    {
		        float fSin = sinf(angle);
		        float fCos = cosf(angle);
		        glVertex2f( h + radius * fSin, k + radius * fCos );
		    }
		
		    glVertex2f(h + 0.0f, k +  radius );
		glEnd();*/

		//SQUARE
		float rangeX = ((CameraSensorRange * sqrt(2)) / 2) * ScaleX;
		float rangeY = ((CameraSensorRange * sqrt(2)) / 2) * ScaleY;
		Point2D temp1,temp2,temp3,temp4;
		temp1.Vertex[0] = h - rangeX;	temp1.Vertex[1] = k - rangeY;
		temp2.Vertex[0] = h + rangeX;	temp2.Vertex[1] = k - rangeY;
		temp3.Vertex[0] = h + rangeX;	temp3.Vertex[1] = k + rangeY;
		temp4.Vertex[0] = h - rangeX;	temp4.Vertex[1] = k + rangeY;
		glColor4f( 0,1,0.9,0.2);
		glBegin(GL_POLYGON);
			glVertex2fv(temp1.Vertex);
			glVertex2fv(temp2.Vertex);
			glVertex2fv(temp3.Vertex);
			glVertex2fv(temp4.Vertex);
		glEnd();
	}

	//DRAW THE MAIN SENSOR
	int tri_radius = 8;
	glDisable(GL_POINT_SMOOTH);
	setColor(x->sensorColor);
	glBegin(GL_TRIANGLES);
		glVertex2f(h - tri_radius,k - tri_radius);
		glVertex2f(h + tri_radius,k - tri_radius);
		glVertex2f(h,k + tri_radius);
	glEnd();

	//DRAW THE RANGE BEAM
	if(ShowBeam == 1)
	{
	
		float temp1 = (p + (CameraSensorAngle / 2)) * (M_PI / 180);
		float temp2 = (p - (CameraSensorAngle / 2)) * (M_PI / 180);
		float radius = MotionSensorRange * ScaleX ;
		const float step = M_PI / 32;  

		glBegin( GL_TRIANGLE_FAN );
			glColor4f( 0,1,0.9,0.20);
			glVertex2f(h, k);
			for ( float angle = temp2; angle < temp1; angle += step )
			{
			    float fSin = sinf(angle);
			    float fCos = cosf(angle);
			    glVertex2f( h + radius * fSin, k + radius * fCos );
			}
		glEnd();
	}
	

	//PRINT NAME
	printText(h - 25 ,k - 25,x -> clientId,12);

}

void drawMotionSensor(MotionSensor *x)
{
	float h = x->positionX;
	float k = x->positionY;
	

	if(ShowRange == 1)
	{
		//DRAW THE RANGE
		glEnable(GL_BLEND);
	  	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		//CIRCLE
		/*float radius = MotionSensorRange * ScaleX;
		const float step = M_PI / 16;         
		glBegin( GL_TRIANGLE_FAN );
		    glColor4f( 1,0,0.6,0.2);
		    glVertex2f(h, k);
		    for ( float angle = 0.0f; angle < ( 2.0f * M_PI ); angle += step )
		    {
		        float fSin = sinf(angle);
		        float fCos = cosf(angle);
		        glVertex2f( h + radius * fSin, k + radius * fCos );
		    }
		
		    glVertex2f(h + 0.0f, k +  radius );
		glEnd();*/

		//SQUARE
		float rangeX = ((MotionSensorRange * sqrt(2)) / 2) * ScaleX;
		float rangeY = ((MotionSensorRange * sqrt(2)) / 2) * ScaleY;
		Point2D temp1,temp2,temp3,temp4;
		temp1.Vertex[0] = h - rangeX;	temp1.Vertex[1] = k - rangeY;
		temp2.Vertex[0] = h + rangeX;	temp2.Vertex[1] = k - rangeY;
		temp3.Vertex[0] = h + rangeX;	temp3.Vertex[1] = k + rangeY;
		temp4.Vertex[0] = h - rangeX;	temp4.Vertex[1] = k + rangeY;
		glColor4f( 1,0,0.6,0.2);
		glBegin(GL_POLYGON);
			glVertex2fv(temp1.Vertex);
			glVertex2fv(temp2.Vertex);
			glVertex2fv(temp3.Vertex);
			glVertex2fv(temp4.Vertex);
		glEnd();
	}

	//DRAW THE MAIN SENSOR
	glDisable(GL_POINT_SMOOTH);
	setColor(x->sensorColor);
	glPointSize(15);
	glBegin(GL_POINTS);
		glVertex2f(x->positionX, x->positionY);
	glEnd();


	//PRINT NAME
	printText(h - 25 ,k - 25,x -> clientId,12);

}

void drawIntrusion(Intrusion *x)
{
		char temp[50] = "Speed:";
		char temp1[20];
		
		int radius = 5;
		float h = x -> positionX;
		float k = x -> positionY;
		const float step = M_PI / 16; 
        
		glBegin( GL_TRIANGLE_FAN );
			setColor(x -> color);
		    glVertex2f(h, k);
		    for ( float angle = 0.0f; angle < ( 2.0f * M_PI ); angle += step )
		    {
		        float fSin = sinf(angle);
		        float fCos = cosf(angle);
		        glVertex2f( h + radius * fSin, k + radius * fCos );
		    }
		    glVertex2f(h + 0.0f, k +  radius );
		glEnd();
		
	//PRINT NAME
	//sprintf(temp1,"%d",IntrusionSpeed);	
	//strcat(temp,temp1);
	//strcat(temp,"km/h");
	//printText(h - 18 ,k - 13,temp);

}


void key(unsigned char k,int x, int y)
{
	
	cout << "HELLO\n";
	if(k == 'r' || k == 'R')
	{
		ShowRange = (ShowRange == 1) ? 0 : 1;
		glutPostRedisplay();
	}

	if(k == 'b' || k == 'B')
	{
		ShowBeam = ShowBeam ? 0 : 1;
		glutPostRedisplay();
	}

}

/*void mouse(int button, int state,int x,int y)
{
	int normalX,normalY;

	normalX = x;
	normalY = abs(WindowY - y);

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		if( x<WindowX  && y<WindowY)
		{

			IntrusionX = normalX;
			IntrusionY = normalY;

			IntrusionArray[IntrusionNumber].positionX = normalX;
			IntrusionArray[IntrusionNumber].positionY = normalY;
			IntrusionArray[IntrusionNumber].color = BLACK;
			//printf("IX:%d\tIY:%d\n",normalX,normalY);
			//computeRange(&IntrusionArray[IntrusionNumber]);
			computeRange(1.50);
			IntrusionNumber = IntrusionNumber + 1;
		}

	glutPostRedisplay();

}*/


void display() 
{

	int i,j;
	char temp[50],temp1[50];
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//DRAW GRID
	glColor3f(0,0,1);
	glLineWidth(1);
	for(i=0; i<(FieldX / ResolutionX); i++)
		for(j=0;j<(FieldY / ResolutionY); j++)
		{
			Point2D temp0 = {Boundary[0].Vertex[0] + (i + 0)*ResolutionX*ScaleX, Boundary[0].Vertex[1] + (j + 0)*ResolutionY*ScaleY};
			Point2D temp1 = {Boundary[0].Vertex[0] + (i + 1)*ResolutionX*ScaleX, Boundary[0].Vertex[1] + (j + 0)*ResolutionY*ScaleY};
			Point2D temp2 = {Boundary[0].Vertex[0] + (i + 1)*ResolutionX*ScaleX, Boundary[0].Vertex[1] + (j + 1)*ResolutionY*ScaleY};
			Point2D temp3 = {Boundary[0].Vertex[0] + (i + 0)*ResolutionX*ScaleX, Boundary[0].Vertex[1] + (j + 1)*ResolutionY*ScaleY};
		
			drawSquare(temp0,temp1,temp2,temp3);
		}


	//DRAW BOUNDARY
	glColor3f(0,0,0);
	glLineWidth(2);
	drawSquare(Boundary[0],Boundary[1],Boundary[2],Boundary[3]);
	
	//print boundary coordinates
	strcpy(temp1,"(");
	sprintf(temp,"%d",(int)Boundary[0].Vertex[0]);	
	strcat(temp1,temp);
	strcat(temp1,",");
	sprintf(temp,"%d",(int)Boundary[0].Vertex[1]);	
	strcat(temp1,temp);
	strcat(temp1,")");
	printText(Boundary[0].Vertex[0]-30 ,Boundary[0].Vertex[1]-20,temp1,18);
	
	strcpy(temp1,"(");
	sprintf(temp,"%d",(int)Boundary[1].Vertex[0]);	
	strcat(temp1,temp);
	strcat(temp1,",");
	sprintf(temp,"%d",(int)Boundary[1].Vertex[1]);	
	strcat(temp1,temp);
	strcat(temp1,")");
	printText(Boundary[1].Vertex[0]-30 ,Boundary[1].Vertex[1]-20,temp1,18);
	
	strcpy(temp1,"(");
	sprintf(temp,"%d",(int)Boundary[2].Vertex[0]);	
	strcat(temp1,temp);
	strcat(temp1,",");
	sprintf(temp,"%d",(int)Boundary[2].Vertex[1]);	
	strcat(temp1,temp);
	strcat(temp1,")");
	printText(Boundary[2].Vertex[0]-30 ,Boundary[2].Vertex[1]+10,temp1,18);
	
	strcpy(temp1,"(");
	sprintf(temp,"%d",(int)Boundary[3].Vertex[0]);	
	strcat(temp1,temp);
	strcat(temp1,",");
	sprintf(temp,"%d",(int)Boundary[3].Vertex[1]);	
	strcat(temp1,temp);
	strcat(temp1,")");
	printText(Boundary[3].Vertex[0]-30 ,Boundary[3].Vertex[1]+10,temp1,18);
	

	//DRAW MOTION SENSOR
	for(i=0;i<MotionSensorNumber;i++)
		drawMotionSensor(&MotionSensorArray[i]);

	//DRAW CAMERA SENSOR
	for(i=0;i<CameraSensorNumber;i++)
		drawCameraSensor(&CameraSensorArray[i]);


	//DRAW LED

	//DRAW INTRUSION
	for(i=0;i<IntrusionNumber;i++)
		drawIntrusion(&IntrusionArray[i]);

	//DRAW INTRUSION TRACKING
	for(i=0;i<IntrusionNumber - 1;i++)
	{
		glLineWidth(1);
		setColor(BLACK);
		glBegin(GL_LINES);
			glVertex2d(IntrusionArray[i].positionX,IntrusionArray[i].positionY);
			glVertex2d(IntrusionArray[i+1].positionX,IntrusionArray[i+1].positionY);
		glEnd();
	}
	
	//INTRUSION DETAILS
	strcpy(temp1,"Intrusion Speed: ");
	sprintf(temp,"%d",IntrusionSpeed);
	strcat(temp1,temp);
	strcat(temp1," km/h");
	printText(200,60,temp1,18);
	
	strcpy(temp1,"Intrusion Position: (");
	sprintf(temp,"%d",(int)IntrusionX);
	strcat(temp1,temp);
	strcat(temp1,",");
	sprintf(temp,"%d",(int)IntrusionY);
	strcat(temp1,temp);
	strcat(temp1,")");
	printText(200,40,temp1,18);
	
	strcpy(temp1,"Intrusion Tracking Client: ");
	strcat(temp1,IntrusionArray[IntrusionNumber-1].clientId);
	printText(200,20,temp1,18);
	
  	glFlush();
	glutSwapBuffers();
}


int main(int argc, char** argv) 
{

	pipeInitialize();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	
  	glutInitWindowPosition(300, 300);
  	glutInitWindowSize(WindowX, WindowY);
  	glutCreateWindow("Field Sensors"); 

  	glutDisplayFunc(display);
	glutTimerFunc(50,pipeFunction,1);
	//glutKeyboardFunc(key);
	
  	init();

  	glutMainLoop();
}
