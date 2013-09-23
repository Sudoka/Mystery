/*
 * Source file for the Hud render model
 */

#include "d3re.h"
#include "mge/GameObject.h"

D3HudRenderModel::D3HudRenderModel(GameObject *parent, Image *img, Rect rcArea) {

    m_pImage = img;
    m_rcDrawArea = rcArea;

    m_iFrameW = 0;
    m_iFrameH = 0;
    m_iRepsW = 1;
    m_iRepsH = 1;

    m_crColor = Color(0xFF, 0xFF, 0xFF);

    m_pParent = parent;
}

D3HudRenderModel::~D3HudRenderModel() {
}

void
D3HudRenderModel::render(RenderEngine *re) {
    D3RE::get()->prepHud();
    Color worldColor = D3RE::get()->getWorldColor();
    Color ourColor = mix(2, &worldColor, &m_crColor);

    //Render engine is responsible for resetting the camera
    float fTexLeft   = m_iFrameW * 1.0F / m_pImage->m_iNumFramesW,
          fTexTop    = m_iFrameH * 1.0F / m_pImage->m_iNumFramesH,
          fTexRight  = m_iFrameW * 1.0F / m_pImage->m_iNumFramesW + m_iRepsW * 1.0F / m_pImage->m_iNumFramesW,
          fTexBottom = m_iFrameH * 1.0F / m_pImage->m_iNumFramesH + m_iRepsH * 1.0F / m_pImage->m_iNumFramesH;

    Point ptPos = getPosition();
    //glTranslatef((int)(ptPos.x + m_rcDrawArea.x), (int)(ptPos.y + m_rcDrawArea.y), (int)(ptPos.z));

    //Bind the texture to which subsequent calls refer to
    glBindTexture( GL_TEXTURE_2D, m_pImage->m_uiTexture );
    //glDepthMask(GL_FALSE);
    glBegin(GL_QUADS);
        glColor3f(ourColor.r / 255.f, ourColor.g / 255.f, ourColor.b / 255.f);
        //Top-left vertex (corner)
        glTexCoord2f(fTexLeft, fTexTop);
        glVertex3f(m_rcDrawArea.x, m_rcDrawArea.y, 0.f);

        //Top-right vertex (corner)
        glTexCoord2f(fTexRight, fTexTop);
        glVertex3f(m_rcDrawArea.x + m_rcDrawArea.w, m_rcDrawArea.y, 0.f);

        //Bottom-right vertex (corner)
        glTexCoord2f(fTexRight, fTexBottom);
        glVertex3f(m_rcDrawArea.x + m_rcDrawArea.w, m_rcDrawArea.y + m_rcDrawArea.h, 0.f);

        //Bottom-left vertex (corner)
        glTexCoord2f(fTexLeft, fTexBottom);
        glVertex3f(m_rcDrawArea.x, m_rcDrawArea.y + m_rcDrawArea.h, 0.f);
    glEnd();
    //glDepthMask(GL_TRUE);
}

Rect
D3HudRenderModel::getDrawArea() {
    Point ptPos = getPosition();
    return Rect(ptPos.x + m_rcDrawArea.x, ptPos.y + m_rcDrawArea.y, m_rcDrawArea.w, m_rcDrawArea.h);
}

Point
D3HudRenderModel::getPosition() {
    Point ptPos = Point();
    if(m_pParent != NULL) {
        ptPos = m_pParent->getPhysicsModel()->getPosition();
    }
    return ptPos;
}
