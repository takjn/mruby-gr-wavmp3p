/*
** mrb_wavmp3p.c - WavMp3p class
**
** Copyright (c) Jun Takeda 2016
**
** See Copyright Notice in LICENSE
*/
#include <new>

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/string.h"
#include "mruby/data.h"

#include <Arduino.h>
#include <utilities.h>
#include <MsTimer2.h>
#include <WavMp3p.h>
#include <SD.h>

#define LED_ON		HIGH
#define LED_OFF		LOW

#define SW_PAUSE	5
#define SW_SKIP		4
#define SW_ON		  0
#define SW_OFF		1
#define DONE mrb_gc_arena_restore(mrb, 0);

WavMp3p *_wavmp3p;

static
void cyclic_handler(void);

void
mrb_wavmp3p_free(mrb_state *mrb, void *ptr)
{
  WavMp3p *wavmp3p = (WavMp3p *)ptr;
  wavmp3p->~WavMp3p();
  mrb_free(mrb, wavmp3p);
}

struct mrb_data_type mrb_wavmp3p_type = {"WavMp3p", mrb_wavmp3p_free};

mrb_value
mrb_wavmp3p_initialize(mrb_state *mrb, mrb_value self)
{
  unsigned long sf;
  mrb_get_args(mrb, "i", &sf);

  void *p = mrb_malloc(mrb, sizeof(WavMp3p));
  WavMp3p *newWavMp3p = new(p) WavMp3p(sf);
  DATA_PTR(self) = newWavMp3p;
  DATA_TYPE(self) = &mrb_wavmp3p_type;
  return self;
}

mrb_value
mrb_wavmp3p_init(mrb_state *mrb, mrb_value self)
{
  int pin_pause, pin_skip;

  mrb_get_args(mrb, "ii", &pin_pause, &pin_skip);

	pinMode(PIN_LED0, OUTPUT);

	if (!SD.begin()) {
    return mrb_bool_value(false);
	}

	pinMode(pin_pause, INPUT_PULLUP);
	pinMode(pin_skip, INPUT_PULLUP);

  return mrb_bool_value(true);
}

/* 
 * Play MP3/Wav file
 *  MP3.play(filename)
 *  filename: filename to play
 *
 *  return error message if error occurred
 */
mrb_value
mrb_wavmp3p_play(mrb_state *mrb, mrb_value self)
{
  WavMp3p *wavmp3p = (WavMp3p *)mrb_get_datatype(mrb, self, &mrb_wavmp3p_type);
  _wavmp3p = wavmp3p;

  mrb_value vfpath;
  char *fpath;

	mrb_get_args(mrb, "S", &vfpath);

  fpath = mrb_str_to_cstr(mrb, vfpath);

	// start timer for pause and stop interrupt
	MsTimer2::set(100, cyclic_handler);
	MsTimer2::start();

	digitalWrite(PIN_LED0, LED_ON);
	char *res = wavmp3p->play(fpath);
	digitalWrite(PIN_LED0, LED_OFF);

	// stop timer
	MsTimer2::stop();

	return mrb_str_new_cstr(mrb, (const char*)res);
}

extern "C"
void mrb_mruby_gr_wavmp3p_gem_init(mrb_state *mrb)
{

  RClass *wavMp3p = mrb_define_class(mrb, "WavMp3p", mrb->object_class);
  MRB_SET_INSTANCE_TT(wavMp3p, MRB_TT_DATA);
  mrb_define_method(mrb, wavMp3p, "initialize", mrb_wavmp3p_initialize, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, wavMp3p, "init", mrb_wavmp3p_init, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, wavMp3p, "play", mrb_wavmp3p_play, MRB_ARGS_REQ(1));

  DONE;
}

extern "C"
void mrb_mruby_gr_wavmp3p_gem_final(mrb_state *mrb)
{
}

// The below handler enables to skip or stop.
static
void cyclic_handler()
{
	static int ct_pause = 0;

	if(SW_ON == digitalRead(SW_PAUSE)){
		if(ct_pause == 2){
			if(_wavmp3p->read_pause()){
				_wavmp3p->pause(0);
				digitalWrite(PIN_LED0, LED_ON);
			}else{
				_wavmp3p->pause(1);
				digitalWrite(PIN_LED0, LED_OFF);
			}
			ct_pause++;
		}else if(ct_pause < 2){
			ct_pause++;
		}
	}else{
		ct_pause = 0;
	}

	static int ct_skip = 0;

	if(SW_ON == digitalRead(SW_SKIP)){
		if(ct_skip == 2){
			_wavmp3p->skip();
			digitalWrite(PIN_LED0, LED_ON);
			ct_skip++;
		}else if(ct_skip < 2){
			ct_skip++;
		}
	}else{
		ct_skip = 0;
	}
}
