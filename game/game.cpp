
#include "game.h"

//SDL includes
#include <SDL.h>
#include "pgl.h"

//#include <SDL_mixer.h>
//#include <SDL_opengl.h>
//#include "SDL_image.h"
#include <fstream>

//Engine includes
#include "mge/ModularEngine.h"
#include "pwe/PartitionedWorldEngine.h"
//#include "ore/OrderedRenderEngine.h"
#include "d3re/d3re.h"
#include "bae/BasicAudioEngine.h"
#include "tpe/TimePhysicsEngine.h"

//Game includes
#include "game/game_defs.h"
#include "game/Player.h"
#include "game/world/SimplePhysicsObject.h"
#include "game/world/Wall.h"
#include "game/ObjectFactory.h"
#include "game/GameManager.h"

//Test includes
#include "mge/PixelMap.h"

using namespace std;

//Function prototypes
void buildWorld();

//Engine initialization, cleanup
WorldEngine   *createWorldEngine() {
    PWE::init();
    return PWE::get();
}

PhysicsEngine *createPhysicsEngine() {
    //TimePhysicsEngine::init();
    TimePhysicsEngine::init();
    return TimePhysicsEngine::get();

}

RenderEngine  *createRenderEngine() {
    D3RE::init();
    return D3RE::get();
}

AudioEngine   *createAudioEngine() {
    BasicAudioEngine::init();
    return BasicAudioEngine::get();
}

void cleanWorldEngine() {
    PWE::clean();
}

void cleanPhysicsEngine() {
    TimePhysicsEngine::clean();
}

void cleanRenderEngine() {
    D3RE::clean();
}

void cleanAudioEngine() {
    BasicAudioEngine::clean();
}

//SDL video flags
int getSDLVideoFlags() {
    return SDL_OPENGL | SDL_HWSURFACE;// | SDL_NOFRAME | SDL_FULLSCREEN;
}

void initWorld() {
    //Perform last-minute setup of the world engine
    PartitionedWorldEngine *we = PWE::get();
    we->setPhysicsEngine(TimePhysicsEngine::get());
    we->setRenderEngine(D3RE::get());

    ObjectFactory::init();
    GameManager::init();
    we->setManager(GameManager::get());

    registerClasses();

    ModularEngine *mge = ModularEngine::get();

    //Map inputs
    mge->mapInput(SDLK_w,     IN_NORTH);
    mge->mapInput(SDLK_d,     IN_EAST);
    mge->mapInput(SDLK_s,     IN_SOUTH);
    mge->mapInput(SDLK_a,     IN_WEST);

    mge->mapInput(SDLK_UP,    IN_NORTH);
    mge->mapInput(SDLK_RIGHT, IN_EAST);
    mge->mapInput(SDLK_DOWN,  IN_SOUTH);
    mge->mapInput(SDLK_LEFT,  IN_WEST);

    mge->mapInput(SDLK_SPACE, IN_CAST);
    mge->mapInput(SDLK_LSHIFT, IN_SHIFT);
    mge->mapInput(SDLK_LCTRL, IN_CTRL);
    mge->mapInput(SDL_BUTTON_LEFT, IN_SELECT);
    mge->mapInput(SDLK_h,     IN_BREAK);
    mge->mapInput(SDLK_F1, IN_TOGGLE_DEBUG_MODE);

    //Load image resources
    ObjectFactory::get()->read("res/game.info");

    PWE::get()->setCurrentArea(0);
    /*
    D3RE::get()->createImage(IMG_PLAYER,   "res/Magus.png", 8, 4);
    D3RE::get()->createImage(IMG_FONT,     "res/gui/font.png", 26, 3);
    D3RE::get()->createImage(IMG_BLOCK,    "res/world/block.png");
    D3RE::get()->createImage(IMG_WALL_TOP, "res/world/wallTop.png");
    D3RE::get()->createImage(IMG_WALL_BOTTOM, "res/world/wallBottom.png");
    D3RE::get()->createImage(IMG_WALL_SIDE, "res/world/wallSide.png");
    */


    //Load audio resources
    BAE::get()->loadSound(AUD_STEP, "res/audio/step.wav");

    //Other singleton initializations
    TextRenderer::init();
/*
    //Initialize world areas
    for(uint ui = 0; ui < GM_NUM_AREAS; ++ui) {
        we->generateArea(ui);
    }
*/


    //Mouse test
    D3RE::get()->setMouseAnim(0);
/*
    //PixelMap test
    PixelMap *pxMap = new PixelMap("res/tests/pixMapColorTest.png", 0);
    for(uint y = 0; y < pxMap->m_iH; ++y) {
        for(uint x = 0; x < pxMap->m_iW; ++x) {
            Color cr = pxMap->m_pData[x][y];
            printf("(%2x,%2x,%2x) ", cr.r, cr.g, cr.b);
        }
        printf("\n");
    }
    delete pxMap;
*/

    D3RE::get()->setBackgroundColor(Color(0x9a,0xd7,0xfb));
    D3RE::get()->hideRealMouse();

    //buildWorld();
}


