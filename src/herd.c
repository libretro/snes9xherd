#include <rthreads/rthreads.h>
#include <rthreads/rsemaphore.h>
#include <retro_miscellaneous.h>
#include "snes9x.h"
#include "fxemu.h"
#include "apu.h"

static int keeprunning;//used to terminate all threads

static sthread_t* superfxthread;
static ssem_t* superfx_start;
static ssem_t* superfx_done;
static void runsuperfx(void* unusedpointer){
   while(1){
      ssem_wait(superfx_start);
      if(!keeprunning)break;
      S9xSuperFXExec();
      ssem_signal(superfx_done);
   }
}

static sthread_t* aputhread;
static ssem_t* apu_start;
static ssem_t* apu_done;
static void runapu(void* unusedpointer){
   while(1){
      ssem_wait(apu_start);
      if(!keeprunning)break;
      S9xAPUExecute();
      ssem_signal(apu_done);
   }
}

int initthreads(){
   keeprunning = 1;
   
   superfx_start  = ssem_new(0);
   superfx_done  = ssem_new(0);
   if(!superfx_start || !superfx_done)return FALSE;
   superfxthread = sthread_create(runsuperfx,NULL);
   if(!superfxthread)return FALSE;
   
   apu_start  = ssem_new(0);
   apu_done  = ssem_new(0);
   if(!apu_start || !apu_done)return FALSE;
   aputhread = sthread_create(runapu,NULL);
   if(!aputhread)return FALSE;
   
   return TRUE;
}

void stopthreads(){
   keeprunning = 0;
   ssem_signal(superfx_start);
   ssem_signal(apu_start);
   sthread_join(superfxthread);
   sthread_join(aputhread);
}

void superfxGO(){
   ssem_signal(superfx_start);
}

void superfxWAIT(){
   ssem_wait(superfx_done);
}

void apuGO(){
   ssem_signal(apu_start);
}

void apuWAIT(){
   ssem_wait(apu_done);
}
