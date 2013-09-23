/*
 * Source file for the sprite render model
 */

#include "d3re.h"
#include "mge/GameObject.h"

D3PrismRenderModel::D3PrismRenderModel(GameObject *parent, Box bxVolume) {
    m_bxVolume = bxVolume;
    m_pParent = parent;

    for(uint i = 0; i < 6; ++i) {
        m_aTextures[i] = 0;
    }
}


D3PrismRenderModel::~D3PrismRenderModel() {
}

void
D3PrismRenderModel::setTexture(int iFace, uint uiTexId) {
    m_aTextures[iFace] = uiTexId;
}

void
D3PrismRenderModel::render(RenderEngine *re) {
    Color worldColor = D3RE::get()->getWorldColor();
    Color ourColor = Color(worldColor); //Eventually there may be a mix done in here

    Point ptPos = getPosition();
    glTranslatef((int)(ptPos.x + m_bxVolume.x), (int)(ptPos.y + m_bxVolume.y), (int)(ptPos.z + m_bxVolume.z));

    glColor3f(ourColor.r / 255.f, ourColor.g / 255.f, ourColor.b / 255.f);

    // Top face (y = v.y)
    renderFace(
        m_aTextures[UP],    //Texture id to bind
        Point(         0.f, m_bxVolume.l,          0.f),
        Point(m_bxVolume.w, m_bxVolume.l,          0.f),
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(         0.f, m_bxVolume.l, m_bxVolume.h)
    );

    // Bottom face (y = 0.f)
    renderFace(
        m_aTextures[DOWN],
        Point(         0.f,          0.f,          0.f),
        Point(m_bxVolume.w,          0.f,          0.f),
        Point(m_bxVolume.w,          0.f, m_bxVolume.h),
        Point(         0.f,          0.f, m_bxVolume.h)
    );
    /*
    renderFace(
        m_aTextures[DOWN],
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h)
    );
    */

    // Front face  (z = v.z)
    renderFace(
        m_aTextures[SOUTH],
        Point(         0.f, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w,          0.f, m_bxVolume.h),
        Point(         0.f,          0.f, m_bxVolume.h)
    );

    // Back face (z = 0.f)
    renderFace(
        m_aTextures[NORTH],
        Point(         0.f, m_bxVolume.l,          0.f),
        Point(m_bxVolume.w, m_bxVolume.l,          0.f),
        Point(m_bxVolume.w,          0.f,          0.f),
        Point(         0.f,          0.f,          0.f)
    );

    // Left face (x = 0.f)
    renderFace(
        m_aTextures[WEST],
        Point(         0.f, m_bxVolume.l, m_bxVolume.h),
        Point(         0.f, m_bxVolume.l,          0.f),
        Point(         0.f,          0.f,          0.f),
        Point(         0.f,          0.f, m_bxVolume.h)
    );

    // Right face (x = 1.0f)
    renderFace(
        m_aTextures[EAST],
        Point(m_bxVolume.w, m_bxVolume.l, m_bxVolume.h),
        Point(m_bxVolume.w, m_bxVolume.l,          0.f),
        Point(m_bxVolume.w,          0.f,          0.f),
        Point(m_bxVolume.w,          0.f, m_bxVolume.h)
    );
}

void
D3PrismRenderModel::renderFace(uint texId, const Point &tl, const Point &tr, const Point &br, const Point &bl) {
    Image *tex = D3RE::get()->getImage(texId);
    if(tex == NULL) return;

    float fTexLeft, fTexTop, fTexRight, fTexBottom;

    //Tiled texture
    fTexLeft   = 0;
    fTexTop    = 0;
    fTexRight  = dist(tl, tr) / tex->w;
    fTexBottom = dist(tl, bl) / tex->h;

    glBindTexture(GL_TEXTURE_2D, tex->m_uiTexture);

    //Must be inside a glBegin() and glEnd() pair
    glBegin(GL_QUADS);
        glTexCoord2f(fTexLeft,  fTexTop);
        glVertex3f(tl.x, tl.y, tl.z);

        glTexCoord2f(fTexRight, fTexTop);
        glVertex3f(tr.x, tr.y, tr.z);

        glTexCoord2f(fTexRight, fTexBottom);
        glVertex3f(br.x, br.y, br.z);

        glTexCoord2f(fTexLeft,  fTexBottom);
        glVertex3f(bl.x, bl.y, bl.z);
    glEnd();
}

Rect
D3PrismRenderModel::getDrawArea() {
    Point ptPos = getPosition();
    return m_bxVolume + ptPos;
}

Point
D3PrismRenderModel::getPosition() {
    Point ptPos = Point();
    if(m_pParent != NULL) {
        ptPos = m_pParent->getPhysicsModel()->getPosition();
    }
    return ptPos;
}
