/*
 * OpenTyrian: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */
#include "video.h"
#include "keyboard.h"
#include "opentyr.h"
#include "palette.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libdragon.h>

const char *scaling_mode_names[ScalingMode_MAX] = {
    "Center",
    "Integer",
    "Fit 8:5",
    "Fit 4:3",
};

int fullscreen_display;
ScalingMode scaling_mode = SCALE_INTEGER;
static SDL_Rect last_output_rect = {0, 0, vga_width, vga_height};

SDL_Surface *VGAScreen, *VGAScreenSeg;
SDL_Surface *VGAScreen2;
SDL_Surface *game_screen;

SDL_Window *main_window;
static SDL_Renderer *main_window_renderer;
static SDL_Texture *main_window_texture;
SDL_PixelFormat *main_window_tex_format;
static bool renderer_needs_reinit;

static void reinit_renderer();
static void recenter_window_in_display();

void init_video(void) {
  if (SDL_WasInit(SDL_INIT_VIDEO))
    return;

  if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) {
    fprintf(stderr, "error: failed to initialize SDL video: %s\n",
            SDL_GetError());
    exit(1);
  }

  // Create the software surfaces that the game renders to. These are all
  // 320x200x8 regardless of the window size or monitor resolution.
  VGAScreen = VGAScreenSeg =
      SDL_CreateRGBSurface(0, vga_width, vga_height, 8, 0, 0, 0, 0);
  VGAScreen2 = SDL_CreateRGBSurface(0, vga_width, vga_height, 8, 0, 0, 0, 0);
  game_screen = SDL_CreateRGBSurface(0, vga_width, vga_height, 8, 0, 0, 0, 0);

  // The game code writes to surface->pixels directly without locking, so make
  // sure that we indeed created software surfaces that support this.
  assert(!SDL_MUSTLOCK(VGAScreen));
  assert(!SDL_MUSTLOCK(VGAScreen2));
  assert(!SDL_MUSTLOCK(game_screen));

  JE_clr256(VGAScreen);

  // Create the window with a temporary initial size, hidden until we set up the
  // scaler and find the true window size
  main_window = SDL_CreateWindow("OpenTyrian", SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED, vga_width, vga_height,
                                 SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);

  if (main_window == NULL) {
    fprintf(stderr, "error: failed to open window: %s\n", SDL_GetError());
  }

  main_window_texture = NULL;
  main_window_tex_format = NULL;
  main_window_renderer = NULL;

  reinit_fullscreen(fullscreen_display);
  reinit_renderer();
  SDL_ShowWindow(main_window);

  input_grab(input_grab_enabled);
}

