/**
 * Brody Thompson
 * CS58
 * Ledyard Bridge Construction Project
 * Implements Threads and Locks
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/random.h>

#define MAX_CARS 100

#define TO_HANOVER 1
#define TO_NORWICH 0

int num_cars; //total number of cars in simulation
int max_cars_on_bridge; //maximum number of cars that can be on the bridge at once

int toHanoverQueue;
int toNorwichQueue;

int carsOnBridge;
int bridgeDirection;



pthread_cond_t condToHanover;
pthread_cond_t condToNorwich;

pthread_t tid;
int thread_id;

pthread_mutex_t mutexCar = PTHREAD_MUTEX_INITIALIZER;

void random_sleep(int me, int min,int max) {

  int rn;
  int delay;
  int delta;
  int mod;

  getrandom(&rn,sizeof(int),0);

  delta = max-min;
  mod = abs(rn) % delta;
  delay = min + mod;

  sleep(delay);

}

// does not return until it is safe for the car to get on the bridge
void arriveBridge(int carDirection)
{
  //lock
  if (pthread_mutex_lock(&mutexCar) != 0) {
    printf("Car mutex failed");
    exit(1);
  };

  if(bridgeDirection == -1 || (toHanoverQueue == 0 && toNorwichQueue == 0 && carsOnBridge == 0)) {
    bridgeDirection = carDirection;
  }

  if (carDirection == TO_HANOVER) {

    toHanoverQueue++;

    while(bridgeDirection == TO_NORWICH || carsOnBridge >= max_cars_on_bridge) {
      printf("Car going to Hanover arrived, waiting for green light\n");
      printf("Cars in To Hanover Queue: %d\n", toHanoverQueue);
      pthread_cond_wait(&condToHanover, &mutexCar);
    }
    toHanoverQueue--;
    carsOnBridge++;
    bridgeDirection = 1;
    printf("Car Heading To Hanover on Bridge\n");
    printf("Cars on bridge now: %d\n", carsOnBridge);
    
    // pthread_mutex_unlock(&mutexCar);



  } else {

    toNorwichQueue++;

    while(bridgeDirection == TO_HANOVER || carsOnBridge >= max_cars_on_bridge) {
      printf("Car going to Norwich arrived, waiting for green light\n");
      printf("Cars in To Norwich queue: %d\n", toNorwichQueue);
      pthread_cond_wait(&condToNorwich, &mutexCar);
    }
    toNorwichQueue--;
    carsOnBridge++;
    bridgeDirection = 0;
    printf("Car Heading To Norwich on Bridge\n");
    printf("Cars on bridge now: %d\n", carsOnBridge);

  }
  // unlock
  if (pthread_mutex_unlock(&mutexCar) != 0) {
    printf("Car mutex failed");
    exit(1);
  };
  
}

// prints the state of the bridge and waiting cars, used to monitor behavior of the system
void onBridge(int direction)
{
  // pthread_mutex_lock(&mutexCar);
  // carsOnBridge++;


  tid = pthread_self();

    // Cast the thread ID to an integer (optional)
  thread_id = (int)tid;

  random_sleep(thread_id, 1, 5);



  printf("Cars on Bridge: %d\n", carsOnBridge);
  printf("Cars in Norwich Queue: %d\n", toNorwichQueue);
  printf("Cars in Hanover Queue: %d\n", toHanoverQueue);

  if(bridgeDirection == TO_HANOVER && carsOnBridge < max_cars_on_bridge && toHanoverQueue > 0) {
    pthread_cond_signal(&condToHanover);
  }
  if (bridgeDirection == TO_NORWICH && carsOnBridge < max_cars_on_bridge && toNorwichQueue > 0) {
    pthread_cond_signal(&condToNorwich);
  }
  
}

void exitBridge(int direction)
{
  
  if (pthread_mutex_lock(&mutexCar) != 0) {
    printf("Car mutex failed");
    exit(1);
  };

  

  if (direction == TO_HANOVER) {
    carsOnBridge--;
    printf("Car exiting bridge towards hanover\n");

    if (toHanoverQueue == 0 && carsOnBridge == 0 && toNorwichQueue > 0) {
      bridgeDirection = 0;
      pthread_mutex_unlock(&mutexCar);
      pthread_cond_signal(&condToNorwich);
    } else {
      bridgeDirection = 1;
      pthread_mutex_unlock(&mutexCar);
      pthread_cond_signal(&condToHanover);
    }

  } else {
    carsOnBridge--;
    printf("Car exiting bridge towards norwich\n");
    if (toNorwichQueue == 0 && carsOnBridge == 0 && toHanoverQueue > 0) {
      bridgeDirection = 1;
      pthread_mutex_unlock(&mutexCar);
      pthread_cond_signal(&condToHanover);
    } else {
      bridgeDirection = 0;
      pthread_mutex_unlock(&mutexCar);
      pthread_cond_signal(&condToNorwich);
    }
  }

}

// taken basic structure from assignment details
void* OneVehicle(void* arg)
{ 
  intptr_t direction = (intptr_t)arg;

  tid = pthread_self();

    // Cast the thread ID to an integer (optional)
  thread_id = (int)tid;

  random_sleep(thread_id, 1, 5);

  arriveBridge(direction);

  onBridge(direction);

  exitBridge(direction);
  // now the car is off
  return NULL;
}

// program can be ran with parameter: (Number of cars, Max Cars on bridge)
int main(int argc, char* argv[])
{
  // makes sure that the number of cars requested isn't more than the simulation can handle

  num_cars = atoi(argv[1]);
  max_cars_on_bridge = atoi(argv[2]);


  toHanoverQueue= 0;
  toNorwichQueue = 0;
  carsOnBridge = 0;
  bridgeDirection = -1;

  if (num_cars > MAX_CARS) {
    exit(1);
  }

  pthread_t carThreads[num_cars];


  //initialize cars as threads
  for (int i = 0; i < num_cars; i++) {
    int carDirection = rand() % 2;
    // printf("Direction: %d\n", carDirection);
    if (pthread_create(&carThreads[i], NULL, &OneVehicle, (void*)(intptr_t)carDirection) != 0) {
      printf("Failed to create thread");
    }
  } 

  // Join and wait for threads to complete
  for (int i = 0; i < num_cars; i++) {
    if (pthread_join(carThreads[i], NULL) != 0) {
      printf("Error joining threads");
    }
  }


  return 0;
}