#include "raylib.h"

//----------------------------------------------------------------------------------
//Algunas Definiciones 
//----------------------------------------------------------------------------------
#define MAX_TUBES 100
#define FLOPPY_RADIUS 15
#define TUBES_WIDTH 40

//----------------------------------------------------------------------------------
// Defuinicion de Typedef y structures
//----------------------------------------------------------------------------------
typedef struct Floppy {
    Vector2 position;
    int radius;
    Color color;
} Floppy;

typedef struct Tubes {
    Rectangle rec;
    Color color;
    bool active;
} Tubes;

//------------------------------------------------------------------------------------
// Declaracion de variables globales
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int hiScore = 0;

static Floppy floppy = { 0 };
static Tubes tubes[MAX_TUBES*2] = { 0 };
static Vector2 tubesPos[MAX_TUBES] = { 0 };
static int tubesSpeedX = 0;
static bool superfx = false;

//------------------------------------------------------------------------------------
// Declaración de funciones 
//------------------------------------------------------------------------------------
static void InitGame(void);         // Inicializar juego
static void UpdateGame(void);       // Actualizar juego (un frame)
static void DrawGame(void);         // Dibujo del juego (un frame)
static void UnloadGame(void);       // Unload game //Descargar juego
static void UpdateDrawFrame(void);  // Actualizar y Dibujar (un frame)
static int  RunGame(void);           // Ejecuta el juego en el menú.

//------------------------------------------------------------------------------------
// Punto de entrada principal del programa
//------------------------------------------------------------------------------------
int RunGame(void)
{
    // Inicializacion
    InitGame();

    InitAudioDevice();
    Music fondo = LoadMusicStream("C:/Proyecto final Programacion/Musica/Stay With Me - Miki Matsubara.mp3");
    PlayMusicStream(fondo);
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Loop del juego principal
    while (!WindowShouldClose())    // Detectar botón de cierre de ventana o tecla ESC
    {  
        // Reproduce la música
        UpdateMusicStream(fondo);
        
        // Actualizar y Dibujar el juego
        UpdateDrawFrame();
    }

    // Desinicialización (detener la música y el juego)
    UnloadGame();         // Descargar datos cargados(texturas, sonidos, etc...)
    UnloadMusicStream(fondo);
    CloseAudioDevice();
    return 0;
}
//------------------------------------------------------------------------------------
// Definiciones de las funciones del módulo(local)
//------------------------------------------------------------------------------------

// Inicializar variables del juego
void InitGame(void)
{
    floppy.radius = FLOPPY_RADIUS;
    floppy.position = (Vector2){160, screenHeight/2 - floppy.radius};
    tubesSpeedX = 6;
    

    for (int i = 0; i < MAX_TUBES; i++)
    {
        tubesPos[i].x = 400 + 280*i;
        tubesPos[i].y = -GetRandomValue(0, 220);
    }

    for (int i = 0; i < MAX_TUBES*2; i += 2)
    {
        tubes[i].rec.x = tubesPos[i/2].x;
        tubes[i].rec.y = tubesPos[i/2].y;
        tubes[i].rec.width = TUBES_WIDTH;
        tubes[i].rec.height = 255;

        tubes[i+1].rec.x = tubesPos[i/2].x;
        tubes[i+1].rec.y = 375 + tubesPos[i/2].y;
        tubes[i+1].rec.width = TUBES_WIDTH;
        tubes[i+1].rec.height = 400;

        tubes[i/2].active = true;
    }

    score = 0;

    gameOver = false;
    superfx = false;
    pause = false;
}

