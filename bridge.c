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

#define MAX_CARS 3

#define TO_HANOVER 1
#define TO_NORWICH 0

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// taken basic structure from assignment details
void OneVehicle(direction)
{
  ArriveBridge(direction);
  // now the car is on the bridge!

  OnBridge(direction);

  ExitBridge(direction);
  // now the car is off
}