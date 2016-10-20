int initthreads();
void stopthreads();

void run_cuttle();
void wait_cuttle();

//do NOT queue new funcions between run_cuttle and wait_cuttle,it may corrupt the task queue
void queuefunction(void (*func)(void));
