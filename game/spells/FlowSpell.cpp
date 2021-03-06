#include "FlowSpell.h"
#include "ElementalVolume.h"
#include "game/spells/ForceField.h"

using namespace std;

#define MIN_NUM_POINTS 3
#define MAX_NUM_POINTS 5

FlowSpell::FlowSpell(int duration, float magnitude)
{
    m_ev = NULL;
    m_iTimer = duration;
    m_fMagnitude = magnitude;
    m_eState = FS_STATE_AWAITING_POINTS;
    m_bWasActivated = false;
}

FlowSpell::~FlowSpell()
{
    if(m_bWasActivated) {
        vector<PointIdPair>::iterator iter;
        for(iter = m_vForcePoints.begin(); iter != m_vForcePoints.end(); ++iter) {
            m_ev->removeForceField(iter->id);
        }
    }
    m_vForcePoints.clear();
}

void
FlowSpell::addPoint(ElementalVolume *ev, const Point &pt) {
    switch(m_eState) {
    case FS_STATE_AWAITING_POINTS:
        if(m_ev == NULL) {
            m_ev = ev;
        } else if(m_ev != ev) {
            m_eState = FS_STATE_INVALID;
        }
        m_vForcePoints.push_back(PointIdPair(pt));
        if(m_vForcePoints.size() >= MIN_NUM_POINTS) {
            m_eState = FS_STATE_READY;
        }
        break;
    case FS_STATE_READY:
        if(m_ev == NULL) {
            m_ev = ev;
        } else if(m_ev != ev) {
            m_eState = FS_STATE_INVALID;
        }
        m_vForcePoints.push_back(PointIdPair(pt));
        if(m_vForcePoints.size() > MAX_NUM_POINTS) {
            m_eState = FS_STATE_INVALID; //Tried to add too many points!
        }
        break;
    case FS_STATE_ACTIVATED:
        m_eState = FS_STATE_INVALID; //Tried to add more points!
        break;
    case FS_STATE_INVALID:
        break;
    default:
        break;
    }
}

bool
FlowSpell::activate() {
    if(m_eState == FS_STATE_READY) {
        if(m_ev == NULL) {
            m_eState = FS_STATE_INVALID;
            return false;
        }
        m_eState = FS_STATE_ACTIVATED;
        m_bWasActivated = true;

        //Create a cycle of line force fields
        for(uint i = 0, j = 1; i < m_vForcePoints.size(); ++i, j++) {
            if(j >= m_vForcePoints.size()) {
                j = 0;
            }
            PointIdPair *p0 = &m_vForcePoints[i];
            PointIdPair *p1 = &m_vForcePoints[j];

            p0->id = m_ev->addForceField(new LineForceField(p0->pt, p1->pt, 0.8f));
        }
        return true;
    }
    return false;
}

SpellState
FlowSpell::getStatus() {
    switch(m_eState) {
    case FS_STATE_AWAITING_POINTS:
        return SPELL_WAITING;
    case FS_STATE_READY:
        return SPELL_READY;
    case FS_STATE_ACTIVATED:
        return SPELL_ACTIVE;
    case FS_STATE_INVALID:
    default:
        return SPELL_INVALID;
    }
}

void
FlowSpell::update() {
    if(m_eState == FS_STATE_ACTIVATED) {
        if(--m_iTimer < 0) {
            m_eState = FS_STATE_INVALID;
        }
    }
}

