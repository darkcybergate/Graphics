#include <iostream>
#include <cmath>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define CANNON 0
#define RED 1
#define GREEN 2
#define BLACK 3

// Function Declarations
void drawScene();
void update(int value);
void generate_spider(int value);
void drawBox(float len);
void drawBall(float rad);
void drawTriangle();
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
void drawCannon();
void drawLaser(int len);
void drawbox(float length);
void drawLine(float len);
void drawSpider(float len);
void initialize_spider();
void GetOGLPos(int x, int y);
void handlemouse_drag(int x,int y);

// Global Variables
float box_len = 22.0f;
float theta = 0.0f; 
float cannon_x = 0.0f;      // initial x-coordinate of the cannon
float cannon_y = -(9*box_len)/20;    // initial y-coordinate of the cannon
float len_cannon = 2.0f;    
float red_boxlen = 3.0f;   // length of red box
float green_boxlen = 3.0f;  // length of green box
float green_boxpos = -9.0f;  // initial position of green box
float red_boxpos = 9.0f;     // initial position of red box
float spider_len=0.5f;       // length of spider
float spider_speed=0.05f;    // initial spider speed
float laser_length=box_len/10;

int flag=CANNON;      // flag indicate what is currently under our control
int laser_count=0;  // count of laser
int spider_count=0; // count of spider
int score=0;        // game score
int pause_flag=0;   // flag to indicate pause
int t=1000;
int game_over_flag=0;

float cannon_left_limit=-box_len/2;
float cannon_right_limit=box_len/2;
float red_left_limit=-box_len/2;
float red_right_limit=box_len/2;
float green_left_limit=-box_len/2;
float green_right_limit=box_len/2;

float spider_low[15]={0.023,0.02,0.03,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.08,0.09,0.1,0.11,0.12};
float spider_medium[15]={0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.08,0.09,0.1,0.11,0.12,0.13,0.135,0.14};
float spider_high[15]={0.055,0.06,0.065,0.07,0.08,0.09,0.1,0.11,0.12,0.13,0.135,0.14,0.145,0.15,0.16};

int lower=0;
int higher=3;
int time_cal=0;
int difficulty=0;

/* Laser is represented as structure*/
typedef struct laser 
{
	float laser_x;       // laser x co-ordinate
	float laser_y;       // laser y-coordinate
	float laser_angle;   // laser angle
	int flag_laser;      // flag of laser indicates whether laser is in screen or not
	float laser_x_translate;   // translate to x-direction
	float laser_y_translate;   // translate to y-direction

}laser;

char text[13]="SCOREBOARD\0";
char score_value[10];
char game_over[13]="GAME OVER\0";
/*Spider represented as structure */
typedef struct spider
{
	float spider_x;    // spider x co-ordinate
	float spider_y;    // spider y co-ordinate
	float spider_vel;   // spider velocity
	float spider_color;  // color of spider
	float spider_hit;    // indicates whether spider is dead or alive
	float taken;

}spider;

typedef struct power
{
	float x;
	float y;
	int power_up_flag;
}power;

spider spider_array[100];                // array of spiders
laser laser_array[100];          // array of lasers
power power_up[100];

float left_limit[100];
float right_limit[100];
int left_count=0;
int right_count=0;
int power_up_count=0;
float power_up_len=1.5f;


// Mouse Variable

float mouseposx;
float mouseposy;
float mouseposz;
float click;


void drawpower(float length)
{
	glBegin(GL_QUADS);
	glVertex2f(-length / 2, -length / 2);
	glVertex2f(length / 2, -length /2);
	glVertex2f(length / 2, length /2);
	glVertex2f(-length / 2, length/2 );
	glEnd();
}


