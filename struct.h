#pragma once

typedef struct uarr {
	unsigned **ar, row, col;
} *puarr;

struct record {
	char name[10],board[20];
	unsigned long long score, time;
};

struct records {
	int len;
	record **ar;
};

struct animlist {
	struct {
		int x, y;
		unsigned val;
	}from, to;
	animlist *next;
};

struct slist {
	char time[20];
	unsigned pos;
	slist *next,*prev;
};
void *destr(records *rec);
slist *destr(slist *sav);
animlist *destr(animlist *a);