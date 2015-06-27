#include <stdio.h>
#include "ui/text.h"
#include "ui/layout.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "core/config_gen_c.def"

struct layout layout;

void rect_array_layout(struct rect_array * array_spec, int i, rect_t * rect){
    int index_x = i;
    int index_y = i;
    if(array_spec->tile > 0){
        // Tile down with `tile` per row
        index_x = i % array_spec->tile;
        index_y = i / array_spec->tile;
    }else if(array_spec->tile < 0){
        // Tile across with `tile` per column
        index_y = i % (-array_spec->tile);
        index_x = i / (-array_spec->tile);
    }
    rect->x = array_spec->x + array_spec->px * index_x;
    rect->y = array_spec->y + array_spec->py * index_y;
    rect->w = array_spec->w;
    rect->h = array_spec->h;
}
void rect_array_origin(struct rect_array * array_spec, rect_t * rect){
    rect->x = 0;
    rect->y = 0;
    rect->w = array_spec->w;
    rect->h = array_spec->h;
}

// BMP images are lazily loaded. Try to load one 
SDL_Texture * image_load(struct image * image){
    if(image->error) return NULL; 
    if(!image->texture){
        char * full_path = strcatdup(config.path.images, image->filename);
        if(!full_path) return NULL;

        SDL_Texture * tmp_tex= IMG_LoadTexture(uic->render,full_path);
        if(!tmp_tex) {
            ERROR("IMG_Load: %s\n", SDL_GetError());
            image->error = 1;
        } else {
          image->texture = tmp_tex;
/*            image->surface = SDL_DisplayFormatAlpha(tmp_surf);
            if(!image->surface){
                ERROR("IMG_Load: %s\n", SDL_GetError());
                image->error = 1;
            }
            SDL_SetAlpha(image->surface, 0, 100);
            SDL_FreeSurface(tmp_surf);*/
        }
        free(full_path);
    }
    return image->texture;
}

// Draws a background image onto a SDL_Surface, defaulting to a solid color if an image isn't found
void fill_background(rect_t * rect, struct background * bg){
    SDL_Texture * image_texture = image_load(&bg->image);
    if(image_texture){
        // width/height are taken from srcrect, x/y are taken from dstrect
        rect_t rwh = {.x = rect->x, .y = rect->y, .w = rect->w, .h = rect->h};
        SDL_RenderCopy(uic->render,image_texture,NULL,&rwh);
//        SDL_BlitSurface(image_surface, &rwh, surface, rect);
    }else{
      fill_rect(rect,&bg->color);
//        SDL_FillRect(surface, rect, map_sdl_color(surface, bg->color));
    }
}
void fill_rect(rect_t *rect, SDL_Color *color)
{
  SDL_SetRenderDrawColor(uic->render,color->r,color->g,color->b,color->a);
  SDL_RenderFillRect(uic->render,rect);
}