void initialize_spider()
{
	for(int i=0;i<100;i++)
	{
		power_up[i].x=0;
		power_up[i].y=0;
		power_up[i].power_up_flag=0;
		spider_array[i].spider_x=0;
		spider_array[i].spider_y=0;
		spider_array[i].spider_vel=0;
		spider_array[i].spider_color=0;
		spider_array[i].spider_hit=0;
		spider_array[i].taken=0;
		laser_array[i].laser_x=0;
		laser_array[i].laser_y=0;
		laser_array[i].laser_angle=0;
		laser_array[i].flag_laser=0;
		laser_array[i].laser_y_translate=0.0f;
		laser_array[i].laser_x_translate=0.0f;
	}
}
void generate_power_up(int value)
{
	if(pause_flag==0)
	{
		power_up[power_up_count].power_up_flag=1;
		
		power_up[power_up_count].x=((rand()%19+rand()%19)%19)-9;
		power_up[power_up_count].y=box_len/2;
		power_up_count++;
		
	}
	glutTimerFunc(40000,generate_power_up,0);  
}

int main(int argc,char **argv)
{
	cout<<"Choose Difficulty : 1) Easy 2) Medium 3) Hard :\n";
	cin >> difficulty;
	initialize_spider();
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	right_limit[0]=box_len/2;
	left_limit[0]=-box_len/2;
	left_count++;
	right_count++;
	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int windowWidth = w * 2 / 3;
	int windowHeight = h * 2 / 3;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	glutCreateWindow("CSE251_sampleCode");  // Setup the window
	initRendering();

	/* Register callbacks */

	glutKeyboardFunc(handleKeypress1);
	glutMouseFunc(handleMouseclick);
	glutReshapeFunc(handleResize);
	glutDisplayFunc(drawScene);
	glutIdleFunc(drawScene);
	glutSpecialFunc(handleKeypress2);
	glutTimerFunc(10, update, 0);
	glutTimerFunc(4000,generate_spider,0);
	glutTimerFunc(40000,generate_power_up,0);
	glutMotionFunc(handlemouse_drag);
	glutMainLoop();
	return 0;
}

// Function called when the window is resized
void handleResize(int w, int h) 
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Initializing some openGL 3D rendering options
void initRendering() 
{
	glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // Setting a background color to Sky Blue r red 0.196078 green 0.8 blue 0.6
}


// creates a spider of random color after every 3 secs
void add_spider(float spider_x,float spider_y,float spider_color,float spider_vel,float spider_hit)
{
	spider_array[spider_count].spider_x=spider_x;
	spider_array[spider_count].spider_y=spider_y;
	spider_array[spider_count].spider_color=spider_color;
	int num=rand()%(higher-lower)+lower;
	if(difficulty==1)
	{
		spider_array[spider_count].spider_vel=spider_low[num];
	}
	else if(difficulty==2)
	{
		spider_array[spider_count].spider_vel=spider_medium[num];
	}
	else if(difficulty==3)
	{
		spider_array[spider_count].spider_vel=spider_high[num];
	}
	spider_array[spider_count].spider_hit=1;
	spider_count++;
	spider_count=spider_count%100;
	return;
}

