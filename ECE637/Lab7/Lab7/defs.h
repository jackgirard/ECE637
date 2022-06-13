#pragma once

unsigned int calculateWMAtLocation(struct TIFF_img input, int xpos, int ypos);

unsigned int** filterImage(struct TIFF_img input);