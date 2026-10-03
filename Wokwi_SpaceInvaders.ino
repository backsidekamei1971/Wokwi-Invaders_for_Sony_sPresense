#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GameObject.h"
#include "MillisTimer.h"
#include "AlienFleet.h"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define MAX_NUM_BULLETS 3


const uint8_t LEFT_BUTTON = 4;
const uint8_t RIGHT_BUTTON = 5;
const uint8_t TRIGGER_BUTTON = 2;
const uint8_t SPEAKER_PIN = 10;


bool weapon_fired = false;
bool explosion = false;
bool spaceship_move = false;
bool soundActivated = false;
uint16_t weapon_fired_length = 3;
uint16_t explosion_length = 4;
uint16_t spaceship_move_length = 2;




Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


typedef enum game_state_t {


WAITING_TO_START,
START_LEVEL,
GAME_ON,
GAME_OVER




} game_state_t;


game_state_t game_state = WAITING_TO_START;


//const unsigned char space_ship[] PROGMEM = {
//  0b00011000,
//  0b00011000,
//  0b10011001,
//  0b11011011,
//  0b11011111,
//  0b01111110,
//  0b00111100,
//  0b00011000,
//};

const unsigned char space_ship[] PROGMEM = {
  B00000010,B00000000,
  B00000111,B00000000,
  B00000111,B00000000,
  B01111111,B11110000,
  B11111111,B11111000,
  B11111111,B11111000,
  B11111111,B11111000,
  B11111111,B11111000,
};

const uint8_t bullet[] PROGMEM = {
0xC0,
0xC0,
0xC0,
0xC0
};




#define NUM_ALIEN_ROWS 2
#define NUM_ALIEN_COLUMNS 8
// GameObject alienFleet[NUM_ALIEN_ROWS][NUM_ALIEN_COLUMNS];


AlienFleet alienFleet(4, 4, alien, 11, 8, &display);
GameObject bullets[MAX_NUM_BULLETS];
GameObject spaceShip(10, 50, space_ship, 12, 14,&display);
void draw_alien_fleet();
void move_alien_fleet();
void soundLoop();
void moveLoop();
void renderLoop();
void controlLoop();
void move_bullets();
void move_space_ship();
void init_buttons();
void game_loop();
bool poll_for_input();
void render_load_screen();
void render_game_over_screen();
void draw_score();
void reset_game();
void end_game();
void set_level(uint8_t currentLevel);
void render_start_level_screen();


uint16_t currentScore = 0;
uint8_t currentLevel = 0;


MillisTimer alienMoveTimer(200);
MillisTimer spaceShipMoveTimer(100);
MillisTimer displayUpdateTimer(33);
MillisTimer createBulletTimer(400);
MillisTimer soundUpdateTimer(10);


void setup() {

//  Wire.setSDA(12);
// Wire.setSCL(13);

init_buttons();


if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
for (;;);
}


display.clearDisplay();
display .display();


for(int i = 0; i < MAX_NUM_BULLETS; i++){
bullets[i] = GameObject(0,0 , bullet, 2, 4, &display);
bullets[i].set_visibility(false);
bullets[i].set_y_speed(3);
bullets[i].set_x_speed(0);
}

render_load_screen();
}


void set_level(uint8_t currentLevel){
for(int i = 0; i < MAX_NUM_BULLETS; i++){


bullets[i].set_visibility(false);
}


alienFleet.reset_aliens();
alienFleet.set_x_pos(4);
alienFleet.set_y_pos(10);
spaceShip.set_x_pos(10);
spaceShip.set_y_pos(45);

alienFleet.set_x_speed(currentLevel);
alienFleet.set_y_speed((currentLevel/2) + 1);

spaceShip.set_x_speed(5);
spaceShip.set_y_speed(0);
}


void loop(){
switch(game_state){
case WAITING_TO_START:
if(poll_for_input()){
game_state = START_LEVEL;
reset_game();
}


break;
case START_LEVEL:
currentLevel++;
noTone(SPEAKER_PIN);
set_level(currentLevel);
render_start_level_screen();
delay(1000);
game_state = GAME_ON;
break;
case GAME_ON:
game_loop();
break;
case GAME_OVER:
noTone(SPEAKER_PIN);
if(poll_for_input()){
game_state = WAITING_TO_START;
render_load_screen();
delay(500);
}
break;
default:
break;
}
}


void game_loop() {
controlLoop();
moveLoop();
if(game_state == GAME_ON){
renderLoop();
if(alienFleet.get_num_active_aliens() == 0){
game_state = START_LEVEL;
}
}
soundLoop();
delay(1);
}


void soundLoop(){
static uint16_t counter = 0;
if(soundUpdateTimer.timeUp() || soundActivated){
soundUpdateTimer.reset();
if(soundActivated){
soundActivated = false;
if(weapon_fired){
counter = weapon_fired_length;
tone(SPEAKER_PIN, 300);
} else if(explosion){
tone(SPEAKER_PIN, 50);
counter = explosion_length;
} else {
tone(SPEAKER_PIN, 200);
counter = spaceship_move_length;
}


}
counter--;
if(counter <= 0 && (weapon_fired || explosion || spaceship_move)){
weapon_fired = false;
explosion = false;
spaceship_move = false;
noTone(SPEAKER_PIN);
}
soundActivated = false;
}
}


