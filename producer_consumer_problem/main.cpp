/*///////////////////////////////////////////////////////////////////////////*/
/*---------------------------------------------------------------------------*/
/*                          D O C U M E N T A T I O N                        */
//---------------------------------------------------------------------------*/
//  Filename:           main.cpp
//  Program:            Producer-Consumer Problem With Threads
//  Title:              Main Program
//
//  Author:             Ibeawuchi Anokam
//  Date of Creation:   03/10/20
//  Version:            1.0.0
//  Revised by:         n/a
//  Revision Date:      n/a
//
//  Copyright:          Copyright © 2020 Ibeawuchi Anokam. 
//                      All rights reserved.
//
//-------------------------------------------------
// PROGRAM DESCRIPTION                            :
//-------------------------------------------------
//
// This is the Producer-consumer problem where there may be 
// multiple producers and consumers accessing a bounded buffer, 
// each of which is running in a thread.
//
//
/*///////////////////////////////////////////////////////////////////////////*/
/*---------------------------------------------------------------------------*/
/*                        G L O B A L    S E C T I O N                       */
//---------------------------------------------------------------------------*/
//------------------------
// System-Defined        :
// Libraries             :
//------------------------
#include <iostream>                        // Cin / Cout Stream

#include <ctime>                           // Enables The Use Of time()

#include <thread>                          // Allows The Program To Have Threading Support
#include <mutex>                           // Allows For Multi-Process Synchronization
#include <condition_variable>              // Allows For Thread Notifications
using namespace std;


//------------------------
// User-Defined          :
// MACROS                :
//------------------------
#define BUFFER_SIZE 10                     // Maximum Buffer Size
#define MAX_THREADS 5                      // Maximum Number Of Threads


//------------------------
// Buffer Variable       :
//------------------------
static int bounded_buffer[ BUFFER_SIZE ];  // Bounded Buffer
static int in_index                  = 0;  // Index Value for Buffer Inputs
static int out_index                 = 0;  // Index Value for Buffer Outputs
static int num_of_buffer_items       = 0;  // Number of Items Inserted Into The Buffer
//------------------------
// Mutable Variables     :
//------------------------
std::mutex synch;                          // Mutal Exclusion Synchronization Primitive-
                                           // Protects Shared Data

std::condition_variable synch_cv;          // Condition Variable Signal-
                                           // Blocks Threads


//------------------------
// Function Prototype(s) :
//------------------------
int  random_task_generator( );              // Generates the  Number of Tasks
                                            // A Users Wants to Employ

void producer(int tasks_in);                // Takes In A Random Number Of
                                            // Tasks (Data) to Deploy

void consumer(int thread_order_val);        // Consumes The Random Number Of
                                            // Tasks (Data) Being Deploy




/*///////////////////////////////////////////////////////////////////////////*/
/*---------------------------------------------------------------------------*/
/*                           M A I N    M O D U L E                          */
/*---------------------------------------------------------------------------*/
//------------------------
// Start Application     :
//------------------------
int main( ){
    
    //--------------------------------------------------
    // Thread Variable Initializations                 :
    //--------------------------------------------------
    std::thread producer_threads[ MAX_THREADS ]; // Multiple Producers Defined
    std::thread consumer_threads[ MAX_THREADS ]; // Multiple Consumers Defined
    
    //--------------------------------------------------
    // Process Variable Initialization                 :
    //--------------------------------------------------
    bool is_done = 0;
    int  in_data;                                // Input Tasks (Data)
    
    int  process_order;                          // The Ticket Number Of What
                                                 // Data Was Processed
    
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
    
    //--------------------------------------------------
    // T H R E A D S  -  Start     [ Declaration ]     :
    //--------------------------------------------------
    // Seeds The Main Thread.
    // Generates A Pseudo-Random Sequence Of Integer Values:
    srand( static_cast<unsigned int>(  time(0))  );
    
        process_order = 0;
          // Initialize Producers and Consumers Threads:
          while( is_done == 0 ){

              if( process_order < MAX_THREADS ){
              //------------------------------------------
              // Input Task (Data) Value                 :
              //------------------------------------------
              in_data = random_task_generator( );
              
              //------------------------------------------
              // RUN THREAD 1                            :
              //------------------------------------------
              // The Producer Threads Run the Producer Function:
              producer_threads[ process_order ]     = std::thread(   producer,
                                                                     in_data     );
                //--------------------------------------
                // RUN THREAD 2, CONCURRENTLY          :
                //--------------------------------------
                // The Consumer Threads Run the Consumer Function:
                consumer_threads[ process_order ] = std::thread( consumer,
                                                                 process_order);

              process_order++;
              }
              else{
                  is_done = 1;
              }
          }
    //--------------------------------------------------
    // T H R E A D S  -  Finish                        :
    //--------------------------------------------------
          // Wait For Threads To Be Done.
          // IFF Done Then Return (Join) Back Into The
          // Main Thread ( " int main() " ):
          process_order = 0;
          while( is_done == 1 ){

            if( process_order < MAX_THREADS ){
            
                consumer_threads[ process_order ].join();
                producer_threads[ process_order ].join();
                
                process_order++;
            }
            else{
                is_done = 0;
            }
          }
    
    // ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~


	//---------------------------------------------------------------------
	// System Specific Macro.                                             :
	// Pause The System                                                   :
	//---------------------------------------------------------------------
	#if defined(_WIN32) || defined(_WIN64)
		system("pause");
	#endif
    //--------------------------------------------------
    // End Application                                 :
    //--------------------------------------------------
    return 0;
}

