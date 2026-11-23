#include "simulator.h"
#include "physics.h"
#include "switches.h"
#include "logger.h"
#include "viewer_iface.h"
#include "util.h"
#include<iostream>
using namespace std;

// Modular simulation loop using primitive variables

bool handleMoveCommand(int targetFloor, int targetRoom,
                       int& currentFloor, int& currentRoom,
                       int& energy, int& time,
                       const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                       const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                       int& emergencyTripsLeft, bool lift2Operational){
                       	char connType;
                       	if(!(isDirectlyConnected(currentFloor,currentRoom, targetFloor, targetRoom,
                             horizontalConn,connType))){
                             	cout<<"You cannot move directly to the target room and floor"<<endl;
                             	return false;
							 }
						if(!(canTraverse( targetFloor, targetRoom, connType,
                           inventory, inventoryCount,emergencyTripsLeft, lift2Operational))){
                 	       if(connType==CONN_LIFT2){
                 	       	  if(lift2Operational){
						         cout<<"You cannot traverse to target room due to unavailability of lift pass"<<endl;
						           return false;
							}
							  else{
							  	cout<<"You cannot traverse to target room since lift2 isn't working and no emergency card or trips left"<<endl;
							  	return false;
							  }
				           }
				           else if(connType==CONN_LOCKED_DOOR){
				           	   cout<<"You cannot traverse to target room since the door is locked and you don't have the master key"<<endl;
				           	   return false;
						   }
						   else{
						   	   cout<<"You cannot traverse to the target room"<<endl;
						   	   return false;
						   }
				}
				if(connType==CONN_DOOR || connType==CONN_STAIRS || connType==CONN_LOCKED_DOOR)
					applyResourceDelta(energy,time,COST_MOVE_ORDINARY_ENERGY, COST_MOVE_ORDINARY_TIME);
				else if(connType==CONN_WINDOW)
				     applyResourceDelta(energy,time,COST_MOVE_WINDOW_ENERGY ,COST_MOVE_WINDOW_TIME  );
				else{
					 applyResourceDelta(energy,time,COST_MOVE_LIFT_ENERGY ,COST_MOVE_LIFT_TIME);
					if(connType==CONN_LIFT2){
						if(!(lift2Operational))
						    emergencyTripsLeft--;
					}
				}
				currentFloor=targetFloor;
				currentRoom=targetRoom;
				return true;
}


bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom,
                                bool objectives[NUM_OBJECTIVES], int& score,
                                char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                                int& entityCount){
                                	if(currentFloor<1 || currentFloor>NUM_FLOORS  || currentRoom<1 || currentRoom>ROOMS_PER_FLOOR || entityCount>MAX_ENTITIES_PER_ROOM || entityCount<0){
                                	      cout<<"Invalid input"<<endl;
						                  return false;}
						            if(code==nullptr){
						                  cout<<"No code is entered"<<endl;
						                  return false;}
						            bool isFound=false;
						            for(int i=0;i<entityCount;i++){
						            	if (roomEntities[currentFloor-1][currentRoom-1][i]==ENT_CABINET_TRAIN_KEY){
						            		isFound=true;
										}
						            }
						            if(isFound==false){
						            	cout<<"Cabinet is not present in this room"<<endl;
						            	return false;
									}
									if(objectives[OBJ_CABINET_UNLOCKED]){
										cout<<"Cabinet is already unlocked"<<endl;
										return false;
									}
									if(!(stringEquals(code,"427"))){
										cout<<"Wrong code is entered"<<endl;
										return false;
									}
									if(objectives[OBJ_CLUE_4]==false || objectives[OBJ_CLUE_2]==false || objectives[OBJ_CLUE_7]==false){
										cout<<"You have not found the clues"<<endl;
										return false;
									}
									objectives[OBJ_CABINET_UNLOCKED]=true;
									applyScoreDelta(score,SCORE_CABINET_UNLOCK);
	                                objectives[OBJ_TRAIN_KEY]=true;
				                    applyScoreDelta(score,SCORE_TRAIN_KEY );
				                    cout<<"Cabinet is unlocked.You have found the train key"<<endl;
				                    return true;
}
