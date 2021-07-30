#define _CRT_SECURE_NO_WARNINGS

#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


extern "C" {
#include"./sdl-2.0.7/include/SDL.h"
#include"./sdl-2.0.7/include/SDL_main.h"
}

#include"struct.h"
#include"sdl2.h"
/*
  0		pusta kafelka
  1		nieruchoma przeszkoda
  n		kfelka o wartoœci 1<<(n-2)
*/

/*
   O   O   O   O
  O O O O O   O O
    O O O OOO  O
  O   O O  O  O O
  OOO  O   O   O
*/

#define EMPTYTILE 0
#define BLOCKTILE 1
#define SPECIALTILES 2
#define TILE2 3
#define WONTILE 13

const char *SAVESFILE = "saves.txt";
const char *RECORDSFILE = "records.txt";
const int BOARDX = 0;
const int BOARDY = 16;
const int BOARDW = 300;
const int BOARDH = 300;

#define ARRLEN(arr)	(sizeof(arr)/sizeof(*arr))

/*
dd.mm.yyyy hh:mm:ss
score time
w h
board[0][0] - board[w-1][h-1];
*/
bool addrecord(const char* src, const char *board, const char* name, unsigned long long score, unsigned long long time) {
	FILE *f = fopen(src, "a");
	if (!f) return false;

	if (fprintf(f, "%s %llu %llu %s\n",board, score,time, name) < 0) return false;

	fclose(f);
	return true;
}

bool readrecord(FILE *f,record *r) {
	//char boardname[20];
	if(fscanf(f,"%s %llu %llu ",r->board, &r->score, &r->time)<2) return false;
	char c ;
	int i = 0;
	while (i < 10 && (c = fgetc(f)) != '\n') {
		r->name[i] = c;
		++i;
	}
	r->name[i] = '\0';
	return true;
}

int cmprcdscore(const void *_a, const  void *_b) {
	record *a = *(record**)_a, *b = *(record**)_b;
	int cmp = 0;
	if (cmp = strcmp(a->board, b->board))return cmp;
	if (cmp = (b->score- a->score))return cmp;
	if (cmp = (a->time- b->time))return cmp;
	return cmp;

}
int cmprcdtime(const void *_a, const  void *_b) {
	record *a = *(record**)_a, *b = *(record**)_b;
	int cmp = 0;
	if (cmp = strcmp(a->name, b->name))return cmp;
	if (cmp = (a->time- b->time))return cmp;
	if (cmp = (b->score- a->score))return cmp;
	return cmp;

}

records *readrecords(const char* src) {
	int size = 10;
	int len = 0;
	records *rcds=(records*)malloc(sizeof(*rcds));
	rcds->ar = (record**)malloc(size * sizeof(*rcds->ar));

	FILE *f = fopen(src, "r");

	record **iter = rcds->ar;
	while (readrecord(f, (*(iter++) = (record*)malloc(sizeof(**rcds->ar)))))
		if (++len== size) {
			rcds->ar = (record**)realloc(rcds->ar, size <<= 1);
			iter = rcds->ar+len;
		}
	free(*--iter);
	*iter=NULL;
//	rcds->ar = (record**)realloc(rcds->ar, len);
	rcds->len = len;
	return rcds;
}
void drawrecords(SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset,int poz,record** iter) {

	const int X = BOARDX, Y = BOARDY, H = 25, W = BOARDW;
	for (int i = poz; i < 10 && *iter; ++i, ++iter)
		drawRecord(screen, charset, X, Y + (i-poz) * H, W, H, *iter, 200000);
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}
void recordslist(const char * src, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset) {


	records *rec= readrecords(RECORDSFILE);
	record **iter = rec->ar;

	SDL_Event event;
	int poz = 0, i = 0;
	//for(int i=poz;i<5&& *iter;++i,++iter)
		//drawRecord(screen, charset, X, Y + i * H, W, H, *iter , 200000);
	while (1) {
		drawrecords(screen, scrtex, renderer, charset, poz, iter);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					destr(rec);
					return;
				case SDLK_DOWN:
					poz += poz <10;
					break;
				case SDLK_UP:
					poz -= poz > 0;
					break;
				case SDLK_p:
					qsort(rec->ar, rec->len, sizeof(*rec->ar), cmprcdscore);
					break;
				case SDLK_t:
					qsort(rec->ar, rec->len, sizeof(*rec->ar), cmprcdtime);
					break;

				case SDLK_RETURN:
					return;
				}
				break;
			};
		};
	}
}


