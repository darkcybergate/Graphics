//	This is a sample program that illustrates OpenGL and GLUT in 3D. It
//	renders a picture of 36 snowmen. The camera can be moved by dragging
//	the mouse. The camera moves forward by hitting the up-arrow key and
//	back by moving the down-arrow key. Hit ESC, 'q' or 'Q' to exit.
//
//	Warning #1: This program uses the function glutSpecialUpFunc, which
//	may not be available in all GLUT implementations. If your system
//	does not have it, you can comment this line out, but the up arrow
//	processing will not be correct.
//----------------------------------------------------------------------

#include <iostream>
#include <stdlib.h> 
#include <math.h> 
#include <stdio.h> 
#include <algorithm>
#include <GL/freeglut.h>
using namespace std;
float previous_height=0;
float current_height=0;
float acc=0.0f;

// The following works for both linux and MacOS
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// escape key (for exit)
#define ESC 27

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#include "imageloader.h"
#include "vec3f.h"
#include<math.h> 

#define WIN_WIDTH      600 
#define WIN_HEIGHT      600    
#define CYCLE_LENGTH   3.3f 
#define ROD_RADIUS      0.05f 
#define NUM_SPOKES      20 
#define SPOKE_ANGLE      18 
#define RADIUS_WHEEL   1.0f 
#define TUBE_WIDTH      0.08f 
#define RIGHT_ROD      1.6f 
#define RIGHT_ANGLE      48.0f 
#define MIDDLE_ROD      1.7f 
#define MIDDLE_ANGLE   106.0f 
#define BACK_CONNECTOR   0.5f 
#define LEFT_ANGLE      50.0f 
#define WHEEL_OFFSET   0.11f 
#define WHEEL_LEN      1.1f 
#define TOP_LEN         1.5f 
#define CRANK_ROD      0.7f 
#define CRANK_RODS      1.12f 
#define CRANK_ANGLE      8.0f 
#define HANDLE_ROD      1.2f 
#define FRONT_INCLINE   70.0f 
#define HANDLE_LIMIT   70.0f 

#define INC_STEERING   2.0f 
#define INC_SPEED      0.05f 


typedef struct coin
{
	float x;
	float y;
	int check;

}coin;

coin a[1000];
int coin_count=0;

/***************************************** 
*    All the Global Variables are Here 
****************************************/ 
/***************************************** 
*   Cycle - related variables 
******************************************/    
GLfloat pedalAngle, speed, steering; 

float rotation;
/******************************* 
*   User view realted variables 
********************************/ 
GLfloat camx,camy,camz; 
GLfloat anglex,angley,anglez; 

/**************************** 
*   Mouse related variables 
****************************/ 
int prevx,prevy; 
GLenum Mouse; 

/************************** 
*   Cycle position related 
*   variables 
***************************/ 
GLfloat xpos,zpos,direction; 

int bhat,times,key1;
float dist,vy,vx,previous_angle,var_angle=0;

void ZCylinder(GLfloat radius,GLfloat length); 
void XCylinder(GLfloat radius,GLfloat length); 

void drawFrame(void); 
void gear( GLfloat inner_radius, GLfloat outer_radius, 
        GLfloat width,GLint teeth, GLfloat tooth_depth ); 
void drawChain(void); 
void drawPedals(void); 
void drawTyre(void); 
void drawSeat(void); 
void help(void); 
void init(void); 
void reset(void); 
void display(void); 
void idle(void); 
void updateScene(void); 
void landmarks(void);    
void special(int key,int x,int y); 
void keyboard(unsigned char key,int x,int y); 
void mouse(int button,int state,int x,int y); 
void motion(int x,int y); 
void reshape(int w,int h); 
void glSetupFuncs(void); 
GLfloat Abs(GLfloat); 
GLfloat degrees(GLfloat); 
GLfloat radians(GLfloat); 
GLfloat angleSum(GLfloat, GLfloat); 


float roll_angle=0.0;
int roll_flag=0;

/************************************* 
*    End of the header file 
*************************************/ 

/************************************** 
*   Start of the source file "cycle.c" 
**************************************/ 
//#include "cycle.h" 

/****************************************** 
*           A 
*          1   ========== 2 
*           /\        |  B 
*          /    \       / 5 
*       E /    \D    / 
*        /        \   / C 
*       /        \ / 
*     3 ==========/ 4 
*          F 
*       1    =   212,82 
*       2   =   368,82 
*       5   =   369,94 
*       3   =   112,220 
*       4   =   249,232 
* 
*       214   =   73 
*       124   =   55 
*       142   =   52 
*       143   =   73 
*       134   =   50 
*       431   =   57 
****************************************/           

void ZCylinder(GLfloat radius,GLfloat length) 
{ 
   GLUquadricObj *cylinder; 
   cylinder=gluNewQuadric(); 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,0.0f); 
      gluCylinder(cylinder,radius,radius,length,15,5); 
   glPopMatrix(); 
} 

void XCylinder(GLfloat radius,GLfloat length) 
{ 
   glPushMatrix(); 
      glRotatef(90.0f,0.0f,1.0f,0.0f); 
      ZCylinder(radius,length); 
   glPopMatrix(); 
} 

// called by idle() 
void updateScene() 
{ 
   GLfloat xDelta, zDelta; 
   GLfloat rotation; 
   GLfloat sin_steering, cos_steering; 

   // if the tricycle is not moving then do nothing 
   if (-INC_SPEED < speed && speed < INC_SPEED) return; 

   if(speed < 0.0f) 
         pedalAngle = speed = 0.0f; 
    
   // otherwise, calculate the new position of the tricycle 
   // and the amount that each wheel has rotated. 
   // The tricycle has moved "speed*(time elapsed)". 
   // We assume that "(time elapsed)=1". 
   pedalAngle=pedalAngle+speed;
   pedalAngle=degrees(angleSum(radians(pedalAngle),speed/RADIUS_WHEEL));
   xDelta = speed*cos(radians(direction + steering)); 
   zDelta = speed*sin(radians(direction + steering)); 
   xpos += xDelta; 
   zpos -= zDelta; 
   pedalAngle = degrees(angleSum(radians(pedalAngle), speed/RADIUS_WHEEL)); 

   // we'll be using sin(steering) and cos(steering) more than once 
   // so calculate the values one time for efficiency 
   sin_steering = sin(radians(steering)); 
   cos_steering = cos(radians(steering)); 

   // see the assignment 3 "Hint" 
   rotation = atan2(speed * sin_steering, CYCLE_LENGTH + speed * cos_steering); 
   direction = degrees(angleSum(radians(direction),rotation)); 
} 

// angleSum(a,b) = (a+b) MOD 2*PI 
// a and b are two angles (radians) 
//  both between 0 and 2*PI 
GLfloat angleSum(GLfloat a, GLfloat b) 
{ 
  a += b; 
  if (a < 0) return a+2*PI; 
  else if (a > 2*PI) return a-2*PI; 
  else return a; 
} 

