#include <stdlib.h>
#include <math.h>
#include "game_states.h"
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define DELAY 10 /* milliseconds */
#define PADDLE_LENGTH 6

// SCREEN BOUNDARIES
int SCREEN_WIDTH, SCREEN_HEIGHT;
int TOP_WALL, BOTTOM_WALL, LEFT_WALL, RIGHT_WALL; // Walls bounding the ball

// IMAGES
char edge_img = '*';
char paddle_img = '|';
char ball_img[] = "O";

// GAME STATES
bool game_over;
bool update_screen = true;
bool TRAIN_AI = false;

GameStates gameState = {1,0,10,0,0,0};
 
// PADDLE LOCATION
int PADDLE_HEIGHT;
int LEFT_PADDLE_Y;
int LEFT_PADDLE_X;
int RIGHT_PADDLE_Y;
int RIGHT_PADDLE_X;

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
    // I like magic numbers too
    int padding = (SCREEN_WIDTH+3)/4; // 4 lines
    draw_line(0, 2, SCREEN_WIDTH, 2, edge_img);

    // Drawing out some displays
    char buf[padding];

    sprintf(buf, "Lives = %d", gameState.lives);
    draw_string(2, 1, buf);

    sprintf(buf, "Score = %d", gameState.score);
    draw_string(2+(padding), 1, buf);

    sprintf(buf, "Level = %d", gameState.level);
    draw_string(2+(padding*2), 1, buf);

    // A E S T H E T I C
    if (gameState.time_s >= 10){
        sprintf(buf, "Time = %d:%d", gameState.time_m, gameState.time_s);
    }
    else{
        sprintf(buf, "Time = %d:0%d", gameState.time_m, gameState.time_s);
    }
    draw_string(2+(padding*3), 1, buf);

    // Draw muh separators
    for (int i = padding; i < SCREEN_WIDTH; i+=padding){
        draw_char(i, 1, edge_img);
    }
}

