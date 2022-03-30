
#ifndef EVALQUEUE
#include "EvalQueue.h"
#define EVALQUEUE
#endif
#ifndef QUEUENODE
#include "QueueNode.h"
#define QUEUENODE
#endif
#ifndef SIMULATION
#include "Simulation.h"
#define SIMULATION
#endif
#ifndef EVENTQUEUE
#include "EventQueue.h"
#define EVENTQUEUE
#endif
#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif
#ifndef MATH
#include <math.h>
#define MATH
#endif
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif



// Initializes the evaluation queue, setting the first arrival of each of the three priorities

struct EvalQueue* InitializeEvalQueue(struct EventQueue* eventQ, int numNurses, int seed, double highprilambda, double highprimu, double medprilambda, double medprimu, double lowprilambda, double lowprimu, double evalmu){
struct EvalQueue* newQueue = malloc(sizeof(struct EvalQueue));

    double highPriArr = ((-1/highprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
    double highPriSer = ((-1/highprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
    double medPriArr = ((-1/medprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
    double medPriSer = ((-1/medprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
    double lowPriArr = ((-1/lowprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
    double lowPriSer = ((-1/lowprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
    double highEvalSer = ((-1/evalmu) * log(1-((double) (rand()+1) / RAND_MAX)));
    double medEvalSer = ((-1/evalmu) * log(1-((double) (rand()+1) / RAND_MAX)));
    double lowEvalSer = ((-1/evalmu) * log(1-((double) (rand()+1) / RAND_MAX)));

    newQueue->nextHighPri = CreateNode(highPriArr, highPriSer, highEvalSer);
    (newQueue->nextHighPri)->priority = 3;
    struct EventQueueNode* high = CreateEvalArrivalEventNode(newQueue->nextHighPri);

    newQueue->nextMedPri = CreateNode(medPriArr, medPriSer, medEvalSer);
    (newQueue->nextMedPri)->priority = 2;
    struct EventQueueNode* med = CreateEvalArrivalEventNode(newQueue->nextMedPri);

    newQueue->nextLowPri = CreateNode(lowPriArr, lowPriSer, lowEvalSer);
    (newQueue->nextLowPri)->priority = 1;
    struct EventQueueNode* low = CreateEvalArrivalEventNode(newQueue->nextLowPri);
  if(highprilambda > 0) {
    eventQ->head = high;
    eventQ->tail = high;
    if(medprilambda > 0) {
      InsertIntoEventQueueInOrder(eventQ, med);
    }
    else {
      free(med);
    }
    if(lowprilambda > 0) {
      InsertIntoEventQueueInOrder(eventQ, low);
    }
    else {
      free(low);
    }
  }
  else if(medPriArr > 0) {
    free(high);
    eventQ->head = med;
    eventQ->tail = med;
    if(lowprilambda > 0) {
    InsertIntoEventQueueInOrder(eventQ, low);
    }
    else {
      free(low);
    }
  }
  else if(lowPriArr > 0) {
    free(high);
    free(med);
    eventQ->head = low;
    eventQ->tail = low;
  }
  else{
    printf("No arrival rates are above 0, so no one will arrive ever!");
    exit(1);
  }



    newQueue->availableNurses = numNurses;
    newQueue->cumulative_waiting = 0.0;
    newQueue->totalInSystem = 0;

  return newQueue;
}

// Function to process the arrival of a patient to the hospital.

void ProcessEvalArrival(struct EventQueue* eventQ, struct EvalQueue* evalQ, struct QueueNode* arrival, int seed, double highprilambda, double highprimu, double medprilambda, double medprimu, double lowprilambda, double lowprimu, double evalmu, int maxCapacity){

if(totalNumberInSystemNow < maxCapacity) {
  prevCurrentTime = current_time;
  current_time = arrival->eval_arrival_time;

      double evalSer = ((-1/evalmu) * log(1-((double) (rand()+1) / RAND_MAX)));

  if(arrival->priority == 3) {

      double highPriArr = current_time + ((-1/highprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double highPriSer = ((-1/highprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextHighPri = CreateNode(highPriArr, highPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextHighPri);
        InsertIntoEventQueueInOrder(eventQ, new);
      (new->qnode)->priority = 3;
  }

  else if(arrival->priority == 2) {

      double medPriArr = current_time + ((-1/medprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double medPriSer = ((-1/medprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextMedPri = CreateNode(medPriArr, medPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextMedPri);
      (new->qnode)->priority = 2;
        InsertIntoEventQueueInOrder(eventQ, new);
  }

  else if(arrival->priority == 1) {

      double lowPriArr = current_time + ((-1/lowprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double lowPriSer = ((-1/lowprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextLowPri = CreateNode(lowPriArr, lowPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextLowPri);
      (new->qnode)->priority = 1;
        InsertIntoEventQueueInOrder(eventQ, new);
  }

  totalNumberInSystemNow++;
  evalQ->totalInSystem++;

  if(evalQ->availableNurses > 0) {
    StartEvaluationService(eventQ, evalQ, arrival);
    DeleteEventNode(eventQ);
  }
  else {
    evalQ->waiting_count++;
    struct EventQueueNode* new = CreateEvalServiceEventNode(arrival);
    InsertIntoEventQueueInOrder(eventQ, new);
    DeleteEventNode(eventQ);
  }
}
else {
  numberOfTurnedAwayPatients++;

    srand(seed);
    double evalSer = ((-1/evalmu) * log(1-((double) (rand()+1) / RAND_MAX)));

  if(arrival->priority == 3) {

      double highPriArr = ((-1/highprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double highPriSer = ((-1/highprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextHighPri = CreateNode(highPriArr, highPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextHighPri);
      InsertIntoEventQueueInOrder(eventQ, new);
  }

  else if(arrival->priority == 2) {

      double medPriArr = ((-1/medprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double medPriSer = ((-1/medprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextMedPri = CreateNode(medPriArr, medPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextMedPri);
      InsertIntoEventQueueInOrder(eventQ, new);
  }

  else if(arrival->priority == 1) {

      double lowPriArr = ((-1/lowprilambda) * log(1-((double) (rand()+1) / RAND_MAX)));
      double lowPriSer = ((-1/lowprimu) * log(1-((double) (rand()+1) / RAND_MAX)));
      evalQ->nextLowPri = CreateNode(lowPriArr, lowPriSer, evalSer);
      struct EventQueueNode* new = CreateEvalArrivalEventNode(evalQ->nextLowPri);
      InsertIntoEventQueueInOrder(eventQ, new);
  }

}

}

// Function to start nurse evaluation

void StartEvaluationService(struct EventQueue* eventQ, struct EvalQueue* evalQ, struct QueueNode* servNode)
{
  DeleteServiceNode(eventQ);
  evalQ->availableNurses--;
  servNode->eval_waiting_time = current_time-(servNode->eval_arrival_time);
  evalQ->cumulative_waiting += servNode->eval_waiting_time;
  struct EventQueueNode* new = CreatePriorityArrivalEventNode(servNode);

if(evalQ->waiting_count > 0) {
  evalQ->waiting_count--;
}
  
  evalQ->totalInSystem--;
  InsertIntoEventQueueInOrder(eventQ, new);
}

// Free evaluation queue 

void FreeEvalQueue(struct EvalQueue* elementQ) {
  free(elementQ->nextHighPri);
  free(elementQ->nextLowPri);
  free(elementQ->nextMedPri);
  free(elementQ);
}