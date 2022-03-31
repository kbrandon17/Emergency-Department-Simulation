 #ifndef SIMULATION
 #include "Simulation.h"
 #define SIMULATION
 #endif
 #ifndef EVALQUEUE
 #include "EvalQueue.h"
 #define EVALQUEUE
 #endif
 #ifndef EVENTQUEUE
 #include "EventQueue.h"
 #define EVENTQUEUE
 #endif
 #ifndef STDLIB
 #include <stdlib.h>
 #define STDLIB
 #endif
 #ifndef STDDEF
 #include <stddef.h>
 #define STDDEF
 #endif
 #ifndef MATH
 #include <math.h>
 #define MATH
 #endif
 #ifndef STDIO
 #include <stdio.h>
 #define STDIO
 #endif
 
 // ------------Simulation variables------------------------------------------------------

    double current_time = 0.0;          // current time during simulation (minutes past 12AM)
    double prevCurrentTime = 0.0;       // to store previous current time to calculate averages
    int hoursPassed;                    // stores number of hours passed to know whether to print stats
    int departure_count;                // total departures of patients leaving hospital
    double avgInSystem;                 // average number of patients in system
    double avgResponseTimeAll;          // average response time of all patients
    double avgResponseTimeHigh;         // average response time of high priority patients
    double avgResponseTimeMed;          // average response time of medium priority patients
    double avgResponseTimeLow;          // average response time of low priority patients
    double avgEvalWaitingTime;          // average waiting time in evaluation queue
    double avgPriorityWaitingTimeAll;   // average waiting time in priority queue of all patients
    double avgPriorityWaitingTimeHigh;  // average waiting time in priority queue of high priority patients
    double avgPriorityWaitingTimeMed;   // average waiting time in priority queue of medium priority patients
    double avgPriorityWaitingTimeLow;   // average waiting time in priority queue of low priority patients
    double avgCleanUpTime;              // average time to clean up the patient room
    int numberOfTurnedAwayPatients;     // total number of turned away patients due to full capacity
    int totalNumberInSystemNow;         // tracks total number of patients in hospital at given moment

// Printing out the report of statistics at every hour

void PrintStatistics(struct Queue* elementQ, struct EvalQueue* evalQ, int hoursPassed){

   if(current_time >= 1440) {
     printf("\nEnd of Simulation - at 12AM the next day:\n");
   }
   else printf("\nAt %d O'Clock:\n", hoursPassed);


  printf("Total departures: %d\n", departure_count);
  printf("Average in system: %.2f\n", avgInSystem);
  printf("Average response time for all patients: %.2f\n", avgResponseTimeAll);
  printf("Average response time for high priority patients: %.2f\n", avgResponseTimeHigh);
  printf("Average response time for medium priority patients: %.2f\n", avgResponseTimeMed);
  printf("Average response time for low priority patients: %.2f\n", avgResponseTimeLow);
  printf("Average Evaluation Waiting Time: %.2f\n", avgEvalWaitingTime);
  printf("Average waiting time for all priority patients: %.2f\n", avgPriorityWaitingTimeAll);
  printf("Average waiting time for high priority patients: %.2f\n", avgPriorityWaitingTimeHigh);
  printf("Average waiting time for medium priority patients: %.2f\n", avgPriorityWaitingTimeMed);
  printf("Average waiting time for low priority patients: %.2f\n", avgPriorityWaitingTimeLow);
  printf("Average cleaning time for patient rooms: %.2f\n", avgCleanUpTime);
  printf("Number of turned away patients due to max capacity: %d\n", numberOfTurnedAwayPatients);

}

// This is the main simulator function
// Runs until 24 hours (1440 minutes)
// Determines what the next event is based on current_time
// Print statistics if current time has passed a full hour

void Simulation(int random_seed, struct EventQueue* eventQ, struct EvalQueue* evalQ, struct Queue* priorityQ, int numNurses, double highPriLambda, double highPriMu, double medPriLambda, double medPriMu, double lowPriLambda, double lowPriMu, double evalMu, double cleanMu, int numJanitors, int numRooms, int maxCapacity)
{
  while(current_time < 1440) {

    if((eventQ->head)->event_type == 1) {
      ProcessEvalArrival(eventQ, evalQ, (eventQ->head)->qnode, random_seed, highPriLambda, highPriMu, medPriLambda, medPriMu, lowPriLambda, lowPriMu, evalMu, maxCapacity);
    }
    else if(((eventQ->head)->event_type == 2) || (eventQ->head)->event_type == 4) {
      struct EventQueueNode* curr = eventQ->head;
      while(curr != NULL) {
        if(curr->event_type == 1) {
          ProcessEvalArrival(eventQ, evalQ, curr->qnode, random_seed, highPriLambda, highPriMu, medPriLambda, medPriMu, lowPriLambda, lowPriMu, evalMu, maxCapacity);
          break;
        }
        else if(curr->event_type == 3) {
          ProcessPriorityArrival(eventQ, evalQ, priorityQ, curr->qnode);
          StartRoomService(eventQ, priorityQ, highPriMu, medPriMu, lowPriMu);
          DeleteEventNode(eventQ);
          break;
        }
        else if(curr->event_type == 5) {
          ProcessPatientDeparture(eventQ, priorityQ, curr->qnode, cleanMu);
          DeleteEventNode(eventQ);
          break;
        }
        else if(curr->event_type == 6) {
          JanitorCleanedRoom(eventQ, priorityQ);
          StartRoomService(eventQ, priorityQ, highPriMu, medPriMu, lowPriMu);
          DeleteEventNode(eventQ);
          break;
        }
        curr = curr->next;
      }
    }
    else if((eventQ->head)->event_type == 3) {
      ProcessPriorityArrival(eventQ, evalQ, priorityQ, (eventQ->head)->qnode);
      DeleteEventNode(eventQ);
      StartRoomService(eventQ, priorityQ, highPriMu, medPriMu, lowPriMu);
    }
    else if((eventQ->head)->event_type == 5) {
      ProcessPatientDeparture(eventQ, priorityQ, (eventQ->head)->qnode, cleanMu);
      DeleteEventNode(eventQ);
    }
    else if((eventQ->head)->event_type == 6) {
      JanitorCleanedRoom(eventQ, priorityQ);
      DeleteEventNode(eventQ);
      StartRoomService(eventQ, priorityQ, highPriMu, medPriMu, lowPriMu);
    }
    while(eventQ->head->event_time > hoursPassed * 60) {
      hoursPassed++;
      PrintStatistics(priorityQ, evalQ, hoursPassed);
    }
  }
  //PrintStatistics(priorityQ, evalQ);
}