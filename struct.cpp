#include<stdlib.h>

#include"struct.h"


slist *destr(slist *sav) {
	if (sav) {
		while (sav->next) {
			sav = sav->next;
			free(sav->prev);
		}
		free(sav);
	}
	return NULL;
}

animlist *destr(animlist *a) {
	if (a) {
		while (a->next) {
			animlist *b = a;
			a = a->next;
			free(b);
		}
		free(a);
	}
	return NULL;
}
void *destr(records *rec) {
	if (rec->ar) {
		free(rec->ar);
		rec->len = -1;
	}
	free(rec);
	return NULL;
}