// Actualizar juego (one frame)
void UpdateGame(void)
{   
    
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            for (int i = 0; i < MAX_TUBES; i++) tubesPos[i].x -= tubesSpeedX;

            for (int i = 0; i < MAX_TUBES*2; i += 2)
            {
                tubes[i].rec.x = tubesPos[i/2].x;
                tubes[i+1].rec.x = tubesPos[i/2].x;
            }

            if (IsKeyDown(KEY_SPACE) && !gameOver) floppy.position.y -= 7;
            else floppy.position.y += 3;

            // Comprobar colisiones
            for (int i = 0; i < MAX_TUBES*2; i++)
            {
                if (CheckCollisionCircleRec(floppy.position, floppy.radius/2, tubes[i].rec))
                {
                    gameOver = true;
                    pause = false;
                }
                else if ((tubesPos[i/2].x < floppy.position.x) && tubes[i/2].active && !gameOver)
                {
                    score += 10;
                    tubes[i/2].active = false;

                    superfx = true;

                    if (score > hiScore) hiScore = score;
                }
            }
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Dibujar juego (one frame)
void DrawGame(void)
{   
    BeginDrawing(); 

        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            Texture2D bird = LoadTexture("C:/Proyecto final Programacion/Imagenes/Flappy_Cool.png");
            DrawTexture(bird,floppy.position.x-50, floppy.position.y-25, YELLOW);

            // Dibujar tubos 
            for (int i = 0; i < MAX_TUBES; i++)   
                DrawRectangle(tubes[i].rec.x, tubes[i].rec.y, tubes[i].rec.width, tubes[i].rec.height, GREEN);
            
            DrawText(TextFormat("%04i", score), 20, 20, 40, GRAY);
            DrawText(TextFormat("HI-SCORE: %04i", hiScore), 20, 70, 20, LIGHTGRAY);

            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, RED);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GREEN);

    EndDrawing();
}

// Descargar las variables del juego
void UnloadGame(void)
{
    //UnloadMusicStream(fondo);
}

// Actualizar y dibujar (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
// Definición de estructuras para el menú
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

//Inicio del menú
int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Flappy Cool. R&J");
    GameScreen currentScreen = LOGO;

    //Inicializacion de variables para el menu
    int framesCounter = 0;          // Cuenta los frames 
    SetTargetFPS(60);               // Establecer la velocidad de fotogramas deseada (frames-per-second)

    // Bucle de juego principal
    while (!WindowShouldClose())    // Detectar botón de cierre de ventana o tecla ESC
    {
        // Actualizar
        //----------------------------------------------------------------------------------
        switch(currentScreen)
        {
            case LOGO:
            {
                // TODO: ¡Actualización de las variables de la pantalla LOGO aquí!

                framesCounter++;    // Contar frames

                // Esperar 3 segundos (180 frames) antes de saltar a la pantalla TITLE 
                if (framesCounter > 180)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                //¡Actualizar las variables de la pantalla TITLLE aquí!

                // Presione Enter para cambiar a la pantalla de juego
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP) || IsKeyPressed(KEY_SPACE))
                {
                    
                    currentScreen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                ////Actualizar las variables del GAMEPLAY aquí
                if (framesCounter > 180)
                {
                    currentScreen = ENDING;
                }
                // Presione enter para saltar la pantalla del ENDING
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = ENDING;
                }
            } break;
            case ENDING:
            {
                // Actualice las variables la pantalla de ENDING!

                // Presione Enter para volver a la pantalla del Titulo
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                }
            } break;
            default: break;
        }
        //----------------------------------------------------------------------------------

        // Dibujar
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(SKYBLUE);

            switch(currentScreen)
            {
                case LOGO:
                {
                    //DibujarTexto("Esperar por 3 SECONDS...", 290, 220, 20, BLACK);
                    //Dibujar la pantalla Logo degradada aquí 
                    if (framesCounter <20)
                    {
                    DrawText("BIENVENIDO A", 250, 160, 40, WHITE);
                    }
                    else if(framesCounter <40)
                    {
                        DrawText("BIENVENIDO A", 250, 160, 40, RAYWHITE);
                    }
                    else if (framesCounter <60)
                    {
                        DrawText("BIENVENIDO A", 250, 160, 40, LIGHTGRAY);
                    }
                    else if (framesCounter <80)
                    {
                        DrawText("BIENVENIDO A", 250, 160, 40, GRAY); 
                    }
                    else if (framesCounter <100)
                    {
                        DrawText("BIENVENIDO A", 250, 160, 40, DARKGRAY); 
                    }
                    else if (framesCounter <180)
                    {
                        DrawText("BIENVENIDO A", 250, 160, 40, BLACK); 
                    }
                } break;
                case TITLE:
                {
                   // Dibujar la pantalla TITTLE aqui
                    DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
                    DrawText("Flappy Cool", 270, 150, 40, YELLOW);
                    DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                   
                } break;
                case GAMEPLAY:
                {
                      RunGame();
                } break;
                case ENDING:
                {
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("GOOD GAME", 270, 150, 40, ORANGE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                   
                } break;
                default: break;
            }

        EndDrawing();
    }

    CloseWindow();        // Cierra la ventana


    return 0;
}