// Function to draw objects on the screen
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();

	// Draw Box
	glTranslatef(0.0f, 0.0f, -30.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	drawBox(box_len);

	// Draw All Laser Beams that are in array of lasers
	for(int i=0;i<laser_count;i++)
	{   
		if(laser_array[i].flag_laser==1)
		{
			glPushMatrix();
			glTranslatef(laser_array[i].laser_x_translate,laser_array[i].laser_y_translate,0.0f);
			glRotatef(laser_array[i].laser_angle,0.0f,0.0f,1.0f);
			glColor3f(1.0f,0.0f,0.0f);
			drawLaser(i);
			glPopMatrix();
		}
	}
	for(int i=0;i<power_up_count;i++)
	{
		if(power_up[i].power_up_flag==1)
		{
			glPushMatrix();
			glTranslatef(power_up[i].x,power_up[i].y,0.0f);
			glColor3f(0.81f,0.71f,0.23f);      // color red 0.81 green 0.71 blue 0.23
			drawpower(power_up_len);
			glPopMatrix();
		}
	}
	if(game_over_flag==1)
	{
		// Indicate Game Over
		
		glPushMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glRasterPos3f(-1.5f, 5.0f, 0.0f);
		for(int i=0;i<10;i++)
		{ 
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, game_over[i]);
		} 
		glPopMatrix();
	}	

	// Draw Score Board

	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos3f(11.5f, 5.0f, 0.0f);
	for(int i=0;i<10;i++) 
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]); 
	glPopMatrix();

	sprintf(score_value,"%d", score); 
	int s_len=strlen(score_value); 
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
  	glRasterPos3f(15.5f, 3.5f, 0.0f);
  	for(int i=0;i<s_len;i++) 
   		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score_value[i]); 
	glPopMatrix();

	//Draw Cannon
	glPushMatrix();
	glTranslatef(cannon_x,cannon_y,0.0f);
	glRotatef(theta,0.0f,0.0f,1.0f);
	drawCannon();
	glPopMatrix();

	//Draw Green Box
	glPushMatrix();
	glTranslatef(green_boxpos,cannon_y+1.5f,0.0f);
	if(flag==GREEN)
	{
		glColor3f(0.5f,1.0f,0.5f);       // 0.5 1.0 0.5 light green   0.0 0.5 0.0 dark green
	}
	else
	{
		glColor3f(0.0f,0.5f,0.0f);    // Green color selected
	}
	drawbox(green_boxlen); 
	glPopMatrix();

	//Draw Red Box
	glPushMatrix();
	glTranslatef(red_boxpos,cannon_y+1.5f,0.0f);
	if(flag==RED)
	{
		glColor3f(0.917f,0.678f,0.917f);     // color red 0.917647 green 0.678431 blue 0.917647
	}
	else
	{
		glColor3f(1.0f,0.0f,0.0f);   // Red color selected
	}
	drawbox(red_boxlen);
	glPopMatrix();

	//Draw Line 
	glPushMatrix();
	glTranslatef(0.0,cannon_y+1.5f,0.0f);
	glColor3f(0.0f,0.0f,0.0f);
	glBegin(GL_LINES);
	glVertex2f(-box_len/2,0.0f);
	glVertex2f(box_len/2,0.0f);
	glEnd();
	glPopMatrix();

	// Draw Spider
	for(int k=0;k<100;k++)
	{
		if(spider_array[k].spider_hit==1)
		{
			glPushMatrix();
			glTranslatef(spider_array[k].spider_x,spider_array[k].spider_y,0.0f);
			int color=spider_array[k].spider_color;

			/*  Change Color Appropriately */
			if(color==RED)
			{
				glColor3f(1.0f,0.0f,0.0f);
			}
			else if(color==GREEN)
			{
				glColor3f(0.0f,0.8f,0.0f);
			}
			else if (color==BLACK)
			{
				glColor3f(0.0f,0.0f,0.0f);
			}
			drawSpider(spider_len);
			glPopMatrix();
		}
	}
	glPopMatrix();
	glutSwapBuffers();   // Print everything on screen
}


// Draw laser
void drawLaser(int len)
{
	glColor3f(1.0f, 0.0f, 0.0f);        
	glBegin(GL_LINES);
	glVertex2f(0.0f,laser_array[len].laser_y-laser_array[len].laser_y_translate);
	glVertex2f(0.0f,laser_array[len].laser_y-laser_array[len].laser_y_translate+laser_length);
	glEnd();
}

// Draw Line
void drawLine(float len)
{
	glBegin(GL_LINES);
	glVertex2f(0.0f,-len/2);
	glVertex2f(0.0f,len/2);
	glEnd();
}

