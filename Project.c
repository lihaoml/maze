/*
 *  Project: An Interactive 3D Maze Game 
 *  Project.c
 * 
 * -----------------------------------------------------------------------------
 *  Student Information
 * -----------------------------------------------------------------------------
 *  Student Name:
 *  Student ID:
 *  E-mail:
 *  Major: Computer Technology / Electronic Information / Software Technology
 *  Year: 3
 * ------------------------------------------------------------------------------
 */
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "Bitmap.h"

#define M_PI 3.141592654

// ============ Global variables =======================
// Maze information
#define MAX_MAZESIZE 20
static int _mapx, _mapz; // Size of the maze 
static int _map[MAX_MAZESIZE][MAX_MAZESIZE];
int _initpos[2];         // Initial position of the player
   
static GLfloat _wallHeight = 1.0; // Height of the wall 
static GLfloat _wallScale = 2.0;  // Scale of the width of the wall 
static GLint fogMode;
   
// Camera setting
GLfloat _viewangle = 45.0; // Angle of view 
GLfloat _viewdepth = 20.0; // View depth 
   
// Define the player information structure   
typedef struct _playerInfo {   
   GLfloat degree;  // Object orientation   
   GLfloat forward, spin;   
   GLfloat pos[3];  // User position   
   GLfloat mySize;  // User radial size   
   GLfloat forwardStepSize; // Step size   
   GLfloat spinStepSize;    // Rotate step size   
} playerInfo;   
   
playerInfo _player;   
   
int _drawmode = 0;

bool right = false, top = false;   
   
int imageWidth, imageHeight,fimageWidth, fimageHeight, dimageWidth, dimageHeight;   
GLubyte *wallimage, *floorimge, *dloorimge;//the image from file   
   
void init();   
void initplayer();   
   
// Capture the BMP file   
GLubyte* TextureLoadBitmap(char *filename, int *w, int *h) // Bitmap file to load   
{   
   BITMAPINFO *info; // Bitmap information   
   void       *bits; // Bitmap pixel bits   
   GLubyte    *rgb;  // Bitmap RGB pixels   
   
   // Try loading the bitmap and converting it to RGB...   
   bits = LoadDIBitmap(filename, &info);   
      
   if (bits==NULL)    
      return (NULL);   
      
   rgb = ConvertRGB(info, bits);   
      
   if (rgb == NULL) {   
      free(info);   
      free(bits);   
   }   
   
   *w = info->bmiHeader.biWidth;   
   *h = info->bmiHeader.biHeight;   
   
   // Free the bitmap and RGB images, then return 0 (no errors).   
      
   free(info);   
   free(bits);   
   return (rgb);   
}   
   
void reshape(int w, int h)   
{   
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);   
   glMatrixMode(GL_PROJECTION);   
   glLoadIdentity();   
   gluPerspective(_viewangle, (GLfloat) w / (GLfloat) h, 0.8, _viewdepth);   
   glMatrixMode(GL_MODELVIEW);   
   glLoadIdentity();   
}   
   
//====== Drawing functions ===============   
void DrawGround()   
{   
   // Draw the ground here
   glPushMatrix();   
      glTranslatef(_wallScale * _mapx / 2.0, 0.0, _wallScale * _mapz / 2.0);   
      glScalef(_wallScale * _mapx, 1.0, _wallScale * _mapz);   

      glColor3f(1.0, 1.0, 1.0); 
  
      glEnable(GL_TEXTURE_2D);   
      glTexImage2D(GL_TEXTURE_2D, 0, 3, fimageWidth, fimageHeight, 0,   
                GL_RGB, GL_UNSIGNED_BYTE, floorimge);    
      glBegin(GL_QUADS);   
         glTexCoord2f(0.0*_mapx, 1.0*_mapz); glVertex3f(-0.5, 0.0, 0.5);   
         glTexCoord2f(1.0*_mapx, 1.0*_mapz); glVertex3f( 0.5, 0.0, 0.5);   
         glTexCoord2f(1.0*_mapx, 0.0*_mapz); glVertex3f( 0.5, 0.0, -0.5);   
         glTexCoord2f(0.0*_mapx, 0.0*_mapz); glVertex3f(-0.5, 0.0, -0.5);   
      glEnd();   
      glDisable(GL_TEXTURE_2D);   
   glPopMatrix();   
}   
   
