
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <string>
#include "hero.hh"
#include "opponent.hh"
#include "enums.hh"

//=================================================
// CONSTANTS
//=================================================

const int WIDTH = 800;
const int HEIGHT = 450;

//=================================================
// PROTOTYPES
//=================================================
void changeState (int &state, int new_state);
void changeBattleState (int &state, int new_state);

void initHero (int hero_class, Hero &hero, ALLEGRO_BITMAP *image);
void drawHeroClasses (ALLEGRO_FONT *font10);
void drawHeroWalking (Hero &hero);
void updateHeroWalking (Hero &hero);
void drawMeme (int random, ALLEGRO_FONT *font10);
void drawBackground (ALLEGRO_BITMAP  *background_image);

void drawHeroFighting (Hero &hero);
void drawHeroSelect (Hero &hero, int hero_select, ALLEGRO_FONT *font10);
int updateHeroAttack (Hero &hero, int attackframe, int hero_speed);
int updateHeroParry (Hero &hero, int attackframe, int hero_speed);
int updateHeroFlee (Hero &hero, int attackframe, int hero_speed);
int updateHeroMeme (int attackframe);

void initOpponent (Opponent &opponent,  ALLEGRO_BITMAP *image);
void drawOpponent (Opponent &opponent);
int updateOpponentAttack (Opponent &opponent, int attackframe);


