/*
 * Source file for the D3RenderEngine
 */

#include "d3re.h"

#include "SDL.h"
#include <gl/gl.h>
#include <gl/glu.h>

#include "mge/GameObject.h"
#include "game/game_defs.h"

using namespace std;

#define CAM_DIST 200.f
#define CAM_ANGLE (9 * M_PI / 32)

//static members
D3RenderEngine *D3RenderEngine::re;

D3RenderEngine::D3RenderEngine() {
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );	//This line used to come directly after SDL_Init()

	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_GREATER, 0.9f);

    //Set the initial window size
    resize(SCREEN_WIDTH, SCREEN_HEIGHT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections

    resetCamera();

    //Other values

    m_fCamDist = CAM_DIST;
    m_fCamAngle = CAM_ANGLE;
    m_ptPos = Point();
    updateCamPos();
    m_crWorld = Color(0xFF, 0xFF, 0xFF);
}

D3RenderEngine::~D3RenderEngine() {
}

void
D3RenderEngine::render() {
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    for(map<float, GameObject *>::iterator it = m_mObjsOnScreen.begin();
            it != m_mObjsOnScreen.end(); ++it) {
        resetCamera();
        it->second->getRenderModel()->render(this);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    SDL_GL_SwapBuffers();   //Should probably be done by the render engine
}

void
D3RenderEngine::manageObjOnScreen(GameObject *obj) {
    //TODO: Optimize!  For now this just adds everything
    bool bIntersects = true; //rcIntersects(obj->getRenderModel()->getDrawArea(), m_rcScreenArea);
    if(bIntersects && !obj->getFlag(D3RE_ON_SCREEN)) {
        addInOrder(obj);
    } else if(bIntersects) {
        resort(obj);
    } else if(obj->getFlag(D3RE_ON_SCREEN)) {
        remove(obj);
    }
}

void
D3RenderEngine::addInOrder(GameObject *obj) {
    float distance = -dist(obj->getPhysicsModel()->getPosition(), m_ptCamPos);
    m_mObjsOnScreen[distance] = obj;
    obj->setFlag(D3RE_ON_SCREEN, true);
}

void
D3RenderEngine::resort(GameObject *obj) {
    //TODO: implement an actual resorting algorithm!
    remove(obj);
    addInOrder(obj);
}

void
D3RenderEngine::remove(GameObject *obj) {
    for(map<float, GameObject *>::iterator it = m_mObjsOnScreen.begin();
            it != m_mObjsOnScreen.end(); ++it) {
        if(obj->getID() == it->second->getID()) {
            m_mObjsOnScreen.erase(it);
            obj->setFlag(D3RE_ON_SCREEN, false);
            return;
        }
    }
}

void
D3RenderEngine::clearScreen() {
    for(map<float, GameObject *>::iterator it = m_mObjsOnScreen.begin();
            it != m_mObjsOnScreen.end(); ++it) {
        it->second->setFlag(D3RE_ON_SCREEN, false);
    }
    m_mObjsOnScreen.clear();
}

void
D3RenderEngine::moveScreenTo(Point pt) {
    m_ptPos = pt;
    updateCamPos();
}

void
D3RenderEngine::moveScreenBy(Point pt) {
    m_ptPos += pt;
    updateCamPos();
}

Image *
D3RenderEngine::createImage(uint id, const char *name, int numFramesH, int numFramesW) {
    if(id < m_vImages.size() && m_vImages[id] != NULL) {
        delete m_vImages[id];
    }
    while(id >= m_vImages.size()) {
        m_vImages.push_back(NULL);
    }
    m_vImages[id] = new Image(name, id, numFramesH, numFramesW);
    return m_vImages[id];
}

Image *
D3RenderEngine::getImage(uint id) {
    if(id < m_vImages.size()) {
        return m_vImages[id];
    }
    //else
    return NULL;
}

void
D3RenderEngine::freeImage(uint id) {
    if(id < m_vImages.size() && m_vImages[id] != NULL) {
        delete m_vImages[id];
        m_vImages[id] = NULL;
    }
}

void
D3RenderEngine::adjustCamDist(float delta) {
    m_fCamDist += delta;
    updateCamPos();
}

void
D3RenderEngine::adjustCamAngle(float delta) {
    m_fCamAngle += delta;
    updateCamPos();
}

void
D3RenderEngine::resetCamera() {
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    gluLookAt(m_ptCamPos.x, m_ptCamPos.y, m_ptCamPos.z, //Camera position
              m_ptPos.x,    m_ptPos.y,    m_ptPos.z,             //Look at this coord
              0,            1,            -1);                   //Up vector
}

void D3RenderEngine::resize(uint width, uint height) {
    // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 1.f, 1024.0f);
}

void
D3RenderEngine::updateCamPos() {
    m_ptCamPos = Point(m_ptPos.x,
                       m_ptPos.y + m_fCamDist * sin(m_fCamAngle),
                       m_ptPos.z + m_fCamDist * cos(m_fCamAngle));
}