void DrawWalls()   
{   
    //Draw the maze's walls here
    glPushMatrix();   
    glColor3f(1.0, 1.0, 1.0);   
    for(int i = 0; i < _mapx; i++)   
        for(int j = 0; j < _mapz; j++)   
        {               
            glEnable(GL_TEXTURE_2D);     
            if(_map[i][j] == 1 && _map[i+1][j] != 1 || _map[i][j] != 1 && _map[i+1][j] == 1)   
            {   
                glPushMatrix();   
                glTexImage2D(GL_TEXTURE_2D, 0, 3, imageWidth, imageHeight, 0,   
                     GL_RGB, GL_UNSIGNED_BYTE, wallimage);    
                glTranslated(i*_wallScale,0.0,j*_wallScale);   
                glBegin(GL_QUADS);   
                glTexCoord2f(0.0, 1.0); glVertex3f( 2.0,  1.0,  0.0);   
                glTexCoord2f(1.0, 1.0); glVertex3f( 2.0,  1.0,  2.0);   
                glTexCoord2f(1.0, 0.0); glVertex3f( 2.0,  0.0,  2.0);   
                glTexCoord2f(0.0, 0.0); glVertex3f( 2.0,  0.0,  0.0);   
                glEnd();   
                glPopMatrix();   
            }   
            if(_map[i][j] == 1 && _map[i][j+1] != 1 || _map[i][j] != 1 && _map[i][j+1] == 1)   
            {   
                glPushMatrix();   
                glTexImage2D(GL_TEXTURE_2D, 0, 3, imageWidth, imageHeight, 0,   
                     GL_RGB, GL_UNSIGNED_BYTE, wallimage);    
                glTranslated(i*_wallScale,0.0,j*_wallScale);   
                glBegin(GL_QUADS);   
                glTexCoord2f(0.0, 1.0); glVertex3f( 2.0,  1.0,  2.0);   
                glTexCoord2f(1.0, 1.0); glVertex3f( 0.0,  1.0,  2.0);   
                glTexCoord2f(1.0, 0.0); glVertex3f( 0.0,  0.0,  2.0);   
                glTexCoord2f(0.0, 0.0); glVertex3f( 2.0,  0.0,  2.0);   
                glEnd();   
                glPopMatrix();   
            }   
            // Draw the Dloor   
            if(_map[i][j] == 3)   
            {   
                glPushMatrix();   
                glTexImage2D(GL_TEXTURE_2D, 0, 3, dimageWidth, dimageHeight, 0,   
                     GL_RGB, GL_UNSIGNED_BYTE, dloorimge);    
                glTranslated(i*_wallScale,0.0,j*_wallScale);   
                glBegin(GL_QUADS);   
                glTexCoord2f(0.0, 1.0); glVertex3f( 0.0,  1.0,  2.0);   
                glTexCoord2f(1.0, 1.0); glVertex3f( 0.0,  1.0,  0.0);   
                glTexCoord2f(1.0, 0.0); glVertex3f( 0.0,  0.0,  0.0);   
                glTexCoord2f(0.0, 0.0); glVertex3f( 0.0,  0.0,  2.0);   
                glEnd();   
                glPopMatrix();   
            }   
            glDisable(GL_TEXTURE_2D);               
        }   
    glPopMatrix();   
}   
   
void DrawPlayer()   
{   
   glPushMatrix();   
      glTranslatef(_player.pos[0], _player.pos[1], _player.pos[2]);   
      glRotated(_player.degree,0.0,1.0,0.0);   
      glutSolidSphere(_player.mySize, 15, 15);   
      glTranslatef(0.15,0.2,0.0);   
      if(_player.forward != 0.0)   
      glRotated(35,1.0,0.0,0.0);   
      glutSolidTorus(0.05,0.05,30,30);   
      glTranslatef(-0.3,0.0,0.0);   
      glutSolidTorus(0.05,0.05,30,30);   
   glPopMatrix();   
}   
// For debugging collision detection
void DrawSphere()
{
   glPushMatrix();
	  glTranslatef(_player.pos[0], _player.pos[1], _player.pos[2]);
	  glColor3f(1.0, 0.0, 1.0); 
	  glutWireSphere(_player.mySize, 15, 15);
   glPopMatrix();
}
 