//=================================================
// MAIN FUNCTION
//=================================================
int main(int argc, char **argv)
{
  //==============================================
  // SHELL VARIABLES
  //==============================================
  bool done = false;
  bool render = false;

  int state = -1;
  int battle_state = -1;
  int hero_select = 0; // used in battle
  int hero_class = 1; // ska ändras

  float gameTime = 0;
  int frames = 0;
  int gameFPS = 0;
  const int FPS = 60; //#masterrace


  //==============================================
  // PRIMITIVE PROJECT VARIABLES
  //==============================================
  bool keys[] = {false, false, false, false, false, false, false};
  Hero hero;
  Opponent opponent;

  int hero_speed = 4;
  int attackframe;

  //==============================================
  // ALLEGRO VARIABLES
  //==============================================
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  ALLEGRO_TIMER *timer;
  ALLEGRO_FONT *font10;
  ALLEGRO_BITMAP *hero_image;
  ALLEGRO_BITMAP *opponent_image;
  ALLEGRO_BITMAP *background_image;
  ALLEGRO_BITMAP *foreground_image;
  ALLEGRO_BITMAP *battle_background_image;

  //==============================================
  // ALLEGRO INIT FUNCTIONS
  //==============================================
  if (!al_init()) {										//initialize Allegro
    return -1;
  }
  display = al_create_display(WIDTH, HEIGHT);			//create display object

  if (!display) {										//test display object
    return -2;
  }

  //==============================================
  // ADDON INSTALL
  //==============================================
  al_install_keyboard();
  al_init_image_addon();
  al_init_font_addon();
  al_init_ttf_addon();
  al_init_primitives_addon();


  //==============================================
  // PROJECT INIT
  //==============================================
  font10 = al_load_font("fonts/PressStart2P.ttf", 10, 0);
  hero_image = al_load_bitmap("graphics/heros.png");
  opponent_image = al_load_bitmap("graphics/dragon.png");
  background_image = al_load_bitmap("graphics/background.png");
  foreground_image = al_load_bitmap("graphics/foreground.png");
  battle_background_image = al_load_bitmap("graphics/battle_background.png");

  //==============================================
  // TIMER INIT AND STARTUP EVENT REGISTER
  //==============================================
  event_queue = al_create_event_queue();
  timer = al_create_timer(1.0 / FPS);

  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());

  srand(time(NULL)); // seeds random after time
  changeState(state, MAINMENU); // set default state
    al_start_timer(timer);
  gameTime = al_current_time();
  while (!done) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev);

    //==============================================
    // INPUT
    //==============================================
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
      switch (ev.keyboard.keycode) {
      case ALLEGRO_KEY_ESCAPE:
        done = true;
        break;
      case ALLEGRO_KEY_LEFT:
        keys[LEFT] = true;
        break;
      case ALLEGRO_KEY_RIGHT:
        keys[RIGHT] = true;
        break;
      case ALLEGRO_KEY_UP:
        keys[UP] = true;
        switch (state) {
        case MAINMENU:
          if (hero_class < 2) {
            hero_class = 8;
          }
          else {
            hero_class -=1;
          }
          break;
        case FIGHTING:
          if (battle_state == HERO_SELECT && hero_select < 1) {
            hero_select = 3;
          }
          else if (battle_state == HERO_SELECT) {
            hero_select -= 1;
          }
        }
        break;
      case ALLEGRO_KEY_DOWN:
        keys[DOWN] = true;
        switch (state) {
        case MAINMENU:
          if (hero_class > 7) {
            hero_class = 1;
          }
          else {
            hero_class += 1;
          }
          break;
        case FIGHTING:
          if (battle_state == HERO_SELECT && hero_select > 2) {
            hero_select = 0;
          }
          else if (battle_state == HERO_SELECT) {
            hero_select += 1;
          }
        }
        break;
      case ALLEGRO_KEY_SPACE:
        keys[SPACE] = true;
        break;
      case ALLEGRO_KEY_X:
        keys[X] = true;
        break;
      case ALLEGRO_KEY_C:
        keys[C] = true;
        break;
      }
	}
    else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
	  switch (ev.keyboard.keycode) {
      case ALLEGRO_KEY_ESCAPE:
        done = true;
        break;
      case ALLEGRO_KEY_LEFT:
        keys[LEFT] = false;
        break;
      case ALLEGRO_KEY_RIGHT:
        keys[RIGHT] = false;
        break;
      case ALLEGRO_KEY_UP:
        keys[UP] = false;
        break;
      case ALLEGRO_KEY_DOWN:
        keys[DOWN] = false;
        break;
      case ALLEGRO_KEY_SPACE:
        keys[SPACE] = false;
        break;
      case ALLEGRO_KEY_X:
        keys[X] = false;
        break;
      case ALLEGRO_KEY_C:
        keys[C] = false;
        break;
      }
	}
    //==============================================
    // GAME UPDATE
    //==============================================
    else if (ev.type == ALLEGRO_EVENT_TIMER) {
      render = true;

      // UPDATE FPS===========
      frames++;
      if (al_current_time() - gameTime >= 1) {
        gameTime = al_current_time();
        gameFPS = frames;
        frames = 0;
      }
      // UPDATE PER STATE=====================
      switch (state) {

      case MAINMENU:
        if (keys[SPACE]) {
          initHero(hero_class - 1, hero, hero_image);
          changeState(state, WALKING);
        }
        break;

      case WALKING:
        hero.animationSpeed = 1;
        if (keys[UP]) {
          hero.walkingDirection = WALK_UP;
          if (hero.y > 0) {
            hero.y -= hero_speed;
          }
        }
        else if (keys[DOWN]) {
          hero.walkingDirection = WALK_DOWN;
          if (hero.y + hero.frameHeight < HEIGHT) {
            hero.y += hero_speed;
          }
        }
        if (keys[LEFT]) {
          hero.walkingDirection = WALK_LEFT;
          if (hero.x > 0) {
            hero.x -= hero_speed;
          }
        }
        else if (keys[RIGHT]) {
          hero.walkingDirection = WALK_RIGHT;
          if (hero.x + hero.frameWidth < WIDTH) {
            hero.x += hero_speed;
          }
        }
        updateHeroWalking(hero);
        if (!keys[UP] && !keys[DOWN] && !keys[LEFT] && !keys[RIGHT]) { // återgå till stillastående
          hero.animationSpeed = 0;
          hero.curFrame = 1;
        }
        else if (!(rand() % 200)){
          initOpponent(opponent, opponent_image);
          changeState(state, FIGHTING);
          changeBattleState(battle_state, HERO_SELECT);
        }
        break;


      case FIGHTING:
        switch (battle_state) {
        case HERO_SELECT:
          if (keys[SPACE]) {
            changeBattleState(battle_state, HERO_ATTACK); // alltid attack just nu
            attackframe = 0;
          }
          break;

        case HERO_ATTACK:
          if (hero_select == ATTACK) {
              if (!updateHeroAttack(hero, attackframe, hero_speed)) {
                changeBattleState(battle_state, OPPONENT_ATTACK);
                attackframe = 0;
              }
              else {
                attackframe += 1;
              }
            }

            else if (hero_select == PARRY) {
              if (!updateHeroParry(hero, attackframe, hero_speed)) {
                changeBattleState(battle_state, OPPONENT_ATTACK);
                attackframe = 0;
              }
              else {
                attackframe += 1;
              }
            }

            else if (hero_select == FLEE) {
              if (!updateHeroFlee(hero, attackframe, hero_speed)) {
                changeBattleState(battle_state, OPPONENT_ATTACK);
                attackframe = 0;
              }
              else {
                attackframe += 1;
              }
            }

            else if (hero_select == MEME) {
              if (!updateHeroMeme(attackframe)) {
                changeBattleState(battle_state, OPPONENT_ATTACK);
                attackframe = 0;
              }
              else {
                attackframe += 1;
              }
            }

          break;

        case OPPONENT_ATTACK:
          if (!updateOpponentAttack(opponent, attackframe)) {
            changeBattleState(battle_state, HERO_SELECT);
          }
          else {
            attackframe += 1;
          }
          break;
        }
      }
    }

    //==============================================
    // RENDER
    //==============================================
    if (render && al_is_event_queue_empty(event_queue)) {
      render = false;
      al_draw_textf(font10, al_map_rgb(255, 0, 255), 5, 5, 0, "FPS: %i", gameFPS);	//display FPS on screen

      // RENDER PER STATE================
      switch (state) {

      case MAINMENU:
        drawHeroClasses(font10);
        al_draw_filled_triangle(40, 30 * hero_class + 5, 45, 30 * hero_class, 40, 30 * hero_class - 5, al_map_rgb(255,0,255));
        break;

      case WALKING:
        drawBackground(background_image);
        drawHeroWalking(hero);
        drawBackground(foreground_image);
        break;

      case FIGHTING:
        drawBackground(battle_background_image);
        switch (battle_state) {

        case HERO_SELECT:
          drawHeroFighting(hero);
          drawOpponent(opponent);
          drawHeroSelect(hero, hero_select, font10);
          break;

        case HERO_ATTACK:
          drawHeroWalking(hero);
          drawOpponent(opponent);
          int random_meme;
          if (attackframe < 1) {
            random_meme = rand() % 10;
          }
          if (hero_select == MEME) {
            drawMeme(random_meme, font10);
          }
          break;

        case OPPONENT_ATTACK:
          drawHeroFighting(hero);
          drawOpponent(opponent);
          break;
        }
      }

      // FLIP BUFFER========================
      al_flip_display();
      al_clear_to_color(al_map_rgb(0,0,0));
    }
  }

  //==============================================
  // DESTROY PROJECT OBJECTS
  //==============================================

  // SHELL OBJECTS=================================
  al_destroy_font(font10);
  al_destroy_timer(timer);
  al_destroy_event_queue(event_queue);
  al_destroy_display(display);

  return 0;
}


