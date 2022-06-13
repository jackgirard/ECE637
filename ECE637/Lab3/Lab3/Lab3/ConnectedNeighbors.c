#include "defs.h"

void ConnectedNeighbors(struct pixel s, double T, unsigned char** img, int width, int height, int* M, struct pixel c[4]) {	
	*M = 0;
	if (s.m - 1 >= 0 && abs(img[s.m][s.n] - img[s.m - 1][s.n]) <= T) {
		c[*M].m = s.m - 1;
		c[*M].n = s.n;
		(*M)++;
	}
	if (s.m + 1 < height && abs(img[s.m][s.n] - img[s.m + 1][s.n]) <= T) {
		c[*M].m = s.m + 1;
		c[*M].n = s.n;
		(*M)++;
	}
	if (s.n - 1 >= 0 && abs(img[s.m][s.n] - img[s.m][s.n - 1]) <= T) {
		c[*M].m = s.m;
		c[*M].n = s.n - 1;
		(*M)++;
	} if (s.n + 1 < width && abs(img[s.m][s.n] - img[s.m][s.n + 1]) <= T) {
		c[*M].m = s.m;
		c[*M].n = s.n + 1;
		(*M)++;
	}
}