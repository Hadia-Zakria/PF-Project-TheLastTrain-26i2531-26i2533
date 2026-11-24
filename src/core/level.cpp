#include "level.h"
#include "util.h"
#include "state.h"
// Modular implementation using primitive arrays for map parsing and verification
#include<iostream>
using namespace std;
const char* getEntityName(char entitySymbol){
	switch(entitySymbol){
		case ENT_NONE: return "empty";
		case ENT_MASTER_KEY: return "Master Key";
		case ENT_ENERGY_PACK: return "Energy Pack";
		case ENT_EMERGENCY_TOOLKIT: return "Emergency Toolkit";
		case ENT_CLUE_4: return "Clue 4";
		case ENT_FUSE : return "Fuse";
		case ENT_CAT: return "Cat";
		case ENT_CONTROL_PANEL: return "Control Panel";
		case ENT_LIFT_PASS: return "Lift Pass";
		case ENT_PASSENGER : return "Passenger";
		case ENT_MEDICAL_KIT : return "Medical Kit";
		case ENT_EMERGENCY_CARD  : return "Emergency Card";
		case ENT_CLUE_2   : return "Clue 2";
		case ENT_CLUE_7   : return "Clue 7";
		case ENT_CABINET_TRAIN_KEY   : return "Cabinet/Train Key";
		case ENT_CAT_CARRIER     : return "Cat Carrier";
		case ENT_OBSERVATION_TOKEN    : return "Observation Token";
		default : return "Unknown Entity";
	}
}


bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1]){
                          	if(startFloor<1 || startFloor>NUM_FLOORS  || startRoom<1 || startRoom>ROOMS_PER_FLOOR){
                                	      cout<<"Invalid input"<<endl;
						                  return false;}
						    char outConnType;
						    for(int i=1;i<=NUM_FLOORS;i++){
						    	for(int j=1;j<=ROOMS_PER_FLOOR;j++){
						    		if( isDirectlyConnected(startFloor,startRoom,i,j, horizontalConn,outConnType)){
						    			return true;
									}
								}
							}
							return false;
						  }
		
		
		
		
		
		
		
		
		
			
		
	}
}
