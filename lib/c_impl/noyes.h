#include "stdlib.h"

// Matrix handling routines.
typedef struct {
  double **data;
  int rows;
  int cols;
} NMatrix;

typedef struct {
  double ***data;
  int rows;
  int cols;
  int z;
} NMatrix3;

NMatrix *new_nmatrix(int rows, int cols);
void free_nmatrix(NMatrix *);

// Segmenter
typedef struct {
  double * buf;
  size_t buflen;
  int winsz;
  int winshift;
} Segmenter;

Segmenter * new_segmenter(int winsz, int winshift);
void free_segmenter(Segmenter *s);
NMatrix * segmenter_apply(Segmenter* self, double * data, int datalen);

// Hamming Window
typedef struct {
  double * buf;
  size_t buflen;
} HammingWindow;

HammingWindow * new_hamming_window(int window_size);
void free_hamming_window(HammingWindow *s);
NMatrix * hamming_window_apply(HammingWindow* self, NMatrix *data);

// Wrapper stuff.  Only ruby related stuff below here.
#include "ruby.h"
void Init_preemphasis();
void Init_segmenter();
void Init_hamming_window();

VALUE nmatrix_2_v(NMatrix *d);
NMatrix * v_2_nmatrix(VALUE value);

