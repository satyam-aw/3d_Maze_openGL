// Libraries; we are using GLU, GLUT, and GLEW, along with C stdlib.
#pragma once
#pragma comment (lib, "glew32s.lib")
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#define GLEW_STATIC
#include <GL/glew.h>
#include <windows.h> //Seems necessary for GLUT
#include <GL/glu.h> 
#include <GL/glut.h>
#include <GL\freeglut.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include "Constants.h"
#include "mazeParser.h"
#include <sstream>

using namespace std;

//Maze compile-time parameters
GLint XSIZE = 8;
GLint YSIZE = 8;
const GLint MAX_APPERROR = 64;
int startTime = clock(), endTime = clock() + 185000;
int sec, mn = 0, hour = 0;
bool gameOver = false, youWon = false;
std::string strSec, strMn, strHour;
// Track if keys are currently held down (Supports both lower & upper case)
bool keys[256] = { false };
bool specialKeys[256] = { false };
// Jump configurations (Adjust these values to change jump feel)
const float GRAVITY = -9.0f;       // Acceleration pulling you down
const float JUMP_VELOCITY = 30.0f;   // Initial upward speed of the jump
const float FLOOR_HEIGHT = -HALF_CUBE;    // Your default standing camera height

// Jump state trackers
float vertical_velocity = 0.0f;
bool is_jumping = false;

// Delta Time variables
int oldTimeSinceStart = 0;

// File-level variables... these are all position-state / input state variables. OpenGL 
//  callbacks with defined signatures must edit these variables, so there's no easy 
//  alternative to giving them file-level scope.
static GLfloat x_at = START_X_AT;
static GLfloat y_at = START_Y_AT;
static GLfloat rot_x = -M_PI_2;
static GLint xin = 0, yin = 0;
static GLfloat camera_y = START_CAMERA_Y;
static GLfloat rot_y = -0.0f;


// Functions

GLint windowwidth()
{
 static int ret=0;
 if(!ret)ret=glutGet(GLUT_SCREEN_WIDTH)-WINDOW_MARGIN;
 return ret;
}

GLint windowheight()
{
 static int ret=0;
 if(!ret)ret=glutGet(GLUT_SCREEN_HEIGHT)-WINDOW_MARGIN;
 return ret;
}

vector<vector<GLint>>maze_innards;

//App-level "init" function
void initgl(GLint width, GLint height) 
{
 glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
 glClearDepth(1.0);
 glEnable(GL_DEPTH_TEST);
 glFrontFace(GL_CCW);
 glShadeModel(GL_SMOOTH); 
 glMatrixMode(GL_PROJECTION);
 gluPerspective(VIEW_FIELD,(GLfloat)width/(GLfloat)height,NEAR_Z,FAR_Z); 
 glMatrixMode(GL_MODELVIEW);
}

/* The function called when our window is resized (not supported with our textur. sys.)*/
void resizer(GLint width, GLint height)
{ 
 if(width!=windowwidth() || height!=windowheight()) exit(0); 
}

void app_assert_success(const char* szz)
{
 if(GLint xerr= glGetError())
 { 
  char szerr[MAX_APPERROR]; 
  sprintf(szerr,"%s , %d",szz,xerr); 
  fprintf(stderr,"%s",szerr); 
  exit(1);
 }
}


