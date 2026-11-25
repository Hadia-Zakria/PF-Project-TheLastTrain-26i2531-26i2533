#include "level.h"
#include "util.h"
#include "state.h"
#include "physics.h"
#include<cstring>
// Modular implementation using primitive arrays for map parsing and verification
#include<iostream>
#include<cstdlib>
#include<fstream>
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
	

bool readInt(const char* text,int& num){
	if(text==nullptr || text[0]=='\0'|| text[0]=='-')
	   return false;
	int i=0;
	while(text[i]!='\0'){
		if(text[i]<'0' || text[i]>'9')
		   return false;
		i++;
	}
	num=atoi(text);
	return true;
}


bool readBool(const char* text,bool& value){
    if(text==nullptr || text[0]=='\0')
	   return false;
	if(stringEquals(text,"YES")){
		value=true;
		return true;
	}
	else if(stringEquals(text,"NO")){
		value=false;
		return true;
	}
	else{
		return false;
    }
}
	
	
bool loadInitialState(const char* filename,
                      int& playerFloor, int& playerRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft){
                      	 bool energyFound=false,timeFound=false,scoreFound=false;
                      	 ifstream file(filename);
                      	 if(!file.is_open()){
                      	 	cout<<"Could not open the file successfully"<<endl;
                      	 	return false;
						   }
						 char line[MAX_LINE_LEN];
						 for(int i=0;i<NUM_OBJECTIVES;i++){
								objectives[i]=false;
							}
						 while(file.getline(line,MAX_LINE_LEN)){
						 	trimWhitespace(line);
						 	if(line[0]=='\0')
						 	   continue;
						    char* colon=strchr(line,':');
						    if(colon==nullptr){
						    	cout<<"Error: Invalid line in initial state"<<endl;
						    	file.close();
						    	return false;
					        }
						    *colon='\0';
						    char* key=line;
						    char* value=colon+1;
						    trimWhitespace(key);
						    trimWhitespace(value);
						    if(stringEquals(key,"INITIAL_ENERGY")){
						    	int num;
						    	if(!readInt(value,num)){
						    	cout<<"Error: Invalid initial energy"<<endl;
						    	file.close();
						    	return false;
								}
								if(num<0 || num>MAX_ENERGY){
								   cout<<"Error: Invalid initial energy"<<endl;
						    	   file.close();
						    	   return false;
								}
								energy=num;
								energyFound=true;
							}
							else if(stringEquals(key,"INITIAL_TIME")){
						    	int num;
						    	if(!readInt(value,num)){
						    	cout<<"Error: Invalid initial time"<<endl;
						    	file.close();
						    	return false;
								}
								if(num<0 || num>MAX_TIME){
								   cout<<"Error: Invalid initial time"<<endl;
						    	   file.close();
						    	   return false;
								}
								time=num;
								timeFound=true;
							}
							else if(stringEquals(key,"INITIAL_SCORE")){
						    	int num;
						    	if(!readInt(value,num)){
						    	cout<<"Error: Invalid initial score"<<endl;
						    	file.close();
						    	return false;
								}
								score=num;
								scoreFound=true;
							}
							else if(stringEquals(key,"MAX_ENERGY")){
						    	int num;
						    	if(!readInt(value,num)){
						    	cout<<"Error: Invalid maximum energy"<<endl;
						    	file.close();
						    	return false;
								}
								if(num!=MAX_ENERGY){
						    	cout<<"Error: Wrong maximum energy"<<endl;
						    	file.close();
						    	return false;
								}
							}
						    else if(stringEquals(key,"MAX_TIME")){
						    	int num;
						    	if(!readInt(value,num)){
						    	cout<<"Error: Invalid maximum time"<<endl;
						    	file.close();
						    	return false;
								}
								if(num!=MAX_TIME){
						    	cout<<"Error: Wrong maximum time"<<endl;
						    	file.close();
						    	return false;
								}
							}
							else if(stringEquals(key,"INITIAL_INVENTORY")){
								if(!stringEquals(value,"NONE")){
								cout<<"Error: Invalid initial inventory"<<endl;
						    	file.close();
						    	return false;
							    } 
							    inventoryCount=0;
						    }
						    else if(stringEquals(key,"PASSENGER_RESCUED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid passenger rescue"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_PASSENGER_RESCUED]=result;
						    }
						    else if(stringEquals(key,"CAT_RESCUED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid cat rescue"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_CAT_SECURED ]=result;
						    }
						    else if(stringEquals(key,"CLUES_COLLECTED")){
						         if(!stringEquals(value,"NONE")){
								cout<<"Error: Invalid clues collected"<<endl;
						    	file.close();
						    	return false;
							    } 
						         
						    }
						    else if(stringEquals(key,"CABINET_UNLOCKED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid unlocked cabinet"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_CABINET_UNLOCKED]=result;
						    }
						    else if(stringEquals(key,"TRAIN_KEY_COLLECTED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid collected train key"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_TRAIN_KEY]=result;
						    }
						    else if(stringEquals(key,"FUSE_INSTALLED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid fuse installed"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_FUSE_INSTALLED]=result;
						    }
						    else if(stringEquals(key,"LIFT_PASS_COLLECTED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid collected lift pass"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_LIFT_PASS]=result;
						    }
						    else if(stringEquals(key,"TRAIN_PREPARED")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid train prepared"<<endl;
						    	    file.close();
						    	    return false;
								 }
								 objectives[OBJ_TRAIN_PREPARED]=result;
						    }
						    else if(stringEquals(key,"PLAYER_AT_TRAIN")){
						         bool result;
						         if(!readBool(value,result)){
						    	    cout<<"Error: Invalid player at train"<<endl;
						    	    file.close();
						    	    return false;
								 }
						    }
					}
						    if(!energyFound){
						    	cout<<"Error: Initial energy is missing"<<endl;
						    	file.close();
						    	return false;
							}
							if(!timeFound){
						    	cout<<"Error: Initial time is missing"<<endl;
						    	file.close();
						    	return false;
							}
							if(!scoreFound){
						    	cout<<"Error: Initial score is missing"<<endl;
						    	file.close();
						    	return false;
							}
							for(int i=0;i<MAX_INVENTORY_CAPACITY;i++){
								inventory[i]=ENT_NONE;
							}
							emergencyTripsLeft=0;
							playerFloor=1;
							playerRoom=1;
						    file.close();
						    return true;
}
		
		
		
		
		
		
			
		
	

