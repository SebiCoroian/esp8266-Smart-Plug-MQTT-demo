#include <FS.h>

#include <ArduinoJson.h>




// wifi manager functions
void saveConfigCallback();


char devkey[30];



// Flag for knowing if the new config should be saved or not
bool shouldSaveConfig = false;
