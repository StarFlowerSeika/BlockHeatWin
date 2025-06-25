/* VC用のためwindows.hを先にincludeする必要がある */
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

/* グローバル変数宣言 */


/* ベクトル型 */
typedef struct{
	GLfloat vector[3];	/* ベクトル[XYZ] */
	GLfloat norm;		/* ベクトルの大きさ */
} Vector;

/* マテリアル（材質）型 */
typedef struct{
	GLfloat diffuse_rgba[4];     /* 拡散反射色 */
	GLfloat specular_rgba[4];    /* 鏡面反射色 */
	GLfloat shininess_rate;      /* 輝きの度合い */
} Material;

/* 弾の型 */
typedef struct {
	Vector position;       /* 位置ベクトル */
	Vector speed;          /* 速度ベクトル */
	Vector addspeed;       /* 加速度ベクトル */
	Material material;     /* 材質 */
	GLint tamaflag;        /* 弾の有効フラグ */
	GLint hitcount;        /* ヒット数 */
	GLint point;           /* 得点 */
	GLint hitflag[10][10];
} Tama;

/* 自機（船）の型 */
typedef struct {
	Vector position;       /* 位置ベクトル */
	Vector speed;          /* 速度ベクトル */
	Vector direction;      /* 向きベクトル */
	GLfloat power;         /* パワー */
	Vector mobility;       /* 機動力 */
	GLint life;            /* 残機 */
	GLfloat hitwidth, hitheight, hitdepth; /* 当たり判定サイズ */
	Material material;     /* 材質 */
} Ship;

/* ステージの型 */
typedef struct {
	GLint block[10][10];
	GLint blocks;
	GLfloat left, right, top, bottom;
	GLint gravity_flag;    /* -1:下向き 0:なし 1:上向き */
	GLfloat blockroll[10][10];           /* ブロック回転 */
	GLfloat blockrollspeed[20];          /* ブロック回転速度 */
	GLint blockpoint[20];                /* ブロック得点 */
	char filename[16];                   /* ファイル名 */
	GLint enemy;
} Stage;

/* ボタン入力状態 */
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
/* グローバル変数定義 */


/* 関数宣言 */
/* キャラクター初期化 */
void Initcharacter(void)
{
	int i;
	/* 自機の初期化 */
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

	/* 弾の初期化 */
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
	/* テキスト用マテリアルの初期化 */
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

/* ここに新しい関数や変数を追加してください */

/* 編集用関数群 */

/* ステージ編集用関数群 */

/* ウィンドウ初期化 */
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

/* ステージ初期化 */
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
	/* ファイル名初期化 */
	sprintf(stage.filename,"UnTitled");
	/* 重力フラグ初期化 */
	stage.gravity_flag = 0;
	

}

/* ライト設定 */
void light_setting(void)
{
	GLfloat v[4] = { 1.0,1.0,1.0,1.0 }; /* RGBA 色成分 */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);    /* 拡散光の設定 */
	glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);  /* ライティングとライト0の有効化 */
	glEnable(GL_NORMALIZE);   /* 法線ベクトルの正規化を有効化 */
	glEnable(GL_DEPTH_TEST);  /* デプステストの有効化 */
}

/* ここに新しい関数や変数を追加してください */

/* ステージ編集用関数群 */


/* ステージ保存 */
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

/* ステージ読込 */
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

/* 透視投影行列設定 */
void tousihoushaei(GLfloat w, GLfloat h){
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	gluPerspective(120,(GLfloat)w/(GLfloat)h,1.0,40.0);
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	gluLookAt(9.5,-1.0,10.0, 9.5,0.0,0.0, 0.0,1.0,0.0);
}

/* 正射影行列設定 */
void seishaei(void)
{
	glViewport(0, 0, 800, 800);          /* ビューポートの設定 */
	glMatrixMode(GL_PROJECTION); glLoadIdentity();   /* 射影行列の初期化 */
	glOrtho(0.0, 40.0, 0.0, 40.0, 0.0, 40.0);     /* 正射影行列の設定 */
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();   /* モデルビュー行列の初期化 */
	gluLookAt(0.0, -5.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

/* 描画処理 */
/* 数字描画 */
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

/* コマンド描画 */
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

/* テキスト描画 */
void textsput(void)
{
	GLint i,j,x,y;
	GLfloat px,py,fi;
	char str[50];

	GLfloat v[][4] = {{1.0,1.0,1.0,1.0},{0.0,0.0,0.0,1.0}};
	glMaterialfv(GL_FRONT,GL_EMISSION, v[0]);
	/* ﾀｵｼﾍｱﾆ､ﾇﾉｽｼｨ */
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

	/* ﾆｩｻ・｡ｼﾍｱﾆ､ﾇﾉｽｼｨ */
	tousihoushaei(800.0,800.0);

	glMaterialfv(GL_FRONT,GL_EMISSION, v[1]);
}

/* ポリゴン描画 */
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

/* ブロック描画 */
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


/* カーソル描画 */
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

/* 内枠描画 */
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
	
/* 外枠描画 */
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

/* パーツ描画 */
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

/* 描画関数 */
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
/* 描画処理 */


/* リサイズ時処理 */
void CALLBACK Reshape(int w, int h)
{
	static GLfloat v[4]={1.0,-1.0,2.0,0.0}; /* 光源の位置ベクトル */
	auxInitPosition(0,0,800,800);
	
	tousihoushaei(800.0,800.0);
    
	
    glLightfv(GL_LIGHT0,GL_POSITION,v);      /* 光源の位置ベクトル */
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	display();
}

/* マウス左ボタンダウン */
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

///* マウス左ボタンアップ */
//void CALLBACK mouse_left_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_LEFT]=0;
//}

///* マウス右ボタンダウン */
//void CALLBACK mouse_right_down(AUX_EVENTREC *event)
//{
//	mouse.Down[_RIGHT]=1;
//}

///* マウス右ボタンアップ */
//void CALLBACK mouse_right_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_RIGHT]=0;
//}

///* マウス中ボタンダウン */
//void CALLBACK mouse_middle_down(AUX_EVENTREC *event)
//{
//	mouse.Down[_MIDDLE] = 1;
//}

//void CALLBACK mouse_middle_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_MIDDLE] = 0;
//}

/* アイドル処理 */
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

/* メイン関数 */
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

	glutMouseFunc(mouse_button); // マウスボタンのコールバック関数を設定

	glutReshapeFunc(Reshape);
	glutIdleFunc(idle_process);
	glutDisplayFunc(display);
	glutMainLoop();
}
