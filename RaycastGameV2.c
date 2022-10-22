#include <stdio.h>
#include <GL/glut.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>
#pragma comment(lib,"winmm.lib")
#include "Textures/Textures.ppm"
#include "Textures/Sword.ppm"
#include "Textures/Sword2.ppm"
#include "Textures/TitleScreen.ppm"
#include "Textures/YouDied.ppm"
#include "Textures/YouWin.ppm"
#include "Textures/Sprites.ppm"
#include "Textures/WitcherUI.ppm"
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //using for degree to radians

//all vars
int pHealth;
int sHealth;
float frame1;
float frame2;
float fps;
int gameState =0;
int timer =0;
float fade=0;
float px;
float py;
float pdx;
float pdy;
float pa;
float degToRad(float a)
{
	return a*M_PI/180.0;
}
float FixAng (float a)
{
	if(a>359)
	{
		a-=360;
	}
	if (a<0)
	{
		a+= 360;
	}
	return a;
}
float dist(float ax, float ay, float bx, float by, float ang)
{
	return(sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay)));
}
typedef struct
{
	int w;
	int a;
	int s;
	int d;
	int f;
}
ButtonKeys;
ButtonKeys Keys;



//fixer functions







//create map array
int mapX=8;
int mapY=8;
int mapSize=64;
int mapWalls[]=
{
	1,1,1,1,1,3,1,1, 
	1,0,0,1,0,0,0,1,
	1,0,0,4,0,0,0,1,
	1,1,4,1,0,0,0,1,
	1,0,0,0,0,1,4,1,
	3,0,0,0,0,1,0,1,
	1,0,0,0,0,1,0,1,
	1,1,3,1,1,1,1,1,
};
int mapFloor[]=
{
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
};
int mapRoof[]=
{
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,
};
typedef struct
{
	int type;  //type of sprite
	int state; //is it there or no
	int map; //what texture is it
	float x,y,z; //position obv...
}
sprite;
sprite sp[4];
int depth[120];

void damage()
{
	int dmg=1 ;
	pHealth-=.03*fps;
	if(pHealth ==0)
		{
			gameState=4;
		}		
}
void Sounds()
{
	PlaySound(TEXT("C:\\Users\\kondi\\OneDrive\\Desktop\\C files\\Sounds\\Pickup.wav"), NULL, SND_FILENAME | SND_ASYNC);	
}

//createSprites
void drawSprite()
{
	int x;
	int y;
	int s;
	if(px<sp[0].x+30 && px>sp[0].x-30&& py<sp[0].y+30&&py>sp[0].y-30) // pick up key
		{
			sp[0].state=0;
			
			
		}
		
		if(px<sp[1].x+30 && px>sp[1].x-30&& py<sp[1].y+30&&py>sp[1].y-30) // enemy damage
		{
			
			damage();
			if(Keys.f==1)
			{
				sp[1].state=0;
			}
							
		}
	//enemy move
	int spx=(int)sp[1].x>>6, spy=(int)sp[1].y>>6;
	int spx_add=((int)sp[1].x+15)>>6, spy_add=((int)sp[1].y+15)>>6;
	int spx_sub=((int)sp[1].x-15)>>6, spy_sub=((int)sp[1].y-15)>>6;
	if(sp[1].x>px && mapWalls[spy*8+spx_sub]==0)
	{
		sp[1].x-=.03*fps;
	}
	if(sp[1].x<px && mapWalls[spy*8+spx_add]==0)
	{
		sp[1].x+=.03*fps;
	}
	if(sp[1].y>py && mapWalls[spy_sub*8+spx]==0)
	{
		sp[1].y-=.03*fps;
	}
	if(sp[1].y<py && mapWalls[spy_add*8+spx]==0)
	{
		sp[1].y+=.03*fps;
	}
	for(s=0; s<2;s++)
	{
		
		float sx=sp[s].x-px;
		float sy=sp[s].y-py;
		float sz=sp[s].z;
		float CS=cos(degToRad(pa));
		float SN=sin(degToRad(pa));
		float a=sy*CS+sx*SN;
		float b=sx*CS-sy*SN;
		sx =a;
		sy =b;
		sx=(sx*108.0/sy)+(120/2);
		sy=(sz*108.0/sy)+(80/2);
		int scale =32*80/b;
		if(scale<0)
		{
			scale=0;
		}
		if(scale>120)
		{
			scale=120;
		}
		//textures
		float t_x=0;
		float t_y=0;
		float t_x_step=31.5/(float)scale;
		float t_y_step=31/(float)scale;
		for(x=sx-scale/2; x<sx+scale/2; x++)
		{
			t_y=31;
			for(y=0; y<scale; y++)
			{
				if(sp[s].state==1 && x>0 && x<120 && b<depth[x])
				{
					int pixel=((int)t_y*32+(int)t_x)*3+(sp[s].map*32*32*3);
	    			int red = Sprites[pixel+0];
					int green = Sprites[pixel+1];
					int blue = Sprites[pixel+2];
					if(red!= 255, green!= 0, blue!=255)
					{
						glPointSize(8);
						glColor3ub(red,green,blue);
						glBegin(GL_POINTS);
						glVertex2i(x*8,sy*8-y*8);
						glEnd();
					}
					t_y-=t_y_step;
					if(t_y<0)
					{
						t_y=0;
					}
				}
			}
			t_x+=t_x_step;
		}
	}
}

