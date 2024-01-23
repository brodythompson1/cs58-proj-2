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

#define MAX_CARS 3

#define TO_HANOVER 1
#define TO_NORWICH 0

int num_cars; //total number of cars in simulation
int max_cars_on_bridge; //maximum number of cars that can be on the bridge at once

int toHanoverQueue;
// int toHanoverLength;
int toNorwichQueue;
// int toNorwichLength;

int carsOnBridge;



pthread_cond_t condToHanover;
pthread_cond_t condToNorwich;



pthread_mutex_t mutexCar = PTHREAD_MUTEX_INITIALIZER;

// does not return until it is safe for the car to get on the bridge
void arriveBridge(int carDirection)
{
  //lock
  if (pthread_mutex_lock(&mutexCar) != 0) {
    printf("Car mutex failed");
    exit(1);
  };

  if (carDirection == TO_HANOVER) {

    toHanoverQueue++;

    while(toNorwichQueue > 0 || carsOnBridge >= max_cars_on_bridge) {
      printf("Car going to Hanover arrived, waiting for green light\n");
      pthread_cond_wait(&condToHanover, &mutexCar);
    }
    toHanoverQueue--;
    printf("Car Heading To Hanover on Bridge\n");



  } else {

    toNorwichQueue++;

    while(toHanoverQueue > 0 || carsOnBridge >= max_cars_on_bridge) {
      printf("Car going to Norwich arrived, waiting for green light\n");
      pthread_cond_wait(&condToNorwich, &mutexCar);
    }
    toNorwichQueue--;
    printf("Car Heading To Norwich on Bridge\n");

  }
  //unlock
  if (pthread_mutex_unlock(&mutexCar) != 0) {
    printf("Car mutex failed");
    exit(1);
  };
  
}

// prints the state of the bridge and waiting cars, used to monitor behavior of the system
void onBridge(int direction)
{
  carsOnBridge++;
  if (direction == TO_HANOVER) {
    printf("To Hanover Bridge Direction\n");
  } else {
    printf("To Norwich Bridge Direction\n");
  }
  printf("Cars on Bridge: %d\n", carsOnBridge);
  printf("Cars in Norwich Queue: %d\n", toNorwichQueue);
  printf("Cars in Hanover Queue: %d\n", toHanoverQueue);
  
}

void exitBridge(int direction)
{
  
  


}

// taken basic structure from assignment details
void* OneVehicle(void* arg)
{ 
  intptr_t direction = (intptr_t)arg;

  //ensures vehicles are only going in the proper directions
  if (direction != TO_HANOVER || direction != TO_NORWICH) {
    printf("Direction is not To Hanover(1) or To Norwich(0)");
  }


  arriveBridge(direction);
  // now the car is on the bridge!

  onBridge(direction);

  exitBridge(direction);
  // now the car is off
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

  if (num_cars > MAX_CARS) {
    exit(1);
  }

  pthread_t carThreads[num_cars];

  int carDirection = rand() % 2;

  //initialize cars as threads
  for (int i = 0; i < num_cars; i++) {
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