#include "config.h"
#include "patch.h"

int checkRequirement() {
    if(!isDirectory(ATMOS_DIR)) {
    	userConfirm("Do you even Atmosphere bro?");
    	return -1;
    }
    return 0;
}