void changeState (int &state, int new_state)
{
  // UPDATE OLD STATE=========================
  switch (state) {
  default: break;
  }

  state = new_state;

  // UPDATE NEW STATE==========================
  switch (state) {
  default: break;
  }
}

void changeBattleState (int &state, int new_state)
{
  // UPDATE OLD STATE=========================
  switch (state) {
  case HERO_ATTACK:
    std::cout << "from hero \n";
    break;
  case HERO_SELECT:
    std::cout << "from selects\n";
    break;
  case OPPONENT_ATTACK:
    std::cout << "from oppo \n";
    break;

  default: break;
  }

  state = new_state;

  // UPDATE NEW STATE==========================
  switch (state) {
  case HERO_ATTACK:
    std::cout << "to hero \n";
    break;
  case HERO_SELECT:
    std::cout << "to selects \n";
    break;
  case OPPONENT_ATTACK:
    std::cout << "to oppo \n";
    break;

  default: break;
  }
}


void initHero (int hero_class, Hero &hero, ALLEGRO_BITMAP *image)
{
  hero.x = WIDTH/2;
  hero.y = HEIGHT/2;
  hero.level = 1;
  hero.xp = 0;
  hero.max_xp = 100;
  hero.maxFrame = 3;
  hero.curFrame = 1;
  hero.frameDelay = 10;
  hero.frameCount = 0;
  hero.frameWidth = 32;
  hero.frameHeight = 32;
  hero.animationColumns = 3;
  hero.walkingDirection = WALK_UP;
  hero.animationSpeed = 1;
  switch (hero_class) {
  case HERO_IKE:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 1;
    hero.column = 2;
    hero.image = image;
    break;
  case HERO_MARTH:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 1;
    hero.column = 3;
    hero.image = image;
    break;
  case HERO_IKAROS:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 1;
    hero.column = 4;
    hero.image = image;
    break;
  case HERO_ZELDA:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 2;
    hero.column = 1;
    hero.image = image;
    break;
  case HERO_ZSSAMUS:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 2;
    hero.column = 2;
    hero.image = image;
    break;
  case HERO_PEACH:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 2;
    hero.column = 3;
    hero.image = image;
    break;
  case HERO_PALUTENA:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 2;
    hero.column = 4;
    hero.image = image;
    break;
  default:
    hero.max_hp = 50;
    hero.hp = 50;
    hero.max_mana = 30;
    hero.mana = 30;
    hero.row = 1;
    hero.column = 1;
    hero.image = image;
    break;
  }
}