//draw 2d flat map using 2 loops, one for x and one for y vals



//raycasting
void castRays()
{
 int r,mx,my,mp,dof,side; 
 float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
 ra=FixAng(pa+30);                                                              //ray set back 30 degrees
 
 for(r=0;r<120;r++)
 {
  int vmt=0;
  int hmt=0;
  //---Vertical--- 
  dof=0; side=0; disV=100000;
  float Tan=tan(degToRad(ra));
       if(cos(degToRad(ra))> 0.001){ rx=(((int)px>>6)<<6)+64;      ry=(px-rx)*Tan+py; xo= 64; yo=-xo*Tan;}//looking left
  else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6) -0.0001; ry=(px-rx)*Tan+py; xo=-64; yo=-xo*Tan;}//looking right
  else { rx=px; ry=py; dof=8;}                                                  //looking up or down. no hit  

  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                     
   if(mp>0 && mp<mapX*mapY && mapWalls[mp]>0){vmt=mapWalls[mp]-1; dof=8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  vx=rx; vy=ry;

  //---Horizontal---
  dof=0; disH=100000;
  Tan=1.0/Tan; 
       if(sin(degToRad(ra))> 0.001){ ry=(((int)py>>6)<<6) -0.0001; rx=(py-ry)*Tan+px; yo=-64; xo=-yo*Tan;}//looking up 
  else if(sin(degToRad(ra))<-0.001){ ry=(((int)py>>6)<<6)+64;      rx=(py-ry)*Tan+px; yo= 64; xo=-yo*Tan;}//looking down
  else{ rx=px; ry=py; dof=8;}                                                   //looking straight left or right
 
  while(dof<8) 
  { 
   mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;                          
   if(mp>0 && mp<mapX*mapY && mapWalls[mp]>0){ hmt=mapWalls[mp]-1; dof=8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);}//hit         
   else{ rx+=xo; ry+=yo; dof+=1;}                                               //check next horizontal
  } 
  float shade=1;
  glColor3f(0,.8,0);
  if(disV<disH)                                                                  //horizontal hit first
  { 
  	hmt=vmt;
  	shade =.5;
  	rx=vx;
    ry=vy;
	disH=disV; 
	glColor3f(0,.6,0);
  }                 
  
    
  int ca=FixAng(pa-ra); 
  disH=disH*cos(degToRad(ca));                            //fix fisheye
  
   
  int lineH = (mapSize*320)/(disH); 
  float ty_step=32.0/(float)lineH;
  float ty_off=0;
  if(lineH>640)
  	{ 
	  ty_off =(lineH-640)/2.0;
	  lineH=640;
	}                                                                            //line height and limit
  int lineOff = 320 - (lineH>>1);
  
  depth[r]=disH;
  //walls textures
  int y;
  float ty =ty_off*ty_step; //+hmt*32;
  float tx;
  if(shade==1)
  {
  	tx=(int)(rx/2.0)%32;
  	if(ra>180)
   	{
   		tx=31-tx;
   	}	
  } 
  else	
  {
  	tx=(int)(ry/2.0)%32;
  	if(ra>90 && ra<270)
	{
		tx=31-tx;
	}
  }
  

  for (y=0; y<lineH; y++)
  {   
  

  	
	
		
	int pixel=((int)ty*32+(int)tx)*3+(hmt*32*32*3);
	int red = Textures[pixel+0]*shade;
	int green = Textures[pixel+1]*shade;
	int blue = Textures[pixel+2]*shade;
	glPointSize(8);
	glColor3ub(red,green,blue);
	glBegin(GL_POINTS);
	glVertex2i(r*8,y+lineOff);
	glEnd();
		
	
  ty+=ty_step;
  } 
  //floors
  for(y=lineOff+lineH; y<640; y++)
  {
  	float dy=y-(640/2);
  	float deg=degToRad(ra);
  	float raFix=cos(degToRad(FixAng(pa-ra)));
  	tx=px/2+cos(deg)*158*32/dy/raFix;
  	ty=py/2-sin(deg)*158*32/dy/raFix;
  	int mp=mapFloor[(int)(ty/32)*mapX+(int)(tx/32)]*32*32;
	int pixel=(((int)(ty)&31)*32+((int)(tx)&31))*3+mp*3;
	int red = Textures[pixel+0]*.5;
	int green = Textures[pixel+1]*.5;
	int blue = Textures[pixel+2]*.5;
	glPointSize(8);
	glColor3ub(red,green,blue);
	glBegin(GL_POINTS);
	glVertex2i(r*8,y);
	glEnd();	
  	 //roofs
  	mp=mapRoof[(int)(ty/32)*mapX+(int)(tx/32)]*32*32;
  	
  	pixel=(((int)(ty)&31)*32+((int)(tx)&31))*3+mp*3;
	red = Textures[pixel+0];
	green = Textures[pixel+1];
	blue = Textures[pixel+2];
	glPointSize(8);
	glColor3ub(red,green,blue);
	glBegin(GL_POINTS);
	glVertex2i(r*8,640-y);
	glEnd();
  	 
  }
 
  
  ra=FixAng(ra-.5);
  
  
}
}
void showUI()
{
	int x;
	int y;
	for(y=0; y<108; y++)
	{
		for(x=0; x<960; x++)
		{ //show idle
			int pixel=(y*960+x)*3;
			int red =   WitcherUI[pixel+0];
			int green = WitcherUI[pixel+1];
			int blue =  WitcherUI[pixel+2];
			glPointSize(8);
			glColor3ub(red,green,blue);
			glBegin(GL_POINTS);
			glVertex2i(x+4,y+534);
			glEnd();
		}
}
}



