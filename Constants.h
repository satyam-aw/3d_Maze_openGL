#pragma once
#include <GL/glut.h>
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<string>

//Constants

extern GLint CONTROLLER_PLAY;
extern GLint WINDOW_STARTX;
extern GLint WINDOW_STARTY;
extern GLint ESCAPE; /* ASCII code for the escape key. */
extern GLint TEXTURE_SIZE;
extern GLint SKY_SIZE_X;
extern GLint SKY_SIZE_Y;
extern GLint BMP_HEADER_SIZE; //Assumed 24-bit depth
extern GLint WINDOW_MARGIN;
extern GLfloat MAZE_EXTREME_LEFT;
extern GLfloat MAZE_EXTREME_TOP;
extern GLfloat HALF_CUBE;
extern GLfloat FULL_CUBE; 
extern GLfloat WALL_HT;
extern GLfloat START_X_AT;
extern GLfloat START_Y_AT;
extern GLfloat START_ROT;
extern GLfloat START_CAMERA_Y;
extern GLfloat CAMERA_SINK;
extern GLfloat VIEW_FIELD;
extern GLfloat NEAR_Z;
extern GLfloat FAR_Z;
extern GLfloat SKY_DISTANCE;
extern GLfloat LEFTMOST_CUBE_CENTER;
extern GLfloat COLLIDE_MARGIN;  //To keep from looking inside the cubes
extern GLfloat ROTATE_MOUSE_SENSE;
extern GLfloat ROTATE_KEY_SENSE;
extern GLfloat WALK_MOUSE_SENSE;
extern GLfloat WALK_KEY_SENSE;
extern GLfloat WALK_MOUSE_REVERSE_SENSE; //Slower when backpedaling
extern GLfloat WALK_KEY_REVERSE_SENSE;
extern GLfloat BOUNCEBACK; //1.0f means none (just reverse collision)
extern GLfloat SKY_SCALE;
extern GLfloat X_INIT;
extern GLfloat Y_INIT;

extern std::vector<std::vector<int>> WALL_TEXTURES;
extern std::vector<std::string> TEXTURE_PATHS;

#define WALL_FILE "./resources/bmp/wall1.bmp"
#define SKY_FILE "./resources/bmp/sky3.bmp"
#define GOVER_FILE "./resources/bmp/gover.bmp"
#define YWON_FILE "./resources/bmp/ywon.bmp"
#define FLOOR_FILE "./resources/bmp/floor.bmp"
#define TEXTURE_PATH_PREFIX "./resources/bmp/"
#define TEXTURE_FILE "./resources/bmp/wall1.bmp"

//Logic constants for map contents
extern GLint SOLUTION_PATH;
extern GLint FALSE_PATH;
extern GLint NO_PATH;

