#include <rthreads/rthreads.h>
#include <retro_miscellaneous.h>
#include "snes9x.h"
#include "fxemu.h"
#include "apu.h"

static int keeprunning;//used to terminate all threads

static sthread_t* superfxthread;
static slock_t* sfxmutex;
static void runsuperfx(void* unusedpointer){
   while(1){
      slock_lock(sfxmutex);
      if(!keeprunning)break;
      S9xSuperFXExec();
      slock_unlock(sfxmutex);
   }
}

static sthread_t* aputhread;
static slock_t* apumutex;
static void runapu(void* unusedpointer){
   while(1){
      slock_lock(apumutex);
      if(!keeprunning)break;
      S9xAPUExecute();
      slock_unlock(apumutex);
   }
}

int initthreads(){
   keeprunning = 1;

   sfxmutex = slock_new();
   if(!sfxmutex)return FALSE;
   slock_lock(sfxmutex);
   superfxthread = sthread_create(runsuperfx,NULL);
   if(!superfxthread)return FALSE;
   
   apumutex = slock_new();
   if(!apumutex)return FALSE;
   slock_lock(apumutex);
   aputhread = sthread_create(runapu,NULL);
   if(!aputhread)return FALSE;

   return TRUE;
}

void stopthreads(){
   keeprunning = 0;
   
   //wait for threads to finish there current iteration
   slock_lock(sfxmutex);
   slock_lock(apumutex);
   
   //threads are now done,call them one last time to tell them to exit
   slock_unlock(sfxmutex);
   slock_unlock(apumutex);
   
   sthread_join(superfxthread);
   sthread_join(aputhread);
   slock_free(sfxmutex);
   slock_free(apumutex);
}

void superfxGO(){
   slock_unlock(sfxmutex);
}

void superfxWAIT(){
   slock_lock(sfxmutex);
}

void apuGO(){
   slock_unlock(apumutex);
}

void apuWAIT(){
   slock_lock(apumutex);
}