void showSword()
{
	int x;
	int y;
	for(y=0; y<128; y++)
	{
		for(x=0; x<128; x++)
		{ //show idle
			int pixel=(y*128+x)*3;
			int red =   Sword[pixel+0];
			int green = Sword[pixel+1];
			int blue =  Sword[pixel+2];
			if(red!=0, green!=0, blue!=0 & Keys.f==0)
			{
				glPointSize(8);
				glColor3ub(red,green,blue);
				glBegin(GL_POINTS);
				glVertex2i(x*4+300,y*4+30);
				glEnd();
			}
			// show swing
			 pixel=(y*128+x)*3;
			 red =   Sword2[pixel+0];
			 green = Sword2[pixel+1];
			 blue =  Sword2[pixel+2];
			 
			if(Keys.f==1)
			{
					
					if(red!=0, green!=0, blue!=0)
					{
						
						glPointSize(8);
						glColor3ub(red,green,blue);
						glBegin(GL_POINTS);
						glVertex2i(x*4+300,y*4+30);
						glEnd();
						
						
					
					}
					
					
					
			}
		
		}
	}
	
}

void screen(int v)
{
	int x;
	int y;
	int *T;
	if(v==1)
	{
		T=TitleScreen;
	}
	if(v==2)
	{
		T=YouWin;
	}
	if(v==3)
	{
		T=YouDied;
	}
	
	for(y=0; y<80; y++)
	{
		for(x=0; x<120; x++)
		{ 
			int pixel=(y*120+x)*3;
			int red =   T[pixel+0]*fade;
			int green = T[pixel+1]*fade;
			int blue =  T[pixel+2]*fade;
				glPointSize(8);
				glColor3ub(red,green,blue);
				glBegin(GL_POINTS);
				glVertex2i(x*8,y*8);
				glEnd();
		}
	}
	if(fade<1)
	{
		fade+=.0008*fps;
	}
	if(fade>1)
	{
		fade=1;
	}
}




void init()
{
	//gray background
	glClearColor(.5,.5,.5,0);
	//playersize
	px=150;
	py=400;
	pa=90;
	//calculate angle on beginplay
	pdx=cos(degToRad(pa));
	pdy=-sin(degToRad(pa));
	mapWalls[19]=4;
	mapWalls[26]=4;
	mapWalls[38]=4;
	//fist type of sprite
	sp[0].type=1; sp[0].state=1; sp[0].map=0; sp[0].x=6.5*64; sp[0].y=1.5*64; sp[0].z=20; 
	//enemy
	sp[1].type=2; sp[1].state=1; sp[1].map=1; sp[1].x=6.5*64; sp[1].y=6*64; sp[1].z=10; 
	pHealth = 100;
	sHealth =100;
	
	

	
	
	
	
}

//display image