// Draw our pong paddles
void draw_paddles(void){
    // Draw left paddle
    // Don't need it on handball level
    if (gameState.level > 1){
        draw_line(LEFT_PADDLE_X, LEFT_PADDLE_Y, LEFT_PADDLE_X, LEFT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
    }

    // Draw right paddle
    draw_line(RIGHT_PADDLE_X, RIGHT_PADDLE_Y, RIGHT_PADDLE_X, RIGHT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
}

// Steps ball back and rebounds it
void rebound_ball(double x_rebound, double y_rebound){
    //sprite_back(sprite_ball);
    double dx = sprite_dx(sprite_ball);
    double dy = sprite_dy(sprite_ball);
    sprite_turn_to(sprite_ball, dx*x_rebound, dy*y_rebound);
}

void check_balls(void){
    // If balls are beyond bounds @ top/bottom
    // re bounce it
    if (sprite_y(sprite_ball) >= BOTTOM_WALL || sprite_y(sprite_ball) <= TOP_WALL){
        rebound_ball(1.0, -1.0);
    }

    // Left wall
    if (gameState.level == 1 || TRAIN_AI){
        if (sprite_x(sprite_ball) <= LEFT_WALL){// || sprite_x(sprite_ball) >= RIGHT_WALL){
            rebound_ball(-1.0, 1.0);
        }
    }

    // Check if it hits the
    // right paddle (user controlled)
    // 1 unit wide
    if (sprite_x(sprite_ball) >= RIGHT_PADDLE_X && sprite_x(sprite_ball) < RIGHT_PADDLE_X + 1){
        
        // If it hits the top paddle
        if (sprite_y(sprite_ball) >= RIGHT_PADDLE_Y && sprite_y(sprite_ball) < RIGHT_PADDLE_Y + 1){
            // Moving downwards
            if (sprite_dy(sprite_ball) > 0){
                // If distance between paddle and top wall is greater than 
                // height of ball, bounce off horizontally
                if (RIGHT_PADDLE_Y-TOP_WALL > 1){
                    rebound_ball(1.0, -1.0);
                    gameState.score = 42;
                }
                else{
                    rebound_ball(-1.0, 1.0);
                }
            }
            else{
                rebound_ball(-1.0, 1.0);
            }
        }
        
        // Hits body
        else if (sprite_y(sprite_ball) >= RIGHT_PADDLE_Y+1 && sprite_y(sprite_ball) <= RIGHT_PADDLE_Y + PADDLE_HEIGHT - 1){
            rebound_ball(-1.0, 1.0);
        }

        // Hits bottom paddle
        else if (sprite_y(sprite_ball) > RIGHT_PADDLE_Y + PADDLE_HEIGHT - 1 && sprite_y(sprite_ball) <= RIGHT_PADDLE_Y + PADDLE_HEIGHT){
            // Moving Upwards
            if (sprite_dy(sprite_ball) < 0){
                // If distance between paddle and bottom wall is greater than
                // height of ball, bounce off horizontally
                if (BOTTOM_WALL - (RIGHT_PADDLE_Y+PADDLE_HEIGHT) > 1){
                    rebound_ball(1.0, -1.0);
                }
                else{
                    rebound_ball(-1.0, 1.0);
                }
            }
            else{
                rebound_ball(-1.0, 1.0);
            }
        }
    }
}

// Setup our variables and sprites
void setup(void) {
    // Screen dimensions brah
    SCREEN_WIDTH = screen_width()-1;
    SCREEN_HEIGHT = screen_height()-1;

    TOP_WALL = 3;
    BOTTOM_WALL = SCREEN_HEIGHT-1;
    LEFT_WALL = 1;
    RIGHT_WALL = SCREEN_WIDTH-1;

    // Our paddle height brah
    if (SCREEN_HEIGHT > 21){
        PADDLE_HEIGHT = 7;
    }

    else{
        int HS = SCREEN_HEIGHT-3;
        PADDLE_HEIGHT = (SCREEN_HEIGHT-HS-1)/2;
    }

    LEFT_PADDLE_Y = (SCREEN_HEIGHT/2)-(PADDLE_HEIGHT/2);
    RIGHT_PADDLE_Y = (SCREEN_HEIGHT/2)-(PADDLE_HEIGHT/2);

    LEFT_PADDLE_X = 3;
    RIGHT_PADDLE_X = SCREEN_WIDTH-3;

    // Our sprite
    sprite_ball = sprite_create(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 1, 1, ball_img);
    sprite_turn_to(sprite_ball, 0.3, 0.3);

}

// Updates ball position
// and draws it
void update_ball(){
    // Update ball position
    sprite_step(sprite_ball);

    // Check for collision
    check_balls();

    // Draw ball
    sprite_draw(sprite_ball);
}

// Gets user input
void get_inputs(void){
    // Get key press
    int key = get_char();

    if (key == 'w'){
        // Checks if paddle top is within bounds
        // move it
        if (RIGHT_PADDLE_Y > 3){
            RIGHT_PADDLE_Y -= 1;
        }
    }
    else if (key == 's'){
        // Checks if paddle top + paddle height is within bounes
        // move it
        if (RIGHT_PADDLE_Y + PADDLE_HEIGHT < SCREEN_HEIGHT-1){
            RIGHT_PADDLE_Y += 1;
        }
    }

    // Level up!
    else if (key == 'l'){
        gameState.level += 1;

        if (gameState.level > 4){
            gameState.level = 1;
        }
    }

    // Reset
    else if (key =='r'){
        setup();
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

        // Check for time.
        // #yolo
        gameState.time_ms += 10;

        if (gameState.time_ms >= 1000){
            gameState.time_s += 1;
            gameState.time_ms = 0;
        }

        if (gameState.time_s >= 60){
            gameState.time_m += 1;
            gameState.time_s = 0;
        }
    }

    cleanup();

    return 0;
}
