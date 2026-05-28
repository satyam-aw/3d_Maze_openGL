#include "drawHUD.h" // Include your updated header
#include "helperMethods.h"
#include "Constants.h"
#include <GL/glut.h> // Assuming your OpenGL/GLUT headers are here

using namespace std;

std::function<void()> drawText(std::string text) {
    // Capture the 'text' string by value into the lambda [text]
    return [text]() {
        int length = text.length();

        // Assuming windowwidth() and windowheight() are globally accessible 
        // or available in the scope where this eventually executes.
        float x = (float)windowwidth() - 210.0f;
        float y = (float)windowheight() - 250.0f;

        glRasterPos2i(x, y);
        for (int i = 0; i < length; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]);
        }
    };
}

std::function<void()> draw_ortho_compass(int rot_x) {
    return [rot_x]() {
        // 5. DEFINE RADIAL ORIENTATION & SPIN
        // Anchor position in pixels (Top Right Corner)
        float cx = (float)windowwidth() - 140.0f;
        float cy = (float)windowheight() - 140.0f;

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
        glVertex2f(70.0f, 0.0f);  // Tip pointing directly along X
        glVertex2f(0.0f, 24.0f);  // Top corner
        glVertex2f(10.0f, 0.0f);  // Inner center groove
        glVertex2f(0.0f, -24.0f);  // Bottom corner

        // Grey Base Weight Tail (Opposite Side)
        glColor3f(0.5f, 0.5f, 0.5f); // Grey
        glVertex2f(-10.0f, 0.0f);  // Center point
        glVertex2f(0.0f, 24.0f);  // Top corner
        glVertex2f(-50.0f, 0.0f);  // Tail end pointing away
        glVertex2f(0.0f, -24.0f);  // Bottom corner
        glEnd();
    };
}


void draw_HUD(const std::function<void()>& drawUI) {
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

    // 4. SWITCH TO THE MODELVIEW MATRIX
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); // Save your 3D camera translation matrix
    glLoadIdentity(); // Reset it to clean slate

    // Create a pixel-for-pixel flat 2D coordinate box matching the viewport
    gluOrtho2D(0.0, (double)windowwidth(), 0.0, (double)windowheight());

    // 5. Draw the HUD
    drawUI();

    // 6. CLEANUP MATRIX STACKS COMPLETELY
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