void drawHeroClasses (ALLEGRO_FONT *font10) {
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 30, 0, "link");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 2*30, 0, "ike");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 3*30, 0, "marth");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 4*30, 0, "ikarus");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 5*30, 0, "zelda");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 6*30, 0, "ZSsamus");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 7*30, 0, "peach");
  al_draw_textf(font10, al_map_rgb(255,255,255), 50, 8*30, 0, "palutena");
}

void drawHeroWalking (Hero &hero) {
  int fx = (hero.curFrame % hero.animationColumns) * hero.frameWidth + (hero.column - 1) * 3 * hero.frameWidth;
  int fy = (hero.walkingDirection * hero.frameHeight) + (hero.row - 1) * 4 * hero.frameHeight;
  al_draw_bitmap_region(hero.image, fx, fy, hero.frameWidth, hero.frameHeight, hero.x, hero.y, 0);
}

void drawHeroFighting (Hero &hero) {
  hero.x = 3 * WIDTH / 4;
  hero.y = HEIGHT / 2;
  int fx = hero.frameWidth + (hero.column - 1) * 3 * hero.frameWidth;
  int fy = (WALK_LEFT * hero.frameHeight) + (hero.row - 1) * 4 * hero.frameHeight;
  al_draw_bitmap_region(hero.image, fx, fy, hero.frameWidth, hero.frameHeight, 3 * WIDTH / 4, HEIGHT / 2, 0);
}

void updateHeroWalking (Hero &hero) {
  if (++hero.frameCount >= hero.frameDelay) {
    hero.curFrame += hero.animationSpeed;
    if (hero.curFrame >= hero.maxFrame) {
      hero.curFrame = 0;
    }// inga negativa ritningar i framebyten..
    hero.frameCount = 0;
  }
}

void initOpponent (Opponent &opponent, ALLEGRO_BITMAP *image) {
  opponent.x = WIDTH / 10;
  opponent.y = HEIGHT / 2;

  opponent.hp = 50;
  opponent.max_hp = 50;

  opponent.action = DRAGON_STAND;

  opponent.image = image;
}

void drawOpponent (Opponent &opponent) {
  if (opponent.action == DRAGON_STAND) {
    al_draw_bitmap_region(opponent.image, 0, 0, 146, 183, opponent.x, opponent.y - 100, 0);
  }
  else {
    al_draw_bitmap_region(opponent.image, 146, 0, 363 - 146, 183, opponent.x, opponent.y - 100, 0);
  }
}

