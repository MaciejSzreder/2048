#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#include"sdl2.h"
#include"struct.h"

#define EMPTYTILE 0
#define BLOCKTILE 1
#define SPECIALTILES 2

#define ARRLEN(arr)	(sizeof(arr)/sizeof(*arr))

extern struct uarr;
typedef uarr *puarr;

void DrawString(SDL_Surface *screen, int x, int y, const char *text,
	SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt úrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) 
// bπdü poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostokπta o d≥ugoúci bokÛw l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};


int getcolor(int val, const SDL_PixelFormat *format) {
	if (val == EMPTYTILE) return SDL_MapRGB(format, 0xaa, 0xbb, 0xcc);
	if (val == BLOCKTILE) return SDL_MapRGB(format, 0, 0, 0);
	return SDL_MapRGB(format, ((val + 1)*val + 2)*val + 3, ((val + 5)*val + 6) + 7, (101 * val) & 0xff);
}
char *getlabel(unsigned val, char* buff) {
	if (val < SPECIALTILES) {
		return &(buff[0] = 0);
	}
	else if (val < sizeof(unsigned) * 8) {
		return _ltoa(1 << (val - SPECIALTILES), buff, 10);
	}
	else {
		strcpy(buff, "2^");
		_ltoa(val - SPECIALTILES, buff + 2, 10);
		return buff;
	}
}
void drawTile(SDL_Surface *screen, SDL_Surface *charset, int x, int y, int w, int h, unsigned val) {
	char buff[30];
			DrawRectangle(screen, x ,y, w, h, SDL_MapRGB(screen->format, 0xaa, 0xbb, 0xcc), getcolor(val, screen->format));
			getlabel(val, buff);
			DrawString(screen, x+w/2-strlen(buff)*4 , y+h/2-4,buff , charset);
}
void drawBoard(SDL_Surface *screen, puarr board, SDL_Surface *charset, int x, int y, int w, int h) {
	char buff[100];
	long double wk = w / board->col+1, hk = h / board->row+1;
	for (int i = 0; i < board->row; ++i)
		for (int j = 0; j < board->col; ++j) {
			drawTile(screen, charset, x + (j * w) / board->col, y + i * h / board->row, wk, hk, board->ar[i][j]);
			/*
			DrawRectangle(screen, 10 + j * 25, 10 + i * 25, 20, 25, SDL_MapRGB(screen->format, 0xaa, 0xbb, 0xcc), getcolor(board->ar[i][j], screen->format));
			DrawString(screen, 10 + j * 25, 10 + i * 25, getlabel(board->ar[i][j], buff), charset);
			//*/
			//*
			//*/
		}
};

void drawTime(SDL_Surface *screen, int start, SDL_Surface *charset) {
	char buff[13];//hh:mm:ss,mss\0	9 znakÛw	99:59:59,999 > 4 dni
	int hh, mm, ss, ms, time;
	time = (SDL_GetTicks() - start);
	ms = time % 1000;
	time /= 1000;
	ss = time % 60;
	time /= 60;
	mm = time % 60;
	time /= 60;
	hh = time;
	snprintf(buff, ARRLEN(buff), "%02d:%02d:%02d"/*",%03d"*/, hh, mm, ss/*, ms*/);
	DrawString(screen, 236, 8, buff, charset);

}

void drawScore(SDL_Surface *screen, int scores, SDL_Surface *charset) {
	char buff[13];
	_ltoa(scores,buff,10);
	DrawString(screen, 2, 8, buff, charset);
	//DrawString(screen, 2, 100, "1111111q1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111", charset);

}

void drawOption(SDL_Surface *screen,SDL_Surface *charset,int x, int y, int w, int h, const char* text,Uint32 color) {
	DrawRectangle(screen, x, y, w, h, color, color);
	DrawString(screen, x, y, text, charset);
}

const int X = 100, Y = 100, H = 25, W = 160;
void drawnextlist(SDL_Surface *screen, SDL_Surface * charset, slist *iter) {
	for (int i = 0; i < 5; ++i) {
		if (iter) {
			drawOption(screen, charset, X, Y + i * H, W, H, iter->time, 4000000);
			iter = iter->prev;
		}
		else
			drawOption(screen, charset, X, Y + i * H, W, H, "", 4000000);
	}
}
void drawprevlist(SDL_Surface *screen, SDL_Surface * charset, slist *iter) {
	for (int i = 4; i >= 0; --i) {
		drawOption(screen, charset, X, Y + i * H, W, H, iter->time, 4000000);
		iter = iter->next;
	}
}


void drawRecord(SDL_Surface *screen, SDL_Surface *charset, int x, int y, int w, int h, record *rec, Uint32 color) {
	char buff[10];
	DrawRectangle(screen, x, y, w, H, color, color);
	DrawString(screen, x, y, rec->board, charset);
	DrawString(screen, x + w / 4, y, rec->name, charset);
	DrawString(screen, x + w / 2, y, ltoa(rec->score, buff, 10), charset);


	int hh, mm, ss, ms, time=rec->time;
	ms = time % 1000;
	time /= 1000;
	ss = time % 60;
	time /= 60;
	mm = time % 60;
	time /= 60;
	hh = time;
	snprintf(buff, ARRLEN(buff), "%02d:%02d:%02d"/*",%03d"*/, hh, mm, ss/*, ms*/);
	DrawString(screen, x + 3*w / 4, y,buff , charset);

	;
}