#ifndef __TFT_COLORS
#define __TFT_COLORS

#define TFT_COLOR(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
// #define COMPLEMENT_COLOR(color) \
//     (TFT_COLOR((255 - ((color >> 16) & 0xFF)), (255 - ((color >> 8) & 0xFF)), (255 - (color & 0xFF))))
// #define PUSHED_COLOR(color) ((((color & 0xF800) >> 1) & 0xF800) | (((color & 0x07E0) >> 1) & 0x07E0) | (((color & 0x001F) >> 1) & 0x001F))

#endif