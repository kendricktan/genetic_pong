#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define DELAY 10 /* milliseconds */
#define PADDLE_LENGTH 6

// SCREEN BOUNDARIES
int SCREEN_WIDTH, SCREEN_HEIGHT;

// IMAGES
char edge_img = '*';
char paddle_img = '|'; 
char ball_img[] = "o";

// GAME STATES
bool game_over;
bool update_screen = true;

int LIVES = 10;
int SCORE = 10;
int LEVEL = 10;
int TIME_MINUTES = 10, TIME_SECONDS = 10;

// PADDLE LOCATION
int PADDLE_HEIGHT;
int LEFT_PADDLE_Y;
int RIGHT_PADDLE_Y; 

// SPRITES
sprite_id sprite_ball;

// Draw bounding boxes
void draw_bounding_boxes(void){
    // Draw muh outlines
    draw_line(0, 0, SCREEN_WIDTH, 0, edge_img);
    draw_line(0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT, edge_img);
    draw_line(0, 0, 0, SCREEN_HEIGHT, edge_img);
    draw_line(SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, edge_img);

    // Draw muh score lines
    int padding = (SCREEN_WIDTH+3)/4; // 4 lines
    draw_line(0, 2, SCREEN_WIDTH, 2, edge_img);
    
    // Draw muh info
    char buf[padding];

    sprintf(buf, "Lives = %d", LIVES);
    draw_string(2, 1, buf);

    sprintf(buf, "Score = %d", SCORE);
    draw_string(2+(padding), 1, buf);

    sprintf(buf, "Level = %d", LEVEL);
    draw_string(2+(padding*2), 1, buf);

    sprintf(buf, "Time = %d:%d", TIME_MINUTES, TIME_SECONDS);
    draw_string(2+(padding*3), 1, buf);

    // Draw muh separators
    for (int i = padding; i < SCREEN_WIDTH; i+=padding){
        draw_char(i, 1, edge_img);
    }
}

// Draw our pong paddles
void draw_paddles(void){
    // Draw left paddle
    draw_line(3, LEFT_PADDLE_Y, 3, LEFT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
    draw_line(SCREEN_WIDTH-3, RIGHT_PADDLE_Y, SCREEN_WIDTH-3, RIGHT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
}

void check_balls(void){
    if (sprite_y(sprite_ball) >= SCREEN_HEIGHT || sprite_y(sprite_ball) <= 2){
        sprite_back(sprite_ball);
        double dx = sprite_dx(sprite_ball);
        double dy = sprite_dy(sprite_ball);
        sprite_turn_to(sprite_ball, dx, -dy);
    }
}

// Setup our variables and sprites
void setup(void) {
    // Screen dimensions brah
    SCREEN_WIDTH = screen_width()-1;
    SCREEN_HEIGHT = screen_height()-1;

    // Our sprite height brah
    if (SCREEN_HEIGHT > 21){
        PADDLE_HEIGHT = 7;
    }

    else{
        int HS = SCREEN_HEIGHT-3;
        PADDLE_HEIGHT = (SCREEN_HEIGHT-HS-1)/2;        
    }

    LEFT_PADDLE_Y = (SCREEN_HEIGHT/2)-(PADDLE_HEIGHT/2);
    RIGHT_PADDLE_Y = (SCREEN_HEIGHT/2)-(PADDLE_HEIGHT/2);

    // Our sprite
    sprite_ball = sprite_create(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1, 1, ball_img);
    sprite_turn_to(sprite_ball, 0, -1);
}

// Updates ball position
// and draws it
void update_ball(){
    check_balls();
    sprite_draw(sprite_ball);
}

// Gets user input
void get_inputs(void){
    // Get key press
    int key = get_char();

    if (key == 'w'){
        // Checks if paddle top is within bounds
        // move it
        if (LEFT_PADDLE_Y > 3){
            LEFT_PADDLE_Y -= 1;
        }
    }
    else if (key == 's'){
        // Checks if paddle top + paddle height is within bounes
        // move it   
        if (LEFT_PADDLE_Y + PADDLE_HEIGHT < SCREEN_HEIGHT-1){
            LEFT_PADDLE_Y += 1;
        }     
    }
}

// Play one turn of game.
void process(void) {
    // Keep the next line intact.
    clear_screen();    

    //  Draw them outlines.
    draw_bounding_boxes();    

    // Draw them sprites
    draw_paddles();

    // Draw them balls
    update_ball();
    
    // User inputs
    get_inputs();
}

// Clean up game
void cleanup(void) {
    // STATEMENTS
}

// Program entry point.
int main(void) {
    setup_screen();

#if defined(AUTO_SAVE_SCREEN)
    auto_save_screen(true);
#endif

    setup();
    show_screen();

    while ( !game_over ) {
        process();

        if ( update_screen ) {
            show_screen();
        }

        timer_pause(DELAY);
    }

    cleanup();

    return 0;
}