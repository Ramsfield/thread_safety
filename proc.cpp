#include<pthread.h>
#include<deque>
#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/wait.h>

struct targs{
  std::deque<int> buffer;
  pthread_cond_t cons_sig;
  pthread_mutex_t lock;
  bool flag;
};

/*
 * First Do a sanity check with functionality that should work
 * 
 * Add a mutex lock for synchronization. Any time anyone wants to buffer, get the lock
 */

void producer(targs * info)
{
  for(int i=0; i < 10000; ++i)
  {
    pthread_mutex_lock(&info->lock);
    info->buffer.push_back(i);
    pthread_cond_signal(&info->cons_sig);
    pthread_mutex_unlock(&info->lock);
  }
  info->flag = true;
}

void consumer(targs * info)
{
  while(true)
  {
    pthread_mutex_lock(&info->lock);
    while(info->buffer.size() == 0 && !info->flag)
    {
      pthread_cond_wait(&info->cons_sig, &info->lock);
    }
    if(info->buffer.size()) //Size no longer results to 0
    {
      int a = info->buffer.front(); //Grab item
      info->buffer.pop_front(); //Remove item
      pthread_mutex_unlock(&info->lock);
      std::cout << "Pulled " << a << " from deque" << std::endl;
    }
    else
    {
      pthread_mutex_unlock(&info->lock);
      return;
    }
  }
}


int main()
{
  targs * info;


  /* 
   * Initialize shared memory -- will need for the locks and signals
   * Shouldn't affect the deque since the deque should be a shared memory item itself
   */
  info = (targs *) mmap(NULL, sizeof(targs), PROT_READ|PROT_WRITE, 
      MAP_SHARED|MAP_ANONYMOUS, -1, 0);

  info->flag = false;

  //Init the cond var
  pthread_condattr_t condAttr;
  pthread_condattr_init(&condAttr);
  pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&info->cons_sig, &condAttr);

  //Init the lock
  pthread_mutexattr_t mutAttr;
  pthread_mutexattr_init(&mutAttr);
  pthread_mutexattr_setpshared(&mutAttr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&info->lock, &mutAttr);

  //Spin off processes
  if(!fork())
  {
    producer(info);
    exit(0);
  }
  if(!fork())
  {
    consumer(info);
    exit(0);
  }

  //Get the producer back
  wait(NULL);

  //Get the consumer back
  while(true)
  {
    //Fire to get consumer back
    pthread_cond_broadcast(&info->cons_sig);

    if(waitpid(-1, NULL, WNOHANG))
      break;
  }

  pthread_condattr_destroy(&condAttr);
  pthread_mutexattr_destroy(&mutAttr);

  pthread_mutex_destroy(&info->lock);
  pthread_cond_destroy(&info->cons_sig);

  munmap(info, sizeof(targs));
  return 0;
}
