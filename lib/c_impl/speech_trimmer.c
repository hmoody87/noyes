#include "ruby.h"
#include "noyes.h"
#include "rnoyes.h"

static int id_push;

VALUE cSpeechTrimmer;

static void speech_trimmer_free(void *p) {
  free_speech_trimmer(p);
}

static VALUE t_init(VALUE self) {
  SpeechTrimmer *st = new_speech_trimmer();
  VALUE stv = Data_Wrap_Struct(cSpeechTrimmer, 0, speech_trimmer_free, st);
  rb_iv_set(self, "@speech_trimmer", stv);
  return self;
}

static VALUE t_enqueue(VALUE self, VALUE obj) {
  NMatrix1 *M = v_2_nmatrix1(obj);
  SpeechTrimmer *st;
  Data_Get_Struct(rb_iv_get(self, "@speech_trimmer"), SpeechTrimmer, st);
  speech_trimmer_enqueue(st, M);
  return Qnil;
}

static VALUE t_dequeue(VALUE self) {
  SpeechTrimmer *st;
  Data_Get_Struct(rb_iv_get(self, "@speech_trimmer"), SpeechTrimmer, st);
  NMatrix1 *N =speech_trimmer_dequeue(st);
  VALUE result = nmatrix1_2_v(N);
  free_nmatrix1(N);
  return result;
}

static VALUE t_eos(VALUE self) {
  SpeechTrimmer *st;
  VALUE stv = rb_iv_get(self, "@speech_trimmer");
  Data_Get_Struct(stv, SpeechTrimmer, st);
  return speech_trimmer_dequeue(st) ? Qtrue : Qfalse;
}

void Init_speech_trimmer() {
  VALUE m_noyes_c = rb_define_module("NoyesC");
  cSpeechTrimmer = rb_define_class_under(m_noyes_c, "SpeechTrimmer", rb_cObject);
  rb_define_method(cSpeechTrimmer, "initialize", t_init, 0);
  rb_define_method(cSpeechTrimmer, "enqueue", t_enqueue, 1);
  rb_define_method(cSpeechTrimmer, "dequeue", t_dequeue, 0);
  rb_define_method(cSpeechTrimmer, "eos?", t_eos, 0);
  id_push = rb_intern("push");
}