/************************************************ 
*   Draw the metal frame of the cycle and also 
*   draw the seat and the back wheel with 
*   this. 
*   All these parts are always together in the 
*   same plane.They never move out ot the 
*   PLANE!   ;) 
************************************************/ 
void drawFrame() 
{ 
   glColor3f(1.0f,0.0f,0.0f); 

   /******************************** 
   *   First draw the all the items 
   *   at the center of the frame. 
   *   Draw the bigger gear,the small 
   *   cylinder acting as the socket 
   *   for the pedals.Also DON'T 
   *   forget to draw the two 
   *   connecting cemtral rods 
   *********************************/ 
   glPushMatrix(); 
      /****************************** 
      *   Allow me to draw the BIGGER 
      *   gear and the socket cylinder 
      *******************************/ 
      glPushMatrix(); 
         /*************************** 
         *   Let the gear have the 
         *   green color 
         ***************************/ 
         glColor3f(0.0f,1.0f,0.0f); 
          
         /************************** 
         *   The gear should be 
         *   outside the frame !!! 
         *   This is the bigger 
         *   GEAR 
         ***************************/ 
         glPushMatrix(); 
            glTranslatef(0.0f,0.0f,0.06f); 
            glRotatef(-2*pedalAngle,0.0f,0.0f,1.0f); 
            gear(0.08f,0.3f,0.03f,30,0.03f); 
         glPopMatrix(); 
         /*************************** 
         *   Restore the color of the 
         *   frame 
         ****************************/ 
         glColor3f(1.0f,0.0f,0.0f); 
         glTranslatef(0.0f,0.0f,-0.2f); 
         ZCylinder(0.08f,0.32f); 
      glPopMatrix(); 
      /***************************** 
      *   Lets first draw the 
      *   rightmost rod of the frame 
      *******************************/ 
      glRotatef(RIGHT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,RIGHT_ROD); 
       
      /******************************* 
      *   Now draw the centre rod of 
      *   the frame which also supports 
      *   the seat 
      *********************************/ 
      glRotatef(MIDDLE_ANGLE-RIGHT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,MIDDLE_ROD); 
      /******************************** 
      *   We have drawn the support.So 
      *   let's draw the seat with a 
      *   new color 
      *********************************/ 
      glColor3f(1.0f,1.0f,0.0f); 
      glTranslatef(MIDDLE_ROD,0.0f,0.0f); 
      glRotatef(-MIDDLE_ANGLE,0.0f,0.0f,1.0f); 
      glScalef(0.3f,ROD_RADIUS,0.25f); 
      drawSeat(); 
      /********************** 
      *   Restore the color ! 
      ************************/ 
      glColor3f(1.0f,0.0f,0.0f); 
   glPopMatrix(); 
   /********************************* 
   *   Draw the horizontal part of 
   *   the frame. 
   *********************************/ 
    
   /********************************* 
   *   Draw the main single rod 
   *   connecting the center of the 
   *   frame to the back wheel of the 
   *   cycle 
   **********************************/ 
   glPushMatrix(); 
      glRotatef(-180.0f,0.0f,1.0f,0.0f); 
      XCylinder(ROD_RADIUS,BACK_CONNECTOR); 
    
      /*********************************** 
      *   Draw the two rods on the either 
      *   side of the wheel 
      *   These rods are part of the 
      *   horizontal part of the cycle 
      ************************************/ 
      glPushMatrix(); 
         glTranslatef(0.5f,0.0f,WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,RADIUS_WHEEL+TUBE_WIDTH); 
      glPopMatrix(); 
      glPushMatrix(); 
         glTranslatef(0.5f,0.0f,-WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,RADIUS_WHEEL+TUBE_WIDTH); 
      glPopMatrix(); 
   glPopMatrix(); 
    
   /************************************ 
   *   Draw the leftmost rods of the 
   *   frame of the cycle 
   *************************************/ 
   glPushMatrix(); 
      glTranslatef(-(BACK_CONNECTOR+RADIUS_WHEEL+TUBE_WIDTH),0.0f,0.0f); 
         /******************************** 
      *   Transalted to the back wheel 
      *   position.Why not draw the back 
      *   wheel and also the gear ? :)) 
      **********************************/ 
      glPushMatrix(); 
         glRotatef(-2*pedalAngle,0.0f,0.0f,1.0f); 
         drawTyre(); 
         glColor3f(0.0f,1.0f,0.0f); 
            gear(0.03f,0.15f,0.03f,20,0.03f); 
         glColor3f(1.0f,0.0f,0.0f); 
      glPopMatrix(); 
      glRotatef(LEFT_ANGLE,0.0f,0.0f,1.0f); 
          
      /************************************ 
      *   Draw the two rods on the either 
      *   side of the wheel connecting the 
      *   backwheel and topmost horizontal 
      *   part of the wheel 
      *************************************/ 
      glPushMatrix(); 
         glTranslatef(0.0f,0.0f,-WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,WHEEL_LEN); 
      glPopMatrix(); 
      glPushMatrix(); 
         glTranslatef(0.0f,0.0f,WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,WHEEL_LEN); 
      glPopMatrix(); 
       
      /***************************** 
      *   Draw the single rod of the 
      *   same setup 
      ******************************/ 
      glTranslatef(WHEEL_LEN,0.0f,0.0f); 
      XCylinder(ROD_RADIUS,CRANK_ROD); 
    
      /***************************** 
      *   Now Draw The topmost 
      *   Horizontal rod 
      *****************************/ 
      glTranslatef(CRANK_ROD,0.0f,0.0f); 
      glRotatef(-LEFT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,TOP_LEN); 
       
      /******************************* 
      *   Now instead of again traversing 
      *   all the way back and again 
      *   forward.WHY NOT DRAW THE 
      *   HANDLE FROM HERE ITSELF? 
      ********************************/ 
      /***************************** 
      *   Now draw the handle and 
      *   small support rod which 
      *   is incorporated in the 
      *   frame itself. 
      *   Set y-axis at te required 
      *   incline. 
      ******************************/ 
      glTranslatef(TOP_LEN,0.0f,0.0f); 
      glRotatef(-FRONT_INCLINE,0.0f,0.0f,1.0f); 
       
      /****************************** 
      *   Draw the small rod that acts 
      *   as the socket joining the 
      *   frame and the handle 
      ******************************/ 
      glPushMatrix(); 
         glTranslatef(-0.1f,0.0f,0.0f); 
         XCylinder(ROD_RADIUS,0.45f); 
      glPopMatrix(); 
       
      /****************************** 
      *   I Hope the handle can rotate 
      *   about its mean position 
      *******************************/ 
      glPushMatrix(); 
         glRotatef(-steering,1.0f,0.0f,0.0f); 
         /****************************** 
         *   Roll back to the height of 
         *   the handle to draw it 
         *******************************/ 
         glTranslatef(-0.3f,0.0f,0.0f); 

         /******************************** 
         *   We cannot use the incline    
         *   the incline to draw the 
         *   horizontal part of the rod 
         ********************************/ 
         glPushMatrix(); 
            glRotatef(FRONT_INCLINE,0.0f,0.0f,1.0f); 

            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,-HANDLE_ROD/2); 
               ZCylinder(ROD_RADIUS,HANDLE_ROD); 
            glPopMatrix(); 

            glPushMatrix(); 
               glColor3f(1.0f,1.0f,0.0f); 
               glTranslatef(0.0f,0.0f,-HANDLE_ROD/2); 
               ZCylinder(0.07f,HANDLE_ROD/4); 
               glTranslatef(0.0f,0.0f,HANDLE_ROD*3/4); 
               ZCylinder(0.07f,HANDLE_ROD/4); 
               glColor3f(1.0f,0.0f,0.0f); 
            glPopMatrix(); 
         glPopMatrix(); 

         /********************************* 
         *   Using this incline now draw 
         *   the handle.Maybe use this again 
         *   to draw the wheel. ;) 
         **********************************/ 
         glPushMatrix(); 
            /**************************** 
            *   Draw the main big rod 
            ****************************/ 
            XCylinder(ROD_RADIUS,CRANK_ROD); 

            /****************************** 
            *   Why not draw the two rods and 
            *   the WHEEL?   :) 
            *   Yes!So,first go to the 
            *   end of the main rod. 
            *******************************/ 
            glTranslatef(CRANK_ROD,0.0f,0.0f); 
            glRotatef(CRANK_ANGLE,0.0f,0.0f,1.0f); 

            /******************************* 
            *   Draw the two rods connecting 
            *   the handle and the front 
            *   wheel. 
            *   The two rods are at a incline 
            *   to the connector. 
            ********************************/ 
            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,WHEEL_OFFSET); 
               XCylinder(ROD_RADIUS,CRANK_RODS); 
            glPopMatrix(); 
            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,-WHEEL_OFFSET); 
               XCylinder(ROD_RADIUS,CRANK_RODS); 
            glPopMatrix(); 
               /******************************** 
            *   Why not draw the wheel. 
            *   The FRONT wheel to be precise 
            *********************************/ 
            glTranslatef(CRANK_RODS,0.0f,0.0f); 
            glRotatef(-2*pedalAngle,0.0f,0.0f,1.0f); 
            drawTyre(); 
         glPopMatrix(); 
      glPopMatrix();   /*   End of the rotation of the handle effect   */ 
   glPopMatrix(); 
} 