void reset_game(){
for(int i = 0; i < MAX_NUM_BULLETS; i++){


bullets[i].set_visibility(false);
}


alienFleet.reset_aliens();
alienFleet.set_x_pos(4);
alienFleet.set_y_pos(10);
spaceShip.set_x_pos(10);
spaceShip.set_y_pos(45);

alienFleet.set_x_speed(2);
alienFleet.set_y_speed(2);
spaceShip.set_x_speed(5);
spaceShip.set_y_speed(0);
currentScore = 0;
currentLevel = 0;
}

bool createBullet(uint8_t x_pos){
for(int i = 0; i < MAX_NUM_BULLETS; i++){
if(!bullets[i].get_visibility()){
bullets[i].set_visibility(true);
bullets[i].set_x_pos(x_pos);
bullets[i].set_y_pos(60);
soundActivated = true;
explosion = true;
return true;
}
}
return false;
}


int currentMoveX = NONE;
int currentMoveY = NONE;


void controlLoop(){
static bool fireWeapon = false;
if(fireWeapon){
fireWeapon = false;
createBullet(spaceShip.get_x_pos() + spaceShip.get_width()/2 - 1);
}

if(!digitalRead(LEFT_BUTTON)){
currentMoveX = MOVE_LEFT;

}else if(!digitalRead(RIGHT_BUTTON)){
currentMoveX = MOVE_RIGHT;
} else{
currentMoveX = NONE;
}

if(!digitalRead(TRIGGER_BUTTON) && createBulletTimer.timeUp()){
createBulletTimer.reset();
fireWeapon = true;
}


}


void check_bullet_collisions(){
for(int i = 0; i < MAX_NUM_BULLETS; i++){
if(bullets[i].get_visibility()){
bool collision = alienFleet.collidesWithAlien(bullets[i]);
if(collision){
bullets[i].set_visibility(false);
currentScore++;
soundActivated = true;
explosion = true;
}
}
}
}


void moveLoop(){
if(alienMoveTimer.timeUp()){
alienMoveTimer.reset();
move_alien_fleet();
check_bullet_collisions();
}


if(spaceShipMoveTimer.timeUp()){
spaceShipMoveTimer.reset();
move_space_ship();
move_bullets();
}
}


void draw_bullets(){
for(int i =0; i< MAX_NUM_BULLETS; i++){
bullets[i].draw(WHITE);
}
}
void renderLoop(){
if(displayUpdateTimer.timeUp()){
displayUpdateTimer.reset();
display.clearDisplay();
draw_alien_fleet();
spaceShip.draw(WHITE);
draw_bullets();
draw_score();
display.display();
}
}


void draw_alien_fleet(){
alienFleet.draw(WHITE);
}


void move_space_ship(){
spaceShip.move_horizontally_stop_at_wall(currentMoveX);
if(currentMoveX != 0){
soundActivated = true;
spaceship_move = true;
}


if(alienFleet.collidesWithAlien(spaceShip)){
end_game();
}
}


void move_alien_fleet(){
static bool at_edge = false;
static int direction = MOVE_RIGHT;
if(at_edge){
at_edge = false;
direction = -1 * direction;
if(alienFleet.move_fleet_vertically(MOVE_DOWN)){
end_game();
}
}
at_edge = alienFleet.move_fleet_horizontally(direction);
}


void move_bullets(){
for(int i =0; i< MAX_NUM_BULLETS; i++){
if(bullets[i].move_vertically(MOVE_UP)){
bullets[i].set_visibility(false);
}
}
}


void init_buttons(){
pinMode(LEFT_BUTTON, INPUT_PULLUP);
pinMode(RIGHT_BUTTON, INPUT_PULLUP);
pinMode(TRIGGER_BUTTON, INPUT_PULLUP);
}


bool poll_for_input(){
return !digitalRead(LEFT_BUTTON) ||
!digitalRead(RIGHT_BUTTON) ||
!digitalRead(TRIGGER_BUTTON);
}


void render_load_screen(){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);


display.setCursor(10,10);
display.print("ALIEN");
display.setCursor(50,28);
display.print("ATTACK");


display.setTextSize(1);
display.setCursor(1,50);
display.print("PRESS BUTTON TO BEGIN");
display.display();
}


void render_game_over_screen(){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);


display.setCursor(5,10);
display.print("GAME OVER");


display.setTextSize(1);
display.setCursor(50,28);
display.print("SCORE: ");
display.print(currentScore);


display.setCursor(1,45);
display.print("PRESS BUTTON");
display.setCursor(1,55);
display.print("TO CONTINUE");


display.display();
}


void end_game(){
game_state = GAME_OVER;
render_game_over_screen();
}


void draw_score(){
display.setTextSize(1);
display.setCursor(0,0);
display.print(currentScore);
}


void render_start_level_screen(){
display.clearDisplay();
display.setTextSize(2);
display.setTextColor(WHITE);


display.setCursor(10,10);
display.print("LEVEL ");
display.print(currentLevel);


display.setTextSize(1);
display.setCursor(50,28);
display.print("SCORE: ");
display.print(currentScore);
display.display();
}