void display(void)   
{   
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
   glColor3f(1.0, 1.0, 1.0);   
   
   glMatrixMode(GL_MODELVIEW);   
   glPushMatrix();   
      gluLookAt(_player.pos[0] - 1.6 * sin(_player.degree * M_PI / 180.0), // eye   
                _player.pos[1] + 0.25,    
                _player.pos[2] - 1.6 * cos(_player.degree* M_PI / 180.0),    
                _player.pos[0], // at   
                _player.pos[1],   
                _player.pos[2],   
                0.0, 1.0, 0.0); // up   
      DrawGround();   
      DrawWalls();   

	  if (_drawmode == 0)
		 DrawPlayer();
	  else
		 DrawSphere();
   glPopMatrix();   
   
   glutSwapBuffers();   
}   
   
void checkcollide()   
{   
    float dx, dz;   
    int  Gx, Gy;   
    //find the position of the Matrix    
    Gx = _player.pos[0]/2;   
    Gy = _player.pos[2]/2;   
       
    // Check the direction of the object   
    if (_player.degree > 180 || _player.degree > -180 && _player.degree < 0)   
        right = true;   
    else   
        right = false;   
    if (_player.degree > 90 && _player.degree < 270 || _player.degree < -90 && _player.degree > -270)   
        top = true;   
    else    
        top = false;   
       
    dx = _player.forward * sin(_player.degree * M_PI / 180.0);   
    dz = _player.forward * cos(_player.degree * M_PI / 180.0);   
    // Check collision of walls   
    if (_player.pos[0] <= Gx*_wallScale+_player.mySize+0.05 && _map[Gx-1][Gy] == 1 && right == true)   
        dx = 0.0;   
    if (_player.pos[0] >= Gx*_wallScale+_wallScale-_player.mySize-0.05 && _map[Gx+1][Gy] == 1 && right == false)   
        dx = 0.0;   
    if (_player.pos[2] <= Gy*_wallScale+_player.mySize+0.05 && _map[Gx][Gy-1] == 1 && top == true)   
        dz = 0.0;   
    if (_player.pos[2] >= Gy*_wallScale+_wallScale-_player.mySize-0.05 && _map[Gx][Gy+1] == 1 && top == false)   
        dz = 0.0;   
       
    // Update the current position   
    _player.pos[0] += dx;   
    _player.pos[2] += dz;   
       
    //if the object touch the door
    if (_map[Gx][Gy] == 3)   
    {   
        _player.pos[0] = _initpos[0]* _wallScale + _wallScale / 2.0;   
        _player.pos[2] = _initpos[1]* _wallScale + _wallScale / 2.0;   
        printf("you'v found the exit!\n");
    }   
}   
   
void move(void)   
{   
   if (_player.spin != 0.0) {   
       _player.degree += _player.spin;   
      if (_player.degree > 360.0) {   
          _player.degree -= 360.0;   
      }   
      else if (_player.degree < -360.0) {   
         _player.degree += 360.0;   
      }   
   }   
   
   if (_player.forward != 0.0) {   
      checkcollide();   
   }   
   glutPostRedisplay();   
}   
   
void keyboard(unsigned char key,int x, int y)   
{   
   switch (key) {   
	  case 's':
	  case 'S':
	     // Change to use sphere for the object
		 _drawmode++;
		 _drawmode %= 2;
		 break;
      case 27:   
         exit(0);   
   }   
}   
   
// Please read this function carefully, you can learn how to    
// make use the mouse buttons to control the Test Object/Player   
void mouse(int button, int state, int x, int y)   
{   
   static int buttonhold = 0;   
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {   
      if (buttonhold >= 2) {   
         // Stop forward and turn right   
         _player.forward = 0.0;   
         _player.spin = -_player.spinStepSize; // Turn right   
      }   
      else   
         _player.spin = 0.0; // Stop turn left   
      buttonhold--;   
   }   
   
   if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {   
      if (buttonhold >= 2) {   
         // Stop forward and turn left   
         _player.forward = 0.0;   
         _player.spin = _player.spinStepSize; // Turn left   
      }   
      else   
         _player.spin = 0.0; // Stop turn right   
      buttonhold--;   
   }   
   
   if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP)) {   
      _player.forward = 0.0;   
   }   
   
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {   
      if (buttonhold > 0) {   
         _player.forward = _player.forwardStepSize;   
         _player.spin = 0.0;   
      }   
      else   
         _player.spin = _player.spinStepSize; // Turn left   
      buttonhold++;   
   }   
   
   if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {   
      if (buttonhold > 0) {   
         _player.forward = _player.forwardStepSize;   
         _player.spin = 0.0;   
      }   
      else   
         _player.spin = -_player.spinStepSize; // Turn right   
      buttonhold++;   
   }   
   
   if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN)) {   
       _player.forward = _player.forwardStepSize;   
   }   
}   
   
