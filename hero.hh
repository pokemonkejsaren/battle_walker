struct Hero
{
  int x;
  int y;
  int max_hp;
  int hp;
  int max_mana;
  int mana;
  int max_xp;
  int xp;
  int level;
  //int status;

  int maxFrame;
  int curFrame;
  int frameCount;
  int frameDelay;
  int animationSpeed;
  int frameWidth;
  int frameHeight;
  int animationColumns;
  int walkingDirection;

  int row;
  int column;

  ALLEGRO_BITMAP *image;
};
