/* VC�p�̂���windows.h����include����K�v������ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>
#include "text_GL.h"

#define _X 0
#define _Y 1
#define _Z 2

#define FPS 10

#define _LEFT 0
#define _MIDDLE 1
#define _RIGHT 2

/* �O���[�o���ϐ��錾 */


/* �x�N�g���^ */
typedef struct{
	GLfloat vector[3];	/* �x�N�g��[XYZ] */
	GLfloat norm;		/* �x�N�g���̑傫�� */
} Vector;

/* �}�e���A���i�ގ��j�^ */
typedef struct{
	GLfloat diffuse_rgba[4];     /* �g�U���ːF */
	GLfloat specular_rgba[4];    /* ���ʔ��ːF */
	GLfloat shininess_rate;      /* �P���̓x���� */
} Material;

/* �e�̌^ */
typedef struct {
	Vector position;       /* �ʒu�x�N�g�� */
	Vector speed;          /* ���x�x�N�g�� */
	Vector addspeed;       /* �����x�x�N�g�� */
	Material material;     /* �ގ� */
	GLint tamaflag;        /* �e�̗L���t���O */
	GLint hitcount;        /* �q�b�g�� */
	GLint point;           /* ���_ */
	GLint hitflag[10][10];
} Tama;

/* ���@�i�D�j�̌^ */
typedef struct {
	Vector position;       /* �ʒu�x�N�g�� */
	Vector speed;          /* ���x�x�N�g�� */
	Vector direction;      /* �����x�N�g�� */
	GLfloat power;         /* �p���[ */
	Vector mobility;       /* �@���� */
	GLint life;            /* �c�@ */
	GLfloat hitwidth, hitheight, hitdepth; /* �����蔻��T�C�Y */
	Material material;     /* �ގ� */
} Ship;

/* �X�e�[�W�̌^ */
typedef struct {
	GLint block[10][10];
	GLint blocks;
	GLfloat left, right, top, bottom;
	GLint gravity_flag;    /* -1:������ 0:�Ȃ� 1:����� */
	GLfloat blockroll[10][10];           /* �u���b�N��] */
	GLfloat blockrollspeed[20];          /* �u���b�N��]���x */
	GLint blockpoint[20];                /* �u���b�N���_ */
	char filename[16];                   /* �t�@�C���� */
	GLint enemy;
} Stage;

/* �{�^�����͏�� */
typedef struct {
	GLint Down[3];
	GLint Up[3];
	GLint parts;
	GLint command;
} Button;

Ship ship;
Tama tama[10];
Stage stage;
Button mouse;
Material textmaterial;
/* �O���[�o���ϐ���` */