// Portions of this code have been borrowed from Brian Paul's Mesa 
// distribution. 
/* 
* Draw a gear wheel.  You'll probably want to call this function when 
* building a display list since we do a lot of trig here. 
* 
* Input:  inner_radius - radius of hole at center 
*      outer_radius - radius at center of teeth 
*      width - width of gear 
*      teeth - number of teeth 
*      tooth_depth - depth of tooth 
*/ 

void gear( GLfloat inner_radius, GLfloat outer_radius, GLfloat width, 
        GLint teeth, GLfloat tooth_depth ) 
{ 
    GLint i; 
    GLfloat r0, r1, r2; 
    GLfloat angle, da; 
    GLfloat u, v, len; 
    const double pi = 3.14159264; 

    r0 = inner_radius; 
    r1 = outer_radius - tooth_depth/2.0; 
    r2 = outer_radius + tooth_depth/2.0; 

    da = 2.0*pi / teeth / 4.0; 

    glShadeModel( GL_FLAT ); 

    glNormal3f( 0.0, 0.0, 1.0 ); 

    /* draw front face */ 
    glBegin( GL_QUAD_STRIP ); 
    for (i=0;i<=teeth;i++) { 
   angle = i * 2.0*pi / teeth; 
   glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 ); 
   glVertex3f( r1*cos(angle), r1*sin(angle), width*0.5 ); 
   glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 ); 
   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 ); 
    } 
    glEnd(); 

    /* draw front sides of teeth */ 
    glBegin( GL_QUADS ); 
    da = 2.0*pi / teeth / 4.0; 
    for (i=0;i<teeth;i++) { 
   angle = i * 2.0*pi / teeth; 

   glVertex3f( r1*cos(angle),      r1*sin(angle),     width*0.5 ); 
   glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),     width*0.5 ); 
   glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), width*0.5 ); 
   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), width*0.5 ); 
    } 
    glEnd(); 


    glNormal3f( 0.0, 0.0, -1.0 ); 

    /* draw back face */ 
    glBegin( GL_QUAD_STRIP ); 
    for (i=0;i<=teeth;i++) { 
   angle = i * 2.0*pi / teeth; 
   glVertex3f( r1*cos(angle), r1*sin(angle), -width*0.5 ); 
   glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 ); 
   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 ); 
   glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 ); 
    } 
    glEnd(); 

    /* draw back sides of teeth */ 
    glBegin( GL_QUADS ); 
    da = 2.0*pi / teeth / 4.0; 
    for (i=0;i<teeth;i++) { 
   angle = i * 2.0*pi / teeth; 

   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 ); 
   glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 ); 
   glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),     -width*0.5 ); 
   glVertex3f( r1*cos(angle),      r1*sin(angle),     -width*0.5 ); 
    } 
    glEnd(); 


    /* draw outward faces of teeth */ 
    glBegin( GL_QUAD_STRIP ); 
    for (i=0;i<teeth;i++) { 
   angle = i * 2.0*pi / teeth; 

   glVertex3f( r1*cos(angle),      r1*sin(angle),      width*0.5 ); 
   glVertex3f( r1*cos(angle),      r1*sin(angle),     -width*0.5 ); 
   u = r2*cos(angle+da) - r1*cos(angle); 
   v = r2*sin(angle+da) - r1*sin(angle); 
   len = sqrt( u*u + v*v ); 
   u /= len; 
   v /= len; 
   glNormal3f( v, -u, 0.0 ); 
   glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),      width*0.5 ); 
   glVertex3f( r2*cos(angle+da),   r2*sin(angle+da),     -width*0.5 ); 
   glNormal3f( cos(angle), sin(angle), 0.0 ); 
   glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da),  width*0.5 ); 
   glVertex3f( r2*cos(angle+2*da), r2*sin(angle+2*da), -width*0.5 ); 
   u = r1*cos(angle+3*da) - r2*cos(angle+2*da); 
   v = r1*sin(angle+3*da) - r2*sin(angle+2*da); 
   glNormal3f( v, -u, 0.0 ); 
   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da),  width*0.5 ); 
   glVertex3f( r1*cos(angle+3*da), r1*sin(angle+3*da), -width*0.5 ); 
   glNormal3f( cos(angle), sin(angle), 0.0 ); 
    } 

    glVertex3f( r1*cos(0.0), r1*sin(0.0), width*0.5 ); 
    glVertex3f( r1*cos(0.0), r1*sin(0.0), -width*0.5 ); 

    glEnd(); 


    glShadeModel( GL_SMOOTH ); 

    /* draw inside radius cylinder */ 
    glBegin( GL_QUAD_STRIP ); 
    for (i=0;i<=teeth;i++) { 
   angle = i * 2.0*pi / teeth; 
   glNormal3f( -cos(angle), -sin(angle), 0.0 ); 
   glVertex3f( r0*cos(angle), r0*sin(angle), -width*0.5 ); 
   glVertex3f( r0*cos(angle), r0*sin(angle), width*0.5 ); 
    } 
    glEnd(); 

} 

