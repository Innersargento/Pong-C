//////////////////////////
//       Include        // 
//////////////////////////
#include <SDL3/SDL.h>
#include <stdio.h>
#include <time.h>

//////////////////////////////////////
//       Função Facilitadora        // 
//////////////////////////////////////
#define SDL_MAIN_USE_CALLBACKS 
#include <SDL3/SDL_main.h> 

/////////////////////////////
//       DEFINIÇões        // 
/////////////////////////////
#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define WINDOW_NAME "Pong"

//////////////////////////
//       STRUCTS        // 
//////////////////////////
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_FRect rects[3];
} Game;

typedef struct {
    float vel_x;
    float vel_y;
    float radius;
} Ball;

//////////////////////////////////////
//       Declarações globais        // 
//////////////////////////////////////

Game game = {
    .window = NULL,
    .renderer = NULL,
    .rects = {0}
};

SDL_Color White = {
    .r = 255,
    .g = 255,
    .b = 255,
    .a = SDL_ALPHA_OPAQUE
};

SDL_Color Black = {
    .r = 0,
    .g = 0,
    .b = 0,
    .a = SDL_ALPHA_OPAQUE
};

Ball ball = {1, 1, 10};

Uint64 old_now = 0;

//////////////////////////
//       FUNÇÕES        // 
//////////////////////////

int init(Game* game){

    if(!SDL_Init(SDL_INIT_VIDEO)){
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        return 1;
    }
    if(!SDL_CreateWindowAndRenderer(
        WINDOW_NAME, 
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0,
        &game->window,
        &game->renderer
        )) {
            fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError()); 
            return 1;
    }

    if(!SDL_SetRenderLogicalPresentation(
        game->renderer, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        SDL_LOGICAL_PRESENTATION_LETTERBOX)){
            fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError()); 
            return 1;
    }

    return 0;
}

void set_ball_pos(float x, float y){

    game.rects[0] = (SDL_FRect) {
        .x = x,
        .y = y,
        .w = 100,
        .h = 100
    };

}

void draw_everything(){

    SDL_SetRenderDrawColor(game.renderer, Black.r, Black.g, Black.b, Black.a);
    SDL_RenderFillRect(game.renderer, &game.rects[0]);

}

void update_ball_pos(Uint64 now, float vel_x, float vel_y){

    float new_x;
    float new_y;

    new_x = game.rects[0].x + (vel_x * (now - old_now));
    new_y = game.rects[0].y + (vel_y * (now - old_now));
    game.rects[0].x = new_x;
    game.rects[0].y = new_y;

}

void draw_canvas(){
    SDL_SetRenderDrawColor(game.renderer, White.a, White.g, White.g, White.a);
    SDL_RenderClear(game.renderer);
}

void detect_collision() {

    if(game.rects[0].y < 0){
        game.rects[0].y += (game.rects[0].y * -1);
        ball.vel_y *= -1;
    }

    if(game.rects[0].x + game.rects[0].w >= SCREEN_WIDTH){
        game.rects[0].x -= ((game.rects[0].x + game.rects[0].w) - SCREEN_WIDTH);
        ball.vel_x *= -1;
    }

    if(game.rects[0].x < 0){
        game.rects[0].x += (game.rects[0].x * -1);
        ball.vel_x *= -1;
    }

    if(game.rects[0].y + game.rects[0].h >= SCREEN_HEIGHT){
        game.rects[0].y -= ((game.rects[0].y + game.rects[0].h) - SCREEN_HEIGHT);
        ball.vel_y *= -1;
    }
}


////////////////////////////////////////
//       FUNÇÕES MAIN CALLBACK        // 
////////////////////////////////////////

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv){

    if(init(&game)) return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
} 

SDL_AppResult SDL_AppIterate(void *appstate){

    const Uint64 now = SDL_GetTicks();   

    draw_canvas();
    detect_collision();
    update_ball_pos(now, ball.vel_x, ball.vel_y);
    draw_everything();
    SDL_RenderPresent(game.renderer);

    old_now = now;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){

    switch(event->type) {
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS; break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:set_ball_pos(event->button.x, event->button.y);break;

        default: return SDL_APP_CONTINUE; 
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){

}