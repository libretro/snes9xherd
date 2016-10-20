#include <rthreads/rthreads.h>
#include <rthreads/rsemaphore.h>
#include <retro_miscellaneous.h>
#include "snes9x.h"
#include "fxemu.h"

static sthread_t* cuttlefishthread;
static int keeprunning;
static ssem_t* cuttle_start;
static ssem_t* cuttle_done;

static void (*tasks[50])();
static int remainingtasks;

static void cuttlelikeafish(void* unusedpointer){
   while(1){
      ssem_wait(cuttle_start);
      if(!keeprunning)break;
      while(remainingtasks >= 0){
         tasks[remainingtasks]();
         remainingtasks--;
      }
      ssem_signal(cuttle_done);
   }
}

int initthreads(){
   remainingtasks = -1;
   keeprunning = 1;
   cuttlefishthread = sthread_create(cuttlelikeafish,NULL);
   cuttle_start = ssem_new(0);
   cuttle_done = ssem_new(0);
   return !cuttlefishthread || !cuttle_start || !cuttle_done;
}

void stopthreads(){
   remainingtasks = -1;
   keeprunning = 0;
   ssem_signal(cuttle_start);
   sthread_join(cuttlefishthread);
   ssem_free(cuttle_start);
   ssem_free(cuttle_done);
}

void run_cuttle(){
   ssem_signal(cuttle_start);
}

void wait_cuttle(){
   ssem_wait(cuttle_done);
}

void queuefunction(void (*func)(void)){
   remainingtasks++;
   tasks[remainingtasks] = func;
}
