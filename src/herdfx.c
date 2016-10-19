#include <rthreads/rthreads.h>
#include <rthreads/rsemaphore.h>
#include <retro_miscellaneous.h>
#include "snes9x.h"
#include "fxemu.h"

static sthread_t* cuttlefishthread;
static int keeprunning;
static ssem_t* cuttle_startframe;
static ssem_t* cuttle_framedone;

static void superfxcuttlefish(void* unusedpointer){
   while(keeprunning){
      ssem_wait(cuttle_startframe);
      S9xSuperFXExec();
      ssem_signal(cuttle_framedone);
   }
}

int startSFXcuttlefish(){
   //S9xSuperFXExec(); on seprate thread
   keeprunning = 1;
   cuttlefishthread = sthread_create(superfxcuttlefish,NULL);
   cuttle_startframe = ssem_new(0);
   cuttle_framedone = ssem_new(0);
   return !cuttlefishthread || !cuttle_startframe || !cuttle_framedone;
}

void stopSFXcuttlefish(){
   keeprunning = 0;
   ssem_signal(cuttle_startframe);
   sthread_join(cuttlefishthread);
   ssem_free(cuttle_startframe);
   ssem_free(cuttle_framedone);
}

void sfxrun_cuttle(){
   ssem_signal(cuttle_startframe);
}

void sfxwait_cuttle(){
   ssem_wait(cuttle_framedone);
}
