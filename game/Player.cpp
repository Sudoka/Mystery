#include "Player.h"
#include "bae/BasicAudioEngine.h"

Player::Player(uint uiId, const Point &ptPos) {
    Image *img = D3RE::get()->getImage(IMG_PLAYER);
    int w = img->w / img->m_iNumFramesW,
        h = img->h / img->m_iNumFramesH;
    m_uiId = uiId;
    m_uiFlags = 0;

    Rect rcDrawArea = Rect(-w / 2, -h / 2, w, h);
    Box bxVolume = Box(ptPos.x - w / 4, ptPos.y, ptPos.z - h / 4, w / 2, h, h / 2);
    m_pPhysicsModel = new TimePhysicsModel(bxVolume);
    m_pRenderModel  = new D3SpriteRenderModel(this, img, rcDrawArea);

    dx = dy = 0;
    state = timer = 0;
    m_iDirection = SOUTH;

    PWE::get()->addListener(this, ON_BUTTON_INPUT);
}

Player::~Player() {
    delete m_pPhysicsModel;
    delete m_pRenderModel;
}


GameObject*
Player::read(const boost::property_tree::ptree &pt, const std::string &keyBase) {
    uint id = pt.get(keyBase + ".id", 0);
    float x = pt.get(keyBase + ".pos.x", 0.f);
    float y = pt.get(keyBase + ".pos.y", 0.f);
    float z = pt.get(keyBase + ".pos.z", 0.f);

    //Put state information here

    return new Player(id, Point(x,y,z));
}

void
Player::write(boost::property_tree::ptree &pt, const std::string &keyBase) {
    std::string key = keyBase + "Player";
    key += m_uiId;
    Point ptPos = m_pPhysicsModel->getPosition();
    pt.put(key + ".id", m_uiId);
    pt.get(keyBase + ".pos.x", 0.f);
    pt.get(keyBase + ".pos.y", 0.f);
    pt.get(keyBase + ".pos.z", 0.f);
}

bool Player::update(uint time) {
    Point mov = Point(dx,0,dy);
    mov.normalize();
    m_pPhysicsModel->applyForce(mov);

    //Camera adjustment
    D3RE::get()->adjustCamAngle(m_fDeltaPitch);
    D3RE::get()->adjustCamDist(m_fDeltaZoom);

    if(dx > 0 && dy == 0) {
        m_iDirection = EAST;
    } else if(dx < 0 && dy == 0) {
        m_iDirection = WEST;
    } else if(dx == 0 && dy > 0) {
        m_iDirection = SOUTH;
    } else if(dx == 0 && dy < 0) {
        m_iDirection = NORTH;
    }
    m_pRenderModel->setFrameW(m_iDirection);

    if(dx == 0 && dy == 0) {
        m_pRenderModel->setFrameH(0);
        timer = -1;
    } else {
        if(timer < 0) {
            timer = 20;
            state = ((state) % 4) + 1;
            m_pRenderModel->setFrameH(state);
        } else {
            --timer;
        }
    }
    Point ptPos = m_pPhysicsModel->getPosition();
    D3RE::get()->moveScreenTo(ptPos);

    return false;
}


void Player::callBack(uint cID, void *data, EventID id) {
    switch(id) {
    case ON_BUTTON_INPUT:
        handleButton((InputData*)data);
        break;
    case ON_COLLISION:
        handleCollision((HandleCollisionData*)data);
        break;
    default:
        break;
    }
}

void Player::handleButton(InputData* data) {
    if(data->getInputState(IN_CTRL)) {
        if(data->getInputState(IN_NORTH)) {
            m_fDeltaPitch = M_PI / 100;
        } else if(data->getInputState(IN_SOUTH)) {
            m_fDeltaPitch = -M_PI / 100;
        } else {
            m_fDeltaPitch = 0.f;
        }
        dy = 0;

        m_fDeltaZoom = 0.f;
    } else if(data->getInputState(IN_SHIFT)) {
        if(data->getInputState(IN_NORTH)) {
            m_fDeltaZoom = -1.0f;
        } else if(data->getInputState(IN_SOUTH)) {
            m_fDeltaZoom = 1.0f;
        } else {
            m_fDeltaZoom = 0.f;
        }
        dy = 0;

        m_fDeltaPitch = 0.f;
    } else {
        if(data->getInputState(IN_NORTH)) {
            dy = -1;
        } else if(data->getInputState(IN_SOUTH)) {
            dy = 1;
        } else {
            dy = 0;
        }
        m_fDeltaZoom = m_fDeltaPitch = 0.f;
    }

    if(data->getInputState(IN_WEST)) {
        dx = -1;
    } else if(data->getInputState(IN_EAST)) {
        dx = 1;
    } else {
        dx = 0;
    }
}

void Player::handleCollision(HandleCollisionData *data) {

}