/****************************************** 
*   Could not model the exact chain 
*   Think it eats up a lot of power if 
*   approximated by a lot of spheres 
*   So approximated with the stippled 
*   lines instead 
******************************************/ 
void drawChain() 
{ 
   GLfloat depth; 
   static int mode=0; 

   glColor3f(0.0f,1.0f,0.0f); 
   glEnable(GL_LINE_STIPPLE); 
   mode=(mode+1)%2; 
    
   if(mode==0 && speed>0) 
      glLineStipple(1,0x1c47); 
   else if(mode==1 && speed>0) 
      glLineStipple(1,0x00FF); 
    
   glBegin(GL_LINES); 
   for(depth=0.06f;depth<=0.12f;depth+=0.01f) 
   { 
      glVertex3f(-1.6f,0.15f,ROD_RADIUS); 
      glVertex3f(0.0f,0.3f,depth); 
       
      glVertex3f(-1.6f,-0.15f,ROD_RADIUS); 
      glVertex3f(0.0f,-0.3f,depth); 
   } 
   glEnd(); 
   glDisable(GL_LINE_STIPPLE); 
} 

void drawSeat() 
{ 
   /********************************* 
   *   Draw the top of the seat 
   **********************************/ 
   glBegin(GL_POLYGON); 
      glVertex3f(-0.1f, 1.0f, -0.5f); 
      glVertex3f(   1.0f, 1.0f, -0.3f); 
      glVertex3f( 1.0f, 1.0f,  0.3f); 
      glVertex3f(-0.1f, 1.0f,  0.5f); 
      glVertex3f(-0.5f, 1.0f,  1.0f); 
      glVertex3f(-1.0f, 1.0f,  1.0f); 
      glVertex3f(-1.0f, 1.0f, -1.0f); 
      glVertex3f(-0.5f, 1.0f, -1.0f); 
   glEnd(); 

   /********************************** 
   *   Draw the bottom base part of the 
   *   seat 
   ************************************/ 
   glBegin(GL_POLYGON); 
      glVertex3f(-0.1f, -1.0f, -0.5f); 
      glVertex3f(   1.0f, -1.0f, -0.3f); 
      glVertex3f( 1.0f, -1.0f,  0.3f); 
      glVertex3f(-0.1f, -1.0f,  0.5f); 
      glVertex3f(-0.5f, -1.0f,  1.0f); 
      glVertex3f(-1.0f, -1.0f,  1.0f); 
      glVertex3f(-1.0f, -1.0f, -1.0f); 
      glVertex3f(-0.5f, -1.0f, -1.0f); 
   glEnd(); 

   /********************** 
   *   Draw the sides! 
   ***********************/ 
   glBegin(GL_QUADS); 
      glVertex3f(1.0f,1.0f,-0.3f); 
      glVertex3f(1.0f,1.0f,0.3f); 
      glVertex3f(1.0f,-1.0f,0.3f); 
      glVertex3f(1.0f,-1.0f,-0.3f); 

      glVertex3f(1.0f,1.0f,0.3f); 
      glVertex3f(-0.1f,1.0f,0.5f); 
      glVertex3f(-0.1f,-1.0f,0.5f); 
      glVertex3f(1.0f,-1.0f,0.3f); 

      glVertex3f(1.0f,1.0f,-0.3f); 
      glVertex3f(-0.1f,1.0f,-0.5f); 
      glVertex3f(-0.1f,-1.0f,-0.5f); 
      glVertex3f(1.0f,-1.0f,-0.3f); 

      glVertex3f(-0.1f,1.0f,0.5f); 
      glVertex3f(-0.5f,1.0f,1.0f); 
      glVertex3f(-0.5f,-1.0f,1.0f); 
      glVertex3f(-0.1f,-1.0f,0.5f); 

      glVertex3f(-0.1f,1.0f,-0.5f); 
      glVertex3f(-0.5f,1.0f,-1.0f); 
      glVertex3f(-0.5f,-1.0f,-1.0f); 
      glVertex3f(-0.1f,-1.0f,-0.5f); 

      glVertex3f(-0.5f,1.0f,1.0f); 
      glVertex3f(-1.0f,1.0f,1.0f); 
      glVertex3f(-1.0f,-1.0f,1.0f); 
      glVertex3f(-0.5f,-1.0f,1.0f); 

      glVertex3f(-0.5f,1.0f,-1.0f); 
      glVertex3f(-1.0f,1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,-1.0f); 
      glVertex3f(-0.5f,-1.0f,-1.0f); 

      glVertex3f(-1.0f,1.0f,1.0f); 
      glVertex3f(-1.0f,1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,1.0f); 

   glEnd(); 


} 

void drawPedals() 
{ 
   glColor3f(0.0f,0.0f,1.0f); 
   /*************************** 
   *   Lets draw the two pedals 
   *   offset from the center 
   *   of the frame. 
   ****************************/ 

   /***************************** 
   *   First draw the one visible 
   *   to the viewer 
   ******************************/ 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,0.105f); 
      glRotatef(-pedalAngle,0.0f,0.0f,1.0f); 
      glTranslatef(0.25f,0.0f,0.0f); 
      /************************* 
      *   Draw the pedal rod 
      *************************/ 
      glPushMatrix(); 
         glScalef(0.5f,0.1f,0.1f); 
         glutSolidCube(1.0f); 
      glPopMatrix(); 

      /************************ 
      *   Draw the resting pad 
      ************************/ 
      glPushMatrix(); 
         glTranslatef(0.25f,0.0f,0.15f); 
         glRotatef(pedalAngle,0.0f,0.0f,1.0f); 
         glScalef(0.2f,0.02f,0.3f); 
         glutSolidCube(1.0f); 
      glPopMatrix(); 

   glPopMatrix(); 

   /******************************* 
   *   Draw the one on the other 
   *   side  of the frame 
   *******************************/ 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,-0.105f); 
      glRotatef(180.0f-pedalAngle,0.0f,0.0f,1.0f); 
      glTranslatef(0.25f,0.0f,0.0f); 

      /*************************** 
      *   Now again draw the pedal 
      *   rod 
      ****************************/ 
      glPushMatrix(); 
         glScalef(0.5f,0.1f,0.1f); 
         glutSolidCube(1.0f); 
      glPopMatrix(); 

      /**************************** 
      *   Draw the resting pad of 
      *   the pedal 
      *****************************/ 
      glPushMatrix(); 
         glTranslatef(0.25f,0.0f,-0.15f); 
         glRotatef(pedalAngle-180.0f,0.0f,0.0f,1.0f); 
         glScalef(0.2f,0.02f,0.3f); 
         glutSolidCube(1.0f); 
      glPopMatrix(); 

   glPopMatrix(); 

   glColor3f(1.0f,0.0f,0.0f); 
} 