void cleanWorld() {
    ObjectFactory::clean();
    GameManager::clean();
}

void buildRoom(Box bxVol) {
    PartitionedWorldEngine *we = PWE::get();

    //"Mins" are inside the wall, "maxes" are outside the wall
    float minEast = bxVol.x,
          maxEast = minEast - WORLD_TILE_SIZE;
    float minWest = bxVol.x + bxVol.w,
          maxWest = minWest + WORLD_TILE_SIZE;
    float minNorth = bxVol.z,
          maxNorth = minNorth - WORLD_TILE_SIZE;
    float minSouth = bxVol.z + bxVol.l,
          maxSouth = minSouth + WORLD_TILE_SIZE;
    float minDown = bxVol.y,
          maxDown = minDown - WORLD_TILE_SIZE;
    float height = bxVol.h,
          width = WORLD_TILE_SIZE;


    Wall *wallNorth = new Wall(we->genId(), IMG_NONE, IMG_NONE, IMG_WALL_SIDE,
                               Box(minEast, minDown, maxNorth, minWest - minEast, height, width), WALL_SOUTH);
    Wall *wallWest  = new Wall(we->genId(), IMG_NONE, IMG_NONE, IMG_WALL_SIDE,
                               Box(maxEast, minDown, minNorth, width, height, minSouth - minNorth), WALL_EAST);
    Wall *wallSouth = new Wall(we->genId(), IMG_NONE, IMG_NONE, IMG_WALL_SIDE,
                               Box(minEast, minDown, minSouth, minWest - minEast, height, width), WALL_NORTH);
    Wall *wallEast  = new Wall(we->genId(), IMG_NONE, IMG_NONE, IMG_WALL_SIDE,
                               Box(minWest, minDown, minNorth, width, height, minSouth - minNorth), WALL_WEST);
    Wall *wallFloor = new Wall(we->genId(), IMG_WALL_TOP, IMG_NONE, IMG_NONE,
                               Box(minEast, maxDown, minNorth, minWest - minEast, minDown - maxDown, minSouth - minNorth), WALL_UP);
    we->add(wallNorth);
    we->add(wallSouth);
    we->add(wallEast);
    we->add(wallWest);
    we->add(wallFloor);
}

void buildWorld() {
    PartitionedWorldEngine *we = PWE::get();

    //Initialize the main game area
    we->setCurrentArea(GM_MAIN_GAME);
    we->setEffectiveArea(GM_MAIN_GAME); //Make sure objects actually get added here

    buildRoom(Box(-128, 0, -256, 256, WORLD_TILE_SIZE * 2, 512));

    SimplePhysicsObject *block = new SimplePhysicsObject(we->genId(), IMG_BLOCK, Box(-32,0,0,32,32,32));
    we->add(block);

/*
    Wall *wall0 = new Wall(we->genId(), IMG_WALL_TOP, IMG_WALL_BOTTOM, IMG_WALL_SIDE,
                          Box(50, -32, 50, 128, 32, 128), WALL_SOUTH | WALL_NORTH | WALL_WEST | WALL_EAST | WALL_DOWN);
    wall0->setColor(Color(0, 0, 0xFF));
    we->add(wall0);
*/

    Player *player = new Player(we->genId(), Point());
    we->add(player);
}

