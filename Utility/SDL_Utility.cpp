#include "SDL_Utility.h"

SDL_Surface* createSurface(int w, int h) {
   SDL_Surface *surface;
   Uint32 rmask, gmask, bmask, amask;

   /* SDL interprets each pixel as a 32-bit number, so our masks must depend
    on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
   rmask = 0xff000000;
   gmask = 0x00ff0000;
   bmask = 0x0000ff00;
   amask = 0x000000ff;
#else
   rmask = 0x000000ff;
   gmask = 0x0000ff00;
   bmask = 0x00ff0000;
   amask = 0xff000000;
#endif

   surface = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, 32, rmask, gmask, bmask, amask);
   if(surface == NULL) {
      fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
      exit(1);
   }

   return surface;
}

void setColor(SDL_Surface* surf, int x, int y, const Color& c) {
   setColor(surf, x, y, SDL_MapRGBA(surf->format, c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()));
}

void setColor(SDL_Surface* surf, int x, int y, Uint32 color) {
   int bpp = surf->format->BytesPerPixel;
   Uint8* p = (Uint8 *) surf->pixels + y * surf->pitch + x * bpp;
   *(Uint32 *) p = color;
}

void setBlendColor(SDL_Surface *surf, int x, int y, const Color& c) {
   SDL_Rect rect = surf->clip_rect;
   if(x < 0 || x >= rect.w) {
      return;
   }
   if(y < 0 || y >= rect.h) {
      return;
   }
   Uint32 *pixel = (Uint32 *) surf->pixels + y * surf->pitch / 4 + x;
   setBlendColor(surf, pixel, c);
}

void setBlendColor(SDL_Surface* surf, Uint32* pixel, const Color& c) {
   Uint8 r, g, b, a;
   SDL_GetRGBA(*pixel, surf->format, &r, &g, &b, &a);
   float dr = r / 255.f;
   float dg = g / 255.f;
   float db = b / 255.f;
   float da = a / 255.f;

   float sa = c.alpha;

   float ao = sa + da * (1.f - sa);
   float ro = (c.red * sa + dr * da * (1.f - sa)) / ao;
   float go = (c.green * sa + dg * da * (1.f - sa)) / ao;
   float bo = (c.blue * sa + db * da * (1.f - sa)) / ao;

   r = ro * 255;
   g = go * 255;
   b = bo * 255;
   a = ao * 255;

   *pixel = SDL_MapRGBA(surf->format, r, g, b, a);
}

void setBlendColor(SDL_Surface * surf, int x, int y, const Color& c, int weight) {
   int alpha = ((c.getAlpha() * weight) >> 8);
   Color color(c.red, c.green, c.blue, alpha / 255.f);
   setBlendColor(surf, x, y, color);
}

Uint32 getPixel(SDL_Surface *surface, int x, int y) {
   int bpp = surface->format->BytesPerPixel;
   /* Here p is the address to the pixel we want to retrieve */
   Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
   
   switch(bpp) {
      case 1: return *p;
         
      case 2: return *(Uint16 *)p;
         
      case 3:
         if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            return p[0] << 16 | p[1] << 8 | p[2];
         }
         return p[0] | p[1] << 8 | p[2] << 16;
         
      case 4: return *(Uint32 *)p;
         
      default: return 0;       /* shouldn't happen, but avoids warnings */
   }
}

void setPixel(SDL_Surface* s, int x, int y, Uint32 pixel) {
   int bpp = s->format->BytesPerPixel;
   /* Here p is the address to the pixel we want to set */
   Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x * bpp;
   *(Uint32 *)p = pixel;
}
