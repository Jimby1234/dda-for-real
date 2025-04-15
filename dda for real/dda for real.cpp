#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <math.h>
#include <iostream>
#include <algorithm>
#include <random>


//textures
#include "textures/materials/Brick.ppm"
#include "textures/materials/metal.ppm"
#include "textures/materials/ground.ppm"
#include "textures/sprites/enemy.ppm"
#include "textures/sprites/lights.ppm"


// used for better number generation
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dist(1, 15);


// vars
const double PI = 3.141592653;
const float FOV = PI / 4.0f;
const float depth = 16.0f;
const olc::vi2d mapDimensions = { 16, 16 };


int map[256] = {
    1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,2,0,0,0,1,
    1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,2,2,2,1,1,1,1,1,1,1
};
const int floorMap[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
const int roofMap[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

float distances[700];
int spriteIDs[700][2]; // this is an array of spriteIDs from the left of the screen to the right of the screen


int amountOfEnemys = 10;
int amountOfLightsources = 1;
int amountOfSprites = amountOfEnemys + amountOfLightsources;
int spriteOrder[100];
double spriteDistance[100];

class person {
public:
    olc::vf2d location;
    float angle = PI/2;
    int state = 0;
    bool isShooting = false;
    float health = 20;
};

class sprite {
public:
    olc::vf2d location = { 0,0 };    // 2d location
    float zHeight = 0.0f;            // how high is it
    int id = 0;                      // is it an enemy or light bulb
    int state = 0;                   // dead different look etc
    int speed = 0;                   // how fast does the sprite move
    olc::Pixel brightness;           // if its a lightsource it will have some brightness in RGB


    sprite(int id, olc::Pixel brightness, olc::vf2d location, int state, float zHeight , int speed)
        : id(id), brightness(brightness), location(location), state(state), zHeight(zHeight), speed(speed) {}
    
};

class Game : public olc::PixelGameEngine {
public:
    Game() {
        sAppName = "raycast";
    }

    olc::Sprite* sprGun1 = nullptr;
    olc::Decal* decGun1 = nullptr;

    olc::Sprite* sprGun2 = nullptr;
    olc::Decal* decGun2 = nullptr;

    person player;

    //function called by other functions
    olc::Pixel Darken(olc::Pixel pixel, float factor) {
        return olc::Pixel(
            std::max(0, int(pixel.r * factor)),
            std::max(0, int(pixel.g * factor)),
            std::max(0, int(pixel.b * factor))
        );
    }

    float wrapAngle(float angle) {
        while (angle < -3.14159f) angle += 2.0f * 3.14159f;
        while (angle > 3.14159f) angle -= 2.0f * 3.14159f;
        return angle;
    }
    int wave;
    void restartGame() {
        
        
        setUp();

        
        Sprites.clear();
        // Create gargoyles
        for (int i = 0; i < amountOfEnemys + wave; i++) {
            Sprites.push_back(sprite(0, olc::Pixel(0, 0, 0), { 0,0 }, 0, 0.0f, 0));
        }
        // Create a lamp (lightsource)
        Sprites.push_back(sprite(1, olc::Pixel(0, 0, 0), { 7.5f, 7.5f }, 0, 32.0f, 0));

        // Set random positions for gargoyles
        for (int i = 0; i < amountOfEnemys + wave; i++) {
            Sprites[i].location = { float(dist(gen)), float(dist(gen)) };
        }
    }

    //functions called at start
    void setUp() {
        //generate random maps
        for (int x = 1; x < 15; x++) {
            for (int y = 1; y < 15; y++) {
                map[y * 16 + x] = (int)(dist(gen) / 15.0f);
        
            }
        
        }

        //generate random spawn for player
        bool spawned = false;
        while (spawned == false) {
            int plyrX = dist(gen);
            int plyrY = dist(gen);

            if (map[plyrY * 16 + plyrX] == 0) {
                // adding 0.5 so we dont get spawned inside a wall boundery
                player.location = { plyrX + 0.5f , plyrY + 0.5f };
                spawned = true;
            }
        }
    }

    //functions called in game loop
    void sortSprites(int* order, double* dist, int amount)
    {
        std::vector<std::pair<double, int>> sprites(amount);
        for (int i = 0; i < amount; i++) {
            sprites[i].first = dist[i];
            sprites[i].second = order[i];
        }
        std::sort(sprites.begin(), sprites.end());
        // restore in reverse order to go from farthest to nearest
        for (int i = 0; i < amount; i++) {
            dist[i] = sprites[amount - i - 1].first;
            order[i] = sprites[amount - i - 1].second;
        }
    }
    
    void buttons(float fElapsedTime) {
        if (GetKey(olc::A).bHeld)
            player.angle -= 1.5f * fElapsedTime; // Turn left
        if (GetKey(olc::D).bHeld)
            player.angle += 1.5f * fElapsedTime; // Turn right
        if (player.angle > 2*PI) {
            player.angle = 0;
        }
        else if (player.angle < 0) {
            player.angle = 2 * PI;
        }
        float moveSpeed = 5.0f; // Units per second

        if (GetKey(olc::S).bHeld) {
            player.location.x -= sinf(player.angle) * moveSpeed * fElapsedTime;
            player.location.y -= cosf(player.angle) * moveSpeed * fElapsedTime;
            if (map[(int)player.location.y * mapDimensions.x + (int)player.location.x] != 0) {
                // undo movement
                player.location.x += sinf(player.angle) * moveSpeed * fElapsedTime;
                player.location.y += cosf(player.angle) * moveSpeed * fElapsedTime;
            }
        }
        if (GetKey(olc::W).bHeld) {
            player.location.x += sinf(player.angle) * moveSpeed * fElapsedTime;
            player.location.y += cosf(player.angle) * moveSpeed * fElapsedTime;
            if (map[(int)player.location.y * mapDimensions.x + (int)player.location.x] != 0) {
                // undo movement
                player.location.x -= sinf(player.angle) * moveSpeed * fElapsedTime;
                player.location.y -= cosf(player.angle) * moveSpeed * fElapsedTime;
            }
        }

        if (GetKey(olc::Key::SPACE).bPressed) {
            player.isShooting = true;
        }
        else {
            player.isShooting = false;
        }

    }

    void renderWall() {
        for (int x = 0; x < ScreenWidth(); x++) {
            // ray angle
            float rayAngle = (player.angle - FOV / 2.0f) + (((float)x / (float)ScreenWidth()) * FOV);

            
            float distanceToWall = 0.0f;
            bool hitWall = false;
            int side = 0;

            // ray direction (sin and cos is swapped due to angle 0 being down)
            float rayDirX = sinf(rayAngle);
            float rayDirY = cosf(rayAngle);

            // delta distances
            float deltaDistX = (rayDirX == 0) ? 1e30f : fabs(1.0f / rayDirX);
            float deltaDistY = (rayDirY == 0) ? 1e30f : fabs(1.0f / rayDirY);

            float sideDistX, sideDistY;
            int stepX, stepY;

            int mapX = (int)player.location.x;
            int mapY = (int)player.location.y;
            

            // step directions
            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (player.location.x - mapX) * deltaDistX;
            }
            else {
                stepX = 1;
                sideDistX = (mapX + 1.0f - player.location.x) * deltaDistX;
            }

            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (player.location.y - mapY) * deltaDistY;
            }
            else {
                stepY = 1;
                sideDistY = (mapY + 1.0f - player.location.y) * deltaDistY;
            }

            // DDA algorithm
            while (!hitWall) {
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                
                if (mapX < 0 || mapX >= mapDimensions.x || mapY < 0 || mapY >= mapDimensions.y) {
                    hitWall = true;
                    distanceToWall = depth;
                    break;
                }

                int mapIndex = mapY * mapDimensions.x + mapX;
                if (map[mapIndex] > 0) {
                    hitWall = true;
                    
                    distanceToWall = map[mapIndex];
                }
            }

            // distance to the wall based on which side was hit.
            if (hitWall) {
                if (side == 0)
                    distanceToWall = (mapX - player.location.x + (1 - stepX) / 2.0f) / rayDirX;
                else
                    distanceToWall = (mapY - player.location.y + (1 - stepY) / 2.0f) / rayDirY;
            }
            else {
                distanceToWall = depth;
            }
            // we add the distance here because we then get the original length not the bullshit thing after
            distances[x] = distanceToWall;

            // correct for the fish-eye effect
            distanceToWall *= cosf(rayAngle - player.angle);

            // calculate ceiling and floor positions for this column
            int ceiling = (float)(ScreenHeight() / 2.0f) - ScreenHeight() / ((float)distanceToWall);
            int floor_ = ScreenHeight() - ceiling;
            int wallHeight = floor_ - ceiling;

            // compute perpendicular wall distance for texture mapping
            float perpWallDist;
            if (side == 0)
                perpWallDist = (mapX - player.location.x + (1 - stepX) / 2.0f) / rayDirX;
            else
                perpWallDist = (mapY - player.location.y + (1 - stepY) / 2.0f) / rayDirY;

            // hit position on the wall for texture mapping
            float wallX;
            if (side == 0)
                wallX = player.location.y + perpWallDist * rayDirY;
            else
                wallX = player.location.x + perpWallDist * rayDirX;
            wallX -= floor(wallX);

            // calculate texture X coordinate
            int texX = (int)(wallX * 32.0f);
            texX = std::clamp(texX, 0, 31);

            // flip texture coordinate if necessary depending on the ray direction.
            if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0))
                texX = 31 - texX;

            
            for (int y = 0; y < ScreenHeight(); y++) {
                float dist = 0.0f;
                if (y <= floor_ && y >= ceiling) {
                    
                    float texYf = (y - ceiling) / (float)wallHeight * 32.0f;
                    int texY = std::clamp((int)(texYf), 0, 31);

                    olc::Pixel shade(255, 255, 255);
                    int mapIndex = mapY * mapDimensions.x + mapX;

                    //distance to lights
                    for (int i = 0; i < amountOfSprites; i++) {
                        if (Sprites[i].id == 1) {
                            dist += sqrtf( (mapX - Sprites[i].location.x) * (mapX - Sprites[i].location.x) + (mapY - Sprites[i].location.y) * (mapY - Sprites[i].location.y));
                        }

                    };

                    float light = 1.0f / (1.0f + dist * 0.4f); 
                    light = std::clamp(light, 0.0f, 2.0f);     

                    if (map[mapIndex] == 1) {  // Brick texture
                        shade.r = Brick[texY * 32 * 3 + texX * 3    ] * light;
                        shade.g = Brick[texY * 32 * 3 + texX * 3 + 1] * light;
                        shade.b = Brick[texY * 32 * 3 + texX * 3 + 2] * light;
                       
                        shade.r = std::clamp((int)shade.r, 0, 255);
                        shade.g = std::clamp((int)shade.g, 0, 255);
                        shade.b = std::clamp((int)shade.b, 0, 255);

                    }
                    else if (map[mapIndex] == 2) {  // Metal texture
                        shade.r = (float)metal[texY * 32 * 3 + texX * 3    ]*light;
                        shade.g = (float)metal[texY * 32 * 3 + texX * 3 + 1]*light;
                        shade.b = (float)metal[texY * 32 * 3 + texX * 3 + 2]*light;

                        shade.r = std::clamp((int)shade.r, 0, 255);
                        shade.g = std::clamp((int)shade.g, 0, 255);
                        shade.b = std::clamp((int)shade.b, 0, 255);
                    }

                    
                    if (side == 1)
                        shade = Darken(shade, 0.7f);

                    Draw(x, y, shade);
                }
            }
        }
    }

    void renderMap() {
        int size = 5;

        // draw the map
        for (int y = 0; y < mapDimensions.y; y++) {
            for (int x = 0; x < mapDimensions.x; x++) {
                int mapIndex = y * mapDimensions.x + x;
                if (map[mapIndex] != 0) {
                    FillRectDecal({ (float)(x * size), (float)(y * size) }, { (float)size, (float)size }, olc::WHITE);
                }
            }
        }

        // draw the player position
        olc::vf2d playerMapPos = player.location * (float)size;
        FillCircle(playerMapPos, 2, olc::WHITE);

        olc::vf2d dir = { sinf(player.angle) * 5, cosf(player.angle) * 5 };
        DrawLine(playerMapPos, dir + playerMapPos);

        //enemies
        for (int i = 0; i < amountOfEnemys + wave; i++) {
            if (Sprites[i].id == 0 && Sprites[i].state == 0) {
                olc::vf2d enemyMapPos = Sprites[i].location * (float)size;
                FillCircle(enemyMapPos, 2, olc::RED);
            }
            
        }
    }
    
    void renderFloor() {
        
        float floorTextureScale = 1; 

        
        float dirX = sinf(player.angle);
        float dirY = cosf(player.angle);

        
        float planeX = sinf(player.angle + PI / 2.0f) * 0.4f;
        float planeY = cosf(player.angle + PI / 2.0f) * 0.4f;

        
        for (int y = ScreenHeight() / 2 + 1; y < ScreenHeight(); y++) {
            
            float rayDirX0 = dirX - planeX;
            float rayDirY0 = dirY - planeY;
            float rayDirX1 = dirX + planeX;
            float rayDirY1 = dirY + planeY;

            
            float posZ = 0.5f * ScreenHeight();

            
            float p = y - ScreenHeight() / 2.0f;

            
            float rowDistance = posZ / p * 2.0f; 

            
            float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / ScreenWidth();
            float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / ScreenWidth();

            
            float floorX = player.location.x + rowDistance * rayDirX0;
            float floorY = player.location.y + rowDistance * rayDirY0;

            for (int x = 0; x < ScreenWidth(); x++) {
                
                int cellX = static_cast<int>(floorX);
                int cellY = static_cast<int>(floorY);

                
                int tx = (int)(32.0f * floorTextureScale * (floorX - std::floor(floorX))) & 31;
                int ty = (int)(32.0f * floorTextureScale * (floorY - std::floor(floorY))) & 31;

                
                floorX += floorStepX;
                floorY += floorStepY;

                
                int mapIndex = cellY * mapDimensions.x + cellX;
                int floorType = floorMap[mapIndex];

                
                olc::Pixel floorPixel;
                if (floorType == 1) {
                    
                    int textureOffset = (ty * 32 + tx) * 3;
                    floorPixel.r = ground[textureOffset];
                    floorPixel.g = ground[textureOffset + 1];
                    floorPixel.b = ground[textureOffset + 2];
                }
                else if (floorType == 2) {
                    
                    int textureOffset = (ty * 32 + tx) * 3;
                    floorPixel.r = metal[textureOffset];
                    floorPixel.g = metal[textureOffset + 1];
                    floorPixel.b = metal[textureOffset + 2];
                }
                else {
                    
                    floorPixel = olc::DARK_GREY;
                }

                
                float distanceFactor = std::min(1.0f, 1.0f / (rowDistance * 0.3f));
                floorPixel = Darken(floorPixel, distanceFactor);

                
                Draw(x, y, floorPixel);
            }
        }
    }

    void renderRoof() {
        float roofTextureScale = 1; 

        
        float dirX = sinf(player.angle);
        float dirY = cosf(player.angle);

        float planeX = sinf(player.angle + PI / 2.0f) * 0.4f;
        float planeY = cosf(player.angle + PI / 2.0f) * 0.4f;

        
        for (int y = 0; y < ScreenHeight() / 2; y++) {
            float rayDirX0 = dirX - planeX;
            float rayDirY0 = dirY - planeY;
            float rayDirX1 = dirX + planeX;
            float rayDirY1 = dirY + planeY;

            float posZ = 0.5f * ScreenHeight();

            float p = (ScreenHeight() / 2.0f) - y;

            float rowDistance = posZ / p * 2.0f;

            float roofStepX = rowDistance * (rayDirX1 - rayDirX0) / ScreenWidth();
            float roofStepY = rowDistance * (rayDirY1 - rayDirY0) / ScreenWidth();

            float roofX = player.location.x + rowDistance * rayDirX0;
            float roofY = player.location.y + rowDistance * rayDirY0;

            for (int x = 0; x < ScreenWidth(); x++) {
                int cellX = static_cast<int>(roofX);
                int cellY = static_cast<int>(roofY);

                int tx = static_cast<int>(32.0f * roofTextureScale * (roofX - std::floor(roofX))) & 31;
                int ty = static_cast<int>(32.0f * roofTextureScale * (roofY - std::floor(roofY))) & 31;

                roofX += roofStepX;
                roofY += roofStepY;

                int mapIndex = cellY * mapDimensions.x + cellX;
                int roofType = roofMap[mapIndex]; 

                olc::Pixel roofPixel;
                if (roofType == 1) {
                    int textureOffset = (ty * 32 + tx) * 3;
                    roofPixel.r = ground[textureOffset];
                    roofPixel.g = ground[textureOffset + 1];
                    roofPixel.b = ground[textureOffset + 2];
                }
                else if (roofType == 2) {
                    int textureOffset = (ty * 32 + tx) * 3;
                    roofPixel.r = metal[textureOffset];
                    roofPixel.g = metal[textureOffset + 1];
                    roofPixel.b = metal[textureOffset + 2];
                }
                else {
                    roofPixel = olc::BLACK;
                }

                float distanceFactor = std::min(1.0f, 1.0f / (rowDistance * 0.3f));
                roofPixel = Darken(roofPixel, distanceFactor);

                Draw(x, y, roofPixel);
            }
        }
    }

    //essentially a list of all the sprites
    std::vector<sprite> Sprites;

    void renderSprites() {
        
        for (int i = 0; i < amountOfSprites+wave; i++)
        {
            spriteOrder[i] = i;
            spriteDistance[i] = ((player.location.x - Sprites[i].location.x) * (player.location.x - Sprites[i].location.x) + (player.location.y - Sprites[i].location.y) * (player.location.y - Sprites[i].location.y)); //sqrt not taken, unneeded
        }

        sortSprites(spriteOrder, spriteDistance, amountOfSprites+wave);

        for (int i = 0; i < amountOfSprites + wave; i++) {
            
            double spriteX = Sprites[spriteOrder[i]].location.x - player.location.x;
            double spriteY = Sprites[spriteOrder[i]].location.y - player.location.y;

            // sin and cos are swapped because when player.angle = 0 the player is facing down
            float dirX = sin(player.angle);
            float dirY = cos(player.angle);

            //very weird its 0.4 and not my FOV however it mostly works and im not trying to fix it lmao
            float planeX = -dirY * 0.4f;
            float planeY = dirX * 0.4f;

            double invDet = 1.0f / (planeX * dirY - dirX * planeY); // used for matrix multiplication

            double transformX = invDet * (-dirY * spriteX + dirX * spriteY);
            double transformY = invDet * (planeY * spriteX - planeX * spriteY);

            int spriteScreenX = int((ScreenWidth() / 2) * (1 - transformX / transformY));

            
            int spriteHeight = abs(int(ScreenHeight() / transformY));

            
            int verticalOffset = int(Sprites[spriteOrder[i]].zHeight * 10 / -transformY);

            int drawStartY = -spriteHeight / 2 + ScreenHeight() / 2 - verticalOffset;
            if (drawStartY < 0) drawStartY = 0;

            int drawEndY = spriteHeight / 2 + ScreenHeight() / 2 - verticalOffset;
            if (drawEndY >= ScreenHeight()) drawEndY = ScreenHeight() - 1;

            int spriteWidth = abs(int(ScreenHeight() / transformY));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0) drawStartX = 0;

            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= ScreenWidth()) drawEndX = ScreenWidth() - 1;

            for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
                
                for (int y = drawStartY; y < drawEndY; y++) {
                    if (-transformY > 0 && stripe > 0 && stripe < ScreenWidth() && abs(transformY) < distances[stripe]) {
                        
                        if (Sprites[spriteOrder[i]].state == 0 && Sprites[spriteOrder[i]].id == 0) {
                            olc::Pixel shade;

                            spriteIDs[stripe][0] = spriteOrder[i];
                            spriteIDs[stripe][1] = Sprites[spriteOrder[i]].id;

                            int d = (y + verticalOffset) * 256 - ScreenHeight() * 128 + spriteHeight * 128;
                            int texY = ((d * 32) / spriteHeight) / 256;
                            int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * 32 / spriteWidth) / 256;

                            int red = std::clamp(texY * 32 * 3 + texX * 3, 0, 3072);
                            int green = std::clamp(texY * 32 * 3 + texX * 3 + 1, 0, 3072);
                            int blue = std::clamp(texY * 32 * 3 + texX * 3 + 2, 0, 3072);

                            shade.r = enemy[red];
                            shade.g = enemy[green];
                            shade.b = enemy[blue];
                            shade.a = 254;

                            if (shade.r != 163 && shade.g != 73 && shade.b != 164) {
                                Draw({ stripe, y }, shade);
                            }
                        }

                        else if (Sprites[spriteOrder[i]].id == 1) {
                            olc::Pixel shade;

                            int d = (y + verticalOffset) * 256 - ScreenHeight() * 128 + spriteHeight * 128;
                            int texY = ((d * 32) / spriteHeight) / 256;
                            int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * 32 / spriteWidth) / 256;

                            int red = std::clamp(texY * 32 * 3 + texX * 3, 0, 3072);
                            int green = std::clamp(texY * 32 * 3 + texX * 3 + 1, 0, 3072);
                            int blue = std::clamp(texY * 32 * 3 + texX * 3 + 2, 0, 3072);

                            shade.r = lights[red];
                            shade.g = lights[green];
                            shade.b = lights[blue];
                            shade.a = 255 - i;

                            if (shade.r != 163 && shade.g != 73 && shade.b != 164) {
                                Draw({ stripe, y }, shade);
                            }
                        }

                    }
                }
            }
            
        }
        
    }

    void updateSprite(float fElapsedTime) {
        for (int i = 0; i < amountOfSprites + wave; i++) {
            if (Sprites[i].id == 0) {
                olc::vf2d directionVector = { Sprites[i].location.x - player.location.x , Sprites[i].location.y - player.location.y };
                float angle = atan2(directionVector.x, directionVector.y);
                Sprites[i].location = { Sprites[i].location.x + -sin(angle) * fElapsedTime,Sprites[i].location.y + -cos(angle) * fElapsedTime };
            }
            
        }
    }

    int counter = 0;
    float coolDown = 0;
    float shotTimer = 0;

    void shooting(float fElapsedTime) {
        
        DrawCircle({ 350, 175 }, 2, olc::DARK_GREEN); // crosshair

        
        if (counter == amountOfEnemys + wave) {
            counter = 0;
            wave += 10;
            restartGame();
        }

        
        if (coolDown > 0) { 
            coolDown -= fElapsedTime; 
        }
        if (shotTimer > 0) { 
            shotTimer -= fElapsedTime; 
        }

       
        if (player.isShooting && coolDown <= 0.0f) {
            coolDown = 0.3f; // how long for the reload
            shotTimer = 0.3f;// how long for the animation

            
            int thing = spriteIDs[350][0];
            int id    = spriteIDs[350][1];
            

            if (id == 0) {
                Sprites[thing].state = 1;
                counter++;
            }
        }

        // render gun decal based on shotTimer
        if (shotTimer > 0) {
            DrawDecal({ 310, 200 }, decGun2, { 0.1f, 0.1f });
        }
        else {
            DrawDecal({ 340, 250 }, decGun1, { 0.1f, 0.1f });
        }
    }



    void collision(float fElapsedTime) {
        DrawStringDecal({320,340}, std::to_string(player.health), olc::RED);
        DrawStringDecal({ 350,0 }, "wave: " + std::to_string(wave / 10));
        for (int i = 0; i < amountOfSprites; i++) {
            if (Sprites[i].state == 0 && Sprites[i].id == 0) {
                float distance = sqrtf((Sprites[i].location.x - player.location.x) * (Sprites[i].location.x - player.location.x) + (Sprites[i].location.y - player.location.y) * (Sprites[i].location.y - player.location.y));
                if (distance < 0.5) {
                    player.health -= 2 * fElapsedTime; // 2 damage per second
                }
            }
            
        }
        if (player.health <= 0) {
            DrawStringDecal({ 0,5 }, "YOU HAVE FAILED! ", olc::RED, {5,5});
            exit(80085);
        }
    }

    bool OnUserCreate() override {
        setUp();

        

        //create 10 gargoyles
        for (int i = 0; i < 10+wave; i++) {
            
            Sprites.push_back(sprite(0, (0, 0, 0), { 0,0 }, 0, 0.0f, 0));
        }
        //create a lamp
        Sprites.push_back(sprite(1, (0, 0, 0), { 7.5f,7.5f }, 0, 32.0f, 0));

        //loading image for use
        sprGun1 = new olc::Sprite("textures/materials/gun.png");
        decGun1 = new olc::Decal(sprGun1);
        sprGun2 = new olc::Sprite("textures/materials/gun (1).png");
        decGun2 = new olc::Decal(sprGun2);

        for (int i = 0; i < amountOfEnemys; i++) {
            //if its a gargoyle
            if (Sprites[i].id == 0) {
                Sprites[i].location = { (float)dist(gen),(float)dist(gen)};
            }
            
        }
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::BLACK);
        buttons(fElapsedTime);
        renderFloor();
        renderRoof();
        renderWall();
        renderMap();
        renderSprites();
        updateSprite(fElapsedTime);
        shooting(fElapsedTime);
        collision(fElapsedTime);
        
        return true;
    }
};

int main() {
    Game demo;
    if (demo.Construct(700, 350, 2, 2))
        demo.Start();
    return 0;
}