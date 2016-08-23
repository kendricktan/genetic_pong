#include <stdlib.h>
#include <math.h>
#include "game_states.h"
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>

#define DELAY 10 /* milliseconds */
#define PADDLE_LENGTH 6

// Black hole counter
int bh_time_elapsed_ms = 0;

// SCREEN BOUNDARIES
int SCREEN_WIDTH, SCREEN_HEIGHT;
int TOP_WALL, BOTTOM_WALL, LEFT_WALL, RIGHT_WALL; // Walls bounding the ball

// IMAGES
char edge_img = '*';
char paddle_img = '|';
char ball_img[] = "O";

char black_hole_img[] = 
/**/          "\\  |  /"
/**/          " \\ | / "
/**/          "--   --"
/**/          " / | \\ "
/**/          "/  |  \\";

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
sprite_id sprite_black_hole;

// Prototypes
void draw_bounding_boxes(void);
void draw_paddles(void);
void rebound_ball(double, double);
void check_balls(void);
void setup(void);
void help_screen(void);
void process(void);
void reset_game_states(void);
void countdown(void);
void check_paddle(int, int);

// Generate random number
double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

// Reset game states
void reset_game_states(void){
    gameState.level = 1;
    gameState.score = 0;
    gameState.lives = 10;
    gameState.time_ms = 0;
    gameState.time_s = 0;
    gameState.time_m = 0;

    bh_time_elapsed_ms = 0;
}

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
    // Don't need it on handball level (level 1)
    if (gameState.level > 1){
        draw_line(LEFT_PADDLE_X, LEFT_PADDLE_Y, LEFT_PADDLE_X, LEFT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
    }

    // Draw right paddle
    draw_line(RIGHT_PADDLE_X, RIGHT_PADDLE_Y, RIGHT_PADDLE_X, RIGHT_PADDLE_Y+PADDLE_HEIGHT, paddle_img);
}

// Steps ball back and rebounds it
void rebound_ball(double x_rebound, double y_rebound){
    sprite_back(sprite_ball);
    double dx = sprite_dx(sprite_ball);
    double dy = sprite_dy(sprite_ball);
    sprite_turn_to(sprite_ball, dx*x_rebound, dy*y_rebound);
}

