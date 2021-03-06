/*
 * PixelMap.h
 * Header file for the PixelMap structure
 * (Images are still loaded the same across all render models, so PixelMap remains
 * in STD)
 */

#ifndef PIXEL_MAP_H
#define PIXEL_MAP_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"
#include "mge/defs.h"
#include <string>
#include <iostream>

struct PixelMap {
public:
	//Constructor/Destructor
//	PixelMap(std::string &sFileName, char cID, int iNumFramesH = 1, int iNumFramesW = 1);
	PixelMap(const std::string &sFileName, uint uiId);
	PixelMap(uint w, uint h, uint uiId);
	virtual ~PixelMap();

	//Members
	std::string m_sImageFileName;
	int m_iW;
	int m_iH;
	uint m_uiId;
    Color **m_pData;
};


#endif
