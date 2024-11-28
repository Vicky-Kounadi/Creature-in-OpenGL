#include <GL/glut.h>   //An interface and windows management library
#include <iostream>
#include <math.h>

//Header files for our OpenGL functions and texture translate
#include "visuals.h"   
#include "BMPLoader.h"

//Texture
unsigned int tex;

//Gum position
float gumX = 0.0;
float gumY = 20.0;
float gumZ = 0.0;

//Gum movement
bool isMoving = false;
float gumRotation = 0.0;
float gumSpeed = 0.05;

//Gum feet animation
bool isFeetForward = true;
float feetSpeed = 0.02;
float maxFeetPos = 7.0;
float feetPos = 0.0;

//Gum jump animation
bool isJumping = false;
float jumpSpeed = 0.0f;
//I calculated that since the gum is 3cm in diameter in real world and here it is 40units
//In equivalent gravity= 9.8m/s^2 is 0.00735units/s^2 here
const float gravity = 0.00735;
const float bounce = 0.5; 
float gumSqueeze = 1.0;

//Camera
// HERE x axis -> SPHERE y axis 
// HERE y axis -> SPHERE z axis 
// HERE z axis -> SPHERE x axis 
// Radius FOR zoom
// Since all is flipped 90 degrees i flip the phi and theta
// Phi   HERE (0-180) up/down move -> SPHERE (0-360) right/left move
// Theta HERE (0-360) right/left move -> SPHERE (0-180) up/down move

//Cartesian
float cameraX = 0.0;
float cameraY = 0.0;
float cameraZ = 0.0;
//Spherical
float cameraPhi = 0.0;   
float cameraTheta = 0.0; 
float cameraRadius = 0.0; 

//TEXTURE
GLuint make_texture(int type, const char* fname)
{
    GLuint tex;

    BMPClass myBMPLoader;
    //Using the BMPClass loader we get the raster off a BMP file
    std::cout << "Loading Texture: " << TranslateBMPError(BMPLoad(fname, myBMPLoader)) << "\n";

    //We "generate" the texture ID, calling glGenTextures
    glGenTextures(1, &tex);

    //We bind the texture to the active texture unit. This tells OpenGL that this is the "active" texture
    glBindTexture(GL_TEXTURE_2D, tex);

    // define what happens if given (s,t) outside [0,1] {GL_REPEAT, GL_CLAMP}
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // define texture filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);

    // load image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
        myBMPLoader.width, myBMPLoader.height,
        0, GL_RGB, GL_UNSIGNED_BYTE, myBMPLoader.bytes);

    return tex;
}

void axes()
{
    glPushMatrix();
    //x axis red
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(50.0, 0.0, 0.0);
    glEnd();

    //y axis green
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 50.0, 0.0);
    glEnd();

    //z axis blue
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 50.0);
    glEnd();

    glPopMatrix();
}