//Loads a texture from a text file and returns its integer OpenGL handle
GLuint maketex(const char* tfile,GLint xSize,GLint ySize) //returns tex. no.
{
 GLuint rmesh;
 FILE * file;
 unsigned char * texdata = (unsigned char*) malloc( xSize * ySize * 3 ); //3 is {R,G,B}

 file = fopen(tfile, "rb" );
 fseek(file,BMP_HEADER_SIZE,SEEK_CUR);
 fread( texdata, xSize * ySize * 3, 1, file ); 
 fclose( file );
 glEnable( GL_TEXTURE_2D );

 char* colorbits = new char[ xSize * ySize * 3]; 

 for(GLint a=0; a<xSize * ySize * 3; ++a) colorbits[a]=0xFF; 

 glGenTextures(1,&rmesh);
 glBindTexture(GL_TEXTURE_2D,rmesh);

 glTexImage2D(GL_TEXTURE_2D,0 ,3 , xSize,
 ySize, 0 , GL_RGB, GL_UNSIGNED_BYTE, colorbits);

 app_assert_success("post0_image");

 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 app_assert_success("pre_getview");

 //Save viewport and set up new one
 GLint viewport[4]; //4 is {X,Y,Width,Height}
 glGetIntegerv(GL_VIEWPORT,(GLint*)viewport);

 app_assert_success("pre_view");
 glViewport(0,0,xSize,ySize);
 app_assert_success("post0_view");

 //Clear target and depth buffers
 glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); 

 glPushMatrix(); //Duplicates MODELVIEW stack top
 glLoadIdentity(); //Replace new top with {1}

 app_assert_success("ogl_mvx");

 glDrawPixels(xSize,ySize,GL_BGR, GL_UNSIGNED_BYTE,texdata);

 app_assert_success("pre_copytext");
 glPopMatrix();
 app_assert_success("copytext2");
 glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
 0,0, xSize, ySize, 0);
 app_assert_success("post_copy");
 
 //Restore viewport
 glViewport(viewport[0],viewport[1],viewport[2],viewport[3]); //{X,Y,Width,Height}
 app_assert_success("ogl_mm1");
 delete[] colorbits;
 free(texdata);
 return rmesh;

}

void wall_vertical(GLfloat x, GLfloat y, GLfloat z, GLuint texture = 1) //Draws a cube centered at (x,y,z)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    // left of cube
    glBegin(GL_QUADS);
    glTexCoord2d(1.0, 1.0);
    glVertex3f(x - HALF_CUBE, WALL_HT / 2, z + HALF_CUBE); // Top Right Of The Quad (Left)
    glTexCoord2d(0.0, 1.0);
    glVertex3f(x - HALF_CUBE, WALL_HT / 2, z - HALF_CUBE); // Top Left Of The Quad (Left)
    glTexCoord2d(0.0, 0.0);
    glVertex3f(x - HALF_CUBE, -WALL_HT / 2, z - HALF_CUBE); // Bottom Left Of The Quad (Left)
    glTexCoord2d(1.0, 0.0);
    glVertex3f(x - HALF_CUBE, -WALL_HT / 2, z + HALF_CUBE); // Bottom Right Of The Quad (Left)
    glEnd();
}

void wall_horizontal(GLfloat x, GLfloat y, GLfloat z, GLuint texture = 1) //Draws a cube centered at (x,y,z)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    // Back
    glBegin(GL_QUADS);
    glTexCoord2d(1.0,1.0); 
    glVertex3f(x + HALF_CUBE, WALL_HT / 2,z-HALF_CUBE); // Top Right Of The Quad (Back)
    glTexCoord2d(0.0,1.0); 
    glVertex3f(x - HALF_CUBE, WALL_HT / 2,z-HALF_CUBE); // Top Left Of The Quad (Back)
    glTexCoord2d(0.0,0.0); 
    glVertex3f(x - HALF_CUBE,-WALL_HT/2,z-HALF_CUBE); // Bottom Left Of The Quad (Back)
    glTexCoord2d(1.0,0.0); 
    glVertex3f(x + HALF_CUBE,-WALL_HT / 2,z-HALF_CUBE); // Bottom Right Of The Quad (Back)
    glEnd();
}

void sky(GLuint haze)
{ 
    //Modelled after cube front
     glBindTexture(GL_TEXTURE_2D,haze);
     glBegin(GL_QUADS); 
     glTexCoord2d(1.0,1.0);
     glVertex3f( (windowwidth()/SKY_SCALE), (windowheight()/SKY_SCALE),-SKY_DISTANCE); 
     glTexCoord2d(0.0,1.0);
     glVertex3f( -(windowwidth()/SKY_SCALE), (windowheight()/SKY_SCALE),-SKY_DISTANCE); 
     glTexCoord2d(0.0,0.0);
     glVertex3f( -(windowwidth()/SKY_SCALE), -(windowheight()/SKY_SCALE),-SKY_DISTANCE); 
     glTexCoord2d(1.0,0.0);
     glVertex3f( (windowwidth()/SKY_SCALE), -(windowheight()/SKY_SCALE),-SKY_DISTANCE); 
     glEnd();
}