void drawTyre(void) 
{ 
   int i; 
   //   Draw The Rim 
   glColor3f(1.0f,1.0f,1.0f); 
   glutSolidTorus(0.06f,0.92f,4,30); 
   //   Draw The Central Cylinder 
   //   Length of cylinder  0.12f 
   glColor3f(1.0f,1.0f,0.5f); 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,-0.06f); 
      ZCylinder(0.02f,0.12f); 
   glPopMatrix(); 
   glutSolidTorus(0.02f,0.02f,3,20); 

   //   Draw The Spokes 
   glColor3f(1.0f,1.0f,1.0f); 
   for(i=0;i<NUM_SPOKES;++i) 
   { 
      glPushMatrix(); 
         glRotatef(i*SPOKE_ANGLE,0.0f,0.0f,1.0f); 
         glBegin(GL_LINES); 
            glVertex3f(0.0f,0.02f,0.0f); 
            glVertex3f(0.0f,0.86f,0.0f); 
         glEnd(); 
      glPopMatrix(); 
   } 

   //   Draw The Tyre 
   glColor3f(0.0f,0.0f,0.0f); 
   glutSolidTorus(TUBE_WIDTH,RADIUS_WHEEL,10,30); 
   glColor3f(1.0f,0.0f,0.0f); 
} 

void init() 
{ 
   GLfloat mat_specular[]={1.0,1.0,1.0,1.0}; 
   GLfloat mat_shininess[]={100.0}; 
   GLfloat light_directional[]={1.0,1.0,1.0,1.0}; 
   GLfloat light_positional[]={1.0,1.0,1.0,0.0}; 
   GLfloat light_diffuse[]={1.0,1.0,1.0}; 

   reset(); 

   glShadeModel(GL_SMOOTH);    

   glLightfv(GL_LIGHT0,GL_POSITION,light_directional); 
   glLightfv(GL_LIGHT0,GL_AMBIENT,light_diffuse); 
   glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse); 
   glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess); 
   glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular); 
   glColorMaterial(GL_FRONT,GL_DIFFUSE); 

   glEnable(GL_LIGHTING); 
   glEnable(GL_LIGHT0); 
   glEnable(GL_COLOR_MATERIAL); 
   glEnable(GL_DEPTH_TEST); 
} 
    
void landmarks(void) 
{ 
      GLfloat i; 
      glColor3f(0.0f,1.0f,0.0f); 

      /************************************ 
      *   Draw the ground for the cycle 
      *   Looks incomplete with it!Don't 
      *   forget to define the normal 
      *   vectors for the vertices. 
      *   gotta fix this bug! 
      ************************************/ 
      glBegin(GL_LINES); 
      for(i=-100.0f ; i<100.0f ; i += 1.0f) 
      { 
         glVertex3f(-100.0f,-RADIUS_WHEEL,i); 
         glVertex3f( 100.0f,-RADIUS_WHEEL,i); 
         glVertex3f(i,-RADIUS_WHEEL,-100.0f); 
         glVertex3f(i,-RADIUS_WHEEL,100.0f); 
      } 
      glEnd(); 
} 


/************************ 
*   Returns the absolute 
*   value of a given 
*   float number 
************************/ 
GLfloat Abs(GLfloat a) 
{ 
   if(a < 0.0f) 
      return -a; 
   else 
      return a; 
} 

/************************ 
*   Returns the value of 
*   the given angle in 
*   degrees 
************************/ 
GLfloat degrees(GLfloat a) 
{ 
   return a*180.0f/PI; 
} 

/************************ 
*   Returns the value of 
*   the given angle in 
*   radians 
************************/ 
GLfloat radians(GLfloat a) 
{ 
   return a*PI/180.0f; 
} 

/************************* 
*   The idle function of 
*   the program which makes 
*   the contniuous loop 
***************************/ 
void idle(void) 
{ 
   updateScene(); 
   glutPostRedisplay(); 
} 

void special(int key,int x,int y) 
{ 
   switch(key) 
   { 
      case GLUT_KEY_UP: 
         camz -= 0.1f; 
         break; 
      case GLUT_KEY_DOWN: 
         camz += 0.1f; 
         break; 
      case GLUT_KEY_LEFT: 
         camx -= 0.1f; 
         break; 
      case GLUT_KEY_RIGHT: 
         camx += 0.1f; 
         break; 
   } 
   glutPostRedisplay(); 
} 

/***************************** 
*   Reset The scene 
*****************************/ 
void reset() 
{ 
   anglex=angley=anglez=0.0f; 
   pedalAngle=steering=0.0f; 
   Mouse=GLUT_UP; 
   pedalAngle=speed=steering=0.0f; 
   camx=camy=0.0f; 
   camz=5.0f; 
   xpos=zpos=0.0f; 
   direction=0.0f; 
} 

void keyboard(unsigned char key,int x,int y) 
{ 
   GLfloat r=0.0f; 

   switch(key) 
   { 
      case 's': 
      case 'S': 
         reset(); 
         break; 
      case 'z': 
         if(steering < HANDLE_LIMIT) 
               steering += INC_STEERING; 
         break; 
      case 'b': 
         if(steering > -HANDLE_LIMIT) 
            steering -= INC_STEERING; 
         break; 
      case '+': 
         speed += INC_SPEED; 
         break; 
      case '-': 
         speed -= INC_SPEED; 
         break;    
      case 27: 
         exit(1); 
   } 
    
   /******************************** 
   *   Where is my Cycle? 
   *********************************/ 

   /********************************* 
   *   When you rotate the handle the 
   *   cycle as a whole does not rotate 
   *   at once immediately. 
   *   For each unit of time, the 
   *   handle slowly begins to align 
   *   with the rest of the body of the 
   *   cycle. 
   *   Tough this is a gross approximation 
   *   it's workig fine now, maybe i'll 
   *   get some bugs. :< 
   *   I Think that the rate at which the 
   *   handle aligns with the body is 
   *   dependant on the speed too!! 
   *   The rate is given by 'delta' 
   *   and the speed is given by 'speed' 
   *   Now there should be no problems 
   ************************************/ 

   /*********************** 
   *   Check out the error 
   *   conditions  ;> 
   ***********************/ 
   pedalAngle += speed; 
   if(speed < 0.0f) 
      speed = 0.0f; 
   if(pedalAngle < 0.0f) 
      pedalAngle = 0.0f; 
   if(pedalAngle >= 360.0f) 
      pedalAngle -= 360.0f; 

   /****************** 
   *   Go! Display ;) 
   *******************/ 
   glutPostRedisplay(); 
} 

