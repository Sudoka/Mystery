/*
 * The D3RenderEngine
 */

#ifndef D3_RENDER_ENGINE_H
#define D3_RENDER_ENGINE_H

#include <list>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include "mge/RenderEngine.h"
#include "mge/Image.h"
#include "mge/defs.h"
#include "mge/Event.h"

#include "ContainerRenderModel.h"

class D3HudRenderModel;
class D3SpriteRenderModel;
class D3DummyObject;

enum RenderFlags {
    D3RE_ON_SCREEN = RENDER_FLAGS_BEGIN,
    D3RE_INVISIBLE,
    D3RE_NUM_FLAGS
};

class D3RenderEngine : public RenderEngine, public Listener {
public:
    static void init()  { re = new D3RenderEngine(); }
    static void clean() { delete re; }
    static D3RenderEngine *get() { return re; }

    virtual void  render();
    virtual Point getRenderOffset() { return m_ptPos; }
    virtual bool  screenHasMoved() { return true; }
    virtual void  manageObjOnScreen(GameObject *obj);
    virtual void  remove(GameObject *obj);
    virtual void  clearScreen();
    virtual void  moveScreenTo(Point pt);
    void moveScreenBy(Point pt);

    Image *createImage(uint id, const char *name, int numFramesH = 1, int numFramesW = 1);
    Image *getImage(uint id);
    void   freeImage(uint id);

    const Color& getWorldColor() const { return m_crWorld; }
    float getColorWeight() { return m_fColorWeight; }
    void setWorldColor(const Color &cr) { m_crWorld = cr; }
    void setColorWeight(float fWeight) { m_fColorWeight = fWeight; }
    void setBackgroundColor(const Color &cr);

    void adjustCamDist(float delta);
    void adjustCamAngle(float delta);

    void prepCamera();
    void prepHud();
    void resize(uint width, uint height);
    Point getCameraPosition() { return m_ptCamPos; }

    void drawBox(const Box &bxVolume, const Color &cr = Color(0xFF, 0xFF, 0xFF));

    void drawCircle(const Point &ptCenter, float radius, const Color &cr = Color(0xFF, 0xFF, 0xFF));


    ContainerRenderModel *getHudContainer() { return m_pHudContainer; }

    uint getScreenWidth() { return m_uiWidth; }
    uint getScreenHeight() { return m_uiHeight; }

    void write(boost::property_tree::ptree &pt, const std::string &keyBase);
    void read(boost::property_tree::ptree &pt, const std::string &keyBase);

    uint getNumImages() { return m_vImages.size(); }

	virtual void callBack(uint uiEventHandlerId, void *data, uint uiEventId);
	virtual uint getId() { return ID_RENDER_ENGINE; }

	Point getMousePos() { return m_ptMouseInWorld; }

	void setDrawCollisions(bool enable) { m_bDrawCollisions = enable; }
	bool getDrawCollisions() { return m_bDrawCollisions; }
    void setMouseAnim(uint uiAnim);

    void showRealMouse();
    void hideRealMouse();

private:
    D3RenderEngine();
    virtual ~D3RenderEngine();

    void enableCameraMode();
    void enableGuiMode();

    void updateCamPos();
    void addInOrder(GameObject *obj);
    void resort(GameObject *obj);

    bool comesBefore(GameObject *obj1, GameObject *obj2);
    void updateMousePos(int x, int y);

    static D3RenderEngine *re;
    Point m_ptPos, m_ptCamPos;
    Color m_crWorld;
    float m_fColorWeight;
    float m_fCamDist, m_fCamAngle;
    int m_iMouseX, m_iMouseY;
    uint m_uiWidth, m_uiHeight;

    std::list<GameObject *> m_lsObjsOnScreen;
    std::vector<Image*> m_vImages;
    ContainerRenderModel *m_pHudContainer;
    bool m_bGuiMode;
    bool m_bDrawCollisions;
    bool m_bDrawRealMouse;

    //Mouse animations
    uint m_uiMouseFrame, m_uiMouseTimer;
    D3SpriteRenderModel *m_pMouseModel;
    D3DummyObject *m_pDummyMouseObj;

    Point m_ptMouseInWorld;
    Vec3f m_v3MouseRay;
};

typedef D3RenderEngine D3RE;

#endif
