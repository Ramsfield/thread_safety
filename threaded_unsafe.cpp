#include<pthread.h>
#include<deque>
#include<iostream>

#define THREADS 2

struct targs{
  std::deque<int> buffer;
};

/*
 * Rework just to have N threads pushing into a deque all at the same time
 */

void thread_fun(targs * info)
{
  for(int i=0; i < 10000; ++i)
  {
    info->buffer.push_back(i);
  }
}


int main()
{
  targs info;

  pthread_t threads[THREADS];

  for(int i=0; i < THREADS; ++i)
  {
    pthread_create(&threads[i], NULL, (void *(*)(void *))thread_fun, &info);
  }

  //Join them back
  for(int i=0; i < THREADS; ++i)
  {
    pthread_join(threads[i], NULL);
  }

  //Print deque
  while(info.buffer.size())
  {
    int a = info.buffer.front();
    info.buffer.pop_front();
    std::cout << a << std::endl;
  }
  return 0;
}
