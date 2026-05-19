#include <GL/glut.h>
#include <cmath>
#include <cstdio>
using namespace std;

int winW = 800, winH = 600;

// Game state
int gameState = 0; 
// 0 = menu, 1 = playing, 2 = paused, 3 = gameover, 4 = won

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

void initGame() {
    score = 0; lives = 3;
    ballX = 400; ballY = 100;
    ballDX = 3; ballDY = 3;
    paddleX = 350;
    for(int i=0; i<5; i++)
        for(int j=0; j<10; j++)
            bricks[i][j] = true;
    gameState = 1;
}

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while(*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}

void drawBigText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while(*text)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text++);
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

            glColor3f(0,0,0);
            glBegin(GL_LINE_LOOP);
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
    if(!anyLeft) gameState = 4;
}

void update(int val) {
    if(gameState != 1) return;

    ballX += ballDX;
    ballY += ballDY;

    if(ballX <= 0 || ballX >= winW) ballDX = -ballDX;
    if(ballY >= winH) ballDY = -ballDY;

    if(ballY <= paddleY+paddleH &&
       ballY >= paddleY &&
       ballX >= paddleX &&
       ballX <= paddleX+paddleW)
        ballDY = fabs(ballDY);

    if(ballY <= 0) {
        lives--;
        if(lives == 0) {
            gameState = 3;
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

    if(gameState == 0) {
        // Menu
        glColor3f(1,1,0);
        drawBigText(310, 450, "DX BALL");

        glColor3f(0, 0.8, 0);
        glBegin(GL_QUADS);
        glVertex2f(300, 300); glVertex2f(500, 300);
        glVertex2f(500, 340); glVertex2f(300, 340);
        glEnd();
        glColor3f(0,0,0);
        drawText(370, 315, "START");

        glColor3f(0.8, 0, 0);
        glBegin(GL_QUADS);
        glVertex2f(300, 240); glVertex2f(500, 240);
        glVertex2f(500, 280); glVertex2f(300, 280);
        glEnd();
        glColor3f(1,1,1);
        drawText(375, 255, "EXIT");

        glColor3f(0.7,0.7,0.7);
        drawText(220, 180, "Mouse / Arrow keys to move paddle");
        drawText(270, 155, "P = Pause    R = Restart");

    } else if(gameState == 1) {
        drawBricks();
        drawPaddle();
        drawBall();
        glColor3f(1,1,1);
        char s[50];
        sprintf(s, "Score: %d  Lives: %d", score, lives);
        drawText(10, 580, s);

    } else if(gameState == 2) {
        drawBricks();
        drawPaddle();
        drawBall();
        glColor3f(1,1,0);
        drawBigText(320, 320, "PAUSED");
        drawText(240, 280, "Press P to continue");

    } else if(gameState == 3) {
        glColor3f(1,0,0);
        drawBigText(290, 320, "GAME OVER");
        char s[50];
        sprintf(s, "Score: %d", score);
        glColor3f(1,1,1);
        drawText(350, 280, s);
        drawText(270, 250, "Press R to restart");

    } else if(gameState == 4) {
        glColor3f(0,1,0);
        drawBigText(320, 320, "YOU WIN!");
        char s[50];
        sprintf(s, "Score: %d", score);
        glColor3f(1,1,1);
        drawText(350, 280, s);
        drawText(270, 250, "Press R to restart");
    }

    glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(gameState == 0) {
            int my = winH - y;
            if(x>=300 && x<=500 && my>=300 && my<=340) {
                initGame();
                glutTimerFunc(16, update, 0);
            }
            if(x>=300 && x<=500 && my>=240 && my<=280) {
                exit(0);
            }
        }
    }
}

void mouseMove(int x, int y) {
    if(gameState != 1) return;
    paddleX = x - paddleW/2;
    if(paddleX < 0) paddleX = 0;
    if(paddleX > winW-paddleW) paddleX = winW-paddleW;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    if(key == 27) exit(0);

    if(key == 'p' || key == 'P') {
        if(gameState == 1) gameState = 2;
        else if(gameState == 2) {
            gameState = 1;
            glutTimerFunc(16, update, 0);
        }
        glutPostRedisplay();
    }

    if(key == 'r' || key == 'R') {
        if(gameState == 3 || gameState == 4 || gameState == 1) {
            initGame();
            glutTimerFunc(16, update, 0);
        }
    }
}

void specialKey(int key, int x, int y) {
    if(gameState != 1) return;
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("DX Ball");
    init();
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutMainLoop();
}