void floor(GLuint grnd)
{   
    //Modelled after cube front
    glBindTexture(GL_TEXTURE_2D, grnd);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBegin(GL_QUADS);
    glTexCoord2d(10.0, 10.0);
    glVertex3f(MAZE_EXTREME_LEFT+ XSIZE*FULL_CUBE, -HALF_CUBE, MAZE_EXTREME_TOP);
    glTexCoord2d(0.0, 10.0);
    glVertex3f(MAZE_EXTREME_LEFT, -HALF_CUBE, MAZE_EXTREME_TOP);
    glTexCoord2d(0.0, 0.0);
    glVertex3f(MAZE_EXTREME_LEFT, -HALF_CUBE, MAZE_EXTREME_TOP + YSIZE * FULL_CUBE);
    glTexCoord2d(10.0, 0.0);
    glVertex3f(MAZE_EXTREME_LEFT + XSIZE * FULL_CUBE, -HALF_CUBE, MAZE_EXTREME_TOP + YSIZE * FULL_CUBE);
    glEnd();
}


void print_maze(GLuint *walls) //Renders the necessary OpenGL cubes
{
 int x,y; 

 for(y=0; y<YSIZE*2 ; ++y ) //Maze proper
 {
  for(x=0; x<XSIZE ; ++x )
  {
      if (y % 2 == 0 && WALL_TEXTURES[y][x]) {
          wall_horizontal(LEFTMOST_CUBE_CENTER + ((GLfloat)x * FULL_CUBE),
              0.0,
              MAZE_EXTREME_TOP + HALF_CUBE + ((GLfloat)(y / 2) * FULL_CUBE), walls[WALL_TEXTURES[y][x] - 1]);
      }

      else if (y % 2 == 1 && WALL_TEXTURES[y][x]) {
          wall_vertical(LEFTMOST_CUBE_CENTER + ((GLfloat)x * FULL_CUBE),
              0.0,
              MAZE_EXTREME_TOP + HALF_CUBE + ((GLfloat)(y / 2) * FULL_CUBE), walls[WALL_TEXTURES[y][x] - 1]);
      }
  }
 }
}

bool collide() //Is player in a state of collision?
{
 int x,y;

 if (gameOver || youWon) return false;

 //Got out of the maze!!!
 if(x_at>=MAZE_EXTREME_LEFT-COLLIDE_MARGIN - HALF_CUBE && 
   x_at<=MAZE_EXTREME_LEFT+ XSIZE* FULL_CUBE+ HALF_CUBE +COLLIDE_MARGIN)
 {
  if( y_at<=MAZE_EXTREME_TOP+COLLIDE_MARGIN - HALF_CUBE ||
    y_at>=MAZE_EXTREME_TOP+YSIZE*FULL_CUBE + HALF_CUBE -COLLIDE_MARGIN)
  {
    youWon = true; camera_y = 0.1; CAMERA_SINK *= -1; rot_y = -M_PI_2;
    return true;
  }
 }
 else {
    youWon = true; camera_y = 0.1; CAMERA_SINK *= -1; rot_y = -M_PI_2;
    return true;
 }

 //Maze proper
 for (y = 0; y < YSIZE * 2; ++y)
 {
     for (x = 0; x < XSIZE; ++x)
     {
         if (y%2 == 0 && WALL_TEXTURES[y][x]) {
             if (y_at <= MAZE_EXTREME_TOP + (y / 2) * FULL_CUBE + COLLIDE_MARGIN &&
                 y_at >= MAZE_EXTREME_TOP + (y / 2) * FULL_CUBE - COLLIDE_MARGIN&&
                 x_at >= MAZE_EXTREME_LEFT + x * FULL_CUBE - COLLIDE_MARGIN &&
                 x_at <= MAZE_EXTREME_LEFT + (x+1) * FULL_CUBE + COLLIDE_MARGIN) {
                 return 1;
             }             
         }
         else if (y%2 == 1 && WALL_TEXTURES[y][x]) {
             if (x_at <= MAZE_EXTREME_LEFT + x * FULL_CUBE + COLLIDE_MARGIN &&
                 x_at >= MAZE_EXTREME_LEFT + x * FULL_CUBE - COLLIDE_MARGIN&&
                 y_at >= MAZE_EXTREME_TOP + (y/2) * FULL_CUBE - COLLIDE_MARGIN &&
                 y_at <= MAZE_EXTREME_TOP + (y/2 + 1) * FULL_CUBE + COLLIDE_MARGIN) {
                 return 1;
             }
         }
     }
 }
 return 0;
}


