#include <raylib.h>
#include <deque>
#include <vector>


int player1_score = 0;
int player2_score = 0;

bool main_menu_active = true;
bool instructions_menu = false;

bool vsCPU = false;
bool mode_selected = false;

bool paused = false;
bool restart_requested = false;

bool gameOver = false;

class Ball
{
    public: 
    
    float x, y;
    float speed_x;
    float speed_y;
    int radius;
    Color color = BLUE;
    std::deque<Vector2> trail;
    std::size_t trail_length = 15; // Length of the trail

    void draw()
    {
        
        // Draw the trail
        for(std::size_t i = 0; i < trail.size(); i++)
        {
            float alpha = (float)(i) / trail.size(); //Add trail effects
            Color trailColor = Color{color.r, color.g, color.b, (unsigned char)(alpha * 180)};
            DrawCircleV(trail[i], radius * 0.7, trailColor); // Draw smaller circles for the trail
            
        }
        
        DrawCircle(x, y, radius, color); //Drawing the main ball
    }

    void update()
    {
        x += speed_x;
        y += speed_y;

        // Bounce off the walls
        if (y + radius >= GetScreenHeight() || y - radius <= 0)
        {
            speed_y *= -1;
        }

        if (x + radius >= GetScreenWidth())
        {
            player1_score++;
            // To Check if player has scored max points
            if (player1_score >= 5)
            {
                gameOver = true;
            }
            resetball();
        }
        
        if (x - radius <= 0)
        {
            player2_score++;
            // To Check if player has scored max points
            if(player2_score >= 5)
            {
                gameOver = true;
            }
            resetball();
        }

        //Add the current position to the trail
        trail.push_back({x,y});
        if(trail.size() > trail_length)
        {
            trail.pop_front();
        }

    }
    
    void resetball()
    {
        x = GetScreenWidth()/2;
        y = GetScreenHeight()/2;

        int speed_choices[2] = {-1, 1};

        speed_x *= speed_choices[GetRandomValue(0,1)];
        speed_y *= speed_choices[GetRandomValue(0,1)];

        color = BLUE;

        trail.clear(); // Clear the trail when the ball is reset
    }

};

class Player
{
    protected:
    void limitMovement()
    {
        if (y < 0) y = 0;
        if (y + height > GetScreenHeight()) y = GetScreenHeight() - height;
    }

    public:

    float x, y;
    float width, height;
    int speed;

    void draw()
    {
        DrawRectangle(x, y, width, height, BLUE);
    }


    void updateManual(int upKey, int downKey)
    {
        if(IsKeyDown(upKey))
        {
            y -= speed;
        }

        if(IsKeyDown(downKey))
        {
            y += speed;
        }

        limitMovement();
    }

    void updateAI(int ball_y) 
    {
        int trackingOffset = 15;
        

        if (y + height/2 > ball_y + trackingOffset)
        {
            y -= speed;
        }
        
        if (y + height/2 < ball_y - trackingOffset)
        {
            y += speed;
        }
        
        limitMovement();
    }
    
};


Ball ball;
Player player1;
Player player2;

// Random Color of Ball when it hits the paddle
Color GetRandomColor()
{
    return Color
    {
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        255
    };
}