/* �֐��錾 */
/* �L�����N�^�[������ */
void Initcharacter(void)
{
	int i;
	/* ���@�̏����� */
	for(i=0; i<3; i++){
		ship.position.vector[i] = 0.0;
		ship.speed.vector[i] = 0.0;
		ship.direction.vector[i] = 0.0;
   	}
	ship.position.vector[_X] = 10.0;
	ship.position.vector[_Y] = -10.0;
	ship.position.vector[_Z] = 0.0; 
	ship.position.norm = 0.0;
	ship.speed.norm = 0.0;
	ship.power = 0.1;
	ship.mobility.vector[0] = 0.1;
	ship.mobility.vector[1] = 0.1;
	ship.mobility.vector[2] = 0.1;
	ship.life = 10;
	ship.hitwidth = 2.0;
	ship.hitheight = 1.0;
	ship.hitdepth = 0.5;
	ship.material.diffuse_rgba[0] = 0.7;
	ship.material.diffuse_rgba[1] = 1.0;
	ship.material.diffuse_rgba[2] = 0.7;
	ship.material.diffuse_rgba[3] = 1.0;
	ship.material.specular_rgba[0] = 1.0;
	ship.material.specular_rgba[1] = 1.0;
	ship.material.specular_rgba[2] = 1.0;
	ship.material.specular_rgba[3] = 1.0;
	ship.material.shininess_rate = 80.0;

	/* �e�̏����� */
	for(i=0; i<10; i++){
		tama[i].material.diffuse_rgba[0] = 0.1*i;
		tama[i].material.diffuse_rgba[1] = sin(0.314*i);
		tama[i].material.diffuse_rgba[2] = (10-i)*0.1;
		tama[i].material.diffuse_rgba[3] = 1.0;
		tama[i].material.specular_rgba[0] = 1.0;
		tama[i].material.specular_rgba[1] = 1.0;
		tama[i].material.specular_rgba[2] = 1.0;
		tama[i].material.specular_rgba[3] = 1.0;
		tama[i].material.shininess_rate = 40.0;
		tama[i].tamaflag = 0;
	}
	/* �e�L�X�g�p�}�e���A���̏����� */
	textmaterial.diffuse_rgba[0] = 1.0;
	textmaterial.diffuse_rgba[1] = 1.0;
	textmaterial.diffuse_rgba[2] = 1.0;
	textmaterial.diffuse_rgba[3] = 1.0;
	textmaterial.specular_rgba[0] = 1.0;
	textmaterial.specular_rgba[1] = 1.0;
	textmaterial.specular_rgba[2] = 1.0;
	textmaterial.specular_rgba[3] = 1.0;
	textmaterial.shininess_rate = 90.0;

}

/* �����ɐV�����֐���ϐ���ǉ����Ă������� */

/* �ҏW�p�֐��Q */

/* �X�e�[�W�ҏW�p�֐��Q */

/* �E�B���h�E������ */
void window(void)
{
	//auxInitDisplayMode(AUX_DOUBLE | AUX_RGBA | AUX_DEPTH);
	//auxInitPosition(0,0,800,800);
	//auxInitWindow("Block");

	// freeglut
	int argc = 1;
	char *argv[1] = { (char*)"Block" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Block");
}

/* �X�e�[�W������ */
void Initstage(void)
{
	int i,j;
	stage.blocks = 0;
	for(i=0; i<10; i++){
		for(j=0; j<10; j++){
			stage.block[i][j] = 0;
		}
	}
	stage.left = 0.0;
	stage.right = 20.0;
	stage.top = 15.0;
	stage.bottom = -15.0;
	/* �t�@�C���������� */
	sprintf(stage.filename,"UnTitled");
	/* �d�̓t���O������ */
	stage.gravity_flag = 0;
	

}

/* ���C�g�ݒ� */
void light_setting(void)
{
	GLfloat v[4] = { 1.0,1.0,1.0,1.0 }; /* RGBA �F���� */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);    /* �g�U���̐ݒ� */
	glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);  /* ���C�e�B���O�ƃ��C�g0�̗L���� */
	glEnable(GL_NORMALIZE);   /* �@���x�N�g���̐��K����L���� */
	glEnable(GL_DEPTH_TEST);  /* �f�v�X�e�X�g�̗L���� */
}

/* �����ɐV�����֐���ϐ���ǉ����Ă������� */

/* �X�e�[�W�ҏW�p�֐��Q */


/* �X�e�[�W�ۑ� */
void savestage(void)
{
	FILE **fp;
	int i,j;
	printf("save stage start\n");
	if((fp = fopen(stage.filename,"w")) == 0){
		printf("save stage err\n");
		exit(0);
		return;
	}
	
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			fprintf(fp,"%d\n",stage.block[i][j]);
		}
	}
	fprintf(fp,"%d\n",stage.gravity_flag);
	fprintf(fp,"%d\n",stage.enemy);
	fclose(fp);
	
	printf("save stage on");
}

