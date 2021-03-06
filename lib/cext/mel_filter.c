#include "c_noyes.h"
#include "r_noyes.h"
#include "math.h"

VALUE cMelFilter;

static void _mel_filter_free(void *p) {
  mel_filter_free(p);
}

static VALUE t_make_filter(VALUE self, VALUE(left), VALUE(center), VALUE(right), VALUE(init_freq), VALUE(delta)) {
  Carr *d = make_filter(NUM2DBL(left), NUM2DBL(center),
                NUM2DBL(right), NUM2DBL(init_freq), NUM2DBL(delta)); 
  if (d) {
    VALUE result = rb_ary_new2(2);
    rb_ary_store(result, 0, INT2FIX(round(d->data[0])));
    int i;
    VALUE filt = rb_ary_new2(d->rows-1);
    for (i=1;i<d->rows;++i) {
      rb_ary_store(filt, i-1, rb_float_new(d->data[i]));
    }
    rb_ary_store(result, 1, filt);
    carr_free(d);
    return result;
  }
  return Qnil;
}

static VALUE t_to_mel(VALUE self, VALUE m) {
  return rb_float_new(mel(NUM2DBL(m)));
}

static VALUE t_to_linear(VALUE self, VALUE f) {
  return rb_float_new(melinv(NUM2DBL(f)));
}

static VALUE t_make_bank_parameters(VALUE self, VALUE srate, VALUE nfft,
                                    VALUE nfilt, VALUE lowerf, VALUE upperf) {
  Cmat *d = make_bank_parameters(NUM2INT(srate), NUM2INT(nfft),
                NUM2INT(nfilt), NUM2INT(lowerf), NUM2INT(upperf)); 
  if (d) {
    VALUE result = rb_ary_new2(d->rows);
    int i,j;
    for (i=0;i<d->rows;++i) {
      VALUE row = rb_ary_new2(d->cols);
      rb_ary_store(result, i, row);
      for (j=0;j<d->cols;++j) {
        rb_ary_store(row, j, rb_float_new(d->data[i][j]));
      }
    }
    cmat_free(d);
    return result;
  }
  return Qnil;
}

static VALUE t_init(VALUE self, VALUE args) {
  int srate=8000, nfft=256, nfilt=32, lowerf=200, upperf=3700;
  int len = RARRAY_LEN(args);
  if (len > 0)
     srate = NUM2INT(rb_ary_entry(args, 0));
  if (len > 1)
    nfft = NUM2INT(rb_ary_entry(args, 1));
  if (len > 2)
    nfilt = NUM2INT(rb_ary_entry(args, 2));
  if (len > 3)
    lowerf = NUM2INT(rb_ary_entry(args, 3));
  if (len > 4)
    upperf = NUM2INT(rb_ary_entry(args, 4));

  MelFilter *s = new_mel_filter(srate, nfft, nfilt, lowerf, upperf);
  VALUE mel_filter = Data_Wrap_Struct(cMelFilter, 0, _mel_filter_free, s);
  rb_iv_set(self, "@mel_filter", mel_filter);

  return self;
}

static VALUE t_left_shift(VALUE self, VALUE obj) {
  Cmat *M = r2cmat(obj);
  VALUE mel_filter = rb_iv_get(self, "@mel_filter");
  MelFilter *s;
  Data_Get_Struct(mel_filter, MelFilter, s);
  Cmat *d = mel_filter_apply(s, M);
  cmat_free(M);

  if (d) {
    VALUE result = rb_ary_new2(d->rows);
    int i,j;
    for (i=0;i<d->rows;++i) {
      VALUE row = rb_ary_new2(d->cols);
      rb_ary_store(result, i, row);
      for (j=0;j<d->cols;++j) {
        rb_ary_store(row, j, rb_float_new(d->data[i][j]));
      }
    }
    cmat_free(d);
    return result;
  }
  return Qnil;
}

void Init_mel_filter() {
  VALUE m_noyes_c = rb_define_module("NoyesC");
  cMelFilter = rb_define_class_under(m_noyes_c, "MelFilter", rb_cObject);
  rb_define_method(cMelFilter, "initialize", t_init, -2);
  rb_define_method(cMelFilter, "<<", t_left_shift, 1);
  rb_define_module_function(cMelFilter, "make_bank_parameters", t_make_bank_parameters, 5);
  rb_define_module_function(cMelFilter, "make_filter", t_make_filter, 5);
  rb_define_module_function(cMelFilter, "to_linear", t_to_linear, 1);
  rb_define_module_function(cMelFilter, "to_mel", t_to_mel, 1);
}
