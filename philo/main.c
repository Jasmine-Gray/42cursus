/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mishimod <mishimod@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 14:46:13 by mishimod          #+#    #+#             */
/*   Updated: 2025/09/03 15:19:47 by mishimod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int a = 0;
pthread_mutex_t mutex;

void *func_thread(void *p) {
  printf("start %d\n", *(int*)p);

  int i=0;
  for(i=0; i < 10000; i++){
    pthread_mutex_lock(&mutex);
	int next = a + 1;
	int now = a;
	a = next;
    pthread_mutex_unlock(&mutex);

	if (now+1 != next) {
	  printf("other theard change %d %d\n", a+1, next);
	}
  }

  return 0;
}

int main(void) {
  pthread_mutex_init(&mutex, NULL);
  
  printf("test\n");

  int b = 42;

  pthread_t pthread;
  pthread_create( &pthread, NULL, &func_thread, &b);

  int i=0;
  for(i=0; i < 10000; i++){
    pthread_mutex_lock(&mutex);
	int next = a + 1;
	int now = a;
	a = next;
    pthread_mutex_unlock(&mutex);

	if (now+1 != next) {
	  printf("other theard change %d %d\n", a+1, next);
	}
  }

  pthread_join(pthread, NULL); // pthreadで作られたスレッドが終わるまで待つ
  printf("a=%d\n", a);

  return 0;
}
