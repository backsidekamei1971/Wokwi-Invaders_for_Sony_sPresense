#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <Adafruit_SSD1306.h>
#include <Arduino.h>


#define MOVE_LEFT -1
#define MOVE_RIGHT 1
#define MOVE_UP -1
#define MOVE_DOWN 1
#define NONE 0


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64


class GameObject {
public:
GameObject() :
x_pos(0),
y_pos(0),
graphic(NULL),
width(0),
height(0),
display(NULL)
{


}


GameObject( uint8_t x_pos, uint8_t y_pos,
const uint8_t *graphic, uint8_t width, uint8_t height,
Adafruit_SSD1306 *display) :
x_pos(x_pos),
y_pos(y_pos),
graphic(graphic),
width(width),
height(height),
display(display)
{
}


void set_x_speed(uint8_t speed){
x_speed = speed;
}


void set_y_speed(uint8_t speed){
y_speed = speed;
}


void set_visibility(bool visibility){
is_visible = visibility ;
}


bool get_visibility(){
return is_visible;
}


void set_x_pos(uint16_t x_p ){
x_pos = x_p;
}


void set_y_pos(int16_t y_p){
y_pos = y_p;
}


int get_x_pos(){
return x_pos;
}


uint8_t get_width(){
return width;
}

bool move_vertically(int direction){
int moveSize = direction * y_speed;
int new_y_pos = y_pos + moveSize;
if(new_y_pos < 0){
y_pos = 0;
} else if(new_y_pos >= (SCREEN_HEIGHT -height)){
y_pos = SCREEN_HEIGHT -height - 1;
} else{
y_pos = new_y_pos;
}
if(y_pos <= 0 || y_pos >= (SCREEN_HEIGHT - 1)-height){
return true;
} else{
return false;
}
}


bool move_horizontally(int direction){
int moveSize = direction * x_speed;
int new_x_pos = x_pos + moveSize;
x_pos = new_x_pos ;
if(x_pos <= 0 || x_pos >= ((SCREEN_WIDTH - 1)-width)){
return true;
} else{
return false;
}
}


bool move_horizontally_stop_at_wall(int direction){
if(direction == MOVE_LEFT && !at_left_edge){
at_left_edge = move_horizontally(direction);
at_right_edge = false;
return at_left_edge;
} else if(direction == MOVE_RIGHT && !at_right_edge){
at_right_edge = move_horizontally(direction);
at_left_edge = false;
return at_right_edge;
} else{
return true ;
}
}


virtual void draw(uint16_t color){
if(is_visible){
display->drawBitmap(x_pos, y_pos, graphic, width, height, color);
}
}


bool collidesWith(const GameObject &other){
bool didNotCollideX = x_pos + width < other.x_pos ||
x_pos > other.width + other.x_pos;
bool didNotCollideY = y_pos + height < other.y_pos ||
y_pos > other.height + other.y_pos;
return !didNotCollideX && !didNotCollideY;
}


bool collidesWith(int16_t other_x, int16_t other_y,
uint8_t other_w,
uint8_t other_h ){
bool didNotCollideX = x_pos + width < other_x ||
x_pos > other_w + other_x;
bool didNotCollideY = y_pos + height < other_y ||
y_pos > other_h + other_y;
return !didNotCollideX && !didNotCollideY;
}




protected:


int16_t x_pos;
int16_t y_pos;
const uint8_t *graphic;
uint8_t width;
uint8_t height;
Adafruit_SSD1306 *display;
uint8_t x_speed;
uint8_t y_speed;
bool is_visible = true;
bool at_left_edge = false;
bool at_right_edge = false;
};


#endif

