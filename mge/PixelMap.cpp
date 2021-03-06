/*
 * PixelMap.cpp
 */

#include "PixelMap.h"

//copyPixel courtesy of http://sdl.beuc.net/sdl.wiki/Pixel_Access, wih some modifications
void copyPixel(SDL_Surface *surface, int x, int y, Color *cr)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    Uint8 r = 0, g = 0, b = 0;
    switch(bpp) {
    case 1:
        SDL_GetRGB(*p, surface->format, &r, &g, &b);
        break;

    case 2:
        SDL_GetRGB(*(Uint16 *)p, surface->format, &r, &g, &b);
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            SDL_GetRGB(p[0] << 16 | p[1] << 8 | p[2], surface->format, &r, &g, &b);
        else
            SDL_GetRGB(p[0] | p[1] << 8 | p[2] << 16, surface->format, &r, &g, &b);
        break;

    case 4:
        SDL_GetRGB(*(Uint32 *)p, surface->format, &r, &g, &b);
        break;

    default:
        break;
    }
    cr->r = r;
    cr->g = g;
    cr->b = b;
}

//===============Constructor/Destructor for the PixelMap structure================

/*
 * Creates an OpenGL texture from the provided bitmap
 * Code thanks to http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL
 * (variable names differ)
 */
PixelMap::PixelMap(const std::string &sFileName, uint uiId) {
    //Load image onto a surface
	SDL_Surface *pSurface = /*SDL_LoadBMP*/IMG_Load(sFileName.c_str()/*sFileName.c_str()*/);

	if( pSurface ) {
		// Check that the image's width is a power of 2
		if ( (pSurface->w & (pSurface->w - 1)) != 0 ) {
			std::cout << "warning: " << sFileName << "'s width is not a power of 2\n";
		}

		// Also check if the height is a power of 2
		if ( (pSurface->h & (pSurface->h - 1)) != 0 ) {
			std::cout << "warning: " << sFileName << "'s height is not a power of 2\n";
		}

		//Store the original width and height of the image
		this->m_iW = pSurface->w;
		this->m_iH = pSurface->h;

        // get the number of channels in the SDL surface
        /*
        int iNumColors = pSurface->format->BytesPerPixel;
        if (iNumColors == 4)     // contains an alpha channel
        {
            if (pSurface->format->Rmask == 0x000000ff)
                m_eTextureFormat = GL_RGBA;
            else
                m_eTextureFormat = GL_BGRA;
        } else if (iNumColors == 3)     // no alpha channel
        {
            if (pSurface->format->Rmask == 0x000000ff)
                m_eTextureFormat = GL_RGB;
            else
                m_eTextureFormat = GL_BGR;
        } else {
            printf("warning: the image is not truecolor..  this will probably break\n");
            // this error should not go unhandled
        }
        */

        //Allocate and fill out the pixel data
        m_pData = (Color**)malloc(sizeof(Color*) * m_iW);
        for(uint x = 0; x < m_iW; ++x) {
            m_pData[x] = (Color*)malloc(sizeof(Color) * m_iH);
            for(uint y = 0; y < m_iH; ++y) {
                //Fill each pixel
                copyPixel(pSurface, x, y, &m_pData[x][y]);
            }
        }

	} else {
		std::cout << "SDL could not load " << sFileName << ": " << SDL_GetError();
		exit(-1);
	}

	// Free the SDL_pSurface only if it was successfully created
	if ( pSurface ) {
		SDL_FreeSurface( pSurface );
	}

	//Initialize other members
	m_sImageFileName = sFileName;
	m_uiId = uiId;
}

PixelMap::PixelMap(uint w, uint h, uint uiId) {
    m_iW = w;
    m_iH = h;

	m_sImageFileName = "";
	m_uiId = uiId;

    m_pData = (Color**)malloc(sizeof(Color*) * m_iW);
    for(uint x = 0; x < m_iW; ++x) {
        m_pData[x] = (Color*)malloc(sizeof(Color) * m_iH);
        for(uint y = 0; y < m_iH; ++y) {
            //Fill each pixel
            m_pData[x][y] = Color();
        }
    }
}

/*
 * Destructor:  Destroys the texture information.
 */
PixelMap::~PixelMap() {
    for(uint x = 0; x < m_iW; ++x) {
        free(m_pData[x]);
    }
    free(m_pData);
}