int prompt(SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset,int x,int y,int w, int h,int maxl, char *buff) {
	Uint32 c1 = 45678;
	SDL_Event event;
	DrawRectangle(screen, x, y, w, h, c1, c1);
	DrawRectangle(screen, x+(w-maxl*8-4)/2, y+h/2-6, maxl * 8 + 4, 12, 0, 0);
	*buff = 0;
	int len = 0;

	while (1) {
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					*buff = 0;
					return -1;
				}if (event.key.keysym.sym == SDLK_RETURN) {
					buff[len] = 0;
					return len;
				}if (event.key.keysym.sym == SDLK_BACKSPACE) {
					if (len){
						DrawRectangle(screen, x + (w - maxl * 8 -16) / 2 + len * 8, y + h / 2 - 4, 8, 8, 0, 0);
						buff[--len] = 0;
					}
				}else if(len<maxl&&'a'<= event.key.keysym.sym&&event.key.keysym.sym<='z') {
					buff[len++] = event.key.keysym.sym;
					buff[len] = 0;
					DrawString(screen, x + (w - maxl * 8 -16) / 2+len*8, y + h / 2 - 4, buff+len-1, charset);
				}
			}
		}
	}
}

bool save(const char* src,puarr board,unsigned long long score,unsigned long long timel) {
	FILE *f=fopen(src, "a");
	if (!f) return false;
	char buff[255];//dd.mm.rrrr hh:mm:ss\0 
	time_t now = time(NULL);
	strftime(buff, ARRLEN(buff), "%d.%m.%Y %H:%M:%S", localtime(&now));
	if (fprintf(f, "%s\n", buff) < 0) return false;
	if (fprintf(f, "%llu %llu %u %u\n", score, timel, board->row, board->col) < 0) return false;
	for (int i = 0; i < board->row; ++i) {
		for (int j = 0; j < board->col; ++j) {
			if (fprintf(f, "%u ", board->ar[i][j])<0)return false;
		}
		fprintf(f,"\n");
	}
	fprintf(f, ";\n");
	fclose(f);
	return true;
}

slist *readsaves(const char *src) {
	FILE *f = fopen(src, "r");
	char time[20], c;
	slist *sav = NULL, **ns = &sav, *os = NULL;
	time[19] = 0;
	int len = 0;
	do {
		*ns = (slist*)malloc(sizeof(**ns));
		(*ns)->time[len=fread((*ns)->time, sizeof(char), 19, f)] = '\0';
		if (len < 19) {
			free(*ns);
			os->next = NULL;
			break;
		}
		(*ns)->pos = ftell(f);
		(*ns)->next = NULL;
		(*ns)->prev = os;
		os = *ns;
		ns = &(*ns)->next;

		while ((c = fgetc(f)) != ';'&&c != EOF);
		//fgetc(f);
	} while (fgetc(f) != EOF);
	fclose(f);
	return os;
}

//load(saveslist("saves.txt"), board, &time, &score);


