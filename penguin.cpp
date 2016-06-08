/***********************************************************
             CSC418/2504, Fall 2009
  
                 penguin.cpp
                 
       Simple demo program using OpenGL and the glut/glui 
       libraries

  
    Instructions:
        Please read the assignment page to determine 
        exactly what needs to be implemented.  Then read 
        over this file and become acquainted with its 
        design.

        Add source code where it appears appropriate. In
        particular, see lines marked 'TODO'.

        You should not need to change the overall structure
        of the program. However it should be clear what
        your changes do, and you should use sufficient comments
        to explain your code.  While the point of the assignment
        is to draw and animate the character, you will
        also be marked based on your design.

***********************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <GL/glut.h>
#include <GL/glui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <map>

#ifndef _WIN32
#include <unistd.h>
#else
void usleep(unsigned int nanosec)
{
    Sleep(nanosec / 1000);
}
#endif


// *************** GLOBAL VARIABLES *************************


const float PI = 3.14159;

// --------------- USER INTERFACE VARIABLES -----------------

// Window settings
int windowID;               // Glut window ID (for display)
GLUI *glui;                 // Glui window (for controls)
int Win[2];                 // window (x,y) size


// ---------------- ANIMATION VARIABLES ---------------------

// Animation settings
int animate_mode = 0;       // 0 = no anim, 1 = animate
int animation_frame = 0;      // Specify current frame of animation

// Joint parameters 
const float JOINT_MIN = -45.0f;
const float JOINT_MAX =  45.0f;
const float HEAD_MIN = -20.0f;
const float HEAD_MAX =  20.0f;
const float LEG_MIN = -30.0f;
const float LEG_MAX =  30.0f;
const float FOOT_MIN = -30.0f;
const float FOOT_MAX =  30.0f;

//float joint_rot = 0.0f;

//////////////////////////////////////////////////////
// TODO: Add additional joint parameters here
//////////////////////////////////////////////////////

// define the range for beak movement
const float BEAK_MIN = -0.8f;
const float BEAK_MAX = 0.8f;

//initial the joint value for differnt part
float upper_beak_rot = 0.0f;
float lower_beak_rot = 0.0f;
float right_foot_rot = 0.0f;
float left_foot_rot = 0.0f;
float right_leg_rot = 0.0f;
float left_leg_rot = 0.0f;
float arm_rot = 0.0f;
float head_rot = 0.0f;

//body size for drawing
const float ARM_WIDTH = 15.0f;
const float ARM_LENGTH = 25.0f;
const float LEG_WIDTH = 8.0f;
const float LEG_LENGTH = 20.0f;
const float FOOT_WIDTH = 18.0f;
const float FOOT_LENGTH = 5.0f;
const float UPPER_BEAK_LENGTH = 5.0f;
const float LOWER_BEAK_LENGTH = 3.0f;
const float BEAK_WIDTH = 15.0f;
const float HEAD_SIZE = 25.0f;
const float TORSO_WIDTH = 40.0f;
const float TORSO_LENGTH = 60.0f;

// put value in map for convenience
std::map<std::string, float *> joint_list;




// ***********  FUNCTION HEADER DECLARATIONS ****************


// Initialization functions
void initGlut(char* winName);
void initGlui();
void initGl();


// Callbacks for handling events in glut
void myReshape(int w, int h);
void animate();
void display(void);

// Callback for handling events in glui
void GLUI_Control(int id);


// Functions to help draw the object
void drawSquare(float size);
void drawCircle(float radius);
void drawTorso();
void drawHead();
void drawArm();
void drawLegs();


// Return the current system clock (in seconds)
double getTime();


// ******************** FUNCTIONS ************************



// main() function
// Initializes the user interface (and any user variables)
// then hands over control to the event handler, which calls 
// display() whenever the GL window needs to be redrawn.
int main(int argc, char** argv)
{

    // Process program arguments
    if(argc != 3) {
        printf("Usage: demo [width] [height]\n");
        printf("Using 300x200 window by default...\n");
        Win[0] = 300;
        Win[1] = 200;
    } else {
        Win[0] = atoi(argv[1]);
        Win[1] = atoi(argv[2]);
    }


    // Initialize glut, glui, and opengl
    glutInit(&argc, argv);
    initGlut(argv[0]);
    initGlui();
    initGl();

    // Invoke the standard GLUT main event loop
    glutMainLoop();

    return 0;         // never reached
}


// Initialize glut and create a window with the specified caption 
void initGlut(char* winName)
{
    // Set video mode: double-buffered, color, depth-buffered
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Win[0],Win[1]);
    windowID = glutCreateWindow(winName);

    // Setup callback functions to handle events
    glutReshapeFunc(myReshape); // Call myReshape whenever window resized
    glutDisplayFunc(display);   // Call display whenever new frame needed 
}


// Quit button handler.  Called when the "quit" button is pressed.
void quitButton(int)
{
  exit(0);
}

// Animate button handler.  Called when the "animate" checkbox is pressed.
void animateButton(int)
{
  // synchronize variables that GLUT uses
  glui->sync_live();

  animation_frame = 0;
  if(animate_mode == 1) {
    // start animation
    GLUI_Master.set_glutIdleFunc(animate);
  } else {
    // stop animation
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

// Initialize GLUI and the user interface
void initGlui()
{
    GLUI_Master.set_glutIdleFunc(NULL);

    // Create GLUI window
    glui = GLUI_Master.create_glui("Glui Window", 0, Win[0]+10, 0);

    // Create a control to specify the rotation of the joint
    //GLUI_Spinner *joint_spinner
        //= glui->add_spinner("Joint", GLUI_SPINNER_FLOAT, &joint_rot);
    //joint_spinner->set_speed(0.1);
    //joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);

    ///////////////////////////////////////////////////////////
    // TODO: 
    //   Add controls for additional joints here
    ///////////////////////////////////////////////////////////
    joint_list["Head Joint"] = &head_rot;
    joint_list["Arm Joint"] = &arm_rot;
    joint_list["Left Leg Joint"] = &left_leg_rot;
    joint_list["Right Leg Joint"] = &right_leg_rot;
    joint_list["Left Foot Joint"] = &left_foot_rot;
    joint_list["Right Foot Joint"] = &right_foot_rot;

    // use for loop to create six joints name and value on Glui
    for (std::map<std::string, float *>::iterator i=joint_list.begin(); i!=joint_list.end(); ++i)
    {
        GLUI_Spinner *joint_spinner
            = glui->add_spinner(i->first.c_str(), GLUI_SPINNER_FLOAT, i->second);
        joint_spinner->set_speed(0.1);
        joint_spinner->set_float_limits(JOINT_MIN, JOINT_MAX, GLUI_LIMIT_CLAMP);
    }

    //add two beak name and value on Glui


    GLUI_Spinner *joint_spinner
        = glui->add_spinner("Upper Beak Joint", GLUI_SPINNER_FLOAT, &upper_beak_rot);
    joint_spinner->set_speed(0.1);
    joint_spinner->set_float_limits(BEAK_MIN, BEAK_MAX, GLUI_LIMIT_CLAMP);
    
    //GLUI_Spinner *
    joint_spinner
        = glui->add_spinner("Lower Beak Joint", GLUI_SPINNER_FLOAT, &lower_beak_rot);
    joint_spinner->set_speed(0.1);
    joint_spinner->set_float_limits(BEAK_MIN, BEAK_MAX, GLUI_LIMIT_CLAMP);

    // Add button to specify animation mode 
    glui->add_separator();
    glui->add_checkbox("Animate", &animate_mode, 0, animateButton);

    
    // Add "Quit" button
    glui->add_separator();
    glui->add_button("Quit", 0, quitButton);

    // Set the main window to be the "active" window
    glui->set_main_gfx_window(windowID);
}


// Performs most of the OpenGL intialization
void initGl(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);
}




// Callback idle function for animating the scene
void animate()
{
    // Update geometry
    const double joint_rot_speed = 0.1;
    double joint_rot_t = (sin(animation_frame*joint_rot_speed) + 1.0) / 2.0;
    // joint_rot = joint_rot_t * JOINT_MIN + (1 - joint_rot_t) * JOINT_MAX;
    
    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function animate the character's joints
    //   Note: Nothing should be drawn in this function!  OpenGL drawing
    //   should only happen in the display() callback.
    ///////////////////////////////////////////////////////////

    //animate part for the penguin
    
    head_rot = joint_rot_t*HEAD_MIN +(1- joint_rot_t) * HEAD_MAX;
    right_leg_rot = joint_rot_t*LEG_MIN + (1- joint_rot_t) * LEG_MAX + 20 ;
    left_leg_rot = joint_rot_t*LEG_MIN + (1- joint_rot_t) * LEG_MAX;
    arm_rot = - (joint_rot_t*JOINT_MIN + (1- joint_rot_t) * JOINT_MAX);
    right_foot_rot = joint_rot_t*FOOT_MIN + (1- joint_rot_t) * FOOT_MAX;
    left_foot_rot = joint_rot_t*FOOT_MIN + (1- joint_rot_t) * FOOT_MAX;
        

    upper_beak_rot = joint_rot_t*BEAK_MIN + (1-joint_rot_t)*JOINT_MAX;

    lower_beak_rot = - (joint_rot_t*BEAK_MIN + (1-joint_rot_t)*JOINT_MAX);
    
    
    



    // Update user interface
    glui->sync_live();

    // Tell glut window to update itself.  This will cause the display()
    // callback to be called, which renders the object (once you've written
    // the callback).
    glutSetWindow(windowID);
    glutPostRedisplay();

    // increment the frame number.
    animation_frame++;

    // Wait 50 ms between frames (20 frames per second)
    usleep(50000);
}


// Handles the window being resized by updating the viewport
// and projection matrices
void myReshape(int w, int h)
{
    // Setup projection matrix for new window
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w/2, w/2, -h/2, h/2);

    // Update OpenGL viewport and internal variables
    glViewport(0,0, w,h);
    Win[0] = w;
    Win[1] = h;
}



// display callback
//
// This gets called by the event handler to draw
// the scene, so this is where you need to build
// your scene -- make your changes and additions here.
// All rendering happens in this function.  For Assignment 1,
// updates to geometry should happen in the "animate" function.
void display(void)
{
    // glClearColor (red, green, blue, alpha)
    // Ignore the meaning of the 'alpha' value for now
    glClearColor(0.7f,0.7f,0.9f,1.0f);

    // OK, now clear the screen with the background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    // Setup the model-view transformation matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ///////////////////////////////////////////////////////////
    // TODO:
    //   Modify this function draw the scene
    //   This should include function calls to pieces that
    //   apply the appropriate transformation matrice and
    //   render the individual body parts.
    ///////////////////////////////////////////////////////////

    // Draw our hinged object
    //const float BODY_WIDTH = 30.0f;
    //const float BODY_LENGTH = 50.0f;
    //const float ARM_LENGTH = 50.0f;
    //const float ARM_WIDTH = 10.0f;
    
    
    
    

    // Push the current transformation matrix on the stack
    glPushMatrix();
        drawTorso();
        //add other part here
        drawHead();
        drawArm();
        drawLegs();
        //drawEye();
        
        // Draw the 'body'
        //glPushMatrix();
            // Scale square to size of body
            //glScalef(BODY_WIDTH, BODY_LENGTH, 1.0);

            // Set the colour to green
            //glColor3f(0.0, 1.0, 0.0);

            // Draw the square for the body
            //drawSquare(1.0);
        //glPopMatrix();
        
        // Draw the 'arm'

        // Move the arm to the joint hinge
        //glTranslatef(0.0, -BODY_LENGTH/2 + ARM_WIDTH, 0.0);

        // Rotate along the hinge
        //glRotatef(joint_rot, 0.0, 0.0, 1.0);

        // Scale the size of the arm
        //glScalef(ARM_WIDTH, ARM_LENGTH, 1.0);

        // Move to center location of arm, under previous rotation
       //glTranslatef(0.0, -0.5, 0.0);

        // Draw the square for the arm
       // glColor3f(1.0, 0.0, 0.0);
       // drawSquare(1.0);

    // Retrieve the previous state of the transformation stack
    glPopMatrix();


    // Execute any GL functions that are in the queue just to be safe
    glFlush();

    // Now, show the frame buffer that we just drew into.
    // (this prevents flickering).
    glutSwapBuffers();
}


// Draw a square of the specified size, centered at the current location
void drawSquare(float width)
{
    // Draw the square
    glBegin(GL_POLYGON);
    glVertex2d(-width/2, -width/2);
    glVertex2d(width/2, -width/2);
    glVertex2d(width/2, width/2);
    glVertex2d(-width/2, width/2);
    glEnd();
}

//add help function to draw a circle
void drawCircle(float radius){

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i ++){
        double theta = i*PI/180;
        glVertex2d(cos(theta)*radius, sin(theta)*radius);
    }
    glEnd();
}





//
//add help function to draw the body of the penguin
void drawTorso(){
    glPushMatrix();
        
            glScalef(TORSO_WIDTH,TORSO_LENGTH,1.0);
            glColor3f(0.8,0.2,1.0);
            //main body
            glBegin(GL_POLYGON);
            glVertex2d(-0.25, 0.5);
            glVertex2d(0.25, 0.5);
            glVertex2d(0.5, -0.25);
            glVertex2d(0.25, -0.5);
            glVertex2d(-0.25, -0.5);
            glVertex2d(-0.5, -0.25);
            glEnd();
       
        
    glPopMatrix();
}

//void drawEye(){
     
//}

void drawHead(){
    glPushMatrix();
        //create head part
        glTranslatef(0.0, TORSO_LENGTH/2, 0.0);
        glRotatef(head_rot, 0.0, 0.0, 1.0);
        glPushMatrix();
            glTranslatef(0.0, HEAD_SIZE/2 - 5.0, 0.0);
            glPushMatrix();
                glScalef(HEAD_SIZE,HEAD_SIZE,1.0);
                glColor3f(0.5,0.8,0.4);
                glBegin(GL_POLYGON);
                glVertex2d(-0.5, 0.0);
                glVertex2d(-0.125, 0.375);
                glVertex2d(0.5, 0.0);
                glVertex2d(0.5, -0.5);
                glVertex2d(-0.5, -0.5);
                glEnd();
            glPopMatrix();
//push matrix for eye and eye all
            //eyes on head
            glPushMatrix();
                glTranslatef(-HEAD_SIZE/4, HEAD_SIZE/8 - 4, 0.0);
                glPushMatrix();
                    
                    glColor3f(0.0,0.0,0.0);
                    drawCircle(3.0);
                glPopMatrix();
                //eye ball in eye
                glPushMatrix();
                glTranslatef(-HEAD_SIZE/4 + 4.5, HEAD_SIZE/8 - 4, 0.0);
                    glPushMatrix();
                        glColor3f(0.0,0.0,0.0);
                        drawCircle(2.0);
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
            
//glPushMatrix for beak
            glPushMatrix();
                glTranslatef(-HEAD_SIZE*0.75,-HEAD_SIZE/4, 0.0f);
                //upperbeak at the front of the head
                glPushMatrix();
                    glTranslatef(0,UPPER_BEAK_LENGTH/2 + upper_beak_rot, 0.0f);
                    glPushMatrix();
                        glScalef(BEAK_WIDTH,UPPER_BEAK_LENGTH,1.0);
                        glColor3f(1.0,0.8,0.2);
                        glBegin(GL_POLYGON);
                        glVertex2d(-0.5, 0);
                        glVertex2d(0.5, 0.5);
                        glVertex2d(0.5, -0.5);
                        glVertex2d(-0.5, -0.5);
                        
                        glEnd();
                    glPopMatrix();
                glPopMatrix();
                //lowerbeak at the front of the head
                glPushMatrix();
                    glTranslatef(0,LOWER_BEAK_LENGTH/2 + lower_beak_rot -3, 0.0f);
                    glPushMatrix();
                        glScalef(BEAK_WIDTH,LOWER_BEAK_LENGTH,1.0);
                        glColor3f(1.0,0.8,0.2);
                        drawSquare(1.0);
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        glColor3f(1,1,1);
        drawCircle(2.5);
    glPopMatrix();
}

void drawArm()
{
    //penguin's arm on Torse
    glPushMatrix();
        glTranslatef(0.5,ARM_LENGTH/2,0.0);
        glRotatef(arm_rot,0.0,0.0,1.0);
        glPushMatrix();
            glTranslatef(0.0f,-ARM_LENGTH/2,0.0);
            glScalef(ARM_WIDTH,ARM_LENGTH,1.0);
            glColor3f(0.2,1.0,0.8);
            glBegin(GL_POLYGON);
            glVertex2d(0.5, 0.5);
            glVertex2d(0.25, -0.5);
            glVertex2d(-0.25, -0.5);
            glVertex2d(-0.5, 0.5);
                        
            glEnd();
            glTranslatef(0.0,ARM_LENGTH/2,0.0);
        glPopMatrix();
        glColor3f(1,1.0,1.0);
        drawCircle(3);
    glPopMatrix();
}

//the legs and feet can be created together and they have same shape, it 
//can be done by drawSquare function
void drawLegs()
{
    //left leg
    glPushMatrix();
        glTranslatef(-TORSO_WIDTH/4,-TORSO_LENGTH/4, 0.0f);
        glPushMatrix();
            glRotatef(left_leg_rot,0.0,0.0,1.0);
            glTranslatef(0.0f, -LEG_LENGTH/2, 0.0f);
            
            glPushMatrix();
                glScalef(LEG_WIDTH,LEG_LENGTH,1.0);
                glColor3f(1.0,0.8,0.2);
                drawSquare(1.0);
            glPopMatrix();
            //left foot on the top of left leg
            glPushMatrix();
                glTranslatef(0.0f,-LEG_LENGTH/2 + 3,0.0);
                glRotatef(left_foot_rot,0.0,0.0,1.0);
                glPushMatrix();
                    glTranslatef(-FOOT_WIDTH/2, 0.0f, 0.0f);
                    glScalef(FOOT_WIDTH,FOOT_LENGTH,1.0);
                    glColor3f(1.0,0.8,0.2);
                    drawSquare(1.0);
                glPopMatrix();
                glColor3f(1.0,1.0,1.0);
                drawCircle(3);
            glPopMatrix();
        glPopMatrix();
        glColor3f(1.0,1.0,1.0);
        drawCircle(3);
    glPopMatrix();
    //right leg on Torso
    glPushMatrix();
        glTranslatef(TORSO_WIDTH/4,-TORSO_LENGTH/4, 0.0f);
        glPushMatrix();
            glRotatef(right_leg_rot,0.0,0.0,1.0);
            glTranslatef(0.0, -LEG_LENGTH/2, 0.0f);
            
            glPushMatrix();
                glScalef(LEG_WIDTH,LEG_LENGTH,1.0);
                glColor3f(1.0,0.8,0.2);
                drawSquare(1.0);
            glPopMatrix();
            //right foot on the right leg of the penguin
            glPushMatrix();
                glTranslatef(0.0,-LEG_LENGTH/2 + 3,0.0);
                glRotatef(right_foot_rot,0.0,0.0,1.0);
                glPushMatrix();
                    glTranslatef(-FOOT_WIDTH/2, 0.0f, 0.0f);
                    glScalef(FOOT_WIDTH,FOOT_LENGTH,1.0);
                    glColor3f(1.0,0.8,0.2);
                    drawSquare(1.0);
                glPopMatrix();
                glColor3f(1.0,1.0,1.0);
                drawCircle(3);
            glPopMatrix();
        glPopMatrix();
        glColor3f(1.0,1.0,1.0);
        drawCircle(3);
    glPopMatrix();
}

