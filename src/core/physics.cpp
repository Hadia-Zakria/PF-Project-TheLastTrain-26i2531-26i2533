#include "physics.h"
#include "util.h"
#include "state.h"
#include "config.h"
// Movement validation and resource adjustments without structures
void applyScoreDelta(int& score,int deltaScore){
	score+=deltaScore;
}

bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol){
	if(inventoryCount>=MAX_INVENTORY_CAPACITY)
	            return false;
	else{
	   inventory[inventoryCount]=itemSymbol;
	   inventoryCount++;
	   return true;
     }
}

bool removeItem(char inventory[MAX_INVENTORY_CAPACITY],int& inventoryCount, char itemSymbol){
	 int index=-1;
	 for(int i=0;i<inventoryCount;i++){
	 	if(inventory[i]==itemSymbol){
	 		index=i;
	 		break;
		 }
	 }
	 if(index==-1)
	   return false;
	 for(int i=index;i<inventoryCount-1;i++)
	   inventory[i]=inventory[i+1];
	inventoryCount--;
	inventory[inventoryCount]=ENT_NONE;
	return true;
}


bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol){
	for(int i=0;i<inventoryCount;i++){
	 	if(inventory[i]==itemSymbol)
	 	   return true;
}
   return false;
}


void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime){
	energy+=deltaEnergy;
	time+=deltaTime;
	energy=clampInt(energy,0,MAX_ENERGY);
	time=clampInt(time,0,MAX_TIME);
}
	

bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType){
                         	outConnType=CONN_NONE;
                         	if(fromF<1 || fromF>NUM_FLOORS || toF<1 || toF>NUM_FLOORS || fromR<1 || fromR>ROOMS_PER_FLOOR || toR<1 || toR>ROOMS_PER_FLOOR)
						        return false;
						    if(fromF!=toF){
						    	if((fromF==1 && fromR==1 && toF==2 && toR==1 ) || (fromF==2 && toF==1 && fromR==1 && toR==1)){
						    		outConnType=CONN_STAIRS;
						    		return true;
								}
							    if((fromF==2 && fromR==1 && toF==3 && toR==1 ) || (fromF==3 && toF==2 && fromR==1 && toR==1)){
						    		outConnType=CONN_STAIRS;
						    		return true;
								}
								if((fromF==4 && fromR==1 && toF==5 && toR==1 ) || (fromF==5 && toF==4 && fromR==1 && toR==1)){
						    		outConnType=CONN_STAIRS;
						    		return true;
								}
								if((fromF==1 && (fromR==2 || fromR==3) && toF==2 && (toR==2 || toR==3)) || (fromF==2 && (fromR==2 || fromR==3) && toF==1 && (toR==2 || toR==3))){
									outConnType=CONN_LIFT1;
									return true;
								} 
								if(((fromF==2 || fromF==4 || fromF==5) && fromR==4  && toR==4 && (toF==2 || toF==4 || toF==5))){
									outConnType=CONN_LIFT2;
									return true;
								}
								return false;
							}       
							
						    if(toR!=fromR+1 && toR!=fromR-1) 
							  return false;
							int floorInd=fromF-1;
							int roomInd=0;
							if(fromR<toR)
							   roomInd=fromR-1;
							else
							   roomInd=toR-1;
							outConnType=horizontalConn[floorInd][roomInd];
							if(outConnType==CONN_NONE || outConnType==CONN_SOLID_WALL)
							   return false;
							return true;
}


bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational){
                 	if(targetFloor<1 || targetFloor>NUM_FLOORS || targetRoom<1 || targetRoom>ROOMS_PER_FLOOR)
						        return false;
					if(connType==CONN_DOOR || connType==CONN_WINDOW || connType==CONN_STAIRS || connType==CONN_LIFT1)
					   return true;
					else if(connType==CONN_LOCKED_DOOR){
						if(hasItem( inventory, inventoryCount, ENT_MASTER_KEY ))
						  return true;
						return false;
					}
					else if(connType==CONN_LIFT2){
						if(lift2Operational){
						   if(hasItem( inventory, inventoryCount, ENT_LIFT_PASS  ))
						       return true;
						    return false;
			           }
			           else{
			           	  if(hasItem( inventory, inventoryCount, ENT_EMERGENCY_CARD  )){
			           	  	if(emergencyTripsLeft>0)
			           	  	  return true;
							 }
							return false;
					   }
					   
			       }
			       else
					   return false;
	}
