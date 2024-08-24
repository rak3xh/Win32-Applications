#include <windows.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <mmsystem.h> // For PlaySound
#include <gdiplus.h> // For GDI+ graphics
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

const int WIDTH = 400;
const int HEIGHT = 400;
const int SQUARE_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct MyPoint {
    int x;
    int y;
};

class SnakeGame {
public:
    SnakeGame() : direction(RIGHT), gameOver(false), score(0), speed(100) {
        snake.push_back({ WIDTH / 2, HEIGHT / 2 });
        spawnFood();
    }

    void changeDirection(Direction newDirection) {
        if ((direction == UP && newDirection != DOWN) ||
            (direction == DOWN && newDirection != UP) ||
            (direction == LEFT && newDirection != RIGHT) ||
            (direction == RIGHT && newDirection != LEFT)) {
            direction = newDirection;
        }
    }

    void update() {
        if (gameOver) return;

        MyPoint newHead = snake.front();

        switch (direction) {
        case UP: newHead.y -= SQUARE_SIZE; break;
        case DOWN: newHead.y += SQUARE_SIZE; break;
        case LEFT: newHead.x -= SQUARE_SIZE; break;
        case RIGHT: newHead.x += SQUARE_SIZE; break;
        }

        // Check for collision with walls
        if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
            gameOver = true;
            PlaySound(TEXT("gameover.wav"), NULL, SND_FILENAME | SND_ASYNC);
            return;
        }

        // Check for collision with self
        for (const auto& segment : snake) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                gameOver = true;
                PlaySound(TEXT("gameover.wav"), NULL, SND_FILENAME | SND_ASYNC);
                return;
            }
        }

        snake.insert(snake.begin(), newHead);

        // Check if the snake has eaten the food
        if (newHead.x == food.x && newHead.y == food.y) {
            PlaySound(TEXT("eat.wav"), NULL, SND_FILENAME | SND_ASYNC);
            spawnFood();
            score += 10;
            speed = max(10, speed - 5); // Increase speed
        }
        else {
            snake.pop_back();
        }
    }

    void draw(Graphics& graphics) {
        SolidBrush snakeBrush(Color(0, 255, 0));
        SolidBrush foodBrush(Color(255, 0, 0));

        for (const auto& segment : snake) {
            graphics.FillRectangle(&snakeBrush, segment.x, segment.y, SQUARE_SIZE, SQUARE_SIZE);
        }

        graphics.FillRectangle(&foodBrush, food.x, food.y, SQUARE_SIZE, SQUARE_SIZE);

        // Draw score
        FontFamily fontFamily(L"Arial");
        Font font(&fontFamily, 16, FontStyleRegular, UnitPixel);
        SolidBrush textBrush(Color(255, 255, 255));
        WCHAR scoreText[50];
        wsprintf(scoreText, L"Score: %d", score);
        graphics.DrawString(scoreText, -1, &font, PointF(10.0f, 10.0f), &textBrush);
    }

    bool isGameOver() const {
        return gameOver;
    }

    int getSpeed() const {
        return speed;
    }

private:
    std::vector<MyPoint> snake;
    MyPoint food;
    Direction direction;
    bool gameOver;
    int score;
    int speed;

    void spawnFood() {
        food.x = (rand() % (WIDTH / SQUARE_SIZE)) * SQUARE_SIZE;
        food.y = (rand() % (HEIGHT / SQUARE_SIZE)) * SQUARE_SIZE;
    }
};

// Global variables
SnakeGame snakeGame;
ULONG_PTR gdiplusToken;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    Graphics* graphics = nullptr;

    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        graphics = new Graphics(hdc);
        snakeGame.draw(*graphics);
        delete graphics;
        EndPaint(hWnd, &ps);
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP: snakeGame.changeDirection(UP); break;
        case VK_DOWN: snakeGame.changeDirection(DOWN); break;
        case VK_LEFT: snakeGame.changeDirection(LEFT); break;
        case VK_RIGHT: snakeGame.changeDirection(RIGHT); break;
        }
        break;
    case WM_TIMER:
        snakeGame.update();
        if (snakeGame.isGameOver()) {
            KillTimer(hWnd, 1);
            MessageBox(hWnd, L"Game Over", L"Snake Xenzia", MB_OK);
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand(static_cast<unsigned>(time(NULL)));

    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"SnakeXenzia";

    if (!RegisterClass(&wc)) {
        return -1;
    }

    HWND hWnd = CreateWindow(L"SnakeXenzia", L"Snake Xenzia", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SetTimer(hWnd, 1, snakeGame.getSpeed(), NULL);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Shutdown GDI+
    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}
