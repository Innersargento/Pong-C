//////////////////////////
//       Include        // 
//////////////////////////
#include <SDL3/SDL.h>
#include <iostream>
#include <initializer_list>
#include <stdio.h>
#include <time.h>
#include <math.h>

//////////////////////////////////////
//       Função Facilitadora        // 
//////////////////////////////////////
#define SDL_MAIN_USE_CALLBACKS 
#include <SDL3/SDL_main.h> 

/////////////////////////////
//       DEFINIÇÕES        // 
/////////////////////////////
#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define WINDOW_NAME "Pong"
#define PLAYER_SPEED 60

//////////////////////////
//       STRUCTS        // 
//////////////////////////

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} Game;

typedef struct {
    float x;
    float y;
} vec2;

//////////////////////////
//       CLASSES        // 
//////////////////////////

class Player;

class Ball {
    private:
        float radius;
        vec2 vel;
        vec2 pos;
    public:
        void setPos(float x, float y){
            pos = {x, y};
        }
        void setVel(float vel_x, float vel_y){
            vel = {vel_x, vel_y};
        }
        void setPos(vec2 pos){
            this->pos = pos;
        }
        void setVel(vec2 vel){
            this->vel = vel;
        }
        void setRadius(float radius){
            this->radius = radius;
        }
        vec2 getVel(){
            return vel;
        }
        vec2 getPos(){
            return pos;
        }
        float getRadius(){
            return radius;
        }
        float getX(){
            return pos.x;
        }
        float getY(){
            return pos.y;
        }
        
        void move_ball(Uint64 now);

        void detect_wall_collision();

        void render_ball(SDL_Renderer* renderer);

        void detect_object_collision(std::initializer_list<Player*> players);

        Ball(){
            pos = {50, 50};
            vel = {1, 1};
            radius = 30.0;
        }
};


class Player {
    private:
        SDL_FRect body;
        int type;
        void setPosY(float y){
            if(y < 0)
                body.y = 0;
            else if(y + body.h > SCREEN_HEIGHT)
                body.y = SCREEN_HEIGHT - body.h;
            else
                body.y = y;
        }
    public:
        void move_player(float y){
            setPosY(body.y - y);
        }
        SDL_FRect getBody(){
            return body;
        }
        Player(int type){
            this->type = type;
            if(type == 1) body = {70, 200, 25, 300};
            else body = {SCREEN_WIDTH - 70, 200, 25, 300};
        }
};

//////////////////////////////////////
//       Declarações globais        // 
//////////////////////////////////////

Game game = {
    .window = NULL,
    .renderer = NULL,
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

Player p1(1);
Player p2(2);
Ball ballObj;

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

void draw_everything(){

    SDL_FRect rects[2] = { p1.getBody(), p2.getBody()};
    SDL_FRect linha = {SCREEN_WIDTH/2, 0, 5, SCREEN_HEIGHT};

    SDL_SetRenderDrawColor(game.renderer, Black.r, Black.g, Black.b, Black.a);
    ballObj.render_ball(game.renderer);
    SDL_RenderFillRect(game.renderer, &rects[0]);
    SDL_RenderFillRect(game.renderer, &rects[1]);
    SDL_RenderFillRect(game.renderer, &linha);

}

void draw_canvas(){
    SDL_SetRenderDrawColor(game.renderer, White.a, White.g, White.g, White.a);
    SDL_RenderClear(game.renderer);
}

void rumble(int speed){
    const bool* keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_W])
        p1.move_player(+speed);

    if(keys[SDL_SCANCODE_S])
        p1.move_player(-speed);

    if(keys[SDL_SCANCODE_UP])
        p2.move_player(+speed);

    if(keys[SDL_SCANCODE_DOWN])
        p2.move_player(-speed);
}


//////////////////////////////////////////
//       FUNÇÕES DAS CLASSE BALL        // 
//////////////////////////////////////////

void Ball::detect_wall_collision() {

    if(pos.x < 0){
        pos.x = 0;
        vel.x *= -1;
    }

    if(pos.x + radius * 2 > SCREEN_WIDTH){
        pos.x = SCREEN_WIDTH - radius * 2;
        vel.x *= -1;
    }

    if(pos.y < 0){
        pos.y = 0;
        vel.y *= -1;
    }

    if(pos.y + radius * 2 > SCREEN_HEIGHT){
        pos.y = SCREEN_HEIGHT - radius * 2;
        vel.y *= -1;
    }

}

void Ball::move_ball(Uint64 now){

    pos.x = pos.x + (vel.x * (now - old_now));
    pos.y = pos.y + (vel.y * (now - old_now));

}

void Ball::render_ball(SDL_Renderer* renderer){

    SDL_FRect pixel {pos.x, pos.y, 5, 5};

    int a = pos.x + radius;
    int b = pos.y + radius;


    for(int x = pos.x; x <= pos.x + (radius * 2); x++){
        for(int y = pos.y; y <= pos.y + (radius * 2); y++){
            if(sqrt(pow(x - a, 2) + pow(y - b, 2)) <= radius){
                pixel.x = x;
                pixel.y = y;
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

}

void Ball::detect_object_collision(std::initializer_list<Player*> players){

    for(Player* player : players){
        if(pos.x               <=         player->getBody().x + player->getBody().w && //
           pos.x + (radius*2)  >=         player->getBody().x && 
           pos.y + (radius*2)  >          player->getBody().y && 
           pos.y               <          player->getBody().y + player->getBody().h) 
        {
            vel.x *= -1;
        }
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
    ballObj.move_ball(now);
    ballObj.detect_wall_collision();
    ballObj.detect_object_collision({&p1, &p2});
    draw_everything();
    SDL_RenderPresent(game.renderer);

    old_now = now;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){

    switch(event->type) {
        case SDL_EVENT_QUIT: return SDL_APP_SUCCESS; break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN: ballObj.setPos(event->button.x, event->button.y); break;

        case SDL_EVENT_KEY_DOWN: rumble(PLAYER_SPEED);
        break;

        default: return SDL_APP_CONTINUE; 
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){

}