/* �X�e�[�W�Ǎ� */
void loadstage(void)
{
	FILE **fp;
	int i,j;
	
	printf("load stage start\n");
	if((fp = fopen(stage.filename,"r")) == 0){
		printf("load stage err\n");
		exit(0);
		return;
	}
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			fscanf(fp,"%d\n",&stage.block[i][j]);
		}
	}
	fscanf(fp,"%d\n",&stage.gravity_flag);
	fscanf(fp,"%d\n",&stage.enemy);
	fclose(fp);
	
	printf("load stage OK\n");
}

/* �������e�s��ݒ� */
void tousihoushaei(GLfloat w, GLfloat h){
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	gluPerspective(120,(GLfloat)w/(GLfloat)h,1.0,40.0);
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	gluLookAt(9.5,-1.0,10.0, 9.5,0.0,0.0, 0.0,1.0,0.0);
}

/* ���ˉe�s��ݒ� */
void seishaei(void)
{
	glViewport(0, 0, 800, 800);          /* �r���[�|�[�g�̐ݒ� */
	glMatrixMode(GL_PROJECTION); glLoadIdentity();   /* �ˉe�s��̏����� */
	glOrtho(0.0, 40.0, 0.0, 40.0, 0.0, 40.0);     /* ���ˉe�s��̐ݒ� */
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();   /* ���f���r���[�s��̏����� */
	gluLookAt(0.0, -5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

/* �`�揈�� */
/* �����`�� */
void Drawnumber(GLint num)
{
	char str[12]={'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
	GLint a=1000000000,i,b,j=0,flag=0;
	if(num != 0){
		for(i=0;i<10;i++){
			if(num>=a || flag){
				flag = 1;
				b = num / a;
				str[j] = '0'+(char)b;
				num -= b*a;
				j++;
			}
			a/=10;
		}
		str[j]='\0';
	}
	else{
		str[0] = '0';
	}
	DrawString(str);
}

/* �R�}���h�`�� */
void commandput(void)
{
	GLint i;
	char command[6][20] = {"File Name","Load","Save","Gravity","Enemy","Display"};
	char str[50];

	for(i=0;i<6;i++){
		glRasterPos3d(34,37-i*2,0);
		DrawString(command[i]);
	}
	glRasterPos3d(35,36,0);
	DrawString(stage.filename);

	sprintf(str,"%d",stage.gravity_flag);
	glRasterPos3d(35,30,0);
	DrawString(str);

	sprintf(str,"%d",mouse.Down[_MIDDLE]);
	glRasterPos3d(35,26,0);
	DrawString(str);
}

/* �e�L�X�g�`�� */
void textsput(void)
{
	GLint i,j,x,y;
	GLfloat px,py,fi;
	char str[50];

	GLfloat v[][4] = {{1.0,1.0,1.0,1.0},{0.0,0.0,0.0,1.0}};
	glMaterialfv(GL_FRONT,GL_EMISSION, v[0]);
	/* ���ͱƤ�ɽ�� */
	seishaei();

	auxGetMouseLoc(&x,&y);
	sprintf(str,"x = %d",x);
	glRasterPos3d(0,7,0);
	DrawString(str);
	sprintf(str,"y = %d",y);
	glRasterPos3d(0,6,0);
	DrawString(str);

	sprintf(str,"Parts=%d Button=%d",mouse.parts,mouse.Down[_LEFT]);
	glRasterPos3d(0,30,0);
	DrawString(str);

	commandput();

	/* Ʃ��E��ͱƤ�ɽ�� */
	tousihoushaei(800.0,800.0);

	glMaterialfv(GL_FRONT,GL_EMISSION, v[1]);
}

/* �|���S���`�� */
void polygonput(GLint mode, GLfloat x, GLfloat y, GLint block)
{
	GLfloat w[2][4]={{1.0,1.0,1.0,1.0},{0.0,0.0,0.0,1.0}};
	GLint mode1;
	mode1 = mode;
	mode -= mouse.Down[_MIDDLE]; 
	if(block<=10){
		if(mode1 || block && mode){
			auxSolidBox(1.8,0.8,(GLfloat)(block/2.0));
		}
		else {
			auxWireBox(1.8,0.8,(GLfloat)(block/2.0));
		}
	}
	else if(block>=11){
		switch(block){
			case 11: glPushMatrix();
			           glRotated(45,0,1,0);
			           if(mode){
						   auxSolidCube(1.5);
					   }
			           else {
						   auxWireCube(1.5);
					   }
			         glPopMatrix();
			         break;
			case 12: glPushMatrix();
			           if(mode){
						   auxSolidTetrahedron(0.6);
					   }
			           else {
						   auxWireTetrahedron(0.6);
					   }
			         glPopMatrix();
			         break;
			case 13: glPushMatrix();
			           if(mode){
						   auxSolidCone(1.0,2.0);
					   }
			           else {
						   auxWireCone(1.0,2.0);
					   }
			         glPopMatrix();
			         break;
		}
		if(block>=14 && block<=20){
			glPushMatrix();
			  glRotatef(51.4*(float)(block-13),1.0,0.0,0.0);
			  auxWireBox(1.8,0.8,0.8);
			glPopMatrix();
			glMaterialfv(GL_FRONT,GL_EMISSION, w[0]);
			glRasterPos3f(x/8.0-2.0,y/8.0,-1.0);
			Drawnumber((block-13)*10);
			glMaterialfv(GL_FRONT,GL_EMISSION, w[1]);
		}
	}
}

/* �u���b�N�`�� */
void blockput(GLfloat x, GLfloat y, GLint block)
{
	GLfloat v[2][4];
	v[0][0]=x/10.0;
	v[0][1]=y/10.0;
	v[0][2]=1.0;
	v[0][3]=1.0;
	v[1][0]=1.0;
	v[1][1]=1.0;
	v[1][2]=1.0;
	v[1][3]=1.0;

	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v[0]);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v[1]);
	glMaterialf(GL_FRONT,GL_SHININESS,64.0);
	glPushMatrix();
	glTranslatef(x,y,0);
	polygonput(0,x,y,block);
	glPopMatrix();
}


/* �J�[�\���`�� */
void cursolput(GLfloat x, GLfloat y, GLint block)
{
	GLfloat v[2][4];
	v[0][0]=x/10.0;
	v[0][1]=y/10.0;
	v[0][2]=1.0;
	v[0][3]=1.0;
	v[1][0]=1.0;
	v[1][1]=1.0;
	v[1][2]=1.0;
	v[1][3]=1.0;

	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v[0]);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v[1]);
	glMaterialf(GL_FRONT,GL_SHININESS,64.0);
	glPushMatrix();
	glTranslatef(x,y,0);
	polygonput(1,x,y,block);
	glPopMatrix();
}