static void reinit_renderer() {
  int bpp = 16; // TODOSDL2
  Uint32 format = bpp == 32 ? SDL_PIXELFORMAT_RGB888 : SDL_PIXELFORMAT_RGBA5551;
  int scaler_w = vga_width;
  int scaler_h = vga_height;

  // (re)-create the pixel format structure
  SDL_FreeFormat(main_window_tex_format);
  main_window_tex_format = SDL_AllocFormat(format);

  // Destroy and re-create renderer
  SDL_DestroyRenderer(main_window_renderer);
  main_window_renderer = SDL_CreateRenderer(main_window, -1, 0);

  if (main_window_renderer == NULL) {
    fprintf(stderr, "error: failed to create renderer: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Destroy and re-create output texture.
  SDL_DestroyTexture(main_window_texture);
  main_window_texture =
      SDL_CreateTexture(main_window_renderer, format,
                        SDL_TEXTUREACCESS_STREAMING, scaler_w, scaler_h);

  if (main_window_texture == NULL) {
    fprintf(stderr, "error: failed to create scaler texture %dx%dx%s: %s\n",
            scaler_w, scaler_h, SDL_GetPixelFormatName(format), SDL_GetError());
    exit(EXIT_FAILURE);
  }
}

static void recenter_window_in_display() {
  int display;
  int win_w, win_h;

  display = SDL_GetWindowDisplayIndex(main_window);
  SDL_GetWindowSize(main_window, &win_w, &win_h);

  SDL_Rect bounds;
  SDL_GetDisplayBounds(display, &bounds);

  SDL_SetWindowPosition(main_window, bounds.x + (bounds.w - win_w) / 2,
                        bounds.y + (bounds.h - win_h) / 2);
}

void reinit_fullscreen(int new_display) {
  fullscreen_display = new_display;

  if (fullscreen_display >= SDL_GetNumVideoDisplays()) {
    fullscreen_display = 0;
  }

  if (fullscreen_display == -1) {
    SDL_SetWindowFullscreen(main_window, SDL_FALSE);
    SDL_SetWindowSize(main_window, vga_width, vga_height);
    recenter_window_in_display();
  } else {
    SDL_SetWindowFullscreen(main_window, SDL_FALSE);
    recenter_window_in_display();
    if (SDL_SetWindowFullscreen(main_window, SDL_WINDOW_FULLSCREEN_DESKTOP) !=
        0) {
      reinit_fullscreen(-1);
      return;
    }
  }

  renderer_needs_reinit = true;
}

void video_on_win_resize() {
  int w, h;
  int scaler_w, scaler_h;

  // Tell video to reinit if the window was manually resized by the user.
  // Also enforce a minimum size on the window.

  SDL_GetWindowSize(main_window, &w, &h);
  scaler_w = vga_width;
  scaler_h = vga_height;

  if (w < scaler_w || h < scaler_h) {
    w = w < scaler_w ? scaler_w : w;
    h = h < scaler_h ? scaler_h : h;

    SDL_SetWindowSize(main_window, w, h);
  }

  renderer_needs_reinit = true;
}

void toggle_fullscreen(void) {
  if (fullscreen_display != -1) {
    reinit_fullscreen(-1);
  } else {
    reinit_fullscreen(SDL_GetWindowDisplayIndex(main_window));
  }
}

bool set_scaling_mode_by_name(const char *name) {
  for (int i = 0; i < ScalingMode_MAX; ++i) {
    if (strcmp(name, scaling_mode_names[i]) == 0) {
      scaling_mode = i;
      return true;
    }
  }
  return false;
}

void deinit_video(void) {
  SDL_FreeFormat(main_window_tex_format);
  SDL_DestroyTexture(main_window_texture);

  SDL_DestroyRenderer(main_window_renderer);
  SDL_DestroyWindow(main_window);

  SDL_FreeSurface(VGAScreenSeg);
  SDL_FreeSurface(VGAScreen2);
  SDL_FreeSurface(game_screen);

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void JE_clr256(SDL_Surface *screen) { SDL_FillRect(screen, NULL, 0); }

void JE_showVGA(void) { scale_and_flip(VGAScreen); }

void nn_16(SDL_Surface *src_surface, SDL_Texture *dst_texture) {
  // const int dst_bpp = 2;

  // int dst_pitch;
  // void *tmp_ptr;
  // SDL_LockTexture(dst_texture, NULL, &tmp_ptr, &dst_pitch);

  // Uint8 *src = src_surface->pixels;
  // for (int y = 0; y < vga_height * vga_width; ++y) {
  //     *(Uint16 *)tmp_ptr = rgb_palette[*src];
  //     tmp_ptr += dst_bpp;
  //     ++src;
  // }

	Uint8 *src = src_surface->pixels, *src_temp;
	Uint8 *dst, *dst_temp;

	int src_pitch = src_surface->pitch;
	int dst_pitch;

	const int dst_Bpp = 2;         // dst_surface->format->BytesPerPixel
	int dst_width, dst_height;
	SDL_QueryTexture(dst_texture, NULL, NULL, &dst_width, &dst_height);
	
	const int height = vga_height, // src_surface->h
	          width = vga_width,   // src_surface->w
	          scale = dst_width / width;
	assert(scale == dst_height / height);

	void* tmp_ptr;
	SDL_LockTexture(dst_texture, NULL, &tmp_ptr, &dst_pitch);
	dst = tmp_ptr;
	
	for (int y = height; y > 0; y--)
	{
		src_temp = src;
		dst_temp = dst;
		
		for (int x = width; x > 0; x--)
		{
			for (int z = scale; z > 0; z--)
			{
				*(Uint16 *)dst = rgb_palette[*src];
				dst += dst_Bpp;
			}
			src++;
		}
		
		src = src_temp + src_pitch;
		dst = dst_temp + dst_pitch;
		
		for (int z = scale; z > 1; z--)
		{
			memcpy(dst, dst_temp, dst_width * dst_Bpp);
			dst += dst_pitch;
		}
	}

	SDL_UnlockTexture(dst_texture);
}

void scale_and_flip(SDL_Surface *src_surface) {
  // int start = get_ticks_ms();
  // Apply all changes to video settings accumulated throughout the frame, here.
  if (renderer_needs_reinit) {
    reinit_renderer();
    renderer_needs_reinit = false;
  }

  // Do software scaling
  // { fprintf(stderr, "\tnn_16 - start frame_time: %u\n", get_ticks_ms() - start); start = get_ticks_ms(); }
  nn_16(src_surface, main_window_texture);
  // { fprintf(stderr, "\tnn_16 frame_time: %u\n", get_ticks_ms() - start); start = get_ticks_ms(); }

  // Clear the window and blit the output texture to it
  SDL_SetRenderDrawColor(main_window_renderer, 0, 0, 0, 255);
  // SDL_RenderClear(main_window_renderer); // no need to clear screen (9 ms)

  const SDL_Rect dst_rect = { 0, 0, 320, 200 };

  SDL_RenderSetViewport(main_window_renderer, NULL);
  SDL_RenderCopy(main_window_renderer, main_window_texture, NULL, &dst_rect);

  // { fprintf(stderr, "\tSDL_RenderPresent - start frame_time: %u\n", get_ticks_ms() - start); start = get_ticks_ms(); }
  SDL_RenderPresent(main_window_renderer);
  // { fprintf(stderr, "\tSDL_RenderPresent frame_time: %u\n", get_ticks_ms() - start); start = get_ticks_ms(); }

  // Save output rect to be used by mouse functions
  last_output_rect = dst_rect;
}

/** Converts the given point from the game screen coordinates to the window
 * coordinates, after scaling. */
void map_screen_to_window_pos(int *inout_x, int *inout_y) {
  *inout_x =
      (*inout_x * last_output_rect.w / VGAScreen->w) + last_output_rect.x;
  *inout_y =
      (*inout_y * last_output_rect.h / VGAScreen->h) + last_output_rect.y;
}

/** Converts the given point from window coordinates (after scaling) to game
 * screen coordinates. */
void map_window_to_screen_pos(int *inout_x, int *inout_y) {
  *inout_x =
      (*inout_x - last_output_rect.x) * VGAScreen->w / last_output_rect.w;
  *inout_y =
      (*inout_y - last_output_rect.y) * VGAScreen->h / last_output_rect.h;
}
