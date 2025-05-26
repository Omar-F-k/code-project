
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstring>

const int WIDTH = 480, HEIGHT = 320;
float ballX = WIDTH / 2.0f, ballY = HEIGHT / 2.0f;
float ballDX = 2.0f, ballDY = -2.0f;
float ballRadius = 8.0f;

float paddleWidth = 75.0f, paddleHeight = 10.0f;
float paddleX = (WIDTH - paddleWidth) / 2.0f;
float paddleSpeed = 6.0f;

bool moveLeft = false, moveRight = false;
bool gameOver = false;

const int brickRows = 4, brickCols = 5;
const float brickWidth = 75.0f, brickHeight = 20.0f;
const float brickPadding = 10.0f, brickOffsetTop = 30.0f, brickOffsetLeft = 35.0f;

struct Brick {
    float x, y;
    bool active;
};

std::vector<Brick> bricks;

int score = 0;
int countdown = 60;
time_t lastTime;

void initBricks() {
    bricks.clear();
    for (int c = 0; c < brickCols; ++c) {
        for (int r = 0; r < brickRows; ++r) {
            Brick b;
            b.x = c * (brickWidth + brickPadding) + brickOffsetLeft;
            b.y = r * (brickHeight + brickPadding) + brickOffsetTop;
            b.active = true;
            bricks.push_back(b);
        }
    }
}

void drawCircle(float x, float y, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex2f(x, y);
    for (int i = 0; i <= 100; ++i) {
        float angle = 2.0f * M_PI * i / 100.0f;
        glVertex2f(x + std::cos(angle) * r, y + std::sin(angle) * r);
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        ++text;
    }
}

void drawBricks() {
    for (const auto& b : bricks) {
        if (b.active) {
            drawRect(b.x, b.y, brickWidth, brickHeight, 1.0f, 0.4f, 0.4f);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if (gameOver) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(WIDTH / 2.0f - 50, HEIGHT / 2.0f, "GAME OVER");
        char scoreStr[32];
        sprintf(scoreStr, "Score: %d", score);
        drawText(WIDTH / 2.0f - 50, HEIGHT / 2.0f + 20, scoreStr);
        glutSwapBuffers();
        return;
    }

    drawCircle(ballX, ballY, ballRadius);
    drawRect(paddleX, HEIGHT - paddleHeight - 5.0f, paddleWidth, paddleHeight, 1.0f, 0.0f, 1.0f);
    drawBricks();

    char timeStr[32];
    sprintf(timeStr, "Time: %d", countdown);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(10.0f, 20.0f, timeStr);

    char scoreStr[32];
    sprintf(scoreStr, "Score: %d", score);
    drawText(10.0f, 40.0f, scoreStr);

    glutSwapBuffers();
}

void update(int value) {
    if (gameOver) return;

    if (std::time(nullptr) - lastTime >= 1) {
        countdown--;
        lastTime = std::time(nullptr);
        if (countdown <= 0) gameOver = true;
    }

    if (moveLeft && paddleX > 0) paddleX -= paddleSpeed;
    if (moveRight && paddleX < WIDTH - paddleWidth) paddleX += paddleSpeed;

    ballX += ballDX;
    ballY += ballDY;

    if (ballX - ballRadius < 0 || ballX + ballRadius > WIDTH) ballDX = -ballDX;
    if (ballY - ballRadius < 0) ballDY = -ballDY;

    if (ballY + ballRadius >= HEIGHT - paddleHeight - 5.0f &&
        ballX > paddleX && ballX < paddleX + paddleWidth) {
        ballDY = -ballDY;
    }

    if (ballY - ballRadius > HEIGHT) {
        gameOver = true;
    }

    for (auto& b : bricks) {
        if (b.active) {
            if (ballX > b.x && ballX < b.x + brickWidth &&
                ballY > b.y && ballY < b.y + brickHeight) {
                ballDY = -ballDY;
                b.active = false;
                score += 10;
                break;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboardDown(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') moveLeft = true;
    if (key == 'd' || key == 'D') moveRight = true;
}

void keyboardUp(unsigned char key, int x, int y) {
    if (key == 'a' || key == 'A') moveLeft = false;
    if (key == 'd' || key == 'D') moveRight = false;
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WIDTH, HEIGHT, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Brick Breaker (OpenGL)");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(16, update, 0);

    initBricks();
    lastTime = std::time(nullptr);

    glutMainLoop();
    return 0;
}
