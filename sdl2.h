#pragma once



#define SCREEN_WIDTH	300
#define SCREEN_HEIGHT	316

extern struct uarr;
extern struct slist;
extern struct record;

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,	Uint32 outlineColor, Uint32 fillColor);


//int getcolor(int val, const SDL_PixelFormat *format);
//char *getlabel(unsigned val, char* buff);
void drawBoard(SDL_Surface *screen, uarr *board, SDL_Surface *charset, int x, int y, int w, int h);

void drawTile(SDL_Surface *screen, SDL_Surface *charset, int x, int y, int w, int h, unsigned val);

void drawTime(SDL_Surface *screen, int start, SDL_Surface *charset);

void drawScore(SDL_Surface *screen, int scores, SDL_Surface *charset);

void drawOption(SDL_Surface *screen, SDL_Surface *charset, int x, int y, int w, int h, const char* text,Uint32 color);

void drawnextlist(SDL_Surface *screen, SDL_Surface * charset, slist *iter);

void drawprevlist(SDL_Surface *screen, SDL_Surface * charset, slist *iter);

void drawRecord(SDL_Surface *screen, SDL_Surface *charset, int x, int y, int w, int h, record *rec, Uint32 color);