void mouse(int button,int state,int x,int y) 
{ 
   switch(button) 
   { 
      case GLUT_LEFT_BUTTON: 
         if(state==GLUT_DOWN) 
         { 
            Mouse=GLUT_DOWN; 
            prevx=x; 
            prevy=y; 
         } 
         if(state==GLUT_UP) 
         { 
            Mouse=GLUT_UP; 
         } 
         break; 
      case GLUT_RIGHT_BUTTON: 
         /*   DO NOTHING   */ 
         break; 
   } 
   glutPostRedisplay(); 
} 

void passive(int x,int y) 
{ 
/*   DO NOTHING   */ 
} 

void motion(int x,int y) 
{ 
   if(Mouse==GLUT_DOWN)    
   { 
      int deltax,deltay; 
      deltax=prevx-x; 
      deltay=prevy-y; 
      anglex += 0.5*deltax; 
      angley += 0.5*deltay; 
      if(deltax!=0 && deltay!=0) 
         anglez += 0.5*sqrt(deltax*deltax + deltay*deltay); 

      if(anglex < 0) 
         anglex+=360.0; 
      if(angley < 0) 
         angley+=360.0; 
      if(anglez < 0) 
         anglez += 360.0; 

      if(anglex > 360.0) 
         anglex-=360.0; 
      if(angley > 360.0) 
         angley-=360.0; 
      if(anglez > 360.0) 
         anglez-=360.0; 
   } 
   else 
   { 
      Mouse=GLUT_UP; 
   } 
   prevx=x; 
   prevy=y; 
   glutPostRedisplay();    
} 

void reshape(int w,int h) 
{ 
   glViewport(0,0,(GLsizei)w,(GLsizei)h); 
   glMatrixMode(GL_PROJECTION); 
   glLoadIdentity(); 
   gluPerspective(60.0,(GLfloat)w/(GLfloat)h,0.1,100.0); 
   //Angle,Aspect Ratio,near plane,far plane 
   glMatrixMode(GL_MODELVIEW); 
   glLoadIdentity(); 
   gluLookAt(camx,camy,camz,  0.0,0.0,0.0,  0.0,1.0,0.0); 
} 

void glSetupFuncs(void) 
{ 
 //  glutDisplayFunc(display); 
   glutReshapeFunc(reshape); 
   glutIdleFunc(idle); 
   glutSpecialFunc(special); 
   glutKeyboardFunc(keyboard); 
   glutMouseFunc(mouse); 
   glutMotionFunc(motion); 
   glutPassiveMotionFunc(passive); 
   glutSetCursor(GLUT_CURSOR_CROSSHAIR); 
} 

void help(void) 
{ 
   printf("Hierarchical 3D Model of a Bicycle\n"); 
   printf("TCS2111- Computer Graphics\n"); 
   printf("Group Project\n\n"); 
   printf("'+' to increase the speed\n"); 
   printf("'-' to decrease the speed\n"); 
   printf("'b' to rotate the handle in clockwise direction\n"); 
   printf("'z' to rotate the handle in anti-clockwise direction\n"); 
   printf("'s' or 'S' to reset the scene\n"); 
   printf("Arrow keys to move the camera\n"); 
   printf("Mouse to move the scene\n"); 
} 

float z_co=0.0f;

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;

			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}

			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}

			computedNormals = false;
		}

		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;

			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}

		int width() {
			return w;
		}

		int length() {
			return l;
		}

		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}

		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}

		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}

			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}

			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);

					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}

					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}

					normals2[z][x] = sum;
				}
			}

			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];

					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}

					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}

			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;

			computedNormals = true;
		}

		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}

	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
Terrain* _terrain;
float fossil_angle=0.0f;

void cleanup() {
	delete _terrain;
}
void initRendering() 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y) are the current camera position. The values (lx, ly)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. The variables isDragging and xDragStart are used to
// monitor the mouse when it drags (with the left button down).
//----------------------------------------------------------------------

// Camera position
float x = 0.0, y = -5.0; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 0.0, ly = 1.0; // camera points initially along y-axis
float angle = 0.0; // angle of rotation for the camera direction
float deltaAngle = 0.0; // additional angle change when dragging

// Mouse drag control
int isDragging = 0; // true when dragging
int xDragStart = 0; // records the x-coordinate when dragging startsl
float box_x=-4.0f;
float box_y=-4.0f;
int view_flag=1;
bool right_turn,left_turn;


//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
float ratio ;
void changeSize(int w, int h) 
{
	ratio =  ((float) w) / ((float) h); // window aspect ratio
	glMatrixMode(GL_PROJECTION); // projection matrix is active
	glLoadIdentity(); // reset the projection
	gluPerspective(45.0, ratio, 0.1, 1000.0); // perspective transformation
	glMatrixMode(GL_MODELVIEW); // return to modelview mode
	glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw one snowmen (at the origin)
//
// A snowman consists of a large body sphere and a smaller head sphere.
// The head sphere has two black eyes and an orange conical nose. To
// better create the impression they are sitting on the ground, we draw
// a fake shadow, consisting of a dark circle under each.
//
// We make extensive use of nested transformations. Everything is drawn
// relative to the origin. The snowman's eyes and nose are positioned
// relative to a head sphere centered at the origin. Then the head is
// translated into its final position. The body is drawn and translated
// into its final position.
//----------------------------------------------------------------------
void drawSnowman()
{
	glColor3f(0.85,0.85,0.1); 
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.75);
	glRotatef(fossil_angle,0,1,0);
	glutSolidCylinder(0.75,0.15, 20, 20);
	glPopMatrix();

}

//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------

