#include "defs.h"
#ifdef _MSC_VER
#include <malloc.h>
#endif
#include <assert.h>

void ConnectedSet(struct pixel s, double T, unsigned char** img, int width, int height, int ClassLabel, unsigned int** seg, int* NumConPixels) {
//	*NumConPixels = 0;
//	struct pixel* B = alloca(sizeof(struct pixel) * width * height);
////#ifdef _MSC_VER
////	struct pixel *B = alloca(sizeof(struct pixel) * width * height);
////#else
////	struct pixel unchecked[width * height];
////#endif
//	unsigned int lenB = 0;
//	unsigned int M = 0;
//	struct pixel c[4];
//
//	if (B == NULL)
//		return;
//
//	B[lenB++] = s;
//
//	while (lenB > 0) {
//
//		assert(lenB < width* height);
//		s = B[lenB - 1];
//		--lenB;
//
//		ConnectedNeighbors(s, T, img, width, height, &M, c);
//
//		if (seg[s.m][s.n] == 0) {
//			(*NumConPixels)++;
//		}
//
//		seg[s.m][s.n] = ClassLabel;
//
//		for (int i = 0; i < M; i++) {
//			if (seg[c[i].m][c[i].n] == 0) {
//				B[lenB++] = c[i];
//			}
//		}
//	}

	// DELETE
	struct pixelList* head, * tail, * tmp;
	struct pixel c[4];
	int M, i;

	(*NumConPixels) = 0;
	head = (struct pixelList*)malloc(sizeof(struct pixelList));
	head->pixel.m = s.m;
	head->pixel.n = s.n;
	head->next = NULL;
	tail = head;

	while (head != NULL) {
		if (seg[head->pixel.m][head->pixel.n] != ClassLabel) {
			seg[head->pixel.m][head->pixel.n] = ClassLabel;
			(*NumConPixels) += 1;
			ConnectedNeighbors(head->pixel, T, img, width, height, &M, c);
			for (i = 0; i < M; i++) {
				if (seg[c[i].m][c[i].n] != ClassLabel) {
					tmp = (struct pixelList*)malloc(sizeof(struct pixelList));
					tmp->pixel.m = c[i].m;
					tmp->pixel.n = c[i].n;
					tmp->next = NULL;
					tail->next = tmp;
					tail = tmp;
				}
			}
		}
		tmp = head->next;
		free(head);
		head = tmp;
	}
}