#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

using namespace std;

#define MIN_LINEAR 0
#define MIN_NEAREST 1
#define MAG_LINEAR 2
#define MAG_NEAREST 3
#define S_REPEAT 4
#define S_CLAMP 5
#define T_REPEAT 6
#define T_CLAMP 7
#define ENV_MODULATE 8
#define ENV_REPLACE 9


void menuCallback(int);
void setCamera(void);
void drawScene(void);
void drawObjects(void);

static int win = 0;
static GLfloat whiteColor[3] = {1.0f, 1.0f, 1.0f};
static GLfloat blackColor[3] = {0.0f, 0.0f, 0.0f};
static GLfloat lightPosition[4] = {0.0f, 0.0f, 0.0f, 1.0f};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glPushMatrix();
    glTranslatef(0.0f, 10.0f, 0.0f);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glPopMatrix();
    
    setCamera();
    
    drawObjects();
    drawScene();
    
    glutSwapBuffers();
}


void idle()
{
    glutPostRedisplay();
}

void setCamera()
{
    glTranslatef(0.0f, 0.0f, -1.2f);
    glRotatef(0.0f, 0.0f, 0.0f, 0.0f);
}

void makeMenu()
{
}

void menuCallback(int option)
{
}

void CreateGlutWindow()
{
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition (10, 10);
    glutInitWindowSize (600, 600);
    win = glutCreateWindow ("N-Body Simulation");
}

void CreateGlutCallbacks()
{
    glutDisplayFunc(display);
    glutIdleFunc(idle);
}

void InitOpenGL()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteColor);
    glEnable(GL_LIGHT0);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void ExitGlut()
{
    glutDestroyWindow(win);
    exit(0);
}








FILE *pFile;
int nL,nN;
float *mass;

int main (int argc, char **argv)
{
    
    
    pFile = fopen ("/Users/spicydog/Development/Java/CPE610/data/nbody/output.txt","r");
    
    fscanf(pFile, "%d %d", &nL, &nN);
    
    
    mass = (float*) malloc(nN*sizeof(float));
    
    for (int i=0; i<nN; i++) {
        fscanf(pFile, "%f ", &mass[i]);
    }
    
    
    glutInit(&argc, argv);
    CreateGlutWindow();
    CreateGlutCallbacks();
    InitOpenGL();
    
    
    glutMainLoop();
    
    ExitGlut();
    return 0;		
}









void drawScene()
{
    // Draw Floor
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, blackColor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(+1.0f, -1.0f, 0.0f);
    glVertex3f(+1.0f, +1.0f, 0.0f);
    glVertex3f(-1.0f, +1.0f, 0.0f);
    glEnd();
    glPopMatrix();
}

#define PI 3.14159265358979323846
#define DEGREE_TO_RAD 0.017453293


void drawCircle(float x, float y, float r, GLfloat *color);


int l = 0;
double avgFps = 0;
static GLfloat particleColor[7][3] = {  {0.0f, 0.0f, 1.0f},
                                        {0.0f, 1.0f, 0.0f},
                                        {1.0f, 0.0f, 0.0f},
                                        {0.0f, 1.0f, 1.0f},
                                        {1.0f, 1.0f, 0.0f},
                                        {1.0f, 0.0f, 1.0f},
                                        {1.0f, 1.0f, 1.0f}};

void drawObjects()
{
    float x;
    float y;
    float m;
    
    clock_t start = clock();
    clock_t end;
    
    if(l++ < nL) {
        
        
        for (int i=0; i<nN; i++) {
            fscanf(pFile, "%f,%f ", &x,&y);
            x = x/200 - 0.8;
            y = y/200 - 0.8;
            m = mass[i]/15000;
            drawCircle(x,y,m,particleColor[i%7]);
            
        }
        
        end = clock();
        double usedClock = (double)(end-start);
        start = end;
        
        double sec = usedClock/CLOCKS_PER_SEC;
        double fps = 1/sec;
        avgFps += fps;
        
        printf("time:%d\t\t clock: %lu\t\t  uSec: %.4f\t\t fps:%.2f\t\tAVG:%.2f\n",
                     l,         end-start,        sec*1000,    fps,      avgFps/l);
        
        
        
    } else {
        printf("End of simulation\n");
        ExitGlut();
    }
    
    
}


void drawCircle(float x, float y, float r, GLfloat *color)
{
    // Draw Floor
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    
    //float DEGREE_TO_RAD = PI/180;
    int N_IN_DEGREE = 1;
    int M_IN_DEGREE = 360;
    int nCount = 0;
    float stVertexArray[2*360];
    
    
    for( int nR = N_IN_DEGREE; nR < M_IN_DEGREE; nR++ )
    {
        float fX = sin(nR * DEGREE_TO_RAD)*r;
        float fY = cos(nR * DEGREE_TO_RAD)*r;
        stVertexArray[nCount*2] = fX + x;
        stVertexArray[nCount*2 + 1] = fY - y;
        nCount++;
    }
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 0, stVertexArray );
    glDrawArrays( GL_LINE_LOOP, 0, nCount );
}