/* ���g�`�� */
void innerwakuput(void)
{
	/* blockput((GLfloat)j*2+1.0, (GLfloat)i, (GLfloat)stage.block[i][j]/2.0); */

	GLfloat v[4]={1.0,1.0,1.0,1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v);
	glMaterialf(GL_FRONT,GL_SHININESS,64.0);
	glPushMatrix();
	glTranslatef(10.0,4.5,0);
	auxWireBox(19.0,10.0,1.0);
	glPopMatrix();
}
	
/* �O�g�`�� */
void wakuput(void)
{
	GLfloat v[4]={1.0,1.0,1.0,1.0};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v);
	glMaterialf(GL_FRONT,GL_SHININESS,60.0);
	glPushMatrix();
	  glTranslatef(stage.left-1.0,0.0,0.0);
	  auxWireBox(1.0,30.0,1.0);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(stage.right+1.0,0.0,0.0);
	  auxWireBox(1.0,30.0,1.0);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(stage.right/2.0,stage.top,0.0);
	  auxWireBox(stage.right+3.0,1.0,1.0);
	glPopMatrix();
}

/* �p�[�c�`�� */
void partsput(void)
{
	GLint x,y,block;
	GLfloat w[2][4]={{1.0,1.0,1.0,1.0},{0.0,0.0,0.0,1.0}};
	seishaei();
	y=0;
		for(x=0;x<=20;x++){
			block = x+y*10;
			glPushMatrix();
			glTranslatef(x*2.0-1.0,43.0-y*4.0,0);
			if(block<=10){
				auxWireBox(1.8,0.8,(GLfloat)(block/2.0));
			}
			else if(block>=11){
				switch(block){
					case 11: glPushMatrix();
					           glRotated(45,0,1,0);
					           auxWireCube(1.5);
					         glPopMatrix();
					break;
					case 12: glPushMatrix();
					           auxWireTetrahedron(0.6);
					         glPopMatrix();
					break;
					case 13: glPushMatrix();
					           auxWireCone(1.0,2.0);
					         glPopMatrix();
					break;
				}
				if(block>=14 && block<=20){
					glPushMatrix();
					  glRotatef(51.4*(float)(block-13),1.0,0.0,0.0);
					  auxWireBox(1.8,0.8,0.8);
					glPopMatrix();
					glMaterialfv(GL_FRONT,GL_EMISSION, w[0]);
					glRasterPos3f(x/24.0-1.3,y/8.0,0.0);
					Drawnumber((block-13)*10);
					glMaterialfv(GL_FRONT,GL_EMISSION, w[1]);
				}
			}
			glPopMatrix();
		}
	tousihoushaei(800.0,800.0);
}

