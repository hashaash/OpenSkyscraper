/*
 *  sprite.cpp
 *  OpenSkyscraper
 *
 *  Created by Fabian Schuiki on 30.07.10.
 *  Copyright 2010 Axamblis. All rights reserved.
 *
 */

#include "sprite.h"

using namespace OSS;


//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Initialization
//----------------------------------------------------------------------------------------------------

Sprite::Sprite() : CoreObject()
{
	color = (color4d){1, 1, 1, 1};
	hidden = false;
	
	texture = NULL;
	textureMode = Sprite::kRectTextureMode;
	textureRect = rectd(0, 0, 1, 1);
	textureScale = 1;
	autoTexRectX = false;
	autoTexRectY = false;
	autoTexRelativeX = false;
	autoTexRelativeY = false;
}

Sprite::~Sprite()
{
}


//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Basic
//----------------------------------------------------------------------------------------------------

const rectd & Sprite::getRect()
{
	return rect;
}

void Sprite::setRect(const rectd & rect)
{
	this->rect = rect;
}

const color4d & Sprite::getColor()
{
	return color;
}

void Sprite::setColor(const color4d & color)
{
	this->color = color;
}

bool Sprite::isHidden()
{
	return hidden;
}

void Sprite::setHidden(bool hidden)
{
	this->hidden = hidden;
}





//----------------------------------------------------------------------------------------------------
#pragma mark -
#pragma mark Drawing
//----------------------------------------------------------------------------------------------------

void Sprite::draw(rectd visibleRect)
{
	if (isHidden()) return;
	
	//Set the color
	glColor4dv(color.v);
	
	//Bind the texture if there's one
	if (texture)
		texture->bind();
	else
		Texture::unbind();
	
	//Autogenerate the texture rect where necessary
	autogenerateTextureRect();
	
	//Perform the drawing according to the texture mode
	if (texture) {
		switch (textureMode) {
			case kRepeatTextureMode: {
				/*if (autoTexRelativeX) {
					sqrt(9);
				}*/
				
				//Move the texture rect into a positive coordinate space to simplify the rest of the process
				rectd transformedTextureRect = textureRect;
				if (transformedTextureRect.origin.x < 0)
					transformedTextureRect.origin.x += -floor(transformedTextureRect.origin.x);
				if (transformedTextureRect.origin.y < 0)
					transformedTextureRect.origin.y += -floor(transformedTextureRect.origin.y);
				
				//Draw the tiles
				rectd textureSubrect;
				for (textureSubrect.origin.x = transformedTextureRect.minX(); textureSubrect.origin.x < transformedTextureRect.maxX(); textureSubrect.origin.x += textureSubrect.size.x) {
					//Calculate the width of the texture subrect
					textureSubrect.size.x = std::min<double>(ceil(textureSubrect.origin.x + 1e-2) - textureSubrect.origin.x, 1);
					textureSubrect.size.x -= std::max<double>(textureSubrect.maxX() - transformedTextureRect.maxX(), 0);
					if (textureSubrect.size.x < 1e-6) break;
					
					for (textureSubrect.origin.y = transformedTextureRect.minY(); textureSubrect.origin.y < transformedTextureRect.maxY(); textureSubrect.origin.y += textureSubrect.size.y) {
						//Calculate the height of the texture subrect
						textureSubrect.size.y = std::min<double>(ceil(textureSubrect.origin.y + 1e-2) - textureSubrect.origin.y, 1);
						textureSubrect.size.y -= std::max<double>(textureSubrect.maxY() - transformedTextureRect.maxY(), 0);
						if (textureSubrect.size.y < 1e-6) break;
						
						//Calculate the tile rect for the given texture subrect
						rectd tileRect = textureSubrect;
						tileRect.origin -= transformedTextureRect.origin;
						tileRect.origin *= rect.size / transformedTextureRect.size;
						tileRect.size *= rect.size / transformedTextureRect.size;
						tileRect.origin += rect.origin;
						
						//Calculate the pixel coordinates of the textureSubrect, since GL_TEXTURE_RECTANGLE_EXT works that way
						rectd pixelSubrect = textureSubrect;
						pixelSubrect.origin.x = fmod(pixelSubrect.origin.x, 1);
						pixelSubrect.origin.y = fmod(pixelSubrect.origin.y, 1);
						pixelSubrect.origin *= texture->size;
						pixelSubrect.size *= texture->size;
						
						//Draw the textured tile
						glBegin(GL_QUADS);
						glTexCoord2d(pixelSubrect.minX(), pixelSubrect.minY()); glVertex2d(tileRect.minX(), tileRect.minY());
						glTexCoord2d(pixelSubrect.maxX(), pixelSubrect.minY()); glVertex2d(tileRect.maxX(), tileRect.minY());
						glTexCoord2d(pixelSubrect.maxX(), pixelSubrect.maxY()); glVertex2d(tileRect.maxX(), tileRect.maxY());
						glTexCoord2d(pixelSubrect.minX(), pixelSubrect.maxY()); glVertex2d(tileRect.minX(), tileRect.maxY());
						glEnd();
					}
				}
			} break;
				
			case kRectTextureMode: {
				//Calculate the pixel coordinates of the textureSubrect, since GL_TEXTURE_RECTANGLE_EXT works that way
				rectd pixelRect = textureRect;
				pixelRect.origin *= texture->size;
				pixelRect.size *= texture->size;
				
				//Draw the textured tile
				glBegin(GL_QUADS);
				glTexCoord2d(pixelRect.minX(), pixelRect.minY()); glVertex2d(rect.minX(), rect.minY());
				glTexCoord2d(pixelRect.maxX(), pixelRect.minY()); glVertex2d(rect.maxX(), rect.minY());
				glTexCoord2d(pixelRect.maxX(), pixelRect.maxY()); glVertex2d(rect.maxX(), rect.maxY());
				glTexCoord2d(pixelRect.minX(), pixelRect.maxY()); glVertex2d(rect.minX(), rect.maxY());
				glEnd();
			} break;
		}
	}
	
	//Or just draw the colored sprite
	else {
		glBegin(GL_QUADS);
		glVertex2d(rect.minX(), rect.minY());
		glVertex2d(rect.maxX(), rect.minY());
		glVertex2d(rect.maxX(), rect.maxY());
		glVertex2d(rect.minX(), rect.maxY());
		glEnd();
	}
}

void Sprite::autogenerateTextureRect()
{
	if (!texture) return;
	
	//Autocalculate the texture rect where requested
	if (autoTexRectX) {
		if (autoTexRelativeX)
			textureRect.origin.x = 0;
		else
			textureRect.origin.x = rect.origin.x / texture->size.x * textureScale;
		textureRect.size.x = rect.size.x / texture->size.x * textureScale;
	}
	if (autoTexRectY) {
		if (autoTexRelativeY)
			textureRect.origin.y = 0;
		else
			textureRect.origin.y = rect.origin.y / texture->size.y * textureScale;
		textureRect.size.y = rect.size.y / texture->size.y * textureScale;
	}
}