void display()
{
	//fps
	frame2 = glutGet(GLUT_ELAPSED_TIME);
	fps=(frame2-frame1);
	frame1=glutGet(GLUT_ELAPSED_TIME);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//start the game
	if(gameState==0)   
	{
		init();
		fade=0;
		timer= 0;
		gameState=1;
	}
	//startscreen
	if(gameState==1)
	{
		screen(1);
		timer+=1*fps;
		if(timer>8000)
		{
			fade=0;
			timer=0;
			gameState=2;
		}
	}
	if(gameState==2)
	{
	//player movement

	if(Keys.a==1)
	{
		pa+=0.2*fps; 
		pa=FixAng(pa);
		pdx=cos(degToRad(pa));
		pdy=-sin(degToRad(pa));
	}
	if(Keys.d==1)
	{
		pa-=0.2*fps; 
		pa=FixAng(pa);
		pdx=cos(degToRad(pa));
		pdy=-sin(degToRad(pa));
	}
	//checking 20 units ahead of player
	int xo=0;
	if(pdx<0)
	{
		xo=-20;
	}
	else
	{
		xo=20;
	}
	
	int yo=0;
	if(pdy<0)
	{
		yo=-20;
	}
	else
	{
		yo=20;
	} 
	//hold x offset
	int ipx=px/64.0;
	int ipx_add_xo=(px+xo)/64;
	int ipx_sub_xo=(px-xo)/64;
	//hold y offset
	int ipy=py/64.0;
	int ipy_add_yo=(py+yo)/64;
	int ipy_sub_yo=(py-yo)/64;
	if(Keys.w==1)
	{
		if(mapWalls[ipy*mapX+ipx_add_xo]==0) //front collision
		{
			px+=pdx*0.2*fps;
		}
		if(mapWalls[ipy_add_yo*mapX+ipx]==0)
		{	
			py+=pdy*0.2*fps;
		}
	}
	if(Keys.s==1)
	{
		if(mapWalls[ipy*mapX+ipx_sub_xo]==0) // back collision
		{
			px-=pdx*0.2*fps;
		}
		if(mapWalls[ipy_sub_yo*mapX+ipx]==0)
		{
			py-=pdy*0.2*fps;
		}
			
	}
	castRays();
	drawSprite();
	showSword();
	showUI();
	if( (int)px>>6==1 && (int)py>>6==1)
	{
		fade=0;
		timer=0;
		gameState=3;
	}
	}
	//end screen
	if(gameState==3)
	{
		screen(2);
		timer+=1*fps;
		if(timer>5000)
		{
			fade=0;
			timer=0;
			gameState=0;
		}
	}
	if(gameState==4)
	{
		screen(3);
		timer+=1*fps;
		if(timer>5000)
		{
			fade=0;
			timer=0;
			gameState=0;
		}
	}
	
	glutPostRedisplay();
	glutSwapBuffers();

}
void ButtonDown (unsigned char key, int x, int y)
{
	if(key=='a')
	{
		Keys.a=1;
	}
	if(key=='d')
	{
		Keys.d=1;
	}
	if(key=='w')
	{
		Keys.w=1;
	}
	if(key=='s')
	{
		Keys.s=1;
	}
	if(key=='f')
	{
		Keys.f=1;
	}
	if(key=='e' && sp[0].state==0)
	{	
		//check x units ahead
		int xo=0;
		if(pdx<0)
		{
			xo=-25;
		}
		else
		{
			xo=25;
		}
		//check y units ahead
		int yo=0;
		if(pdy<0)
		{
			yo=-25;
		}
		else
		{
			yo=25;
		}
		int ipx=px/64.0;
		int ipx_add_xo=(px+xo)/64;
		int ipy=py/64.0;
		int ipy_add_yo=(py+yo)/64;
		if (mapWalls[ipy_add_yo*mapX+ipx_add_xo]==4)
		{
			mapWalls[ipy_add_yo*mapX+ipx_add_xo]=0;
		}
			 
		
		
	}
}
void ButtonUp (unsigned char key, int x, int y)
{
	if(key=='a')
	{
		Keys.a=0;
	}
	if(key=='d')
	{
		Keys.d=0;
	}
	if(key=='w')
	{
		Keys.w=0;
	}
	if(key=='s')
	{
		Keys.s=0;
	}
	if(key=='f')
	{
		Keys.f=0;
	}
}
void resize(int w, int h)
{
	glutReshapeWindow(960,640);
}




int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(960, 640);
	glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)/2-1280/2, glutGet(GLUT_SCREEN_HEIGHT)/2-640/2);
	glutCreateWindow("Witcher 1990 by QS");
	gluOrtho2D(0,960,640,0);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(ButtonDown);
	glutKeyboardUpFunc(ButtonUp);
	glutMainLoop();
}