void drawHeroSelect (Hero &hero, int hero_select, ALLEGRO_FONT *font10)
{
  int draw_height = 30;
  al_draw_filled_rectangle(0,HEIGHT - 150,WIDTH,HEIGHT, al_map_rgb(0,0,200));
  al_draw_filled_rectangle(5,HEIGHT - 150 + 5,WIDTH - 5,HEIGHT - 5, al_map_rgb(0,100,0));
  al_draw_textf(font10, al_map_rgb(255,0,0), 30,  HEIGHT - 140  ,0,"Attack" );
  al_draw_textf(font10, al_map_rgb(255,0,0), 30,  HEIGHT - 140 + draw_height  ,0,"Parry" );
  al_draw_textf(font10, al_map_rgb(255,0,0), 30,  HEIGHT - 140 + 2 * draw_height  ,0,"Flee" );
  al_draw_textf(font10, al_map_rgb(255,0,0), 30,  HEIGHT - 140 + 3 * draw_height  ,0,"Meme" );
  al_draw_filled_triangle(15, HEIGHT - 145 + hero_select * draw_height ,25, HEIGHT - 140 + hero_select * draw_height, 15, HEIGHT - 135 + hero_select * draw_height, al_map_rgb(255,0,0));
}

int updateHeroAttack (Hero &hero, int attackframe, int hero_speed)
{
  hero.animationSpeed = 1;
  hero.walkingDirection = WALK_LEFT;
  if (attackframe < 30) {
    hero.x -= hero_speed;
  }
  else if (attackframe < 60) {
    hero.x += hero_speed;
    //std::cout << "a ";
  }
  else {
    hero.animationSpeed = 0;
    return 0;
    std::cout << "fertjsch";
  }
  return 1;
}

int updateOpponentAttack (Opponent &opponent, int attackframe)
{
  if (attackframe < 60) {
    opponent.action = DRAGON_BREATH;
  }
  else {
    opponent.action = DRAGON_STAND;
    return 0;
  }
  return 1;
}

int updateHeroParry (Hero &hero, int attackframe, int hero_speed)
{
  hero.animationSpeed = 1;
  if (attackframe < 20) {
    hero.walkingDirection = WALK_RIGHT;   ;
    hero.x -= hero_speed;
  }
  else if (attackframe < 40) {
    hero.walkingDirection = WALK_LEFT;
    hero.x += hero_speed;
    //std::cout << "a ";
  }
  else {
    hero.walkingDirection = WALK_LEFT;
    hero.animationSpeed = 0;
    return 0;
    std::cout << "fertjsch";
  }
  return 1;
}

int updateHeroFlee (Hero &hero, int attackframe, int hero_speed)
{
  hero.animationSpeed = 1;
  if (attackframe < 20) {
    hero.walkingDirection = WALK_RIGHT;   ;
    hero.x += 2 * hero_speed;
  }
  else if (attackframe < 100) {
    hero.walkingDirection = WALK_LEFT;
    hero.x -= hero_speed / 2;
    //std::cout << "a ";
  }
  else {
    hero.walkingDirection = WALK_LEFT;
    hero.animationSpeed = 0;
    return 0;
    std::cout << "fertjsch";
  }
  return 1;
}

int updateHeroMeme (int attackframe)
{
  if (attackframe < 60) {
    return 1;
  }
  return 0;
}

void drawMeme (int random, ALLEGRO_FONT *font10)
{
  int x_pos = 30;
  int y_pos = 140;
  switch (random) {
  case 0:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"A cat is fine too");
    return;
  case 1:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"A leigh widte wailingu");
    return;
  case 2:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Yo can tell manns thas de man ho slei korren haffan");
    return;
  case 3:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Proprietary software");
    return;
  case 4:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Spotsk");
    return;
  case 5:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Maski");
    return;
  case 6:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Ledsen, Svampbob");
    return;
  case 7:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Born lucky");
    return;
  case 8:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Lucky to be born");
    return;
  case 9:
    al_draw_textf(font10, al_map_rgb(0,0,255), x_pos,  y_pos ,0,"Ny exotisk form av meme");
    return;
  }
}

void drawBackground (ALLEGRO_BITMAP  *background_image)
{
  al_draw_bitmap(background_image, 0, 0, 0);
  return;
}
