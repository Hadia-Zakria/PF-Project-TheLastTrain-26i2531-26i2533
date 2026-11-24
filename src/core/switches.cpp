#include "switches.h"
#include "util.h"

// Door lock mechanics, lifts, and cabinet verification without structures


// Lift 1 only connects floor 1 and floor 2
bool canUseLift1(int fromFloor, int toFloor) {
    if (fromFloor == 1 && toFloor == 2) return true;
    if (fromFloor == 2 && toFloor == 1) return true;
    return false;
}

// Lift 2 only stops at floors 2, 4 and 5
bool canUseLift2(int fromFloor, int toFloor, bool lift2Operational, int emergencyTripsLeft) {
    bool fromOk = (fromFloor == 2 || fromFloor == 4 || fromFloor == 5);
    bool toOk   = (toFloor == 2 || toFloor == 4 || toFloor == 5);

    if (!fromOk || !toOk) return false;
    if (fromFloor == toFloor) return false;

    if (lift2Operational) return true;      // lift is repaired
    if (emergencyTripsLeft > 0) return true; // use an emergency trip
    return false;
}

// The locked door needs the Master Key in the inventory (key is not used up)
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount) {
    for (int i = 0; i < inventoryCount; i++) {
        if (inventory[i] == ENT_MASTER_KEY) {
            return true;
        }
    }
    return false;
}

// Takes the fuse out of the inventory and installs it in the control panel
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score) {
    if (objectives[OBJ_FUSE_INSTALLED]) return false;   // already installed

    // find the fuse in the inventory
    int position = -1;
    for (int i = 0; i < inventoryCount; i++) {
        if (inventory[i] == ENT_FUSE) {
            position = i;
            break;
        }
    }
    if (position == -1) return false;   // player does not have the fuse

    // remove it by shifting the later items one step left
    for (int i = position; i < inventoryCount - 1; i++) {
        inventory[i] = inventory[i + 1];
    }
    inventoryCount--;
    inventory[inventoryCount] = ENT_NONE;

    objectives[OBJ_FUSE_INSTALLED] = true;
    score = score + SCORE_INSTALL_FUSE;

    // Lift 2 works normally when fuse is installed AND lift pass is collected
    if (objectives[OBJ_LIFT_PASS]) {
        objectives[OBJ_LIFT2_ACTIVE] = true;
    }
    return true;
}

// Code is correct only if it is "427" and all 3 clues were found
bool verifyCabinetCode(const char* enteredCode, const bool objectives[NUM_OBJECTIVES]) {
    if (enteredCode == nullptr) return false;

    if (!objectives[OBJ_CLUE_4] || !objectives[OBJ_CLUE_2] || !objectives[OBJ_CLUE_7]) {
        return false;
    }

    if (enteredCode[0] == '4' && enteredCode[1] == '2' &&
        enteredCode[2] == '7' && enteredCode[3] == '\0') {
        return true;
    }
    return false;
}
