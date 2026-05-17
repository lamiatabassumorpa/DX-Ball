#include <GL/glut.h>
#include <cmath>
#include <cstdio>
using namespace std;

// Window
int winW = 800, winH = 600;

// Paddle
float paddleX = 350, paddleY = 30;
float paddleW = 100, paddleH = 15;

// Ball
float ballX = 400, ballY = 100;
float ballR = 10;
float ballDX = 3, ballDY = 3;

// Bricks
bool bricks[5][10];
int score = 0, lives = 3;
bool gameOver = false, gameWon = false;

void initBricks() {
    for(int i=0; i<5; i++)
        for(int j=0; j<10; j++)
            bricks[i][j] = true;
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while(*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}

void drawPaddle() {
    glColor3f(0, 1, 0);
    glBegin(GL_QUADS);
    glVertex2f(paddleX, paddleY);
    glVertex2f(paddleX+paddleW, paddleY);
    glVertex2f(paddleX+paddleW, paddleY+paddleH);
    glVertex2f(paddleX, paddleY+paddleH);
    glEnd();
}

void drawBall() {
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0; i<=360; i++) {
        float theta = i * 3.1416/180;
        glVertex2f(ballX + ballR*cos(theta),
                   ballY + ballR*sin(theta));
    }
    glEnd();
}

void drawBricks() {
    for(int i=0; i<5; i++) {
        for(int j=0; j<10; j++) {
            if(!bricks[i][j]) continue;
            if(i==0) glColor3f(1,0,0);
            else if(i==1) glColor3f(1,0.5,0);
            else if(i==2) glColor3f(1,1,0);
            else if(i==3) glColor3f(0,1,0);
            else glColor3f(0,0,1);

            float x = 50 + j*72;
            float y = 450 + i*35;
            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x+62, y);
            glVertex2f(x+62, y+25);
            glVertex2f(x, y+25);
            glEnd();
        }
    }
}

void checkBrickCollision() {
    bool anyLeft = false;
    for(int i=0; i<5; i++) {
        for(int j=0; j<10; j++) {
            if(!bricks[i][j]) continue;
            anyLeft = true;
            float x = 50 + j*72;
            float y = 450 + i*35;
            if(ballX>=x && ballX<=x+62 &&
               ballY>=y && ballY<=y+25) {
                bricks[i][j] = false;
                ballDY = -ballDY;
                score += 10;
            }
        }
    }
    if(!anyLeft) gameWon = true;
}

void update(int val) {
    if(gameOver || gameWon) return;

    ballX += ballDX;
    ballY += ballDY;

    // Wall collision
    if(ballX <= 0 || ballX >= winW) ballDX = -ballDX;
    if(ballY >= winH) ballDY = -ballDY;

    // Paddle collision
    if(ballY <= paddleY+paddleH &&
       ballY >= paddleY &&
       ballX >= paddleX &&
       ballX <= paddleX+paddleW)
        ballDY = -ballDY;

    // Ball fell down
    if(ballY <= 0) {
        lives--;
        if(lives == 0) {
            gameOver = true;
        } else {
            ballX = 400; ballY = 100;
            ballDX = 3; ballDY = 3;
        }
    }

    checkBrickCollision();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if(gameOver) {
        glColor3f(1,0,0);
        drawText(330, 300, "GAME OVER");
        char s[50];
        sprintf(s, "Score: %d", score);
        drawText(350, 270, s);
        drawText(270, 240, "Press R to restart");
        glFlush();
        return;
    }

    if(gameWon) {
        glColor3f(0,1,0);
        drawText(330, 300, "YOU WIN!");
        char s[50];
        sprintf(s, "Score: %d", score);
        drawText(350, 270, s);
        glFlush();
        return;
    }

    drawBricks();
    drawPaddle();
    drawBall();

    // HUD
    glColor3f(1,1,1);
    char s[50];
    sprintf(s, "Score: %d  Lives: %d", score, lives);
    drawText(10, 580, s);

    glFlush();
}

void mouseMove(int x, int y) {
    paddleX = x - paddleW/2;
    if(paddleX < 0) paddleX = 0;
    if(paddleX > winW-paddleW) paddleX = winW-paddleW;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if(key == 27) exit(0); // ESC
    if(key == 'r' || key == 'R') {
        gameOver = false;
        gameWon = false;
        score = 0;
        lives = 3;
        ballX = 400; ballY = 100;
        ballDX = 3; ballDY = 3;
        initBricks();
        glutTimerFunc(16, update, 0);
    }
}

void specialKey(int key, int x, int y) {
    if(key == GLUT_KEY_LEFT) paddleX -= 20;
    if(key == GLUT_KEY_RIGHT) paddleX += 20;
    if(paddleX < 0) paddleX = 0;
    if(paddleX > winW-paddleW) paddleX = winW-paddleW;
    glutPostRedisplay();
}

void init() {
    glClearColor(0,0,0,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("DX Ball");

    init();
    initBricks();

    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMove);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
}