// Check if ball collides with paddle
void check_paddle(int paddle_x, int paddle_y){
    // Check if it hits the
    // right paddle (user controlled)
    // 1 unit wide
    if (sprite_x(sprite_ball) >= paddle_x && sprite_x(sprite_ball) < paddle_x + 1){
        // Add score, determine how it bounces later
        if (sprite_y(sprite_ball) >= paddle_y && sprite_y(sprite_ball) <= paddle_y + PADDLE_HEIGHT){
            gameState.score += 1;
        }

        // If it hits the top paddle
        if (sprite_y(sprite_ball) >= paddle_y && sprite_y(sprite_ball) < paddle_y + 1){
            // Moving downwards
            if (sprite_dy(sprite_ball) > 0){
                // If distance between paddle and top wall is greater than
                // height of ball, bounce off horizontally
                if (paddle_y-TOP_WALL > 1){
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
        else if (sprite_y(sprite_ball) >= paddle_y+1 && sprite_y(sprite_ball) <= paddle_y + PADDLE_HEIGHT - 1){
            rebound_ball(-1.0, 1.0);
        }

        // Hits bottom paddle
        else if (sprite_y(sprite_ball) > paddle_y + PADDLE_HEIGHT - 1 && sprite_y(sprite_ball) <= paddle_y + PADDLE_HEIGHT){
            // Moving Upwards
            if (sprite_dy(sprite_ball) < 0){
                // If distance between paddle and bottom wall is greater than
                // height of ball, bounce off horizontally
                if (BOTTOM_WALL - (paddle_y+PADDLE_HEIGHT) > 1){
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

// Basic AI for left paddle
void update_base_ai(void){
    // If ball is higher than paddle
    // Try to center it
    if (sprite_y(sprite_ball) < LEFT_PADDLE_Y + 3){
        if (LEFT_PADDLE_Y > 3){
            LEFT_PADDLE_Y -= 1;
        }
    }
    else if (sprite_y(sprite_ball) > LEFT_PADDLE_Y + 3){
        if (LEFT_PADDLE_Y + PADDLE_HEIGHT < SCREEN_HEIGHT-1){
            LEFT_PADDLE_Y += 1;
        }
    }
}

void check_balls(void){
    // Accelerate ball to black hole
    if (gameState.level == 3){
        bh_time_elapsed_ms += DELAY;

        if (bh_time_elapsed_ms >= 5000){
            double rebound_x = 0.002, rebound_y = 0.0002;

            // Sprite ball is accelerating to left
            if (sprite_x(sprite_ball) > sprite_x(sprite_black_hole)){
                rebound_x = -rebound_x;
            }
            // Accelerating down
            if (sprite_y(sprite_ball) > sprite_y(sprite_black_hole)){
                rebound_y = -rebound_y;
            }
            
            double dx = sprite_dx(sprite_ball);
            double dy = sprite_dy(sprite_ball);

            // Threshold dx 
            // > 0.1 or < -0.1
            // it will be the minimum velocity speed it can reach
            if (((dx > 0) && dx+rebound_x > 0.1) || ((dx < 0) && (dx+rebound_x < -0.1))){        
                sprite_turn_to(sprite_ball, dx+rebound_x, dy+rebound_y);
            }
        }
    }
    else if(gameState.level != 3){
        bh_time_elapsed_ms = 0;
    }

    // If balls are beyond bounds @ top/bottom
    // re bounce it
    if (sprite_y(sprite_ball) >= BOTTOM_WALL || sprite_y(sprite_ball) <= TOP_WALL){
        rebound_ball(1.0, -1.0);
    }

    // Left wall only bounce @ level 1
    if (gameState.level == 1 || TRAIN_AI){
        if (sprite_x(sprite_ball) <= LEFT_WALL){// || sprite_x(sprite_ball) >= RIGHT_WALL){
            rebound_ball(-1.0, 1.0);
        }
    }

    // Check if ball smashed with paddle
    check_paddle(RIGHT_PADDLE_X, RIGHT_PADDLE_Y);

    // Left Paddle only exists at level 1
    if (gameState.level > 1){
        check_paddle(LEFT_PADDLE_X-1, LEFT_PADDLE_Y);

        // Update AI after collision checking
        update_base_ai();
    }

    // Check if ball is out of bounds
    if (sprite_x(sprite_ball) > RIGHT_WALL || sprite_x(sprite_ball) < LEFT_WALL){
        setup(); // reset ball position        
        clear_screen();
        show_screen();
        gameState.lives -= 1;                

        if (gameState.lives <= 0){
            game_over = true;
        }
        else{
            countdown();// countdown ball
        }    
    }
}

// Countdown for ball
void countdown(void){
    for (int i = 3; i > 0; i--){
        char b = i + '0'; // convert int to char

        // Draw char
        draw_char(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-5, b);
        show_screen();
        timer_pause(300);
    }
    //clear_screen();
    draw_string(SCREEN_WIDTH/2-10, SCREEN_HEIGHT/2-5, "!@-- Game Starto --@!");
    show_screen();
    timer_pause(1000);
}

// Setup our variables and sprites
void setup(void) {
    // Blachole thingy
    bh_time_elapsed_ms = 0;

    // Game over
    game_over = false;

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

    double dx = fRand(0.2, 0.3);
    double dy = fRand(0.2, 0.3);
    
    sprite_turn_to(sprite_ball, dx, dy); // random velocity

    sprite_black_hole = sprite_create(SCREEN_WIDTH/2-3, SCREEN_HEIGHT/2-1, 7, 5, black_hole_img);    
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

// Help screen!!!
void help_screen(void){
    // Clears any buffer @ screen
    clear_screen();

    // Draw help text
    // magic numbers!!!
    // jks just minusing length of string to center them
    draw_string(SCREEN_WIDTH/2-21, SCREEN_HEIGHT/2-5, "Kendrick Tan; n9701583; CAB202; 2016 Sem 2");
    draw_string(SCREEN_WIDTH/2-9, SCREEN_HEIGHT/2-3, "-- Genetic Pong --");
    draw_string(SCREEN_WIDTH/2-10, SCREEN_HEIGHT/2, "'l' to change levels");
    draw_string(SCREEN_WIDTH/2-6, SCREEN_HEIGHT/2+1, "'r' to reset");
    draw_string(SCREEN_WIDTH/2-12, SCREEN_HEIGHT/2+2, "'h' to display help text");
    draw_string(SCREEN_WIDTH/2-5, SCREEN_HEIGHT/2+4, "-- Rules --");
    draw_string(SCREEN_WIDTH/2-10, SCREEN_HEIGHT/2+6, "'w' to move paddle up");
    draw_string(SCREEN_WIDTH/2-11, SCREEN_HEIGHT/2+7, "'s' to move paddle down");

    show_screen();
    wait_char();
    clear_screen();
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
        reset_game_states();             
    }

    // Help
    else if (key == 'h'){
        help_screen();
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

    // Only draw black hole if level is 3
    if (gameState.level == 3 && bh_time_elapsed_ms >= 5000){
        sprite_draw(sprite_black_hole);
    }

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

    do{
        setup();
        help_screen();    

        // Countdown mofo!!
        countdown();
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

        clear_screen();

        draw_string(SCREEN_WIDTH/2-14, SCREEN_HEIGHT/2, "Game over, play again? (y/n)");
        show_screen();
    }while(wait_char() == 'y');

    cleanup();

    return 0;
}