int saveslist(const char * src, SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset) {

	const int X = 100, Y = 100, H = 25, W =160;

	slist *sav = readsaves(SAVESFILE);
	slist *iter = sav;

	SDL_Event event;
	int poz = 0, i = 0;

	drawnextlist(screen, charset, sav);
	drawOption(screen, charset, X, Y+ i * H, W, H, sav->time, 200000);
	while (1) {
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					destr(sav);
					return -1;
				case SDLK_DOWN:
					if (sav->prev) {
						drawOption(screen, charset, X, Y + i * H, W, H, sav->time, 8000000);
						++i;
						if (i == 5) {
							++poz;
							i = 0;
							drawnextlist(screen, charset, sav->prev);
						}
						sav = sav->prev;
						drawOption(screen, charset, X, Y + i * H, W, H, sav->time, 200000);
					}
					break;
				case SDLK_UP:
					if (sav->next) {
						drawOption(screen, charset, X, Y + i * H, W, H, sav->time, 4000000);
						--i;
						if (i == -1) {
							++poz;
							i = 4;
							drawprevlist(screen, charset, sav->next);
						}
						sav = sav->next;
						drawOption(screen, charset, X, Y + i * H, W, H, sav->time, 200000);
					}
					break;
				case SDLK_r:
					
					
				case SDLK_RETURN:
					auto pos = sav->pos;
					destr(sav);
					return pos;
				}
				break;
			};
		};
	}
	auto pos = sav->pos;
	destr(sav);
	return pos;
}

puarr newboard(puarr, int, int);

puarr load(const char *src,int pos, puarr board,unsigned *time,unsigned long long *score) {
	if (pos < 0) return board;
	FILE *f = fopen(src, "r");
	if (!f) return NULL;
	unsigned r, c;
	unsigned long long t, s;
	puarr b;
	fseek(f,pos,SEEK_SET );
	if(fscanf(f,"%llu %llu %u %u", &s, &t,&c,&r)<4)return NULL;
	*time = t;
	*score = s;
	b = newboard(board, r, c);
	if (!b) return NULL;

	for (int i = 0; i < r; ++i)
		for (int j = 0; j < c; ++j) 
			if (fscanf(f, "%u", &b->ar[i][j]) < 0) b->ar[i][j] = 0;

	fclose(f);
	return b;
}


puarr newboard(puarr board, int row, int col) {
	if (!board) {
		board = (puarr)malloc(sizeof(uarr));
		board->col = board->row = 0;
		board->ar = NULL;
	}
	if (!board->ar) {
		for (int i = 1; i < board->row; ++i)
			free(board->ar[i]);
		free(board->ar);
	}

	board->ar = (unsigned**)malloc(row * sizeof(*board->ar));			//ar[y] rows
	for (int i = 0; i < row; ++i) {
		board->ar[i] = (unsigned *)calloc(col, sizeof(**board->ar));	//ar[y][x] cell
	}
	board->col = col;
	board->row = row;
	//*
	for (int i = 0; i < board->row; ++i)
		for (int j = 0; j < board->col; ++j)
			board->ar[i][j] = 2 + rand() % 5;
	//*/
	return board;
}