void update(void) 
{
		fossil_angle+=0.5f;
		if(fossil_angle==360)
		{
			fossil_angle=0;
		}
		if(left_turn)
		{
			angle+=1.0f;
			//	printf("lx->  %f   ly-> %f\n",-sin(DEG2RAD(angle)),cos(DEG2RAD(angle)));
		}
		if(right_turn)
		{
			angle-=1.0f;
			//	printf("lx->  %f   ly-> %f\n",-sin(DEG2RAD(angle)),cos(DEG2RAD(angle)));
		}
		lx = -sin(DEG2RAD(angle));
		ly = cos(DEG2RAD(angle));
      if(deltaMove)
      {
         acc+=0.001f;
      }
      if(deltaMove==-1)
      {
         acc=0;
      }
      if(!deltaMove)
      {
         if(acc>0)
         {
            acc-=0.001;
         }
      }

		if (deltaMove)
		{ // update camera position
			box_x += deltaMove * lx * (0.1) + acc*lx;
			box_y += deltaMove * ly * (0.1) + acc*ly;
		}
      else
      {
         box_x+=0.1*lx*acc;
         box_y+=0.1*ly*acc;
      }

      if(roll_flag==1)
      {
         if(roll_angle<45)
         {
            roll_angle+=1;
         }
      }
      else if(roll_flag==2)
      {
         if(roll_angle>-45)
         {
            roll_angle-=1;
         }
      }
   
	glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------


float heightAt(Terrain* terrain, float x, float z) {
	//Make (x, z) lie within the bounds of the terrain
	if (x < 0) {
		x = 0;
	}
	else if (x > terrain->width() - 1) {
		x = terrain->width() - 1;
	}
	if (z < 0) {
		z = 0;
	}
	else if (z > terrain->length() - 1) {
		z = terrain->length() - 1;
	}

	//Compute the grid cell in which (x, z) lies and how close we are to the
	//left and outward edges
	int leftX = (int)x;
	if (leftX == terrain->width() - 1) {
		leftX--;
	}
	float fracX = x - leftX;

	int outZ = (int)z;
	if (outZ == terrain->width() - 1) {
		outZ--;
	}
	float fracZ = z - outZ;

	//Compute the four heights for the grid cell
	float h11 = terrain->getHeight(leftX, outZ);
	float h12 = terrain->getHeight(leftX, outZ + 1);
	float h21 = terrain->getHeight(leftX + 1, outZ);
	float h22 = terrain->getHeight(leftX + 1, outZ + 1);

	//Take a weighted average of the four heights
	return (1 - fracX) * ((1 - fracZ) * h11 + fracZ * h12) +
		fracX * ((1 - fracZ) * h21 + fracZ * h22);
}

float prevHeight=0.0f,h1=0.0,h2=0.0;
int jump=0;
void renderScene(void) 
{
   
	int i, j;

	// Clear color and depth buffers
	glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Reset transformations
	glLoadIdentity();

	


	if(view_flag==2)
	{
		// HELICOPTER VIEW
		gluLookAt(
				box_x-(10)*lx,     box_y-(10)*ly,    z_co + 13.0,
				box_x + lx, box_y + ly, 2.0 + z_co,
				0.0,    0.0,    1.0); 
	}
	else if(view_flag==1)
	{
		// Follow Cam
		gluLookAt(
				box_x - 7*lx , box_y - 7*ly , 3.0 + z_co ,
				box_x + lx , box_y + ly , 2.0 + z_co ,
				0.0,0.0,1.0
			 ); 
	}
	else if(view_flag==3)
	{
		// Driver View
		gluLookAt(
				box_x - lx, box_y - ly, 3.0 +z_co,
				(box_x + 2.0*lx) , (box_y+2.0*ly), 2.5+z_co,
				0.0,0.0,1.0	
			 );
	}
	else if (view_flag==4)
	{    // Wheel View
		gluLookAt(
				box_x+3*lx,box_y+3*ly,0.7 + z_co,
				box_x + 5.0*lx , box_y+5.0*ly , 1 + z_co,
				0.0,0.0,1.0
			 );
	}
	else if(view_flag==5)
	{   // Overhead View
		gluLookAt(
				box_x, box_y,10.0+z_co,
				box_x,box_y,3.0+z_co,
				lx, ly,3.0
			 );

	}
	else if(view_flag==6)
	{
		gluLookAt(
				box_x + 12.0 , box_y +2.5 , 2.0 + z_co ,
				box_x +lx , box_y + ly, 2.0 + z_co ,
				0.0,0.0,1.0
			 ); 
	}
   z_co=heightAt(_terrain,box_x,box_y);

	glPushMatrix();
	//	glTranslatef(0.0f, 0.0f, -10.0f);
	//	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	//	glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	//	float scale = 1000.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	//	glScalef(scale, scale, scale);
	//	glTranslatef(-(float)(_terrain->width() - 1) / 2,
	//				 -(float)(_terrain->length() - 1) / 2,0.0f);

//	glColor3f(0.3f, 0.9f, 0.0f);
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) 
		{	
				Vec3f normal = _terrain->getNormal(x, z);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x,z, _terrain->getHeight(x, z));
				normal = _terrain->getNormal(x, z + 1);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x,z+1, _terrain->getHeight(x, z + 1));
		
		}
		glEnd();
	}

	glColor3f(0.0f, 0.0f, 1.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) 
		{	
			if(_terrain->getHeight(x,z) <= 7.0f)
			{
				Vec3f normal = _terrain->getNormal(x, z);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x,z,-8.0f);
				normal = _terrain->getNormal(x, z + 1);
				glNormal3f(normal[0], normal[2], normal[1]);
				glVertex3f(x,z+1, -8.0f);
			}
		}
		glEnd();
	}
	glPopMatrix();

	// Draw 36 snow men
//	for(i=0;i<coin_count;i++)
	for(int i=0;i<20;i++)
   {
      glPushMatrix();
      int c1=a[i].x;
      int c2=a[i].y;
      int q=heightAt(_terrain,c1,c2);
      glTranslatef(c1,c2,q);
      glRotatef(90,1.0,0.0,0.0);
      drawSnowman();
      glPopMatrix();
   }

	
	
	z_co=heightAt(_terrain,box_x,box_y);
	float z2=heightAt(_terrain,box_x+lx*2.0,box_y+ly*2.0);
   h1=z2;
   h2=h1;
   if(jump==1)
   {
      h1=prevHeight-0.1;
      z2=h1;
      z_co=h1;
   }
   if(prevHeight-h2>0.1)
   {
      jump=1;
   }
   else
   {
      jump=0;
   }
   prevHeight=h1;
//	float z1=heightAt(_terrain,box_x-lx*2.0,box_y-ly*2.0);
	
	/*if(!bhat)
	{
		previous_angle=var_angle;
	}*/
   
	
   
   
   var_angle=(atan((z2-z_co)/4)*180)/PI; 
      //Choose a sticker or emoticon


	/*if(z_co>z2 && bhat==0 && (times%10==0 || key1==0))
	{
		bhat=1;
		key1=1;
		dist=z_co;

		vy=0.4*sin(previous_angle);
		vx=0.4*cos(previous_angle);
	}
	else
	{
		if(dist<z_co && bhat==1)
		{
			bhat=0;
			vx=vy=0;
			times=1;
		}
	}
	if(bhat==1)
	{
		var_angle=(atan(vy/vx)*180)/3.14;
		z_co=dist;
	}
	times++;*/
	/*glPushMatrix();
	glTranslatef(box_x+4.0*lx,box_y+4.0*ly,z2+1.0f);
	glRotatef(angle,0,0,1);
	glColor3f(0,0.0,0.0);
	glPushMatrix();
	glColor3f(0.0,0.0,0.0);
	glRotatef(90.0,0.0,-1.0,0.0);
	glTranslatef(-0.6,0.0,-0.25);
	glutSolidCylinder(0.5,0.5,20,20);
	glPopMatrix();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex3f(box_x,box_y,z_co+1.0f);
	glVertex3f(box_x+4.0*lx,box_y+4.0*ly,z2+1.0f);
	glEnd();
	glPopMatrix();
*/
//	cout<<box_x<<" "<<box_y<<"\n";
//	z_co=heightAt(_terrain,box_x,box_y);

	/*glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex3f(box_x,box_y,3.0+z_co);
	glVertex3f(box_x+(10)*sin(var_angle),box_y+(10)*cos(var_angle),3.0+z_co);
	glEnd();*/

	

	glPushMatrix();
	glTranslatef(box_x,box_y,z_co+1.0f);
	glRotatef(angle,0,0,1);

	glPushMatrix();

	//glPushMatrix();
      //   glBegin(GL_LINES);
       //  glVertex3f(0.0,0.0,0.0);
       //  glVertex3f(0.0,5.0,0.0);
       //  glEnd();
       //  glPopMatrix();
        // cout<<var_angle<<"\n";
	glRotatef(var_angle,1.0,0.0,0.0);
	
	/*     glPushMatrix();
         glBegin(GL_LINES);
         glVertex3f(0.0,0.0,0.0);
         glVertex3f(0.0,5.0,0.0);
         glEnd();
         glPopMatrix();*/
	// Drawing Bike starts here ------------------############--------------------

