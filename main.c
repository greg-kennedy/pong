/* Pong demo
   Shows a few basics of SDL - event polling, drawing to the screen,
   loading BMP files, opening windows, etc.
   
   Written for UAGDC - http://uagdc.webhop.org
   
   Greg Kennedy
   9/22/04
   hornpipe2@hotmail.com */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include "SDL/SDL.h"

/* This is the timing block.  It's here to make porting from Windows to Linux
   just a bit easier.  If you need to time something, you should put this at the
   start of your code and use getMS(), which returns a long int of a number of
   milliseconds.  This will do for basic timing, though there are much more accurate
   methods available. */

#if defined (WIN32)
#include <windows.h>
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")

long getMS() {
	return GetTickCount();
}
#else
long getMS() {
	struct timeval temptime;
	gettimeofday(&temptime,NULL);
	return temptime.tv_usec / 1000 + temptime.tv_sec * 1000;
}
#endif

/* I decided not to break the program up into functions because it's pretty simple.
   On a larger project you might want to have a draw() routine, for example, which
   handles just redrawing the screen.  It keeps everything a lot more manageable. */

int main (int argc, char *argv[])
{
	SDL_Rect paddle1,paddle2,ball;		/* The SDL rectangles for the images */
	SDL_Surface *pad1,*pad2,*bal,*screen; /* These hold the bitmaps and the screen surface */
	SDL_Event event; /* This is for the events (keypresses, etc) */

    int left=200,right=200,ballx=300,bally=200,done=0,key=0;
    double angle=0,speed=5;
    long firsttime, secondtime;    /* all the game specific variables */
    
    /* Initialize SDL */
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        printf ("Couldn't initialize SDL: %s\n", SDL_GetError ());
        exit (1);
    }
    atexit (SDL_Quit);

    /* Set 640x480 16-bits video mode.  This line will open the window. */
    screen = SDL_SetVideoMode (640, 480, 16, SDL_SWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        printf ("Couldn't set 640x480x16 video mode: %s\n",SDL_GetError ());
        exit (2);
    }
    SDL_WM_SetCaption ("SDL Pong Demo", NULL);        /* Sets the title for the SDL window */
    
    pad1=SDL_LoadBMP("paddle1.bmp");    /*SDL_LoadBMP will load a bitmap at the file location
                                specified into the surface. VERY handy. */
    pad2=SDL_LoadBMP("paddle2.bmp");
    bal=SDL_LoadBMP("ball.bmp");
    
    if (pad1==NULL || pad2==NULL || bal==NULL) /* If any bitmap fails to load the surface pointer
                                            will be empty. */
    {
        printf("Couldn't load all images.\n");
        exit(3);
    }    

    paddle1.w=pad1->w;        /* Set the rectangles to match the size of the bitmaps. */
    paddle1.h=pad1->h;
    paddle1.x=64;
    paddle2.w=pad2->w;
    paddle2.h=pad2->h;
    paddle2.x=560;
    ball.w=bal->w;
    ball.h=bal->h;

    firsttime=getMS();    /* Start the game timer. */

    while (!done)
    {
        /* Check for events */
        while(firsttime+15>secondtime)    /* Check the time to see if we should go on. */
        {
        secondtime=getMS();
            
		if (SDL_PollEvent(&event)>0)        /* If an event has occurred... */
        switch (event.type)    /* This switch determines what sort of event happened (keyboard, mouse, etc) */
        {
            case SDL_KEYDOWN:        /* If it was a keypress, set a flag */
                if (event.key.keysym.sym == SDLK_UP)
                    key=-10;
				if (event.key.keysym.sym == SDLK_DOWN)
				    key=10;
				if (event.key.keysym.sym == SDLK_ESCAPE)
					done=1;
                break;
            case SDL_KEYUP:        /* If it was a key release, unset the flag */
                if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
                    key=0;
                break;
            case SDL_QUIT:       /* If they clicked the X in the corner, quit */
                done = 1;
                break;
            default:
                break;
        }
        }    
        
        firsttime=secondtime;            /* reset the frame timer */
        
        left=left+key;
        if (left<0) left=0;
        if (left>416) left=416;        /* move left player's paddle */
        
        paddle1.y=left;        /* update the image location on screen */
        paddle2.y=right;
        
        ballx=ballx+(int)(speed*cos(angle/360*6.28));        /* Here we figure out where
                                        the ball has moved since last frame. */
        bally=bally+(int)((double)abs((int)speed)*sin(angle/360*6.28));
        if (bally<0){
            bally=-bally;
            angle=-angle;
        }else if(bally>464){        /* wall collisions... */
            bally=464-(bally-464);
            angle=-angle;
        }
        
        if (ballx<81){            /* check to see if it hit the left paddle */
            ballx=81;
            if (bally>=left && bally<left+24)        /* For fun I thought I'd split
                                                the paddle into three parts and let
                                                the edges have an effect on the angle. */
            {
                angle=angle-15;
                speed=.5-speed;
            }else if (bally>=left+24 && bally<=left+40){
                speed=.5-speed;
            }else if (bally>left+40 && bally<=left+64){
                angle=angle+15;
                speed=.5-speed;
            }else{
                speed=-5;
                ballx=300;
                bally=200;
            }
        }    
        if (ballx>543){                /* Here's where we check the right paddle */
            ballx=543;
            if (bally>=right && bally<right+24)
            {
                angle=angle+15;
                speed=-.5-speed;
            }else if (bally>=right+24 && bally<=right+40){
                speed=-.5-speed;
            }else if (bally>right+40 && bally<=right+64){
                angle=angle-15;
                speed=-.5-speed;
            }else{
                speed=5;
                ballx=300;
                bally=200;
            }    
        }
        
        if (angle>80) angle=80;         /* don't let angles get too ridiculous */
        if (angle<-80) angle=-80;
        
        if (right<bally-40)        /* This is the four-line AI... just keeps the
                                paddle centered on the ball. */
                right=right+2;
        else if (right>bally-24)
                right=right-2;

        ball.x=ballx;                /* update ball position on screen */
        ball.y=bally;
        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));        /*black out the screen */
        SDL_BlitSurface(pad1,NULL,screen,&paddle1);        /* put left paddle down */
        SDL_BlitSurface(pad2,NULL,screen,&paddle2);        /* right paddle */
        SDL_BlitSurface(bal,NULL,screen,&ball);        /* ball */

        SDL_Flip(screen);        /* This will swap the buffer with the main screen.  It makes
                for a smooth update.  NOTE THAT YOU WON'T SEE ANYTHING UNLESS YOU PUT AN
                SDL_Flip(screen) IN YOUR PROGRAM SOMEWHERE! */
    }

    return 0;
}
