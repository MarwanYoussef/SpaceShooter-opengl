//Included Libraries
#include <stdlib.h>
#include <glut.h>
#include <string>

//Global Variables
int playerX = 350;	//Player x-coordinates
int playerY = 75;	//Player y-coordinates
int playerTranslationX = 0;	//Player moving
int attackerX = 0;	//Attacker x-coordinates
int attackerY = 0;	//Attacker y-coordinates
int attackerLife = 100;	//Attacker life
int BezierA[2];		// bezier A point
int BezierB[2];		// bezier B point
int BezierC[2];		// bezier C point
int BezierD[2];		// bezier D point
int heroBulletX;	//Player bullet x-coordinate
int heroBulletY = 75;	//Player bullet y-coordinates
int heroBulletR = 5;	//Player bullet radius
int attackerBulletX;	//Attacker bullet x-cordinates
int attackerBulletY;	//Attacker bullet y-coordinates
int level = 1;				//Level
int score = 0;				//Game score
int obstacleX = 200;		//Obstacle x-coordinates
int obstacleY = 600;		//Obstacle y-coordinates
float t = 0;				//Time
double playerRotation = 0;
double attackerRotation = 0;
bool heroBullet = false;		//Control flags
bool attackerBullet = false;	//Flag for controlling enemy bullets
bool directionE = true;			//False left and true right
bool direction = true;
bool obstacle = true;			//Obstacle one flag

								//Methods
								//Displaying Text Method
void displayRasterText(int x, int y, const char *string) {
	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}


//Translation based on cursor movement
void Mouse(int x, int y) {
	playerTranslationX = x;
	glutPostRedisplay();
}

//Draw rectangle function
void drawRectangle(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

//Draw circle function
void drawCircle(float x, float y, float r, int num_segments) {
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < num_segments; i++)
	{
		float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
		float xr = r * cosf(theta);
		float yr = r * sinf(theta);

		glVertex2f(xr + x, yr + y);

	}
	glEnd();
}

int random_at_most(int max) {
	unsigned int
		// max <= RAND_MAX < ULONG_MAX, so this is okay.
		num_bins = (unsigned int)max + 1,
		num_rand = (unsigned int)RAND_MAX + 1,
		bin_size = num_rand / num_bins,
		defect = num_rand % num_bins;

	int x;
	do {
		x = rand();
	} while (num_rand - defect <= (unsigned int)x);
	return x / bin_size;
}

int RandomBetween(int min, int max) {
	return min + random_at_most(max - min);
}

//Draw bullets
void bulletDraw(int x, int y, int r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric *quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
}


//Animation function
void animate() {

	if (obstacle) {
		if (direction) {
			if (obstacleX > 700) {
				direction = false;
			}
			else {
				if (obstacleX < 0) {
					direction = true;
				}
			}
		}
	}

	if (heroBullet) {
		if (heroBulletY < 700) {
			if (heroBulletY > 300 && (heroBulletX<60||heroBulletX>100)) {
				heroBulletY = 1000;
			}
			else {
				heroBulletY++;
			}
		}
		else {
			heroBulletY = 75;
			heroBullet = false;
		}
	}

	if (attackerBullet) {
		if (attackerBulletY > 0) {
			attackerBulletY--;
		}

		else {
			attackerBulletY = 0;
			attackerBullet = false;
		}
	}

	if (directionE) {
		if (t > 1)
			directionE = false;
		t += 0.001;
	}
	else {
		if (t<0)
			directionE = true;
		t -= 0.001;
	}

	if (t<0) {
		BezierC[0] = RandomBetween(100, 750);
		BezierC[1] = RandomBetween(350, 650);
		BezierB[0] = RandomBetween(100, 750);
		BezierB[1] = RandomBetween(350, 650);
	}

	attackerX = pow((1 - t), 3)*BezierA[0] + 3 * t*pow((1 - t), 2)*BezierB[0] + 3 * pow(t, 2)*(1 - t)*BezierC[0] + pow(t, 3)*BezierD[0];
	attackerY = pow((1 - t), 3)*BezierA[1] + 3 * t*pow((1 - t), 2)*BezierB[1] + 3 * pow(t, 2)*(1 - t)*BezierC[1] + pow(t, 3)*BezierD[1];

	if ((heroBulletX >= (attackerX - 16.25)) && heroBulletX <= (attackerX + 70) && heroBulletY == (attackerY - 10)) {
		score += 10;
		heroBulletY = 75;
		attackerLife -= 10;
		glClearColor(1, 0, 1, 0);
		heroBullet = false;
		if (attackerLife <= 0) {
			for (int i = 0; i < 10000; i++);
		}
	}

	if ((attackerBulletX >= (playerTranslationX - 70)) && attackerBulletY <= 73 && attackerBulletY >= 60 && attackerBulletX <= (playerTranslationX - 15)) {
		attackerBulletY = 0;
		attackerBullet = false;
		glClearColor(1, 0, 0, 0);
	}

	if (attackerLife <= 0) {
		level++;
		attackerLife = 100;
		glClearColor(0, 0, 0, 0);
	}

	glutPostRedisplay();
}


