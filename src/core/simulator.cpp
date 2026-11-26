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
                                	if(currentFloor<1 || currentFloor>NUM_FLOORS){
                                		cout<<"Invalid floor"<<endl;
						                  return false;
									}
								    if(currentRoom<1 || currentRoom>ROOMS_PER_FLOOR )
									{
									  cout<<"Invalid room"<<endl;
						                  return false;}
								    if(entityCount>MAX_ENTITIES_PER_ROOM || entityCount<0){
                                	      cout<<"Invalid Entity Count"<<endl;
						                  return false;
								    }
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


bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom,
                          int& energy, int& time, int& score,
                          char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int& entityCount,
                          char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                          bool objectives[NUM_OBJECTIVES]){
                          	if(currentFloor<1 || currentFloor>NUM_FLOORS){
                                		cout<<"Invalid floor"<<endl;
						                  return false;
									}
								    if(currentRoom<1 || currentRoom>ROOMS_PER_FLOOR )
									{
									  cout<<"Invalid room"<<endl;
						                  return false;}
								    if(entityCount>MAX_ENTITIES_PER_ROOM || entityCount<0){
                                	      cout<<"Invalid Entity Count"<<endl;
						                  return false;
								    }
							bool itemFound=false;
							int i=0;
							for(;i<entityCount;i++){
								if(roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol){
								    itemFound=true;
								    break;
								}
						    } 
						    if(itemFound==false){
						    	cout<<"The desired item of user isn't present in the room"<<endl;
						    	return false;
							}
							if(itemSymbol==ENT_NONE || itemSymbol==ENT_CLUE_4 || itemSymbol==ENT_CLUE_2 || itemSymbol==ENT_CLUE_7 || itemSymbol==ENT_CONTROL_PANEL || itemSymbol==ENT_CABINET_TRAIN_KEY ){
								cout<<"The desired item of user can't be collected"<<endl;
						    	return false;
							}
							if(itemSymbol==ENT_CAT){
							   	if(!hasItem(inventory, inventoryCount, ENT_CAT_CARRIER)){
							   		cout<<"The cat can't be collected due to absence of cat carrier"<<endl;
						    	    return false;
								   }
								objectives[OBJ_CAT_SECURED]=true;
								applyScoreDelta(score, SCORE_SECURE_CAT);
								applyResourceDelta(energy, time,COST_ITEM_USE_ENERGY,COST_ITEM_USE_TIME);
								cout<<"Cat successfully secured"<<endl;
						    }
						    else if(itemSymbol==ENT_PASSENGER){
							   	if(!hasItem(inventory, inventoryCount, ENT_MEDICAL_KIT)){
							   		cout<<"The passenger can't be rescued due to absence of medical kit"<<endl;
						    	    return false;
								}
								objectives[OBJ_PASSENGER_RESCUED]=true;
								applyScoreDelta(score, SCORE_RESCUE_PASSENGER);
								applyResourceDelta(energy, time,COST_ITEM_USE_ENERGY,COST_ITEM_USE_TIME);
								cout<<"Passenger successfully rescued"<<endl;
						    }
						    else if(itemSymbol==ENT_ENERGY_PACK){
						    	applyResourceDelta(energy, time,BONUS_ENERGY_PACK ,0);
						    	cout<<"Energy Pack successfully used"<<endl;
							}
							else if(itemSymbol==ENT_EMERGENCY_TOOLKIT){
						    	applyResourceDelta(energy, time, BONUS_EMERGENCY_TOOLKIT,0);
						    	cout<<"Emergency Toolkit successfully used"<<endl;
							}
							else if(itemSymbol==ENT_OBSERVATION_TOKEN){
						    	applyResourceDelta(energy, time,0,BONUS_OBSERVATION_TOKEN );
						    	cout<<"Observation Token successfully used"<<endl;
							}
							else if(itemSymbol== ENT_MASTER_KEY){
								if(!addItem(inventory, inventoryCount, ENT_MASTER_KEY)){
									cout<<"Master Key cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
								applyScoreDelta(score, SCORE_KEY_COLLECT );
								objectives[OBJ_MASTER_KEY]=true;
								cout<<"Master Key successfully collected"<<endl;
							}
							else if(itemSymbol== ENT_FUSE ){
								if(!addItem(inventory, inventoryCount, ENT_FUSE )){
									cout<<"Fuse cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
								objectives[OBJ_FUSE_COLLECTED]=true;
								cout<<"Fuse successfully collected"<<endl;
							}
							else if(itemSymbol== ENT_LIFT_PASS ){
								if(!addItem(inventory, inventoryCount,ENT_LIFT_PASS )){
									cout<<"Lift Pass cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
								objectives[OBJ_LIFT_PASS ]=true;
								applyScoreDelta(score,SCORE_LIFT_PASS  );
								cout<<"Lift Pass successfully collected"<<endl;
							}
						    else if(itemSymbol== ENT_MEDICAL_KIT){
								if(!addItem(inventory, inventoryCount,ENT_MEDICAL_KIT )){
									cout<<"Medical Kit cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
							    cout<<"Medical Kit successfully collected"<<endl;
							}
							else if(itemSymbol== ENT_EMERGENCY_CARD){
								if(!addItem(inventory, inventoryCount,ENT_EMERGENCY_CARD )){
									cout<<"Emergency Access Card cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
							    cout<<"Emergency Access Card successfully collected"<<endl;
							}
							else if(itemSymbol== ENT_CAT_CARRIER){
								if(!addItem(inventory, inventoryCount,ENT_CAT_CARRIER)){
									cout<<"Cat Carrier cannot be collected since the inventory has reached its maximum capacity"<<endl;
									return false;
								}
								applyResourceDelta(energy, time,COST_ITEM_PICKUP_ENERGY,COST_ITEM_PICKUP_TIME);
								objectives[OBJ_CAT_CARRIER]=true;
							    cout<<"Cat Carrier successfully collected"<<endl;
							}
							else{
								cout<<"Invalid item.Doesn't exist"<<endl;
								return false;
							}
							for(int j=i;j<entityCount-1;j++){
									roomEntities[currentFloor-1][currentRoom-1][j]=roomEntities[currentFloor-1][currentRoom-1][j+1];
								}
							roomEntities[currentFloor-1][currentRoom-1][entityCount-1]=ENT_NONE;
							entityCount--;
							return true;
}
