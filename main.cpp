#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
using namespace std;

int winW = 800, winH = 600;

// Game state
int gameState = 0;
// 0 = menu, 1 = playing, 2 = paused, 3 = gameover, 4 = won

// Paddle
float paddleX = 350, paddleY = 30;
float paddleW = 100, paddleH = 15;
float normalPaddleW = 100;

// Ball
float ballX = 400, ballY = 100;
float ballR = 10;
float ballDX = 3, ballDY = 3;

// Bricks
bool bricks[5][10];
int score = 0, lives = 3;

// Perks
bool perkFalling = false;
float perkX, perkY;
int perkType = 0; // 1 = wide, 2 = fireball, 3 = extralife

bool wideActive = false;
int wideTimer = 0;

bool fireballActive = false;
int fireballTimer = 0;

bool gameStarted = false; // game শুরু হয়েছে কিনা

void initGame() {
    score = 0; lives = 3;
    ballX = 400; ballY = 100;
    ballDX = 3; ballDY = 3;
    paddleX = 350;
    paddleW = normalPaddleW;
    wideActive = false;
    wideTimer = 0;
    fireballActive = false;
    fireballTimer = 0;
    perkFalling = false;
    for(int i=0; i<5; i++)
        for(int j=0; j<10; j++)
            bricks[i][j] = true;
    gameState = 1;
    gameStarted = true;
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
    if(wideActive)
        glColor3f(0, 1, 1);
    else
        glColor3f(0, 1, 0);
    glBegin(GL_QUADS);
    glVertex2f(paddleX, paddleY);
    glVertex2f(paddleX+paddleW, paddleY);
    glVertex2f(paddleX+paddleW, paddleY+paddleH);
    glVertex2f(paddleX, paddleY+paddleH);
    glEnd();
}