//	glPushMatrix();
	glColor3f(1.0,0.0,0.0); 
//	glRotatef((atan(z2-z_co/4)*180)/PI,1,0,0);
glPushMatrix();    
      /******************************* 
       *    Prepare the rotations 
       *    and start doing the 
       *    remaining scene 
       *******************************/ 
    //  glRotatef(angley,1.0f,0.0f,0.0f); 
    //  glRotatef(anglex,0.0f,1.0f,0.0f); 
    //  glRotatef(anglez,0.0f,0.0f,1.0f); 

      /*********************** 
       *    Start rendering    
       *    the scene; 
       *    the bicycle ;) 
       **********************/     
    

      /**************************** 
      *   Move the cycle. 
      ****************************/ 
  //    cout<<anglex<<" "<<angley<<" "<<anglez<<" "<<xpos<<" "<<zpos<<" "<<direction<<"\n";
     glPushMatrix(); 
	 glRotatef(90.0f,1.0f,0.0f,0.0f);
       //  glTranslatef(xpos,0.0f,zpos);
         
      glRotatef(90,0.0,1.0,0.0);
       	//glRotatef(180,1.0,0.0,0.0);

     //    glRotatef(direction,0.0f,1.0f,0.0f); 
         // Draw New Axis Start ---------------------------
/*
         glPushMatrix();
         glBegin(GL_LINES);
         glVertex3f(0.0,3.0,0);
         glVertex3f(3.0,3.0,0);
         glEnd();
         glPopMatrix();*/

         // Draw Axis Done ------------------------------
         glRotatef(roll_angle,-1.0,0.0,0.0);

         drawFrame();    
         drawChain(); 
         drawPedals(); 
      glPopMatrix(); 



   glPopMatrix(); //	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	
	glutSwapBuffers(); // Make it all visible
} 

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
	if (key == ESC || key == 'q' || key == 'Q')
	{
		exit(0);
	}
	if(key==49)
	{
		view_flag=1;
	}
	if(key==50)
	{
		view_flag=2;
	}
	if(key==51)
	{
		view_flag=3;
	}
	if(key==52)
	{
		view_flag=4;
	}
	if(key==53)
	{
		view_flag=5;
	}
	if(key==54)
	{
		view_flag=6;
	}
   if(key=='a')
   {
      roll_flag=1;
   }
   if(key=='d')
   {
      roll_flag=2;
   }
} 

void pressSpecialKey(int key, int xx, int yy)
{
	if(key==GLUT_KEY_UP)
	{
		deltaMove = 1.0;
	}
	else if(key==GLUT_KEY_DOWN)
	{
		deltaMove = -1.0; 
	}
	else if(key==GLUT_KEY_LEFT)
	{
		left_turn=true;
	}
	else if(key==GLUT_KEY_RIGHT)
	{
		right_turn=true;
	}
} 

void releaseSpecialKey(int key, int x, int y) 
{
	if(key==GLUT_KEY_UP)
	{
		deltaMove = 0.0; 
	}
	else if(key==GLUT_KEY_DOWN)
	{
		deltaMove = 0.0; 
	}
	else if(key==GLUT_KEY_LEFT)
	{
		left_turn=false;
	}
	else if(key==GLUT_KEY_RIGHT)
	{
		right_turn=false;
	}
} 

//----------------------------------------------------------------------
// Process mouse drag events
// 
// This is called when dragging motion occurs. The variable
// angle stores the camera angle at the instance when dragging
// started, and deltaAngle is a additional angle based on the
// mouse movement since dragging started.
//----------------------------------------------------------------------
void mouseMove(int x, int y) 
{ 	
	if (isDragging) 
	{ // only when dragging
		// update the change in angle
		deltaAngle = (x - xDragStart) * 0.005;

		// camera's direction is set to angle + deltaAngle
		lx = -sin(angle + deltaAngle);
		ly = cos(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON) 
	{
		if (state == GLUT_DOWN) 
		{ // left mouse button pressed
			isDragging = 1; // start dragging
			xDragStart = x; // save x where button first pressed
		}
		else 
		{ /* (state = GLUT_UP) */
			angle += deltaAngle; // update camera turning angle
			isDragging = 0; // no longer dragging
		}
	}
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void releaseNormalKey(unsigned char key,int x,int y)
{
   if(key=='a')
   {
      roll_flag=0;
      roll_angle=0;
   }
   if(key=='d')
   {
      roll_flag=0;
      roll_angle=0;
   }
}

int main(int argc, char **argv) 
{
	printf("\n\
			-----------------------------------------------------------------------\n\
			OpenGL Sample Program:\n\
			- Drag mouse left-right to rotate camera\n\
			- Hold up-arrow/down-arrow to move camera forward/backward\n\
			- q or ESC to quit\n\
			-----------------------------------------------------------------------\n");

	// general initializations
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 400);
	glutCreateWindow("OpenGL/GLUT Sampe Program");
	initRendering();
	//init(); 
  	//glSetupFuncs();
	_terrain = loadTerrain("heightmap.bmp", 20);
   for(int i=0;i<20;i++)
   {
      int c1=rand()%100;
      int c2=rand()%100;
      a[i].x=c1;
      a[i].y=c2;
   }
	// register callbacks
	glutReshapeFunc(changeSize); // window reshape callback
	glutDisplayFunc(renderScene); // (re)display callback
	glutIdleFunc(update); // incremental update 
	glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
	glutMouseFunc(mouseButton); // process mouse button push/release
	glutMotionFunc(mouseMove); // process mouse dragging motion
	glutKeyboardFunc(processNormalKeys); // process standard key clicks
   glutKeyboardUpFunc(releaseNormalKey);
	glutSpecialFunc(pressSpecialKey); // process special key pressed
	// Warning: Nonstandard function! Delete if desired.
	glutSpecialUpFunc(releaseSpecialKey); // process special key release
	
	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0; // this is just to keep the compiler happy
}