//enum dir { up, down, left, right };
int shift(puarr board, SDL_Keycode d,animlist **a) {
	int score = 0;
	animlist *c = NULL;
	switch (d) {
//*
#define SHIFT(ij, inij, enij, chij, ji, inji, enji, chji, ik, jk,ink) {\
	for (int ij = inij; ij enij; chij ij) {\
		int k = ink;\
		for (int ji = inji; ji enji; chji ji) {\
			if (board->ar[i][j] == BLOCKTILE) k = ji;		/*dla blokady*/\
			else if (board->ar[i][j] >= SPECIALTILES) {		/*dla normalnych*/\
				if (board->ar[i][j] == board->ar[ik][jk]) {		/*dla równych*/\
					animlist *b=(animlist*)malloc(sizeof(animlist));\
					b->from.val=board->ar[i][j];\
					b->from.x=j;\
					b->from.y=i;\
					\
					score += 1 << ++board->ar[ik][jk];\
					board->ar[i][j] = EMPTYTILE;\
					\
					b->to.val=board->ar[ik][jk];\
					b->to.x=jk;\
					b->to.y=ik;\
					b->next=c;\
					c=b;\
					\
					chji k;\
				}\
				else if (board->ar[ik][jk] == EMPTYTILE) {			/*dla pustego*/\
					animlist *b=(animlist*)malloc(sizeof(animlist));\
					b->from.val=board->ar[i][j];\
					b->from.x=j;\
					b->from.y=i;\
					\
					board->ar[ik][jk] = board->ar[i][j];\
					board->ar[i][j] = EMPTYTILE;\
					\
					b->to.val=board->ar[ik][jk];\
					b->to.x=jk;\
					b->to.y=ik;\
					b->next=c;\
					c=b;\
				}\
				else {											/*dla ró¿nych*/\
					chji k;\
					if (ji != k) {\
						animlist *b=(animlist*)malloc(sizeof(animlist));\
						b->from.val=board->ar[i][j];\
						b->from.x=j;\
						b->from.y=i;\
						\
						board->ar[ik][jk] = board->ar[i][j]; \
						board->ar[i][j] = EMPTYTILE; \
						\
						b->to.val=board->ar[ik][jk];\
						b->to.x=jk;\
						b->to.y=ik;\
						b->next=c;\
						c=b;\
					}\
				}\
			}\
		}\
	}\
}

	case SDLK_LEFT:
		SHIFT(i, 0, < board->row, ++, j, 1, < board->col, ++, i, k, 0);
		break;
	case SDLK_RIGHT:
		SHIFT(i, 0, < board->row, ++, j, board->col - 2, >= 0, --, i, k, board->col - 1);
		break;
	case SDLK_UP:
		SHIFT(j, 0, < board->col, ++, i, 1, < board->row, ++, k, j, 0);
		break;
	case SDLK_DOWN:
		SHIFT(j, 0, < board->col, ++, i, board->row - 2, >= 0, --, k, j, board->row - 1);
		break;
	}
	*a = c;
	return score;
}