// Draw Shape of Spider
void drawSpider(float length)
{
	glBegin(GL_QUADS);
	/* Main Body of Spider */
	glVertex2f(-length / 2, -length / 2);         
	glVertex2f(length / 2, -length / 2);
	glVertex2f(length / 2, length / 2);
	glVertex2f(-length / 2, length / 2);
	glEnd();
	/*Tenticals of Spider */
	glBegin(GL_LINE_STRIP);
	glVertex3f(-length/2,length/2,0.0f);
	glVertex3f(-length,length/2,0.0f);
	glVertex3f(-length,2.0*length,0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(-length/2,0.0f,0.0f);
	glVertex3f(-length,0.0f,0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(-length/2,-length/2,0.0f);
	glVertex3f(-length,-length/2,0.0f);
	glVertex3f(-length,-2.0*length,0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(length/2,length/2,0.0f);
	glVertex3f(length,length/2,0.0f);
	glVertex3f(length,2.0*length,0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(length/2,0.0f,0.0f);
	glVertex3f(length,0.0f,0.0f);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3f(length/2,-length/2,0.0f);
	glVertex3f(length,-length/2,0.0f);
	glVertex3f(length,-2.0*length,0.0f);
	glEnd();
}

// Draw red and green Box

void drawbox(float length)
{
	glBegin(GL_QUADS);
	glVertex2f(-length / 2, -length / 2);
	glVertex2f(length / 2, -length /2);
	glVertex2f(length / 2, length );
	glVertex2f(-length / 2, length );
	glEnd();
}

// Draw cannon 
void drawCannon()
{
	// #declare LightGrey = color red 0.658824 green 0.658824 blue 0.658824
	// SkyBlue color red 0.196078 green 0.6 blue 0.8
	
	if(flag==CANNON)
	{
		glColor3f(0.6588f,0.6588f,0.6588f);
	}
	else
	{
		glColor3f(0.4392f,0.576f,0.8588f);
	}  
	glBegin(GL_QUADS);      
	glVertex2f(0.5*len_cannon,0.0f);
	glVertex2f(0.5*len_cannon,0.5*len_cannon);
	glVertex2f(-0.5*len_cannon,0.5*len_cannon);
	glVertex2f(-0.5*len_cannon,0.0f);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(0.8*len_cannon,0.0f);
	glVertex2f(len_cannon,0.0f);
	glVertex2f(len_cannon,0.75*len_cannon);
	glVertex2f(0.8*len_cannon,0.75*len_cannon);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-len_cannon,0.0f);
	glVertex2f(-0.8*len_cannon,0.0f);
	glVertex2f(-0.8*len_cannon,0.75*len_cannon);
	glVertex2f(-len_cannon,0.75*len_cannon);
	glEnd();
	glBegin(GL_QUADS);            
	glVertex2f(-0.2*len_cannon,0.0f);
	glVertex2f(0.2*len_cannon,0.0f);
	glVertex2f(0.2*len_cannon,len_cannon);
	glVertex2f(-0.2*len_cannon,len_cannon);
	glEnd();
	glBegin(GL_QUADS);            
	glVertex2f(-0.5*len_cannon,0.2*len_cannon);
	glVertex2f(-0.5*len_cannon,0.3*len_cannon);
	glVertex2f(-0.8*len_cannon,0.3*len_cannon);
	glVertex2f(-0.8*len_cannon,0.2*len_cannon);
	glEnd();
	glBegin(GL_QUADS);            
	glVertex2f(0.5*len_cannon,0.3*len_cannon);
	glVertex2f(0.5*len_cannon,0.2*len_cannon);
	glVertex2f(0.8*len_cannon,0.2*len_cannon);
	glVertex2f(0.8*len_cannon,0.3*len_cannon);
	glEnd();
}

// Draw the main game area with red background
void drawBox(float len) 
{   

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex2f(-len / 2, -len / 2);
	glVertex2f(len / 2, -len / 2);
	glVertex2f(len / 2, len / 2);
	glVertex2f(-len / 2, len / 2);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Add Spider after every 3 secs
void generate_spider(int value)
{
	if(pause_flag==0)
	{
		float color_spider=0.0f,spider_x=0.0f,spider_y=0.0f,spider_vel=0.1f,spider_hit=0.0f;
		int num_of_spider=rand()%2+1;
		for(int i=0;i<num_of_spider;i++)
		{
			color_spider = (rand()%3+rand()%3)%3+1;
			spider_x=(rand()%19+rand()%19)%19;
			spider_x-=9;
			spider_y=box_len/2;
			add_spider(spider_x,spider_y,color_spider,spider_vel,spider_hit);
		}
	}
	glutTimerFunc(5000,generate_spider,0);   
}


// Function to handle all calculations in the scene
// updated evry 10 milliseconds
void update(int value)
{
	if(pause_flag==0)
	{
		int j=0;
		for(int i=0;i<spider_count;i++)
		{
			if(spider_array[i].spider_hit==1 && spider_array[i].spider_y>cannon_y+1.5f)
			{
				spider_array[i].spider_y-=spider_array[i].spider_vel;
				for(j=0;j<100;j++)
				{
					if(laser_array[j].flag_laser==1)
					{

						float actual_y=laser_array[j].laser_y_translate + (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*cos(DEG2RAD(laser_array[j].laser_angle));
						float actual_x = laser_array[j].laser_x_translate - (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*sin(DEG2RAD(laser_array[j].laser_angle));
						if((actual_y>=spider_array[i].spider_y-2*spider_len) && (actual_y<=spider_array[i].spider_y+2*spider_len)
								&& (actual_x>=spider_array[i].spider_x-spider_len) && (actual_x<=spider_array[i].spider_x+spider_len)
						  )
						{
							spider_array[i].spider_hit=0;
							laser_array[j].flag_laser=0;
							system("aplay explosion.wav &");
							if(spider_array[i].spider_color==BLACK)
							{
								score++;
							}
						}
						
					}
				}
			}
			if(spider_array[i].spider_hit==1 && spider_array[i].spider_y>cannon_y+1.5f && spider_array[i].spider_y<=cannon_y+1.5f+red_boxlen)
			{
				if(((spider_array[i].spider_x>=red_boxpos-red_boxlen/2) && (spider_array[i].spider_x<=red_boxpos+red_boxlen/2)))
				{
					spider_array[i].spider_hit=0;
					if(spider_array[i].spider_color==RED)
					{
						score++;
					}
					else
					{
						score--;
					}
				}
				else if (((spider_array[i].spider_x<=green_boxpos+green_boxlen/2) && (spider_array[i].spider_x>=green_boxpos-green_boxlen/2)))
				{
					spider_array[i].spider_hit=0;
					if(spider_array[i].spider_color==GREEN)
					{
						score++;
					}
					else
					{
						score--;
					}
				}
			}
			if(spider_array[i].spider_y<cannon_y+1.5f && spider_array[i].taken==0)
			{
				if(spider_array[i].spider_x<=cannon_x+len_cannon && spider_array[i].spider_x>=cannon_x-len_cannon)
				{
					game_over_flag=1;
					pause_flag=1;
					system("aplay gameover.wav &");
		//			exit(0);
				}
				score-=5;
				spider_array[i].taken=1;
				right_limit[right_count]=spider_array[i].spider_x-spider_len;
				right_count++;
				left_limit[left_count]=spider_array[i].spider_x+spider_len;
				left_count++;
				for(int i=0;i<right_count;i++)
				{
					if(cannon_x<right_limit[i])
					{
						if(cannon_right_limit>right_limit[i])
						{
							cannon_right_limit=right_limit[i];
						}
					}
				
				}
				for(int i=0;i<left_count;i++)
				{
					if(cannon_x>left_limit[i])
					{
						if(cannon_left_limit<left_limit[i])
						{
							cannon_left_limit=left_limit[i];
						}
					}
			
				}
			}
		}
		for(int j=0;j<laser_count;j++)
		{
			float actual_y=laser_array[j].laser_y_translate + (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*cos(DEG2RAD(laser_array[j].laser_angle));
			float actual_x = laser_array[j].laser_x_translate - (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*sin(DEG2RAD(laser_array[j].laser_angle));
			for(int k=0;k<power_up_count;k++)
			{
					if(power_up[k].power_up_flag==1 && laser_array[j].flag_laser==1)
					{
						if((actual_y>=power_up[k].y-power_up_len) && (actual_y<=power_up[k].y+power_up_len) && (actual_x>=power_up[k].x-power_up_len) && (actual_x<=power_up[k].x+power_up_len))
						{
														
							for(int p=0;p<spider_count;p++)
							{
								power_up[k].power_up_flag=0;
								spider_array[p].spider_hit=0;
								laser_array[j].flag_laser=0;
								cannon_left_limit=-box_len/2;
								cannon_right_limit=box_len/2;
								system("aplay big_explosion.wav &");
							}
						}	
					}
			}
		}
		for(j=0;j<laser_count;j++)
		{
			if(laser_array[j].flag_laser==1)
			{
				laser_array[j].laser_y+=spider_speed*4;
				float actual_y=laser_array[j].laser_y_translate + (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*cos(DEG2RAD(laser_array[j].laser_angle));
				float actual_x = laser_array[j].laser_x_translate - (laser_array[j].laser_y-laser_array[j].laser_y_translate+laser_length)*sin(DEG2RAD(laser_array[j].laser_angle));       
				if(actual_y>=box_len/2)
				{
					laser_array[j].flag_laser=0;
				}
				else if((actual_x>=box_len/2 && actual_y<=box_len/2) || (actual_x<=-box_len/2 && actual_y<=box_len/2))
				{
					laser_array[j].laser_y_translate=actual_y;
					laser_array[j].laser_x_translate=actual_x;
					laser_array[j].laser_y=laser_array[j].laser_y_translate;
					laser_array[j].laser_angle=-laser_array[j].laser_angle;         
				}
			}
		}
		for(int l=0;l<power_up_count;l++)
		{
			power_up[l].y-=spider_speed;
		}

		t+=10;
		time_cal+=10;
		if(time_cal>=20000 && higher<14)
		{
			time_cal=0;
			lower+=1;
			higher+=1;
		}
	}
	glutTimerFunc(10,update,0);
}


void handleKeypress1(unsigned char key, int x, int y) 
{
	if(key==32 && flag==CANNON && t>=1000)   // Shoot laser when "space" is pressed and cannon is selected
	{
		t=0;
		system("aplay LASER1.WAV &");
		laser_array[laser_count].laser_x=cannon_x;
		laser_array[laser_count].laser_y=cannon_y;
		laser_array[laser_count].laser_x_translate=cannon_x;
		laser_array[laser_count].laser_y_translate=cannon_y;
		laser_array[laser_count].laser_angle=theta;
		laser_array[laser_count].flag_laser=1;
		laser_count+=1;
		laser_count=laser_count%100;
	}
	if (key==98)  // "b" key to control laser
	{
		flag = CANNON;
	}
	else if (key==103)   // "g" key to control green box
	{
		flag = GREEN;
	}
	else if (key==114)  // "r" key to control red box
	{
		flag = RED;
	}
	else if(key==112)
	{
		if(pause_flag==1 && game_over_flag==0)
		{
			pause_flag=0;
		}
		else
		{
			pause_flag=1;
		}
	}
	else if (key == 27 || key==113) 
	{
		exit(0);     // escape key or "q" is pressed
	}
}

void handleKeypress2(int key, int x, int y) 
{
	if(pause_flag==0)
	{
	if(flag==CANNON)    // If cannon is selected
	{
		if (key == GLUT_KEY_LEFT)          // Move left if "LEFT ARROW" is pressed and range is correct
		{
			if(cannon_x - len_cannon -0.5f >= cannon_left_limit)
			{
				cannon_x -= spider_speed*10;
			}
		}
		else if (key == GLUT_KEY_RIGHT)       // Move right if "RIGHT ARROW" is pressed and range is correct
		{
			if(cannon_x + len_cannon + 0.5f <= cannon_right_limit )
			{
				cannon_x += spider_speed*10;
			}
		}
		else if (key == GLUT_KEY_UP)      // Move cannon upwards if "UP ARROW" is pressed and range is correct
		{
			if(theta<=60)
			{
				theta+=10.0f;
			}
		}
		else if (key == GLUT_KEY_DOWN)    // Move cannon downwards if "DOWN ARROW" is pressed and range is correct
		{
			if(theta>=-60)
			{
				theta-=10.0f;
			}
		}
	}
	if(flag==RED)
	{
		if(key==GLUT_KEY_LEFT)      // Move left if "LEFT ARROW" is pressed and range is correct
		{
			if((red_boxpos-red_boxlen/2) - 0.3f >= red_left_limit)
			{
				red_boxpos-=spider_speed*10;
			}
		}
		else if (key == GLUT_KEY_RIGHT)     // Move right if "RIGHT ARROW" is pressed and range is correct
		{
			if((red_boxpos+red_boxlen/2) + 0.3f <= red_right_limit)
			{
				red_boxpos+=spider_speed*10;
			}
		}
	}

	if(flag==GREEN)
	{
		if(key==GLUT_KEY_LEFT)         // Move left if "LEFT ARROW" is pressed and range is correct
		{
			if((green_boxpos-green_boxlen/2) - 0.3f >= green_left_limit)
			{
				green_boxpos-=spider_speed*10;
			}
		}
		else if (key == GLUT_KEY_RIGHT)     // Move right if "RIGHT ARROW" is pressed and range is correct
		{
			if((green_boxpos+green_boxlen/2) + 0.3f <= green_right_limit)
			{
				green_boxpos+=spider_speed*10;
			}
		}
	}
}
}


void GetOGLPos(int x, int y)
{
        GLint viewport[4];
        GLdouble modelview[16];
        GLdouble projection[16];
        GLfloat winX, winY, winZ;
        GLdouble posX, posY, posZ;
 
        glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
        glGetDoublev( GL_PROJECTION_MATRIX, projection );
        glGetIntegerv( GL_VIEWPORT, viewport );
 
        winX = (float)x;
        winY = (float)viewport[3] - (float)y;
        glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
        gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
        mouseposx=posX;
        mouseposy=posY;
        mouseposz=0;
}



void handlemouse_drag(int x,int y)
{
        GetOGLPos(x,  y);
        if(click==1)
        {
        		
                if(flag==CANNON &&  mouseposx>cannon_left_limit+len_cannon && mouseposx<cannon_right_limit-len_cannon)
                {
                        cannon_x=mouseposx;
                }
                else if(flag==RED && (mouseposx>-box_len/2+red_boxlen/2) && (mouseposx<box_len/2-red_boxlen/2))
                {
                        red_boxpos=mouseposx;
                }
                else if(flag==GREEN && (mouseposx>-box_len/2+green_boxlen/2) && (mouseposx<box_len/2-green_boxlen/2))
                {
                        green_boxpos=mouseposx;
                }
        }
        else if (click==2)
        {      
                flag==CANNON;
                float s1=mouseposx-cannon_x;
                float s2=mouseposy-cannon_y;
                float temp_angle=-atan(float(s1)/s2)*180/PI;
                if(temp_angle>=-60 && temp_angle<=60)
                {
                        theta=-atan(float(s1)/s2)*180/PI;
                }
        }
}



void handleMouseclick(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN)
    {
                if (button == GLUT_LEFT_BUTTON)
                {      
                        //              if(theta<=60)
                        //                      theta += 15;
                        click=1;
                        GetOGLPos(x,  y);
                       //  cout << mouseposx<< " "<<mouseposy<<" "<<cannon_x<<" "<<cannon_y<<endl;
                        float x1=cannon_x-len_cannon ; //tri_x-tank1_wid/2-tank3_wid-tank4_wid-tank1_wid/2;
                        float x2=cannon_x+len_cannon;                     //tri_x+tank1_wid/2+tank3_wid+tank4_wid+tank1_wid/2;
                        float y1=cannon_y;
                        float y2=cannon_y+len_cannon;
                        if(mouseposx>=x1 && mouseposx<=x2 && mouseposy>=y1 &&  mouseposy<=y2 )
                            {    flag=CANNON;}

                        float x11=green_boxpos-green_boxlen/2;                       //x_green-radius;
                        float x22=green_boxpos+green_boxlen/2;      
                        float y11=green_boxpos-green_boxlen/2;
                        float y22=green_boxpos+green_boxlen/2;
                       
                        if(mouseposx>=x11 && mouseposx<=x22 && mouseposy>=y11 &&  mouseposy<=y22 )
                                flag=GREEN;

                        float x111=red_boxpos-red_boxlen/2;
                        float x222=red_boxpos+red_boxlen/2;       
                        float y111=red_boxpos-red_boxlen/2;
                        float y222=red_boxpos+red_boxlen/2;
                       // cout << mouseposx << endl;     
                        if(mouseposx>=x111 && mouseposx<=x222 && mouseposy>=y111 &&  mouseposy<=y222 )
                        {
                                flag=RED;
                        }
 
                        //cout << x << " " <<  y << endl;
                }
                else if (button == GLUT_RIGHT_BUTTON)
                {
                        //              if(theta>=-60)
                        //                      theta -= 15;
                        click=2;
                }
    }
  
}
