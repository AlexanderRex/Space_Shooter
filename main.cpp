#include<iostream>
#include"SFML\Graphics.hpp"
#include"SFML\Window.hpp"
#include"SFML\System.hpp"
#include"SFML\Audio.hpp"
#include<math.h>
#include<cstdlib>
#include<vector>
//
using namespace sf;

//
////VECTOR MATH
////LENGTH OF VECTOR
////|V| = sqrt(pow(2, Vx) + pow(2, Vy));
////NORMALIZE
////U = V/|V| = (Vx, Vy) / sqrt(pow(2, Vx) + pow(2, Vy));
//
class Bullet
{
public:
   Sprite shape;

   Bullet(Texture *texture, Vector2f pos)
   {
      this->shape.setTexture(*texture);

      this->shape.setScale(0.07f, 0.07f);

      this->shape.setPosition(pos);

   }

   //~Bullet();
};

class Player
{
public:

   Sprite shape;
   Texture *texture;
   
   int HP;
   int HPMax;

   std::vector<Bullet> bullets;

   Player(Texture *texture)
   {
      this->HPMax = 10;
      this->HP = this->HPMax;

      this->texture = texture;
      this->shape.setTexture(*texture);

      this->shape.setScale(0.45f, 0.45f);
   }

   //~Player();
};

class Enemy
{     
public:

   Sprite shape;
   Texture* texture;

   int HP;
   int HPMax;

   Enemy(Texture *texture, Vector2u windowSize)
   {
      this->HPMax = rand() % 3 + 1;
      this->HP = this->HPMax;

      
      this->shape.setTexture(*texture);

      this->shape.setScale(0.15f, 0.15f);
      this->shape.setPosition(windowSize.x - this->shape.getGlobalBounds().width +100, rand() %int (windowSize.y - this->shape.getGlobalBounds().height));
   };

   //~Enemy() {};

};