void initplayer()   
{   
   // Initilaize the player   
   // You may try change the values as you like   
   _player.degree = 0.0; // User orientation   
   _player.mySize = 0.2; // User radial size   
   _player.forward = 0.0;   
   _player.forwardStepSize = 0.05; // Step size   
   _player.spin = 0.0;   
   _player.spinStepSize = 5.0; // Rotate step size   
   
   // Init the player's position based on the postion on the map   
   _player.pos[0] = _initpos[0] * _wallScale + _wallScale / 2.0;   
   _player.pos[1] = _player.mySize;   
   _player.pos[2] = _initpos[1] * _wallScale + _wallScale / 2.0;   

}   
   
void init()   
{   
   initplayer();   
   
   glutInitDisplayMode(GL_DEPTH | GLUT_RGB);   
   glEnable(GL_DEPTH_TEST);   
   /*
   //initial the fog effect   
    glEnable(GL_FOG);   
   {   
      GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1.0};   
   
      fogMode = GL_EXP;   
      glFogi(GL_FOG_MODE, fogMode);   
      glFogfv(GL_FOG_COLOR, fogColor);   
      glFogf(GL_FOG_DENSITY, 0.35);   
      glHint(GL_FOG_HINT, GL_DONT_CARE);   
      glFogf(GL_FOG_START, 1.0);   
      glFogf(GL_FOG_END, 5.0);   
   } 
   */
    glClearColor(0.5, 0.5, 0.5, 1.0);   
      
   //initial the Texture Mapping   
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);   
   wallimage = TextureLoadBitmap("WALL.BMP", &imageWidth, &imageHeight);   
   floorimge = TextureLoadBitmap("FLOOR.BMP", &fimageWidth, &fimageHeight);   
   dloorimge = TextureLoadBitmap("DOOR.BMP", &dimageWidth, &dimageHeight);   
   
   //initial the light   
   GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};   
   GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};   
   
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);   
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);   
   
   glShadeModel(GL_SMOOTH);   
   glEnable(GL_LIGHTING);   
   glEnable(GL_LIGHT0);   
   
   //initial the material   
   GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};   
   GLfloat mat_diffuse[] = {1.0, 1.0, 0.0, 1.0};   
   GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 0.0};   
   GLfloat mat_shininess = {100.0};   
   
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);   
   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);   
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);   
   glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);   
   
}   
   
// Read in the maze map   
int readmap(char* filename)    
{   
   FILE* fp;   
   int i, j;   
   char tmp[MAX_MAZESIZE];   
   
   fp = fopen(filename,"r");   
   
   if (fp) {   
      fscanf(fp, "%d", &_mapx);   
      fscanf(fp, "%d", &_mapz);   
      for (j = 0; j < _mapz; j++) {   
         fscanf(fp, "%s", tmp);   
         for (i = 0; i < _mapx; i++) {   
            _map[i][j] = tmp[i] - '0';   
            if (_map[i][j] == 2) {   
               // Save the initial position   
               _initpos[0] = i;   
               _initpos[1] = j;   
            }   
            printf("%d", _map[i][j]);   
         }   
         printf("\n");   
      }   
      fclose(fp);   
   }   
   else {   
      printf("Error Reading Map file!\n");   
      return 0;   
   }   
   return 1;   
}   
   
void main(int argc,char **argv)   
{     
    if (argc >= 2) {
	  srand(time(NULL));
	  if (readmap(argv[1]) == 0 )
	  	 exit(0);   
   
      glutInit(&argc, argv);   
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);   
      glutInitWindowSize(400, 300);   
      glutInitWindowPosition(250, 250);   
      if (glutCreateWindow("An Interactive 3D Maze Game") == GL_FALSE)   
         exit(-1);   
      init();   
      glutDisplayFunc(display);   
      glutReshapeFunc(reshape);   
      glutKeyboardFunc(keyboard);   
      glutMouseFunc(mouse);   
      glutIdleFunc(move);   
      glutMainLoop();   
   } 
   else
 	  printf("Usage %s <mapfile>\n", argv[0]);
}    

