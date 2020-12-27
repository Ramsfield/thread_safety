#include<pthread.h>
#include<deque>
#include<iostream>

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
  targs info;

  info.flag = false;

  //Init the cond var
  pthread_cond_init(&info.cons_sig, NULL);

  //Init the lock
  pthread_mutex_init(&info.lock, NULL);

  pthread_t threads[2];

  //Spin off threads
  pthread_create(threads, NULL, (void * (*)(void*)) &producer, &info);
  pthread_create(threads+1, NULL, (void * (*)(void*)) &consumer, &info);

  //Join them back
  pthread_join(threads[0], NULL);
  //Ensure Consumer coming back -- Flag **IS** set so the producer can't wait anymore
  pthread_cond_broadcast(&info.cons_sig);

  pthread_join(threads[1], NULL);

  pthread_mutex_destroy(&info.lock);
  pthread_cond_destroy(&info.cons_sig);
  return 0;
}