void drawBall() {
    if(fireballActive)
        glColor3f(1, 0.5, 0);
    else
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

void drawHeart(float x, float y) {
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(int i=0; i<=360; i++) {
        float theta = i * 3.1416/180;
        float hx = x + 7 * sin(theta) * sin(theta) * sin(theta);
        float hy = y + 7 * (0.8*cos(theta) - 0.3*cos(2*theta) - 0.1*cos(3*theta));
        glVertex2f(hx, hy);
    }
    glEnd();
}

void drawPerk() {
    if(!perkFalling) return;
    if(perkType == 1) {
        glColor3f(0, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(perkX-10, perkY-10);
        glVertex2f(perkX+10, perkY-10);
        glVertex2f(perkX+10, perkY+10);
        glVertex2f(perkX-10, perkY+10);
        glEnd();
        glColor3f(0,0,0);
        drawText(perkX-5, perkY-5, "W");
    } else if(perkType == 2) {
        glColor3f(1, 0.3, 0);
        glBegin(GL_QUADS);
        glVertex2f(perkX-10, perkY-10);
        glVertex2f(perkX+10, perkY-10);
        glVertex2f(perkX+10, perkY+10);
        glVertex2f(perkX-10, perkY+10);
        glEnd();
        glColor3f(1,1,1);
        drawText(perkX-5, perkY-5, "F");
    } else if(perkType == 3) {
        drawHeart(perkX, perkY);
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

            if(fireballActive) {
                if(ballX>=x && ballX<=x+62 &&
                   ballY>=y && ballY<=y+25) {
                    bricks[i][j] = false;
                    score += 10;
                }
            } else {
                if(ballX>=x && ballX<=x+62 &&
                   ballY>=y && ballY<=y+25) {
                    bricks[i][j] = false;
                    ballDY = -ballDY;
                    score += 10;
                    if(!perkFalling && rand()%3 == 0) {
                        perkFalling = true;
                        perkX = x + 31;
                        perkY = y;
                        perkType = (rand()%3) + 1;
                    }
                }
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
        perkFalling = false;
        fireballActive = false;
        if(lives == 0) {
            gameState = 3;
        } else {
            ballX = 400; ballY = 100;
            ballDX = 3; ballDY = 3;
        }
    }

    // Perk falling
    if(perkFalling) {
        perkY -= 3;
        if(perkY <= paddleY+paddleH &&
           perkX >= paddleX &&
           perkX <= paddleX+paddleW) {
            perkFalling = false;
            if(perkType == 1) {
                wideActive = true;
                wideTimer = 300;
                paddleW = 180;
            } else if(perkType == 2) {
                fireballActive = true;
                fireballTimer = 300;
            } else if(perkType == 3) {
                lives++;
            }
        }
        if(perkY <= 0) perkFalling = false;
    }

    // Wide timer
    if(wideActive) {
        wideTimer--;
        if(wideTimer <= 0) {
            wideActive = false;
            paddleW = normalPaddleW;
        }
    }

    // Fireball timer
    if(fireballActive) {
        fireballTimer--;
        if(fireballTimer <= 0) {
            fireballActive = false;
        }
    }

    checkBrickCollision();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if(gameState == 0) {
        glColor3f(1,1,0);
        drawBigText(310, 450, "DX BALL");

        // Start button
        glColor3f(0, 0.8, 0);
        glBegin(GL_QUADS);
        glVertex2f(300, 350); glVertex2f(500, 350);
        glVertex2f(500, 390); glVertex2f(300, 390);
        glEnd();
        glColor3f(0,0,0);
        drawText(370, 365, "START");

        // Resume button (only if game started)
        if(gameStarted) {
            glColor3f(0, 0.5, 1);
            glBegin(GL_QUADS);
            glVertex2f(300, 295); glVertex2f(500, 295);
            glVertex2f(500, 335); glVertex2f(300, 335);
            glEnd();
            glColor3f(1,1,1);
            drawText(365, 310, "RESUME");
        }

        // Exit button
        glColor3f(0.8, 0, 0);
        glBegin(GL_QUADS);
        glVertex2f(300, 240); glVertex2f(500, 240);
        glVertex2f(500, 280); glVertex2f(300, 280);
        glEnd();
        glColor3f(1,1,1);
        drawText(375, 255, "EXIT");

        glColor3f(0.7,0.7,0.7);
        drawText(220, 180, "Mouse / Arrow keys to move paddle");
        drawText(270, 155, "P = Pause   M = Menu   R = Restart");
        drawText(200, 130, "W = Wide Paddle  F = Fireball  Heart = Extra Life");

    } else if(gameState == 1) {
        drawBricks();
        drawPaddle();
        drawBall();
        drawPerk();
        glColor3f(1,1,1);
        char s[50];
        sprintf(s, "Score: %d  Lives: %d", score, lives);
        drawText(10, 580, s);
        if(wideActive) {
            glColor3f(0,1,1);
            drawText(620, 580, "WIDE!");
        }
        if(fireballActive) {
            glColor3f(1,0.5,0);
            drawText(680, 580, "FIRE!");
        }

    } else if(gameState == 2) {
        drawBricks();
        drawPaddle();
        drawBall();
        drawPerk();
        glColor3f(1,1,0);
        drawBigText(320, 320, "PAUSED");
        drawText(240, 280, "Press P to continue");
        drawText(255, 250, "Press M for Menu");

    } else if(gameState == 3) {
        glColor3f(1,0,0);
        drawBigText(290, 320, "GAME OVER");
        char s[50];
        sprintf(s, "Score: %d", score);
        glColor3f(1,1,1);
        drawText(350, 280, s);
        drawText(270, 250, "Press R to restart");
        drawText(270, 220, "Press M for Menu");

    } else if(gameState == 4) {
        glColor3f(0,1,0);
        drawBigText(320, 320, "YOU WIN!");
        char s[50];
        sprintf(s, "Score: %d", score);
        glColor3f(1,1,1);
        drawText(350, 280, s);
        drawText(270, 250, "Press R to restart");
        drawText(270, 220, "Press M for Menu");
    }

    glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if(gameState == 0) {
            int my = winH - y;
            // Start
            if(x>=300 && x<=500 && my>=350 && my<=390) {
                initGame();
                glutTimerFunc(16, update, 0);
            }
            // Resume
            if(gameStarted && x>=300 && x<=500 && my>=295 && my<=335) {
                gameState = 1;
                glutTimerFunc(16, update, 0);
            }
            // Exit
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

    if(key == 'm' || key == 'M') {
        if(gameState == 1 || gameState == 2) {
            gameState = 0;
            glutPostRedisplay();
        }
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