int main()
{
   srand(time(NULL));

   RenderWindow window(VideoMode(800, 600), "Asteroid adventure", Style::Default);
   window.setFramerateLimit(60);

   //init text

   Font font;
   font.loadFromFile("res/Starjedi.ttf");

   //init textures
   Texture playerTex;
   playerTex.loadFromFile("res/falcon.png");

   Texture enemyTex;
   enemyTex.loadFromFile("res/asteroid.png");

   Texture bulletTex;
   
   bulletTex.loadFromFile("res/laser.png");

   Texture backTex;
   backTex.loadFromFile("res/background.jpg");

   //background init
   Sprite background[2];
   background[0].setTexture(backTex);
   background[1].setTexture(backTex);
   background[1].setPosition(backTex.getSize().x, 0);
   Music back_sound;
   back_sound.openFromFile("res/asteroid_field.ogg");
   back_sound.setLoop(true);
   back_sound.play();

   //UI Text
   Text score;
   score.setFont(font);
   score.setCharacterSize(20);
   score.setFillColor(Color::Yellow);
   score.setPosition(10.f, 10.f);

   //gamemode
   bool game_over = false;
   Text gameOverText;
   gameOverText.setFont(font);
   gameOverText.setCharacterSize(30);
   gameOverText.setFillColor(Color::Red);
   gameOverText.setPosition(100.f, window.getSize().y/2);
   gameOverText.setString("Game over, press R to restart");

   //player init
   Player player(&playerTex);
   int shootTimer = 20;
   int scorecount = 0;
   //player sound
   SoundBuffer shoutBuffer;
   shoutBuffer.loadFromFile("res/shout.ogg");
   Sound shoutSound;
   shoutSound.setBuffer(shoutBuffer);
   //player text
   Text hpText;
   hpText.setFont(font);
   hpText.setCharacterSize(12);
   hpText.setFillColor(Color::White);


   //enemy init
   int enemySpawnTimer = 0;
   std::vector<Enemy> enemies;
   //enemy sound
   SoundBuffer explosionBuffer;
   explosionBuffer.loadFromFile("res/explosion.ogg");
   Sound explosionSound;
   explosionSound.setBuffer(explosionBuffer);
   //enemy text
   Text ehpText;
   ehpText.setFont(font);
   ehpText.setCharacterSize(12);
   ehpText.setFillColor(Color::White);



   while (window.isOpen())
   {
      Event event;
      while (window.pollEvent(event))
      {
         if (event.type == Event::Closed)
            window.close();
      }

      if (game_over && Keyboard::isKeyPressed(Keyboard::R))
      {
         // Reset all game states and objects
         player.HP = player.HPMax;
         player.bullets.clear();
         enemies.clear();
         scorecount = 0;
         shootTimer = 20;
         enemySpawnTimer = 0;
         game_over = false;
         continue; // Continue to the next frame
      }

      if (player.HP > 0)
      {

         //Background fakeanim
         for (int i = 0; i < 2; i++)
         {
            background[i].move(-2.f, 0.f); // Adjust speed as needed

            if (background[i].getPosition().x <= -background[i].getGlobalBounds().width)
            {
               background[i].setPosition(background[i].getGlobalBounds().width, 0.f);
            }
         }


         //Player
         if (Keyboard::isKeyPressed(Keyboard::W))
            player.shape.move(0.f, -10.f);
         if (Keyboard::isKeyPressed(Keyboard::S))
            player.shape.move(0.f, 10.f);
         if (Keyboard::isKeyPressed(Keyboard::A))
            player.shape.move(-10.f, 0.f);
         if (Keyboard::isKeyPressed(Keyboard::D))
            player.shape.move(10.f, 0.f);

         hpText.setPosition(player.shape.getPosition().x, player.shape.getPosition().y - hpText.getGlobalBounds().height);
         hpText.setString(std::to_string(player.HP) + "/" + std::to_string(player.HPMax));

         //Collision with window

         if (player.shape.getPosition().x <= 0)//Left
            player.shape.setPosition(0.f, player.shape.getPosition().y);

         if (player.shape.getPosition().x >= window.getSize().x - player.shape.getGlobalBounds().width)//Right
            player.shape.setPosition(window.getSize().x - player.shape.getGlobalBounds().width, player.shape.getPosition().y);

         if (player.shape.getPosition().y <= 0)//Top
            player.shape.setPosition(player.shape.getPosition().x, 0.f);

         if (player.shape.getPosition().y >= window.getSize().y - player.shape.getGlobalBounds().height)//Down
            player.shape.setPosition(player.shape.getPosition().x, window.getSize().y - player.shape.getGlobalBounds().height);

         //Update Controls
         if (shootTimer < 13)
         {
            shootTimer++;
         }

         if (Keyboard::isKeyPressed(Keyboard::Space) && shootTimer >= 13) //Shooting
         {
            shoutSound.play();
            player.bullets.push_back(Bullet(&bulletTex, Vector2f(
               player.shape.getPosition().x + player.shape.getGlobalBounds().width / 2,
               player.shape.getPosition().y + player.shape.getGlobalBounds().height / 2
            )));
            shootTimer = 0; //reset timer
         }

         //Bullets

         for (size_t i = 0; i < player.bullets.size(); i++)
         {
            //Move
            player.bullets[i].shape.move(30.f, 0.f);

            //Out of window bounds
            if (player.bullets[i].shape.getPosition().x > window.getSize().x)
            {
               player.bullets.erase(player.bullets.begin() + i);
               break;
            }
            //Enemy collision
            for (size_t k = 0; k < enemies.size(); k++)
            {
               if (player.bullets[i].shape.getGlobalBounds().intersects(enemies[k].shape.getGlobalBounds()))
               {
                  explosionSound.play();
                  if (enemies[k].HP <= 1)
                  {
                     scorecount += enemies[k].HPMax;
                     enemies.erase(enemies.begin() + k);
                  }
                  else
                     enemies[k].HP--;//Enemy take damage
                  player.bullets.erase(player.bullets.begin() + i);
                  break;
               }
            }
         }

         //Enemy

         if (enemySpawnTimer < 20)
            enemySpawnTimer++;

         if (enemySpawnTimer >= 20)
         {
            enemies.push_back(Enemy(&enemyTex, window.getSize()));
            enemySpawnTimer = 0;//reset timer
         }

         for (size_t i = 0; i < enemies.size(); i++)
         {
            enemies[i].shape.move(-6.f, 0.f);

            if (enemies[i].shape.getPosition().x <= 0 - enemies[i].shape.getGlobalBounds().width)
            {
               enemies.erase(enemies.begin() + i);
               break;
            }

            if (enemies[i].shape.getGlobalBounds().intersects(player.shape.getGlobalBounds()))
            {
               explosionSound.play();
               enemies.erase(enemies.begin() + i);
               player.HP--; //player take damage
               break;
            }
         }

         //UI Update

         score.setString("Score :" + std::to_string(scorecount));

         //Draw
      }
      else
      {
         game_over = true;
      }
      window.clear();
      //background
      for (auto& bg : background)
         window.draw(bg);
      //player 
      window.draw(player.shape);

      for (size_t i = 0; i < player.bullets.size(); i++)
      {
         window.draw(player.bullets[i].shape);
      }

      //enemy
      for (size_t i = 0; i < enemies.size(); i++)
      {

         ehpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPMax));
         ehpText.setPosition(enemies[i].shape.getPosition().x, enemies[i].shape.getPosition().y - ehpText.getGlobalBounds().height);
         ehpText.setString(std::to_string(enemies[i].HP) + "/" + std::to_string(enemies[i].HPMax));

         window.draw(ehpText);
         window.draw(enemies[i].shape);

      }

      //UI
      window.draw(score);
      window.draw(hpText);

      if (game_over)
   {
      window.draw(gameOverText);
   }
      
      window.display();

   }

   return 0;
}