void draw(SDL_Surface *screen, SDL_Texture *scrtex,SDL_Renderer *renderer,SDL_Surface *charset,uarr *board,long long tstart, unsigned long long score) {

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	DrawRectangle(screen, 0, 0, SCREEN_WIDTH, 16, niebieski, niebieski);

	sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
	DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
	drawBoard(screen, board, charset, BOARDX, BOARDY, BOARDW, BOARDH);
	drawTime(screen, tstart, charset);
	drawScore(screen, score, charset);
	//DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, _ltoa(score,buff,10), charset);

	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

const int ANIMATION = 25;
void anim(SDL_Surface *screen, SDL_Texture *scrtex, SDL_Renderer *renderer, SDL_Surface *charset,animlist *a,puarr board) {
	//for (animlist *iter = a; iter; iter = iter->next)
		//drawTile(screen,charset,20,20,100/board->row,100/board->col,0);
//	for (animlist *iter = a; iter; iter = iter->next)
//		iter->state = 0;
	int tstart = SDL_GetTicks(),tnow;
	do {
		tnow = fmin((double)SDL_GetTicks() - tstart,(double)ANIMATION);

		for (animlist *iter = a; iter; iter = iter->next) {
			drawTile(screen, charset, BOARDX + (iter->from.x + double(iter->to.x - iter->from.x)*tnow/ ANIMATION)*BOARDW / board->col , BOARDY + (iter->from.y + double(iter->to.y - iter->from.y)*tnow/ ANIMATION)*BOARDH / board->row+1 , BOARDW / board->col+1, BOARDH / board->row, iter->from.val);
		}
			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

		for (animlist *iter = a; iter; iter = iter->next) {
			drawTile(screen, charset, BOARDX + (iter->from.x + (iter->to.x - iter->from.x)*tnow / ANIMATION)*BOARDW / board->col, BOARDY + (iter->from.y + (iter->to.y - iter->from.y)*tnow/ ANIMATION)* BOARDH / board->row , BOARDW / board->col+1, BOARDH / board->row+1, EMPTYTILE);
		}
	} while (tnow < ANIMATION);
	drawBoard(screen,board,charset,BOARDX,BOARDY,BOARDW,BOARDH);
}
bool iswon(animlist *a) {
	for (animlist *iter = a; iter; iter = iter->next)
		if (iter->to.val == WONTILE)
			return true;
	return false;
}
bool insertrand(puarr board) {
	int i = 10,col = rand() % board->col,row = rand() % board->row;
	for(int i=10;i&& board->ar[row][col];--i){
		col = rand() % board->col;
		row = rand() % board->row;
	};
	if (board->ar[row][col]) {
		for (int i = 0; i < board->row; ++i)
			for (int j = 0; j < board->col; ++j)
				if (!board->ar[i][j]) {
					{
						board->ar[i][j] = TILE2;
						return true;
					}
				}
	}
	else {
		board->ar[row][col] = TILE2;
		return true;
	}
	return false;
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int quit,rc;
	bool won=0;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	char* buff[10];

	srand(time(0));
	char *boardname=NULL;
	puarr board;
	long long tstart;
	unsigned long long score;
	if (argc >= 2) {
		boardname = argv[1];
		unsigned time;
		board=load(boardname, 0, NULL, &time, &score);
		tstart = SDL_GetTicks() - time;
		insertrand(board);
	}else {
		boardname = "standard";
		board = newboard(NULL, 4, 8);
		tstart = 0;
		score = 0;
	}
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Maciej Szreder 175670");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);



	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);


	quit = 0;
	animlist *a = NULL;

	while(!quit) {
		draw(screen, scrtex, renderer, charset, board, tstart, score);

		// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = 1;
						break;
					case SDLK_UP:
					case SDLK_DOWN:
					case SDLK_LEFT:
					case SDLK_RIGHT:
						destr(a);
						score+=shift(board, event.key.keysym.sym,&a);
						anim(screen, scrtex, renderer, charset, a, board);
						if(a) insertrand(board);
						break;
					case SDLK_n:
						board = newboard(board,board->row, board->col);
						tstart = SDL_GetTicks();
						won = 0;
						score = 0;
						break;
					case SDLK_KP_MINUS:
						if (!--board->row) board->row=1;
						if (!--board->col) board->col=1;
						board = newboard(board, board->row, board->col);
						tstart = SDL_GetTicks();
						won = 0;
						score = 0;
						destr(a);
						a = NULL;
						break;
					case SDLK_KP_PLUS:
						board = newboard(board,++board->row, ++board->col);
						tstart = SDL_GetTicks();
						won = 0;
						score = 0;
						destr(a);
						a = NULL;
						break;
					case SDLK_u:
						for (animlist *iter = a; iter; iter = iter->next) {
							board->ar[iter->to.y][iter->to.x] = (iter->from.val == iter->to.val) ? EMPTYTILE : iter->from.val;
							board->ar[iter->from.y][iter->from.x] = iter->from.val;

							unsigned tmp;
							tmp = iter->to.y;
							iter->to.y = iter->from.y;
							iter->from.y = tmp;
							tmp = iter->to.x;
							iter->to.x = iter->from.x;
							iter->from.x = tmp;
							tmp = iter->to.val;
							iter->to.val = iter->from.val;
							iter->from.val = tmp;
						}
						anim(screen,scrtex,renderer,charset,a,board);
						break;
					case SDLK_s:
						save(SAVESFILE, board, score, SDL_GetTicks() - tstart);
						break;
					case SDLK_l: 
						unsigned time;
						load(SAVESFILE,saveslist(SAVESFILE,screen,scrtex,renderer,charset), board, &time, &score);
						tstart = (long long)SDL_GetTicks() - time;
						break;
					case SDLK_r:
						recordslist(RECORDSFILE, screen, scrtex, renderer, charset);
						break;
					}
					if (!won) {
						int time = SDL_GetTicks() - tstart;
						char buff[10],name[20];
						if ((won = iswon(a)) && prompt(screen, scrtex, renderer, charset, BOARDX, BOARDY, BOARDW, BOARDH, 9, buff) > 0) {
							sprintf_s(name, 20, "%ux%u:%s", board->row, board->col, boardname);
							addrecord(RECORDSFILE,name, buff, score, time);
						}
						tstart = SDL_GetTicks() - time;
					}
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