//Bezier curve
int* bezier(float t, int* p0, int* p1, int* p2, int* p3) {
	int res[2];
	res[0] = pow((1 - t), 3)*p0[0] + 3 * t*pow((1 - t), 2)*p1[0] + 3 * pow(t, 2)*(1 - t)*p2[0] + pow(t, 3)*p3[0];
	res[1] = pow((1 - t), 3)*p0[1] + 3 * t*pow((1 - t), 2)*p1[1] + 3 * pow(t, 2)*(1 - t)*p2[1] + pow(t, 3)*p3[1];

	return res;
}

//Fire key method
void Key(unsigned char key, int x, int y) {
	if (key == 'f') {
		heroBulletX = playerTranslationX;
		heroBullet = true;
	}
}

//Bullet movement function
void bulletMove() {
	if (heroBullet && heroBulletY > 700) {
		heroBulletY = 0;
		heroBullet = false;
	}
	else {
		if (heroBullet)
			heroBulletY++;
	}
}


//Timer function
void Timer(int time) {
	attackerBullet = true;
	attackerBulletX = attackerX;
	attackerBulletY = attackerY;
	glutTimerFunc(5 * 1000, Timer, 0);
	glutPostRedisplay();
}


//Display Function
void Display() {
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS);
	for (float t = 0; t < 1; t += 0.0001)
	{
		int* p = bezier(t, BezierA, BezierB, BezierC, BezierD);
		glVertex3f(p[0], p[1], 0);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(playerTranslationX - 350, 0, 0);


	drawRectangle(335, 50, 30, 10);
	drawCircle(335, 50, 5, 10);
	drawCircle(365, 50, 5, 10);

	glBegin(GL_TRIANGLES);
	glVertex2f(340, 55);
	glVertex2f(350, 73);
	glVertex2f(360, 55);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(attackerX - 22.5, attackerY - 22.5, 0);
	glTranslatef(attackerX, attackerY, 0);
	glTranslatef(-attackerX, -attackerY, 0);

	glPushMatrix();
	glTranslated(10, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(17.5, 25);
	glVertex2f(20, 25);
	glVertex2f(20, 27.5);
	glVertex2f(17.5, 27.5);
	glEnd();

	glPopMatrix();
	glBegin(GL_QUADS);
	glVertex2f(17.5, 25);
	glVertex2f(20, 25);
	glVertex2f(20, 27.5);
	glVertex2f(17.5, 27.5);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(15, 30);
	glVertex2f(15, 27.5);
	glVertex2f(12.5, 27.5);
	glVertex2f(12.5, 25);
	glVertex2f(10, 25);
	glVertex2f(10, 20);
	glVertex2f(12.5, 20);
	glVertex2f(12.5, 15);
	glVertex2f(15, 15);
	glVertex2f(15, 17.5);
	glVertex2f(32.5, 17.5);
	glVertex2f(32.5, 15);
	glVertex2f(35, 15);
	glVertex2f(35, 20);
	glVertex2f(37.5, 20);
	glVertex2f(37.5, 25);
	glVertex2f(35, 25);
	glVertex2f(35, 27.5);
	glVertex2f(32.5, 27.5);
	glVertex2f(32.5, 30);
	glEnd();

	glPopMatrix();

	char levels[15] = {};
	char level2[15] = "Level: ";
	displayRasterText(600, 600, strcat(level2, strcpy(levels, std::to_string(level).c_str())));

	char damage[10] = {};
	displayRasterText(110, 650, strcat(strcpy(damage, std::to_string(attackerLife).c_str()), "/100"));
	displayRasterText(80, 600, "Damage: ");

	char scores[15] = {};
	char scores2[15] = "Score: ";
	displayRasterText(350, 600, strcat(scores2, strcpy(scores, std::to_string(score).c_str())));

	if (heroBullet) {
		glPushMatrix();
		bulletDraw(heroBulletX, heroBulletY, heroBulletR);
		glPopMatrix();
	}

	if (attackerBullet) {
		glPushMatrix();
		bulletDraw(attackerBulletX + 40, attackerBulletY, 10);
		glPopMatrix();
	}

	if (obstacle) {
		glPushMatrix();
		glColor3f(1, 1, 0);
		drawRectangle(obstacleX, 300, 100, 15);
		glPopMatrix();
	}

	glFlush();
}

void main(int argc, char** argr) {
	glutInit(&argc, argr);
	glutInitWindowSize(800, 700);
	glutInitWindowPosition(150, 0);

	BezierA[0] = 50;
	BezierA[1] = 500;
	BezierB[0] = 600;
	BezierB[1] = 300;
	BezierC[0] = 500;
	BezierC[1] = 300;
	BezierD[0] = 750;
	BezierD[1] = 500;

	glutCreateWindow("Endless Shooting game");
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gluOrtho2D(0.0, 800, 0.0, 700);
	glutDisplayFunc(Display);
	glutIdleFunc(animate);
	glutTimerFunc(0, Timer, 0);
	glutKeyboardFunc(Key);
	glutPassiveMotionFunc(Mouse);
	glutMainLoop();
}