void floor()
{
    glColor3f(1.0, 1.0, 1.0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-500.0f, 0.0f, -500.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-500.0f, 0.0f, 500.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(500.0f, 0.0f, 500.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(500.0f, 0.0f, -500.0f);
    glEnd();

    glEnable(GL_BLEND);

    glColor4f(1.f, 1.f, .5f, 0.6f);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, tex);

}

void gum()
{
    //Unbind texture on floor
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Feet
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glTranslatef(15, -15, 10 +feetPos);
        glRotatef(45, 0, 1, 0);
        glScalef(0.6, 0.5, 1.1);
        glutSolidSphere(10, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.0, 0.0, 0.3);
        glTranslatef(-15, -15, 10 - feetPos);
        glRotatef(-45, 0, 1, 0);
        glScalef(0.6, 0.5, 1.1);
        glutSolidSphere(10, 50, 50);
        glutSolidSphere(10, 50, 50);
    glPopMatrix();

    //Eyes
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(7, 10, 16);
        glutSolidSphere(7, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(-7, 10, 16);
        glutSolidSphere(7, 50, 50);
    glPopMatrix();

    //Pupils
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.2);
        glTranslatef(7, 11, 20);
        glutSolidSphere(5, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(8, 12, 23);
        glutSolidSphere(2, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(7, 10, 23);
        glutSolidSphere(2, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.0, 0.0, 0.2);
        glTranslatef(-7, 11, 20);
        glutSolidSphere(5, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(-6, 12, 23);
        glutSolidSphere(2, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glTranslatef(-7, 10, 23);
        glutSolidSphere(2, 50, 50);
    glPopMatrix();

    //Bow
    glPushMatrix();
        glColor3f(1.0, 0.50, 0.70);
        glTranslatef(0, 20, 8);
        glutSolidSphere(3, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 0.50, 0.70);
        glTranslatef(10, 20, 8);
        glRotatef(-90, 0, 1, 0);
        glutSolidCone(5, 12, 50, 50);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1.0, 0.50, 0.70);
        glTranslatef(-10, 20, 8);
        glRotatef(90, 0, 1, 0);
        glutSolidCone(5, 12, 50, 50);
    glPopMatrix();

    //Nose
    glPushMatrix();
        glColor4f(0.0, 0.0, 1.0, 0.7);
        glTranslatef(0, 3, 18);
        glScalef(0.7, 0.6, 0.7);
        glutSolidSphere(5, 50, 50);
    glPopMatrix();

    //Render transparent objects AFTER solid ones cause of Zbuffer
    //Body
    glPushMatrix();
        glColor4f(0.0, 0.0, 1.0, 0.7);
        glutSolidSphere(20, 50, 50);
    glPopMatrix();

}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera position
    gluLookAt(cameraX, cameraY, cameraZ, 
        0.0, 0.0, 0.0,      // make this line gumX, gumY, gumZ, to focus on gum    
        0.0, 1.0, 0.0);         
   
    //scene
    axes();
    floor();

    glEnable(GL_BLEND);

    glScalef(1.0, gumSqueeze, 1.0);
    glTranslatef(gumX, gumY, gumZ);
    glRotatef(gumRotation, 0, 1, 0);
    gum();

    glutSwapBuffers();

}

void calcDirection(float angle, float& dirX, float& dirZ)
{
    //Moires se aktinia
    float angleRad = angle * 0.0174533;
    dirX = sinf(angleRad); 
    dirZ = cosf(angleRad);

    //Kanoniko dianysma
    float length = sqrt(dirX * dirX + dirZ * dirZ); //metro
    dirX /= length;
    dirZ /= length;
}

void Idle()
{
    //Walking
    if (isMoving)
    {
        //Finds direction of movement
        float dirX, dirZ;
        calcDirection(gumRotation, dirX, dirZ);

        //Move according to speed and direction
        gumX += gumSpeed * dirX;
        gumZ += gumSpeed * dirZ;

     
        //Feets move between maxFeetPos (-7 and 7) if reach edges change direction
        if (fabs(feetPos) >= maxFeetPos)
            isFeetForward = !isFeetForward; //if true make it false and the opposite

        //If feet go back or forth
        if (isFeetForward)
            feetPos += feetSpeed;
        else
            feetPos -= feetSpeed;
        
        //Doesnt leave the floor
        if (gumX>=500 || gumX<=-500 || gumZ >= 500 || gumZ <= -500)
            gumRotation += 180.0;
    }
    else
    {
        // Reset feet animation
        feetPos = 0.0;
        isFeetForward = true;
    }

    if (isJumping)
    {
        // Reduce speed for free fall (gravity effect)
        jumpSpeed -= gravity;

        // Move the gum in the y-axis based on jump speed
        gumY += jumpSpeed;


        if (gumY < 20.0 && jumpSpeed < 0.0)
        {
            gumY = 20.0; //snap the gum to the floor level

            gumSqueeze -= 0.05;
            jumpSpeed *= -bounce;

            //if the jump speed becomes small enough go back to normal
            if (fabs(jumpSpeed) < 0.1)
            gumSqueeze =1;
        }
    }

    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    //ESCAPE
    case 27: 
        exit(0);
        break;
    //MOVES
    case 'w':
        isMoving = !isMoving; //if true make it false and the opposite
        break;
    //ROTATES
    case 'n':
        gumRotation -= 5.0;
        break;
    case 'm':
        gumRotation += 5.0;
        break;
    //SPEED
    case 'a':
        gumSpeed += 0.1; 
        feetSpeed += 0.01;
        break;
    case 'z':
        gumSpeed -= 0.1; 
        gumSpeed = fmaxf(gumSpeed, 0.05); //speed decreases but it doesnt stop moving
        feetSpeed -= 0.01;
        feetSpeed = fmaxf(feetSpeed, 0.02);
        break;
    //JUMPS
    case 'j':
        if (!isJumping || gumY == 20.0) //jumping if not already jumping or if the gum is on the floor
        {
            isJumping = true;
            jumpSpeed = 1.5;
        }
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void Resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (h == 0)
        h = 1;

    float aspect = static_cast<float>(w) / static_cast<float>(h);
    gluPerspective(60.0, aspect, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

void Setup()
{
   glShadeModel(GL_SMOOTH);

   glEnable(GL_DEPTH_TEST);
   glDepthFunc( GL_LEQUAL );
   glClearDepth(1.0);

   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CCW);

   //Set up light source
   GLfloat ambientLight[] = { 0.5, 0.5, 0.5, 1.0 };
   GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat lightPos[] = { 0.0, 100.0, 0.0, 1.0 }; 

   glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
   glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight );
   glLightfv( GL_LIGHT0, GL_POSITION,lightPos );

    //Polygon rendering mode and material properties
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0);

    // Grey background
    glClearColor(0.5, 0.5, 0.5, 1.0);

    //Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Floor Texture
    glEnable(GL_TEXTURE_2D);
    tex = make_texture(GL_LINEAR, "images/sprinkles.bmp");

    // Camera good spherical initial position after many tries
    cameraPhi = 70.0;
    cameraTheta = 60.0;
    cameraRadius = 200.0;
    // From spherical to cartesian for render glutLookAt to make the scene
    float thetaRad = cameraTheta * 0.0174533f; //degrees to radians
    float phiRad = cameraPhi * 0.0174533f;
    cameraX = cameraRadius * sinf(phiRad) * cosf(thetaRad);
    cameraZ = cameraRadius * sinf(phiRad) * sinf(thetaRad);
    cameraY = cameraRadius * cosf(phiRad);

}

void SpecialKeys(int key, int x, int y)
{
    const float rotationSpeed = 2.0;
    const float zoomSpeed = 10.0;

    switch (key)
    {
    case GLUT_KEY_LEFT:
        cameraTheta += rotationSpeed;
        break;
    case GLUT_KEY_RIGHT:
        cameraTheta -= rotationSpeed;
        break;
    case GLUT_KEY_UP:
        //if reach floor flip sides -skip under floor
        if (cameraPhi == -90.0)
            cameraPhi = 90.0;
        else
            cameraPhi -= rotationSpeed;
        break;
    case GLUT_KEY_DOWN:
        //if reach floor flip sides - skip under floor
        if (cameraPhi == 90.0)
            cameraPhi = -90.0;
        else
            cameraPhi += rotationSpeed;
        break;
    case GLUT_KEY_PAGE_UP:
        cameraRadius -= zoomSpeed;
        cameraRadius = fmaxf(cameraRadius, 0.1); //radius cannot negative
        break;
    case GLUT_KEY_PAGE_DOWN:
        cameraRadius += zoomSpeed;
        cameraRadius = fminf(cameraRadius, 1350.0);
        break;
    }

    // From spherical to cartesian for render glutLookAt to make the scene
    float thetaRad = cameraTheta * 0.0174533f; //degrees to radians
    float phiRad = cameraPhi * 0.0174533f;
    cameraX = cameraRadius * sinf(phiRad) * cosf(thetaRad);
    cameraZ = cameraRadius * sinf(phiRad) * sinf(thetaRad);
    cameraY = cameraRadius * cosf(phiRad);

    glutPostRedisplay();
}

