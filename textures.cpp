#include "textures.h"
#include "helperMethods.h"
#include "Constants.h"

using namespace std;

void wall_vertical(GLfloat x, GLfloat y, GLfloat z, GLuint texture) 
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2d(1.0, 1.0);   glVertex3f(x - HALF_CUBE, WALL_HT / 2, z + HALF_CUBE); 
    glTexCoord2d(0.0, 1.0);   glVertex3f(x - HALF_CUBE, WALL_HT / 2, z - HALF_CUBE); 
    glTexCoord2d(0.0, 0.0);   glVertex3f(x - HALF_CUBE, -WALL_HT / 2, z - HALF_CUBE); 
    glTexCoord2d(1.0, 0.0);   glVertex3f(x - HALF_CUBE, -WALL_HT / 2, z + HALF_CUBE); 
    glEnd();
}

void wall_horizontal(GLfloat x, GLfloat y, GLfloat z, GLuint texture) 
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glTexCoord2d(1.0, 1.0);   glVertex3f(x + HALF_CUBE, WALL_HT / 2, z - HALF_CUBE); 
    glTexCoord2d(0.0, 1.0);   glVertex3f(x - HALF_CUBE, WALL_HT / 2, z - HALF_CUBE); 
    glTexCoord2d(0.0, 0.0);   glVertex3f(x - HALF_CUBE, -WALL_HT / 2, z - HALF_CUBE); 
    glTexCoord2d(1.0, 0.0);   glVertex3f(x + HALF_CUBE, -WALL_HT / 2, z - HALF_CUBE); 
    glEnd();
}

void sky(GLuint haze)
{ 
     glBindTexture(GL_TEXTURE_2D, haze);
     glBegin(GL_QUADS); 
     glTexCoord2d(1.0, 1.0);  glVertex3f((windowwidth() / SKY_SCALE), (windowheight() / SKY_SCALE), -SKY_DISTANCE); 
     glTexCoord2d(0.0, 1.0);  glVertex3f(-(windowwidth() / SKY_SCALE), (windowheight() / SKY_SCALE), -SKY_DISTANCE); 
     glTexCoord2d(0.0, 0.0);  glVertex3f(-(windowwidth() / SKY_SCALE), -(windowheight() / SKY_SCALE), -SKY_DISTANCE); 
     glTexCoord2d(1.0, 0.0);  glVertex3f((windowwidth() / SKY_SCALE), -(windowheight() / SKY_SCALE), -SKY_DISTANCE); 
     glEnd();
}

void floor(GLuint grnd)
{   
    glBindTexture(GL_TEXTURE_2D, grnd);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBegin(GL_QUADS);
    glTexCoord2d(10.0, 10.0); glVertex3f(MAZE_EXTREME_LEFT + XSIZE * FULL_CUBE, -HALF_CUBE, MAZE_EXTREME_TOP);
    glTexCoord2d(0.0, 10.0);  glVertex3f(MAZE_EXTREME_LEFT, -HALF_CUBE, MAZE_EXTREME_TOP);
    glTexCoord2d(0.0, 0.0);   glVertex3f(MAZE_EXTREME_LEFT, -HALF_CUBE, MAZE_EXTREME_TOP + YSIZE * FULL_CUBE);
    glTexCoord2d(10.0, 0.0);  glVertex3f(MAZE_EXTREME_LEFT + XSIZE * FULL_CUBE, -HALF_CUBE, MAZE_EXTREME_TOP + YSIZE * FULL_CUBE);
    glEnd();
}

void print_maze(GLuint *walls) 
{
    int x, y; 
    for(y = 0; y < YSIZE * 2; ++y) 
    {
        for(x = 0; x < XSIZE; ++x)
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