/* �`��֐� */
void CALLBACK display(void)
{
	GLint i,j,k,x,y;
	
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	x = (GLint)(ship.position.vector[_X]/2.0);
	y = (GLint)ship.position.vector[_Y];

	if(x>=0 && x<10 && y>=1 && y<11){
		cursolput((GLfloat)x*2.0+1.0, (GLfloat)y-1.0, stage.block[y-1][x]);
	}
	else{
		cursolput((GLfloat)x*2.0+1.0, (GLfloat)y-1.0, 0);
	}

	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(stage.block[i][j] <=20 && (j!=x || (i+1)!=y)){
				blockput((GLfloat)j*2.0+1.0, (GLfloat)i, stage.block[i][j]);
			}
		}
	}

	/* innerwakuput(); */
	wakuput();

	textsput();

	partsput();

	glFlush();
	auxSwapBuffers();
	
	
	
}
/* �`�揈�� */


/* ���T�C�Y������ */
void CALLBACK Reshape(int w, int h)
{
	static GLfloat v[4]={1.0,-1.0,2.0,0.0}; /* �����̈ʒu�x�N�g�� */
	auxInitPosition(0,0,800,800);
	
	tousihoushaei(800.0,800.0);
    
	
    glLightfv(GL_LIGHT0,GL_POSITION,v);      /* �����̈ʒu�x�N�g�� */
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	display();
}

/* �}�E�X���{�^���_�E�� */
void CALLBACK mouse_button(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLint nx, ny, mx, my;
		GLfloat x, y;
		auxGetMouseLoc(&mx, &my);
		if (my < 70) {
			mouse.parts = mx / 40 + 1;
		}
		else if (mx >= 680 && mx <= 780 && my >= 128 && my <= 223) {
			mouse.command = (int)(my - 128) / 35;
			switch (mouse.command)
			{
				/*
				case 0 : inputfilename();
				break;
				*/
			case 1: loadstage();
				break;
			case 2: savestage();
			}
		}
		else {
			mouse.Down[_LEFT] = 1;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouse.Down[_LEFT] = 0;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		mouse.Down[_RIGHT] = 1;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		mouse.Down[_RIGHT] = 0;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		mouse.Down[_MIDDLE] = 1;
	}
	else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		mouse.Down[_MIDDLE] = 0;
	}
}