void drawText(int x, int y, string text = "not") {

    std::string str;
    if (text == "not") {

        int n = ((endTime - clock()) / 1000);

        sec = n % 60;
        mn = n / 60;
        hour = n / 3600;
        std::stringstream ss;
        ss << sec;
        strSec = ss.str();
        ss.str(std::string());
        ss.clear();
        ss << mn;
        strMn = ss.str();
        ss.str(std::string());
        ss.clear();
        ss << hour;
        strHour = ss.str();
        str = strHour + "::" + strMn + "::" + strSec;
        if (n < 0) {
            gameOver = true;camera_y = 0.1;CAMERA_SINK *= -1; rot_y = rot_x = -M_PI_2;
        }
    }
    else {
        str = text;
    }
    int length = str.length();
    glMatrixMode(GL_PROJECTION);
    double* matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)str[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

void draw_ortho_compass()
{
    // 1. Save all existing 3D attributes and server states
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // 2. Disable EVERYTHING that could interfere with flat colors
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    // Turn off vertex arrays in case your maze rendering engine leaves them active
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // 3. SWITCH TO THE PROJECTION MATRIX (Crucial step)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); // Save your 3D perspective setup matrix
    glLoadIdentity(); // Reset it to clean slate

    // Create a pixel-for-pixel flat 2D coordinate box matching the viewport
    gluOrtho2D(0.0, (double)windowwidth(), 0.0, (double)windowheight());

    // 4. SWITCH TO THE MODELVIEW MATRIX
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Save your 3D camera translation matrix
    glLoadIdentity(); // Reset it to clean slate

    // 5. DEFINE RADIAL ORIENTATION & SPIN
    // Anchor position in pixels (Top Right Corner)
    float cx = (float)windowwidth() - 70.0f;
    float cy = (float)windowheight() - 70.0f;

    // Convert rot_x radians back into degrees for OpenGL's rotation tool
    // (We multiply by -57.2957f because OpenGL rotates counter-clockwise)
    float rot_degrees = rot_x * 57.2957795f;

    // Move drawing origin to the compass center, spin it, then draw locally
    glTranslatef(cx, cy, 0.0f);
    glRotatef(rot_degrees, 0.0f, 0.0f, 1.0f);

    // 6. DRAW THE COMPASS OBJECT (Pointing towards standard X-Axis)
    glBegin(GL_QUADS);
    // Red Pointer Arrow Pointing Right (+X Direction)
    glColor3f(1.0f, 0.2f, 0.2f); // Red
    glVertex2f(35.0f, 0.0f);  // Tip pointing directly along X
    glVertex2f(0.0f, 12.0f);  // Top corner
    glVertex2f(5.0f, 0.0f);  // Inner center groove
    glVertex2f(0.0f, -12.0f);  // Bottom corner

    // Grey Base Weight Tail (Opposite Side)
    glColor3f(0.5f, 0.5f, 0.5f); // Grey
    glVertex2f(-5.0f, 0.0f);  // Center point
    glVertex2f(0.0f, 12.0f);  // Top corner
    glVertex2f(-25.0f, 0.0f);  // Tail end pointing away
    glVertex2f(0.0f, -12.0f);  // Bottom corner
    glEnd();

    // 7. CLEANUP MATRIX STACKS COMPLETELY
    // Pop the Modelview transformation modifications
    glPopMatrix();

    // Switch back to Projection and pop it back to your original 3D perspective
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // Reset the matrix focus context mode back to normal operations
    glMatrixMode(GL_MODELVIEW);

    // Restore all pipeline properties (Lighting, textures, masks) exactly how they were
    glPopAttrib();
}


void resetGame() {
    x_at = X_INIT;
    y_at = Y_INIT;
    gameOver = youWon = false;
    
    startTime = clock(), endTime = clock() + 185000;
    sec, mn = 0, hour = 0;
       
    rot_x = -M_PI_2;
    xin = 0, yin = 0;
    camera_y = START_CAMERA_Y;
    CAMERA_SINK *= -1;
    rot_y = -M_PI_2;
}

// Define a mouse sensitivity variable (Adjust this to your liking)
const float MOUSE_SENSITIVITY = 0.003f;

void move_relative(GLfloat forward_amt, GLfloat strafe_amt)
{
    GLfloat orig_x = x_at;
    GLfloat orig_y = y_at;

    // Forward/Backward vectors
    GLfloat fx = cos(rot_x) * forward_amt;
    GLfloat fy = sin(rot_x) * forward_amt;

    // Strafe Right/Left vectors (A = Left, D = Right)
    GLfloat sx = -sin(rot_x) * strafe_amt;
    GLfloat sy = cos(rot_x) * strafe_amt;

    x_at += fx + sx;
    y_at += fy + sy;

    if (collide())
    {
        x_at -= BOUNCEBACK * (fx + sx);
        y_at -= BOUNCEBACK * (fy + sy);
    }
    if (collide())
    {
        x_at = orig_x;
        y_at = orig_y;
    }
}

void update_movement()
{
    // Calculate Delta Time
    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (timeSinceStart - oldTimeSinceStart) / 1000.0f;
    oldTimeSinceStart = timeSinceStart;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    // Handle Jump Physics if jumping
    if (is_jumping && !gameOver && !youWon) {
        // 1. Apply gravity to vertical speed over time
        vertical_velocity += GRAVITY * deltaTime;

        // 2. Move the camera height based on current vertical speed
        camera_y += vertical_velocity * deltaTime;
        // 3. Check if we hit or passed below the floor level
        if (camera_y <= FLOOR_HEIGHT) {
            camera_y = FLOOR_HEIGHT; // Snap safely to floor
            vertical_velocity = 0.0f;
            is_jumping = false;      // Ready to jump again
        }
    }

    // --- Keep your existing horizontal walking / strafing code below ---
    float current_walk_speed = WALK_KEY_SENSE * 60.0f * deltaTime;
    float current_reverse_speed = WALK_KEY_REVERSE_SENSE * 60.0f * deltaTime;
    float current_turn_speed = ROTATE_KEY_SENSE * 60.0f * deltaTime;

    float forward_amt = 0.0f;
    float strafe_amt = 0.0f;

    if (keys['w'] || keys['W'] || specialKeys[GLUT_KEY_UP])    forward_amt += current_walk_speed;
    if (keys['s'] || keys['S'] || specialKeys[GLUT_KEY_DOWN])  forward_amt -= current_reverse_speed;
    if (keys['a'] || keys['A'])                                strafe_amt -= current_walk_speed;
    if (keys['d'] || keys['D'])                                strafe_amt += current_walk_speed;

    if (forward_amt != 0.0f || strafe_amt != 0.0f) {
        move_relative(forward_amt, strafe_amt);
    }

    if (camera_y <= FLOOR_HEIGHT) { // Changed from <=0.0f to match your baseline floor check
        if (specialKeys[GLUT_KEY_RIGHT]) rot_x += current_turn_speed;
        if (specialKeys[GLUT_KEY_LEFT])  rot_x -= current_turn_speed;
    }
}


void drawscene()
{  
 static bool init=0;
 static GLuint textureNums = TEXTURE_PATHS.size();
 static GLuint* walls = new GLuint[textureNums]; /*Textures for the cube*/
 static GLuint haze; /*Texture for the sky*/
 static GLuint gover; /*Texture for the sky*/
 static GLuint ywon; /*Texture for the sky*/
 static GLuint grnd; /*Texture for the sky*/
 
 if(!init)
 { 
  init=1;
  for(int i=0;i<textureNums;i++)
    walls[i] = maketex(&TEXTURE_PATHS[i][0], TEXTURE_SIZE, TEXTURE_SIZE);
  haze=maketex(SKY_FILE,SKY_SIZE_X,SKY_SIZE_Y);
  gover = maketex(GOVER_FILE, 800, 512);
  ywon = maketex(YWON_FILE, 800, 512);
  grnd=walls[textureNums-1];
  CONTROLLER_PLAY = min(windowwidth(), windowheight())/2.3f;
  x_at = X_INIT;
  y_at = Y_INIT;
 }

 if(abs(xin)>40 && abs(yin) > 40 && abs(xin - windowwidth())>40&&abs(yin - windowheight()) > 40)
 {
    if (yin<CONTROLLER_PLAY || yin>(windowheight() - CONTROLLER_PLAY))
        rot_y -= (yin - (windowheight() / 2.0f)) * ROTATE_MOUSE_SENSE* windowwidth() / windowheight();

    if (rot_y < 0) rot_y = max(rot_y, -M_PI_2);
    else rot_y = min(rot_y, M_PI_2);
  
    if(xin<CONTROLLER_PLAY || xin>(windowwidth()-CONTROLLER_PLAY))
        rot_x+=(xin-(windowwidth()/2.0f))*ROTATE_MOUSE_SENSE;
 }
 
 glLoadIdentity(); // Make sure we're no longer rotated.

 update_movement(); // Calculate smooth positions right before drawing

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


 if (!gameOver && !youWon) {
     sky(haze); //Draw sky
     drawText(700 - 100, 500, "Time Remaining: ");
     drawText(700, 500);
 }
 else {
     cout << camera_y << "\n";
     if (camera_y > 200) resetGame();

     if (gameOver) {
         sky(gover); //Draw sky
         drawText(700, 500, "Time's Up!");
     }
     else if (youWon) {
         sky(ywon); //Draw sky
         drawText(700, 500, "You Won!");
     }
} 

 gluLookAt(x_at,camera_y,y_at,x_at+cos(rot_x),camera_y+sin(rot_y), y_at + sin(rot_x), 0.0, 1.0, 0.0);

 if (camera_y > 0.0) camera_y -= CAMERA_SINK;

 print_maze(walls);// Draw the walls

 floor(grnd); //Draw floor
 draw_ortho_compass();
 glutSwapBuffers();
}


void mouse(int x, int y)
{
    int cx = windowwidth() / 2;
    int cy = windowheight() / 2;

    if (x == cx && y == cy) return;

    int dx = x - cx;
    int dy = y - cy;

    // Horizontal rotation (Yaw)
    rot_x += dx * MOUSE_SENSITIVITY;

    // Vertical rotation (Pitch)
    rot_y -= dy * MOUSE_SENSITIVITY; // Subtracting ensures looking up moves camera up

    // Clamp vertical look so you can't flip upside down (approx 85 degrees)
    if (rot_y > 1.48f)  rot_y = 1.48f;
    if (rot_y < -1.48f) rot_y = -1.48f;

    glutWarpPointer(cx, cy);
}

// --- NORMAL KEYS ---
void keypress(unsigned char key, int x, int y)
{
    if (key == ESCAPE) exit(0);

    // Trigger jump if Spacebar is pressed and player is currently on the ground
    if (key == ' ' && !is_jumping) {
        vertical_velocity = JUMP_VELOCITY;
        is_jumping = true;
    }

    keys[key] = true;
}

void keypress_up(unsigned char key, int x, int y)
{
    keys[key] = false; // Key was released
}

// --- ARROW / SPECIAL KEYS ---
void arrows(int key, int x, int y)
{
    if (key < 256) specialKeys[key] = true;
}

void arrows_up(int key, int x, int y)
{
    if (key < 256) specialKeys[key] = false;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowwidth(), windowheight());
    glutInitWindowPosition(WINDOW_STARTX, WINDOW_STARTY);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    // FIXED: You must create the window BEFORE registering callbacks or setting up GL
    int window = glutCreateWindow("openGLmaze by _Satyam_");

    glutDisplayFunc(drawscene);
    glutIdleFunc(drawscene); // Keeps the loop executing continuously
    glutReshapeFunc(resizer);

    // Smooth Input Registrations
    glutKeyboardFunc(keypress);
    glutKeyboardUpFunc(keypress_up);
    glutSpecialFunc(arrows);
    glutSpecialUpFunc(arrows_up);

    glutPassiveMotionFunc(mouse);
    glutSetCursor(GLUT_CURSOR_NONE);

    initgl(windowwidth(), windowheight());
    glDisable(GL_ALPHA_TEST);

    // Prime the clock right before entering loop
    oldTimeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    glutWarpPointer(windowwidth() / 2, windowheight() / 2);

    parseMaze(XSIZE, YSIZE, "maze1.txt");
    glutMainLoop();

    return 0;
}

