#ifndef HELPER_DATA
#define HELPER_DATA
//--------------------------------------------------------------------
enum walkDir_t {walkForward, walkBackward, walkLeft, walkRight, walkIdle};  // 0,1,2,3,4
walkDir_t dir;

enum state_t {stateOFF, stateON, stateWALK, stateSTAND, stateHELLO, stateSCARED, stateFLATTEN};
// 0,1,2,3,4,5,6
state_t state;
//--------------------------------------------------------------------
// Structure used to send and receive data
// Must match the sender structure
//--------------------------------------------------------------------
typedef struct struct_message_t {
  float ax;
  float ay;
  walkDir_t walkDir;
  state_t state;
} struct_message;

//--------------------------------------------------------------------
// print struct_message
//--------------------------------------------------------------------
void printData(struct_message_t& data){
  Serial.printf("Data :: ax=%3.2f ay=%3.2f mode=%d state=%d\n",
                data.ax, data.ay, data.walkDir, data.state);
}
//--------------------------------------------------------------------
#endif //HELPER_DATA