int main()
{
    const int screen_width = 1280;
    const int screen_height = 720;

    InitWindow(screen_width, screen_height, "Retro Pong Rebooted - by Shanmukha");
    SetTargetFPS(60);

    //Ball initialization 
    ball.x = screen_width/2;
    ball.y = screen_height/2;;
    ball.speed_x = 7;
    ball.speed_y = 7;
    ball.radius = 20;

    //Player initialization
    player1.width = 25;
    player1.height = 120;
    player1.x = 10;
    player1.y = screen_height/2 - player1.height/2;
    player1.speed = 6;

    //Player2 initialization
    player2.width = 25;
    player2.height = 120;
    player2.x = screen_width - player2.width - 10;
    player2.y = screen_height/2 - player2.height/2;
    player2.speed = 6;

    //Main game loop
    while(WindowShouldClose() == false)
    {
        BeginDrawing();
        ClearBackground(BLACK);

        //Mode selection
        if(main_menu_active)
        {
            int mainTitleFontSize = 50;
            const char * mainTitle = "RETRO PONG REBOOTED";
            int mainTitleWidth = MeasureText (mainTitle, mainTitleFontSize);
            DrawText(mainTitle, screen_width/2 - mainTitleWidth/2, screen_height/2 - 140, mainTitleFontSize, BLUE);

            int myNameFontSize = 30;
            const char * myName = " CREATED BY SHANMUKHA";
            int myNameWidth = MeasureText(myName, myNameFontSize);
            DrawText (myName, screen_width/2 - myNameWidth/2, screen_height/2 - 70 , myNameFontSize, LIGHTGRAY);

            int pressSpaceFontSize = 40;
            const char * pressSpace = "PRESS SPACEBAR TO START THE GAME";
            int pressSpaceWidth = MeasureText(pressSpace, pressSpaceFontSize);
            DrawText(pressSpace, screen_width/2 - pressSpaceWidth/2, screen_height/2 + 10, pressSpaceFontSize, BLUE);

            int pressIFontSize = 40;
            const char * pressI = "PRESS 'I' FOR CONTROLS";
            int pressIWidth = MeasureText(pressI, pressIFontSize);
            DrawText(pressI, screen_width/2 - pressIWidth/2, screen_height/2 + 70, pressIFontSize, BLUE);
            
            if(IsKeyPressed(KEY_SPACE))
            {
                main_menu_active = false;
            }
            
            if(IsKeyPressed(KEY_I))
            {
                instructions_menu = true;
                main_menu_active = false;
            }

            EndDrawing();
            continue;
            
        }
        
        if (instructions_menu)
        {

            const char *instructionsTitle = "CONTROLS & GAMEPLAY INSTRUCTIONS";
            int titleFontSize = 40;
            int titleWidth = MeasureText(instructionsTitle, titleFontSize);
            DrawText(instructionsTitle, screen_width / 2 - titleWidth / 2, 100, titleFontSize, BLUE);

            int fontSize = 25;
            int yStart = 180;
            int yGap = 35;

            DrawText("PLAYER 1: W (UP), S (DOWN)", 100, yStart, fontSize, LIGHTGRAY);
            DrawText("PLAYER 2: UP ARROW (UP), DOWN ARROW (DOWN)", 100, yStart + yGap, fontSize, LIGHTGRAY);
            DrawText("P: Pause / Resume", 100, yStart + yGap * 2, fontSize, LIGHTGRAY);
            DrawText("R: Restart", 100, yStart + yGap * 3, fontSize, LIGHTGRAY);
            DrawText("ESC: Quit", 100, yStart + yGap * 4, fontSize, LIGHTGRAY);
            DrawText("M: Return to Main Menu (after Game Over)", 100, yStart + yGap * 5, fontSize, LIGHTGRAY);
            DrawText("I: View Instructions", 100, yStart + yGap * 6, fontSize, LIGHTGRAY);

            // Press B to return to Main Menu
            const char *backMsg = "Press B to go back to the Main Menu";
            int backMsgWidth = MeasureText(backMsg, fontSize);
            DrawText(backMsg, screen_width / 2 - backMsgWidth / 2, screen_height - 100, fontSize, BLUE);

            if(IsKeyPressed(KEY_B))
            {
                instructions_menu = false;
                main_menu_active = true;
            }

            EndDrawing();
            continue;

        }

        if (!mode_selected)
        {
            const char *title = "SELECT GAME MODE";
            int titleFontSize = 40;
            int titleWidth = MeasureText(title, titleFontSize);
            DrawText(title, screen_width/2 - titleWidth/2, 200, titleFontSize, BLUE);

            int textFontSize = 30;
            const char * text1 = "PRESS 1 : PLAYER VS PLAYER";
            const char * text2 = "PRESS 2 : PLAYER VS CPU";
            const char * text3 = "PRESS M TO EXIT TO MAIN MENU";
            const char * text4 = "PRESS ESCAPE TO EXIT";
            int text1Width = MeasureText(text1, textFontSize);
            int text2Width = MeasureText(text2, textFontSize);
            int text3Width = MeasureText(text3, textFontSize);
            int text4Width = MeasureText(text4, textFontSize);
            DrawText(text1, screen_width/2 - text1Width/2, 300, textFontSize, LIGHTGRAY);
            DrawText(text2, screen_width/2 - text2Width/2, 350, textFontSize, LIGHTGRAY);
            DrawText(text3, screen_width/2 - text3Width/2, 400, textFontSize, LIGHTGRAY);
            DrawText(text4, screen_width/2 - text4Width/2, 450, textFontSize, LIGHTGRAY);
            

            if(IsKeyPressed(KEY_ONE))
            {
                vsCPU = false;
                mode_selected = true;
            }

            if(IsKeyPressed(KEY_TWO))
            {
                vsCPU = true;
                mode_selected = true;
            }

            if(IsKeyPressed(KEY_M))
            {
                main_menu_active = true;
                mode_selected = false;
            }

            EndDrawing();
            continue;
        }

        // Input for pause and restarting game
        if(IsKeyPressed(KEY_P))
        {
            paused = !paused;
        }

        if(IsKeyPressed(KEY_R))
        {
            restart_requested = true;
        }

        if(IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        //Update
        
        if (!paused && !gameOver)
        {
            ball.update();
            player1.updateManual(KEY_W, KEY_S);

            if(vsCPU)
            {
                player2.updateAI(ball.y);
            }
            else
            {
                player2.updateManual(KEY_UP, KEY_DOWN);
            }

            //Collision detection if ball collides with players
            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player1.x, player1.y, player1.width, player1.height}))
            {
                ball.speed_x *= -1;
                ball.color = GetRandomColor();
            }

            if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player2.x, player2.y, player2.width, player2.height}))
            {
                ball.speed_x *= -1;
                ball.color = GetRandomColor();
            }
        }

        //If restart is requested, reset scores and ball position
        if (restart_requested)
        {
            player1_score =0;
            player2_score = 0;
            ball.resetball();

            player1.y = screen_height/2 - player1.height/2;
            player2.y = screen_height/2 - player2.height/2;

            paused = false;
            restart_requested = false;
            gameOver = false;
        }
        

        //Drawing        
        if (!paused && !gameOver)
        {
            DrawCircle(screen_width/2, screen_height/2, 100, Color{0, 121, 241, 150});
            ball.draw();
            player1.draw();
            player2.draw();
            DrawText(TextFormat("%i", player1_score), screen_width/4 - 20, 20, 80, BLUE);
            DrawText(TextFormat("%i", player2_score), 3 * screen_width/4 - 20, 20, 80, BLUE);
            DrawLine(screen_width/2, 0, screen_width/2, screen_height, BLUE);

            int uiFontSize = 15;
            const char * uiTextP = "Press P for Pause";
            const char * uiTextR = "Press R for Restart";
            int uiTextPWidth = MeasureText(uiTextP, uiFontSize);
            int uiTextRWidth = MeasureText(uiTextR, uiFontSize);
            DrawText(uiTextP, screen_width/4 - uiTextPWidth/2, screen_height - 50, uiFontSize, LIGHTGRAY);
            DrawText(uiTextR, 3 * screen_width/4 - uiTextRWidth/2, screen_height - 50, uiFontSize, LIGHTGRAY);
        }
        else if(paused && !gameOver)
        {
            int pauseFontSize = 30;
            const char * pauseMsgText = "PAUSED - PRESS 'P' TO RESUME";
            int pauseMsgTextWidth =  MeasureText(pauseMsgText, pauseFontSize);
            DrawText(pauseMsgText, screen_width/2 - pauseMsgTextWidth/2, screen_height/2 - 20, pauseFontSize, LIGHTGRAY);

            int restartFontSize = 30;
            const char * restartMsgText = "PRESS 'R' TO RESTART";
            int restartMsgTextWidth =  MeasureText(restartMsgText, restartFontSize);
            DrawText(restartMsgText, screen_width/2 - restartMsgTextWidth/2, screen_height/2 + 20, restartFontSize, LIGHTGRAY);

            int menuFontSize = 30;
            const char * menuText = "PRESS 'M' TO RETURN TO MAIN MENU";
            int menuTextWidth =  MeasureText(menuText, restartFontSize);
            DrawText(menuText, screen_width/2 - menuTextWidth/2, screen_height/2 + 60, menuFontSize, LIGHTGRAY);

            //RETURN TO MAIN MENU ON PRESSING M
            if (IsKeyPressed(KEY_M))
            {
                main_menu_active = true;
                mode_selected = false;
                paused = false;
                restart_requested = false;
                gameOver = false;

                player1_score =0;
                player2_score = 0;
                ball.resetball();

                player1.y = screen_height/2 - player1.height/2;
                player2.y = screen_height/2 - player2.height/2;

            }

        }
        else if(gameOver)
        {
            int winMsgFontSize = 30;

            const char * winMsg;
            if (player1_score >= 5)
            {
                winMsg = "PLAYER 1 WINS";
            }
            if (player2_score >= 5)
            {
                winMsg = "PLAYER 2 WINS";
            }

            int winMsgTextWidth = MeasureText(winMsg, winMsgFontSize);
            DrawText(winMsg, screen_width/2 - winMsgTextWidth/2, screen_height/2 - 20, winMsgFontSize, LIGHTGRAY);

            int restartFontSize = 30;
            const char * restartMsgText = "PRESS 'R' TO RESTART";
            int restartMsgTextWidth =  MeasureText(restartMsgText, restartFontSize);
            DrawText(restartMsgText, screen_width/2 - restartMsgTextWidth/2, screen_height/2 + 20, restartFontSize, LIGHTGRAY);

            int mainMenuFontSize = 30;
            const char * mainMenuMsgText = "PRESS 'M' TO RETURN TO MAIN MENU";
            int mainMenuMsgTextWidth =  MeasureText(mainMenuMsgText, mainMenuFontSize);
            DrawText(mainMenuMsgText, screen_width/2 - mainMenuMsgTextWidth/2, screen_height/2 + 60, mainMenuFontSize, LIGHTGRAY);

            //RETURN TO MAIN MENU ON PRESSING M
            if (IsKeyPressed(KEY_M))
            {
                main_menu_active = true;
                mode_selected = false;
                paused = false;
                restart_requested = false;
                gameOver = false;

                player1_score =0;
                player2_score = 0;
                ball.resetball();

                player1.y = screen_height/2 - player1.height/2;
                player2.y = screen_height/2 - player2.height/2;

            }

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}