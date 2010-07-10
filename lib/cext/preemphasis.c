#include "ruby.h"
#include "c_noyes.h"
#include "r_noyes.h"

static int id_push;

VALUE cPreemphasizer;

static void preemphasizer_free(void *p) {
  free_preemphasizer(p);
}

static VALUE t_init(VALUE self, VALUE args) {
  int len = RARRAY_LEN(args);
  double factor = 0.97;
  if (len > 0) {
     factor = NUM2DBL(rb_ary_entry(args, 0));
  }
  Preemphasizer *pre = new_preemphasizer(factor);
  VALUE prev = Data_Wrap_Struct(cPreemphasizer, 0, preemphasizer_free, pre);
  rb_iv_set(self, "@preemphasizer", prev);
  return self;
}

static VALUE t_left_shift(VALUE self, VALUE obj) {
  Carr *M = r2carr(obj);
  Preemphasizer *pre;
  VALUE prev = rb_iv_get(self, "@preemphasizer");
  Data_Get_Struct(prev, Preemphasizer, pre);
  Carr *N = preemphasizer_apply(pre, M);
  VALUE result = carr2r(N);
  narr_free(N);
  narr_free(M);
  return result;
}

void Init_preemphasis() {
  VALUE m_noyes_c = rb_define_module("NoyesC");
  cPreemphasizer = rb_define_class_under(m_noyes_c, "Preemphasizer", rb_cObject);
  rb_define_method(cPreemphasizer, "initialize", t_init, -2);
  rb_define_method(cPreemphasizer, "<<", t_left_shift, 1);
  id_push = rb_intern("push");
}