///* �}�E�X���{�^���A�b�v */
//void CALLBACK mouse_left_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_LEFT]=0;
//}

///* �}�E�X�E�{�^���_�E�� */
//void CALLBACK mouse_right_down(AUX_EVENTREC *event)
//{
//	mouse.Down[_RIGHT]=1;
//}

///* �}�E�X�E�{�^���A�b�v */
//void CALLBACK mouse_right_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_RIGHT]=0;
//}

///* �}�E�X���{�^���_�E�� */
//void CALLBACK mouse_middle_down(AUX_EVENTREC *event)
//{
//	mouse.Down[_MIDDLE] = 1;
//}

//void CALLBACK mouse_middle_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_MIDDLE] = 0;
//}

/* �A�C�h������ */
void CALLBACK idle_process(void)
{
	GLint x,y,nx,ny;
	GLfloat fx,fy;
	fx = (GLfloat)x;
	auxGetMouseLoc(&x,&y);
	fx = (GLfloat)(x-200.0)/15.0;
	fy = (GLfloat)(420.0-y)/15.0;
	ship.position.vector[_X] = fx;
	ship.position.vector[_Y] = fy;

	if(ship.position.vector[_X] < 0){
		ship.position.vector[_X] = 0;
	}
	if(ship.position.vector[_X] > 18.5){
		ship.position.vector[_X] = 18.5;
	}

	if(mouse.Down[_LEFT]){
		fx = ship.position.vector[_X];
		fy = ship.position.vector[_Y];
		nx = (GLint)(fx/2.0);
		ny = (GLint)fy - 1.0;
	
		if(nx>=0 && nx<10 && ny>=0 && ny<10){
			stage.block[ny][nx] = mouse.parts;
		}
	}
	else if(mouse.Down[_RIGHT]){
		fx = ship.position.vector[_X];
		fy = ship.position.vector[_Y];
		nx = (GLint)(fx/2.0);
		ny = (GLint)fy - 1.0;
		if(nx>=0 && nx<10 && ny>=0 && ny<10){
			stage.block[ny][nx] = 0;
		}
	}

	display();
}	

/* ���C���֐� */
void main(int argv,char **argc)
{
	Initstage();
	printf("Initstage OK\n");

	strcpy(stage.filename,argc[1]);

	printf("OK1\n");
	while(stage.filename[0]=='\0')
	{
		printf("Input FileName ? ");
		scanf("%s",stage.filename);
	}
	printf("OK2\n");
	window();
	printf("window OK\n");
	Initcharacter();
	printf("Initcharacter OK\n");

	light_setting();
	printf("light setting OK\n");
	//auxReshapeFunc(Reshape);

	glColor4f(1.0,1.0,1.0,1.0);
	MakeBitMapFont();

	//auxMouseFunc(AUX_LEFTBUTTON,AUX_MOUSEDOWN,mouse_left_down); 
	//auxMouseFunc(AUX_LEFTBUTTON,AUX_MOUSEUP,mouse_left_up);
	//auxMouseFunc(AUX_RIGHTBUTTON,AUX_MOUSEDOWN,mouse_right_down); 
	//auxMouseFunc(AUX_RIGHTBUTTON,AUX_MOUSEUP,mouse_right_up); 
	//auxMouseFunc(AUX_MIDDLEBUTTON,AUX_MOUSEDOWN,mouse_middle_down); 
	//auxMouseFunc(AUX_MIDDLEBUTTON,AUX_MOUSEUP,mouse_middle_up); 

	//auxIdleFunc(idle_process);
	//auxMainLoop(display);

	glutMouseFunc(mouse_button); // �}�E�X�{�^���̃R�[���o�b�N�֐���ݒ�

	glutReshapeFunc(Reshape);
	glutIdleFunc(idle_process);
	glutDisplayFunc(display);
	glutMainLoop();
}
