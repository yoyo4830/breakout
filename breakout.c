//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// define pAddle Width & Height
#define PADWIDTH 100
#define PADHGHT 6
// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
	
    // keep playing until game over
    double velocityy = 4.0;
    double velocityx = drand48()* 3.3;
    while (lives > 0 && bricks > 0)
    {
       // follow mouse forever
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure paddle follows top cursor
                double x = getX(event) - getWidth(paddle) / 2;
      
                setLocation(paddle, x, 550);
            }
        }
    
		// ball bounce
       // initial velocity
  
    
    // bounce forever
        // move circle along y-axis
        move(ball, velocityx , velocityy);

        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            velocityx = -velocityx;
        }
        // bounce off top
        if (getY(ball) <= 0)
        {
        	velocityy = -velocityy;
        }

        // bounce off left edge of window
        if (getX(ball) <= 0)
        {
            velocityx = -velocityx;
        }
        // linger before moving again
        pause(10);
        // collide with paddle
       GObject object = detectCollision(window, ball);       
       if (object != NULL) 
       {  
       if (object == paddle)        
       {
            if (velocityy > 0)
            {         
            velocityy = -velocityy;
            }           
       }              
        // update scorboard & remove bricks hit
       else if (strcmp(getType(object), "GRect") == 0 )
     //  else 
        {
        	points++;
            removeGWindow(window, object);
        	velocityy = -velocityy;
        	updateScoreboard(window, label, points);
         }
       }
        
        // restart game after losing life

        if (getY(ball) + getHeight(ball) > getHeight(window))
        {
            lives--;
            if (lives >= 2 || lives == 0)
            {
                char s[12];
                sprintf(s, "%i lives left", lives);
                setLabel(label, s);
                setLocation(label, 180, 13);
            }
            else if (lives == 1)
            {
                char s[12];
                sprintf(s, "%i life left", lives);
                setLabel(label, s);
                setLocation(label, 180, 13);
            }
            waitForClick();
            setLocation(ball, 180, 100);
        }
        if (points == 50)
        {
            char s[12];
            sprintf(s, "You won!!");
            setLabel(label, s);
            setLocation(label, 175, 300);
        }
       
        
    }
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
  
}
/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
	int locx = 5;
	int locy = 30;
	for (int i = 0; i < 5; i++)
	{
    	for (int j = 0; j < 10; j++)
    	{
    		GRect brick = newGRect(locx, locy, 36, PADHGHT);
    		setColor (brick, "MAGENTA");
    		setFilled (brick, true);
    		add (window, brick);
    		// 
    		if (locx < 324)
    		{
    		locx = locx + 39;
    		}
    	}
    	locx = 5;
    	locy = locy + 10;
	}	
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(190, 150, 20, 20);
	setFilled (ball, true);
    // add circle to window
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(180, 550, PADWIDTH, PADHGHT);
    setColor (paddle, "MAGENTA");
    setFilled (paddle, true);
    add (window, paddle);
    return paddle;
}
/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // TODO
    GLabel Scoreboard = newGLabel("");
    add (window, Scoreboard);
    setLocation(Scoreboard, 300, 200);
    return Scoreboard;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[13];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
