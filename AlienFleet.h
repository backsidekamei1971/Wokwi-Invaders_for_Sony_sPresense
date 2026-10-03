#ifndef ALIEN_FLEET_H
#define ALIEN_FLEET_H
#include "GameObject.h"


#define NUM_ALIEN_ROWS 2
#define NUM_ALIEN_COLUMNS 8
#define NUM_ALIENS (NUM_ALIEN_ROWS * NUM_ALIEN_COLUMNS)
#define ALIEN_STATUS_ARRAY_SIZE ((NUM_ALIENS +7)/8)


//const unsigned char alien_moving[] PROGMEM = {
// 0b00100000,
// 0b10010001,
// 0b10111111,
// 0b11101110,
// 0b11111111,
// 0b01111111,
// 0b00110001,
// 0b0100000
//};

const unsigned char alien_moving[] PROGMEM = {
  B00100000,B10000000,
  B00010001,B00000000,
  B10111111,B10100000,
  B10101110,B10100000,
  B11111111,B11100000,
  B00111111,B10000000,
  B00100000,B10000000,
  B01000000,B01000000
};

//const unsigned char alien[] PROGMEM = {
// 0b00011000,
// 0b11111111,
// 0b11011011,
// 0b11111111,
// 0b11000011,
// 0b00000000,
// 0b00000000,
// 0b00000000
//};

const unsigned char alien[] PROGMEM = {
  B00001111,B00000000,
  B01111111,B11100000,
  B11111111,B11110000,
  B11100110,B01110000,
  B11111111,B11110000,
  B00111001,B11000000,
  B01100110,B01100000,
  B00110000,B11000000
  };


class AlienFleet: public GameObject{
public:


AlienFleet( uint8_t x_pos, uint8_t y_pos,
const uint8_t *graphic, uint8_t width, uint8_t height,
Adafruit_SSD1306 *display):
GameObject(x_pos, y_pos, graphic, width, height, display)
{
reset_aliens();
}


bool move_fleet_horizontally(int direction){
move_horizontally(direction);
int right_edge = x_pos + (width + COL_GAP) * (NUM_ALIEN_COLUMNS ) - COL_GAP;
int left_edge = x_pos;
if(left_edge <= 0 || right_edge >= ((SCREEN_WIDTH - 1))){
return true;
}
else
{
return false;
}
}




bool move_fleet_vertically(int direction){
move_vertically(direction);
int lower_edge = y_pos + (height + ROW_GAP) * (NUM_ALIEN_ROWS ) - ROW_GAP;
if(lower_edge > SCREEN_HEIGHT){
move_vertically(MOVE_UP);
return true;
}
return false;
}


bool collidesWithAlien(GameObject &obj){
for(int y = 0; y < NUM_ALIEN_ROWS; y++){
for(int x = 0; x < NUM_ALIEN_COLUMNS; x++){
if(get_alien_status(get_alien_index(y,x))){
bool collisions = obj.collidesWith(x_pos + x * (width + COL_GAP), y_pos + y * (height + ROW_GAP),
width,
height ) ;
if (collisions){
set_alien_inactive(get_alien_index(y,x));
return true;
}
}
}
}
return false;
}


uint8_t get_num_active_aliens(){
uint8_t count = 0;
for(int y = 0; y < NUM_ALIEN_ROWS; y++){
for(int x = 0; x < NUM_ALIEN_COLUMNS; x++){
if(get_alien_status(get_alien_index(y,x))){
count++;
}
}
}
return count;
}


void draw(uint16_t color) override{
for(int y = 0; y < NUM_ALIEN_ROWS; y++){
for(int x = 0; x < NUM_ALIEN_COLUMNS; x++){
uint16_t index = get_alien_index(y,x);
if(get_alien_status(get_alien_index(y,x))){
const uint8_t *bmp = NULL;
if(index%4 == 0){
bmp = alien_moving;
}
else{
bmp = alien;
}
display->drawBitmap(x_pos + x * (width + COL_GAP),
y_pos + y * (height + ROW_GAP), bmp, width, height, color);
}
}
}
}


uint8_t get_alien_index(uint8_t row, uint8_t col){
return row * NUM_ALIEN_COLUMNS + col;
}


bool get_alien_status(uint8_t index){
uint8_t bytePosition = index / 8;
uint8_t bitShiftAmount = (7 - index % 8);
return (alienStatus[bytePosition] >> (bitShiftAmount)) & 1;
}


void reset_aliens(){
for(int i= 0; i < ALIEN_STATUS_ARRAY_SIZE;i++){
alienStatus[i] = 0xFF;
}
}






bool set_alien_inactive(uint8_t index){
uint8_t bytePosition = index / 8;
uint8_t bitShiftAmount = (7 - index % 8);
uint8_t bitMask = ~(1 << bitShiftAmount);
return alienStatus[bytePosition] &= bitMask;
}






private:
const uint8_t COL_GAP = 2;
const uint8_t ROW_GAP = 2;
uint8_t alienStatus[ALIEN_STATUS_ARRAY_SIZE + 7] = {0xFF};




};






#endif