/*///////////////////////////////////////////////////////////////////////////*/
/*---------------------------------------------------------------------------*/
/*                   F U N C T I O N    D E F I N I T I O N S                */
/*---------------------------------------------------------------------------*/
int  random_task_generator( ){
//-----------------------------------------------------------------------------
// Function Description                                                       :
//-----------------------------------------------------------------------------
// Psuedo-Random Number Generator.
// It Generates a Specific ranges of values for the Process Thread.
//
// The range spans from 1 to 15.
//-----------------------------------------------------------------------------
    
    
    //----------------------------------------
    // Variable Declarations                 :
    //----------------------------------------
    int number_of_tasks;
    
    //----------------------------------------
    // Variable Declarations                 :
    //----------------------------------------
    number_of_tasks = (rand() % 100) + 1 ;  // 1 to 15
    
    //----------------------------------------
    // Return The Number Of Tasks            :
    //----------------------------------------
    return number_of_tasks;
    
}

void producer( int tasks_in ){
//-----------------------------------------------------------------------------
// Function Description                                                       :
//-----------------------------------------------------------------------------
// This Function Is Called By The Producer Thread.
// It Produces Data Thats Stored In The Bounded Buffer, One At A Time.
//-----------------------------------------------------------------------------
    
    
  //--------------------------------------------
  // Timer Variable Initialization             :
  //--------------------------------------------
  using namespace std::literals::chrono_literals;
    
    
  //--------------------------------------------
  // Synchronization Of Threads                :
  //--------------------------------------------
  std::unique_lock<std::mutex> lock(synch);
      
    
      //----------------------------------------
      //    Wait() Condition                   :
      //----------------------------------------
      while( num_of_buffer_items == BUFFER_SIZE ){
        synch_cv.wait(lock);
      }
      //----------------------------------------
      //    Wait() Condition Finished          :
      //----------------------------------------
      bounded_buffer[ in_index ] = tasks_in;
      in_index = ( in_index + 1 ) % BUFFER_SIZE;
          //------------------------------------
          //  Update The Number Of Items       :
          //  Available To The Process         :
          //------------------------------------
          num_of_buffer_items++;
        
          //------------------------------------
          // Show User The Producer Is Working :
          //------------------------------------
          cout << "Item Created"
               << endl << endl;
    
      //----------------------------------------
      // Unblocks one of the threads currently :
      // In   " Wait() Condition " .           :
      // If no threads are waiting, the        :
      // function does nothing. (Must specify) :
      //----------------------------------------
      synch_cv.notify_one(); // Notify The Consumer That There Is
                             // Data Avaialable
        
        
      //------------------------
      // System Sleep Delay    :
      //------------------------
      // Adds a screen delay between Screen Outputs:
      std::this_thread::sleep_for(2s);
    
}

void consumer( int thread_order_val ){
//-----------------------------------------------------------------------------
// Function Description                                                       :
//-----------------------------------------------------------------------------
// This Function Is Called By The Consumer Thread.
// It Consumes Data From The Bounded Buffer, One At A Time.
//-----------------------------------------------------------------------------
  
    
  //--------------------------------------------
  // Timer Variable Initialization             :
  //--------------------------------------------
  using namespace std::literals::chrono_literals;

    
  //--------------------------------------------
  // Variable Declarations                     :
  //--------------------------------------------
  int tasks_out; // Stores the current task value
    
    
  //--------------------------------------------
  // Synchronization Of Threads                :
  //--------------------------------------------
  std::unique_lock<std::mutex> lock(synch);
    
      //----------------------------------------
      //    Wait() Condition                   :
      //----------------------------------------
      while(num_of_buffer_items == 0){
        synch_cv.wait(lock);
      }
      //----------------------------------------
      //    Wait() Condition Finished          :
      //----------------------------------------
      // Store the Output Task Value:
      tasks_out = bounded_buffer[ out_index ];
      out_index = ( out_index + 1 ) % BUFFER_SIZE;
        //------------------------------------
        //  Update The Number Of Items       :
        //  Available To The Process         :
        //------------------------------------
        num_of_buffer_items--;
      
        //------------------------------------
        // Show User The Consumption Of Data :
        //------------------------------------
        cout << "Item No."
             << thread_order_val + 1         // The +1 accounts for the
                                             // 'process_order_val' index starting
                                             // at 0
            << "  :  "
            << "  The Item Value,  "
            << tasks_out
            << "  , Has Been Consumed."
            << endl << endl;
       
    
      //----------------------------------------
      // Unblocks one of the threads currently :
      // In   " Wait() Condition " .           :
      // If no threads are waiting, the        :
      // function does nothing. (Must specify) :
      //----------------------------------------
      synch_cv.notify_one(); // Wake up the producer thread
    
      //------------------------
      // System Sleep Delay    :
      //------------------------
      // Adds a screen delay between Screen Outputs:
      std::this_thread::sleep_for(2s);
}
