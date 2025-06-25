/* VC用のためwindows.hを先にincludeする必要がある */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <time.h>
#include <math.h>
#include "text_GL.h"

#define  FPS 30

#define _X 0
#define _Y 1
#define _Z 2

#define _LEFT 0
#define _MIDDLE 1
#define _RIGHT 2

/* グローバル変数宣言 */
/* ベクトル型 */
typedef struct{
	GLfloat vector[3]; /* ベクトル[XYZ] */
	GLfloat norm;      /* ベクトルの大きさ */
} Vector;

/* マテリアル（材質）型 */
typedef struct {
	GLfloat diffuse_rgba[4];     /* 拡散反射色 */
	GLfloat specular_rgba[4];    /* 鏡面反射色 */
	GLfloat shininess_rate;      /* 輝きの度合い */
} Material;


/* 自機の型 */
typedef struct {
	Vector position;       /* 位置ベクトル */
	Vector speed;          /* 速度ベクトル */
	Vector direction;      /* 向きベクトル */
	GLfloat power;         /* パワー */
	Vector mobility;       /* 機動力 */
	GLint life;            /* 残機 */
	GLint balls;           /* ボール数 */
	GLfloat hitwidth, hitwidth0[2], hitheight, hitdepth; /* 当たり判定サイズ */
	Material material;     /* 材質 */
	int score;             /* スコア */
	int highscore;         /* ハイスコア */
	GLint deadcount;       /* 死亡回数 */
	GLint newrecoad;       /* 新記録フラグ */
	GLint gamemode;        /* ゲームモード */
} Ship;

/* ボールの型 */
typedef struct {
	Vector position;       /* 位置ベクトル */
	Vector speed;          /* 速度ベクトル */
	Vector addspeed;       /* 加速度ベクトル */
	Material material;     /* 材質 */
	GLint tamaflag;        /* 弾の有効フラグ */
	GLint hitcount;        /* ヒット数 */
	GLint point;           /* 得点 */
	GLint hitflag[10][10];
	GLint time;            /* 経過時間 */
} Tama;


/* ステージの型 */
typedef struct {
	GLint block[10][10];
	GLint blocks;
	GLfloat left, right, top, bottom;
	GLint gravity_flag;    /* -1:下向き 0:なし 1:上向き */
	GLfloat blockroll[10][10];           /* ブロック回転 */
	GLfloat blockrollspeed[20];          /* ブロック回転速度 */
	GLint blockpoint[20];                /* ブロック得点 */
	GLint enemy;
	char filename[16][16];   /* ファイル名リスト */
	char filenamedata[16];   /* ステージファイル名 */
	GLint now;               /* 現在のステージ番号 */
	GLint last;              /* 最終ステージ番号 */
	GLint demotime;          /* デモ時間 */
	GLint display;
	GLfloat vmax[2];         /* 最大速度 */
	GLint debugflag;
	GLfloat demoview[9];
	GLfloat gameview[9];
} Stage;

/* ボタン入力状態 */
typedef struct {
	GLint Down[3];
	GLint Up[3];
	GLint mouseX;
	GLint mouseY;
} Button; 

Ship ship;
Tama tama[10],shot[10],hit[10],collision[10];
Stage stage;
Button mouse;
Material textmaterial;

static GLint display_num;


/* グローバル変数宣言ここまで */


/* 関数宣言 */
/* キャラクター初期化 */
void Initcharacter(void)
{
	int i,j;
	/* 自機の初期化 */
	for(i=0; i<3; i++){
		ship.position.vector[i] = 0.0f;
		ship.speed.vector[i] = 0.0f;
		ship.direction.vector[i] = 0.0f;
   	}
	ship.position.vector[_X] = 10.0f;
	ship.position.vector[_Y] = -10.0f;
	ship.position.vector[_Z] = 0.0f; 
	ship.position.norm = 0.0f;
	ship.speed.norm = 0.0f;
	ship.power = 0.1f;
	ship.mobility.vector[0] = 0.1f;
	ship.mobility.vector[1] = 0.1f;
	ship.mobility.vector[2] = 0.1f;
	ship.life = 10;
	ship.balls = 10;
	ship.hitwidth0[0] = 5.0f;
	ship.hitwidth0[1] = 1.0f;
	ship.hitheight = 1.0f;
	ship.hitdepth = 0.5f;
	ship.material.diffuse_rgba[0] = 0.7f;
	ship.material.diffuse_rgba[1] = 1.0f;
	ship.material.diffuse_rgba[2] = 0.7f;
	ship.material.diffuse_rgba[3] = 1.0f;
	ship.material.specular_rgba[0] = 1.0f;
	ship.material.specular_rgba[1] = 1.0f;
	ship.material.specular_rgba[2] = 1.0f;
	ship.material.specular_rgba[3] = 1.0f;
	ship.material.shininess_rate = 80.0f;

	stage.vmax[0] = 0.9f;
	stage.vmax[1] = 0.5f;


	/* 弾の初期化 */
	for(i=0; i<10; i++){
		tama[i].material.diffuse_rgba[0] = 0.1f*i;
		tama[i].material.diffuse_rgba[1] = (GLfloat)sinf(0.314f*i);
		tama[i].material.diffuse_rgba[2] = (10-i)*0.1f;
		tama[i].material.diffuse_rgba[3] = 1.0f;
		tama[i].material.specular_rgba[0] = 1.0f;
		tama[i].material.specular_rgba[1] = 1.0f;
		tama[i].material.specular_rgba[2] = 1.0f;
		tama[i].material.specular_rgba[3] = 1.0f;
		tama[i].material.shininess_rate = 40.0f;
		tama[i].tamaflag = 0;
		hit[i].tamaflag=0;
		collision[i].tamaflag=0;
		for(j=0;j<10;j++){tama[i].hitflag[i][j] = 0;}
	}

	/* テキスト用マテリアルの初期化 */
	textmaterial.diffuse_rgba[0] = 1.0f;
	textmaterial.diffuse_rgba[1] = 1.0f;
	textmaterial.diffuse_rgba[2] = 1.0f;
	textmaterial.diffuse_rgba[3] = 1.0f;
	textmaterial.specular_rgba[0] = 1.0f;
	textmaterial.specular_rgba[1] = 1.0f;
	textmaterial.specular_rgba[2] = 1.0f;
	textmaterial.specular_rgba[3] = 1.0f;
	textmaterial.shininess_rate = 90.0f;
	stage.gameview[0] = 9.5f;
	stage.gameview[1] = 5.0f;
	stage.gameview[2] = 10.0f;
	stage.gameview[3] = 9.5f;
	stage.gameview[4] = 0.0f;
	stage.gameview[5] = 0.0f;
	stage.gameview[6] = 0.0f;
	stage.gameview[7] = 1.0f;
	stage.gameview[8] = 0.0f;
	for(i=0;i<9;i++){
		stage.demoview[i] = stage.gameview[i];
	}
	stage.demoview[2] = 30.0f;
	stage.demotime = 200;
}

/* グローバル変数初期化ここまで */

/* 関数定義ここから */

/* ウィンドウ初期化 */
void window(void)
{
	//auxInitDisplayMode(AUX_DOUBLE | AUX_RGBA | AUX_DEPTH);
	//auxInitPosition(0,0,800,800);
	//auxInitWindow("Block");
	// freeglut
	int argc = 1;
	char* argv[1] = { (char*)"Block" };
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Block");
}


/* ファイル初期化 */
void Initfile(void)
{
	FILE *fp;
	GLint i;
	errno_t err;
	if((err = fopen_s(&fp, "block_stage_filename.dat","r")) != 0){
		printf("Can't open block_stage_filename.dat\n");
		exit(0);
	}

	if (fp == NULL) {
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		exit(1);
	}

	fscanf_s(fp,"%d",&stage.last);

	for(i=0;i<stage.last;i++){
		//fscanf_s(fp,"%s",&stage.filename[i]);
        for (i = 0; i < stage.last; i++) {  
           fscanf_s(fp, "%15s", stage.filename[i], (unsigned)_countof(stage.filename[i]));  
        }
	}
	fclose(fp);
	
	if((err = fopen_s(&fp, "block_rollspeed.dat", "r")) != 0) {
		printf("Can't open block_rollspeed.dat\n");
		exit(0);
	}

	if (fp == NULL) {
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		exit(1);
	}

	for(i=0;i<=20;i++){
		fscanf_s(fp,"%f",&stage.blockrollspeed[i]);
	}
	fclose(fp);

}

/* ハイスコア読込 */
void loadhighscore(void)
{
	FILE *fp;
	errno_t err;
	char filename[2][40]={"block_normal_highscore.dat","block_tamayoke_highscore.dat"};
	if((err = fopen_s(&fp, filename[ship.gamemode],"r")) != 0){
		printf("Can't open block_normal_highscore.dat\n");
		exit(0);
	}
	if (fp == NULL) {
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		exit(1);
	}
	fscanf_s(fp,"%d",&ship.highscore);

	fclose(fp);
}

/* ハイスコア保存 */
void savehighscore(void)
{
	FILE *fp;
	char filename[2][40]={"block_normal_highscore.dat","block_tamayoke_highscore.dat"};
	errno_t err;
	if((err = fopen_s(&fp, filename[ship.gamemode],"w")) != 0){
		printf("Can't open block_normal_highscore.dat\n");
		exit(0);
	}
	if (fp == NULL) {
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		exit(1);
	}
	fprintf_s(fp,"%d",ship.highscore);
	fclose(fp);
}

/* ステージ初期化 */
void Initstage(void)
{
	int i,j;
	FILE *fp;
	GLint block;
	errno_t err;

	stage.blocks = 0;
	stage.left = 0.0f;
	stage.right = 20.0f;
	stage.top = 15.0f;
	stage.bottom = -10.0f;
	stage.gravity_flag = 0;
	stage.enemy = 0;
	
	if((err = fopen_s(&fp, stage.filename[stage.now],"r")) != 0){
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		return;
	}
	
	if (fp == NULL) {
		printf("Can't open stage file %s\n", stage.filename[stage.now]);
		exit(1);
	}

	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			fscanf_s(fp,"%d\n",&stage.block[i][j]);
			block=stage.block[i][j];
			stage.blockroll[i][j] = 0;
			if(block>=1 && block<=10 || block==12 || block ==13){
				stage.blocks++;
			}
		}
	}

	
	fscanf_s(fp,"%d\n",&stage.gravity_flag);
	
	fscanf_s(fp,"%d\n",&stage.enemy);
	
	fclose(fp);
	
}

/* ライト設定 */
void light_setting(void)
{
	GLfloat v[4] = { 1.0,1.0,1.0,1.0 }; /* RGBA光源色 */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, v);    /* 拡散光設定 */
	/* glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE); */ /* ローカルビューア有効化（未使用） */
	glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);  /* ライティング有効化 */
	glEnable(GL_NORMALIZE);   /* 法線ベクトル正規化 */
	glEnable(GL_DEPTH_TEST);  /* デプステスト有効化 */
}

/* ここまで関数定義 */

/* ｲ靂ﾌﾉｽｼｨｴﾘｿｲ */

// 幅 w, 高さ h, 奥行 d の直方体を描画
void glutSolidBox(GLfloat w, GLfloat h, GLfloat d)
{
	glPushMatrix();
	glScalef(w, h, d);
	glutSolidCube(1.0); // 1.0サイズの立方体をスケーリング
	glPopMatrix();
}
void glutWireBox(GLfloat w, GLfloat h, GLfloat d)
{
	glPushMatrix();
	glScalef(w, h, d);
	glutWireCube(1.0); // 1.0サイズの立方体をスケーリング
	glPopMatrix();
}

/* 数値を文字列として描画 */
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
// 修正: sprintf_s のエラーを修正するために、適切な型を使用します。  
// ship.score が int 型であることを確認し、string の型とサイズを適切に設定します。  

void textsput(void)  
{  
   char string[50]; // string のサイズを十分に確保  
   GLint i;  
   GLfloat px, py, fi;  
   GLfloat v[][4] = {{1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};  
   glMaterialfv(GL_FRONT, GL_EMISSION, v[0]);  

   glRasterPos3d(0, 0, 0);  
   Drawnumber(ship.balls);  

   // スコア表示  
   sprintf_s(string, sizeof(string), "Score %7d", ship.score);  
   glRasterPos3f(-40.0f, 50.0f, 1.0f);  
   DrawString(string);  

   // ハイスコア表示  
   sprintf_s(string, sizeof(string), "High Score %7d", ship.highscore);  
   glRasterPos3f(-45.0f, 60.0f, 1.0f);  
   DrawString(string);  

   // ライフ表示  
   sprintf_s(string, sizeof(string), "Life %2d", ship.life);  
   glRasterPos3f(0, 50.0f, 1.0f);  
   DrawString(string);  

   px = -40.0f;  
   py = 45.0f;  
   fi = 0;  
   if (stage.demotime == 0)  
   {  
       for (i = 0; i < 10; i++)  
       {  
           if (hit[i].tamaflag == 1)  
           {  
               sprintf_s(string, sizeof(string), "%6d HIT", tama[i].hitcount);  
               glRasterPos3f(hit[i].position.vector[_X] - 2.0f, hit[i].position.vector[_Y] - 0.5f, 1.0f);  
               DrawString(string);  
           }  
           else if (hit[i].tamaflag == 2 && tama[i].point)  
           {  
               fi += 0.5f;  
               sprintf_s(string, sizeof(string), "%6d Pts.", tama[i].point);  
               glRasterPos3f(ship.position.vector[_X] - 1.0f, ship.position.vector[_Y] + 0.75f + fi, 1.0f);  
               DrawString(string);  
           }  
           if (collision[i].tamaflag == 1)  
           {  
               sprintf_s(string, sizeof(string), "Collision! %6d Pts.", tama[i].point);  
               glRasterPos3f(px, py, 1.0f);  
               DrawString(string);  
               px += 3.0f;  
               py -= 3.7f;  
           }  
       }  

       sprintf_s(string, sizeof(string), "Block %4d", stage.blocks);  
       glRasterPos3f(20.0f, 30.0f, 1.0f);
       DrawString(string);  

       sprintf_s(string, sizeof(string), "Dead Count %d", ship.deadcount);  
       glRasterPos3f(20.0f, 34.0f, 1.0f);  
       DrawString(string);  
   }  

   if (stage.blocks == 0 && stage.now != stage.last)  
   {  
       sprintf_s(string, sizeof(string), "\"%s\" Crear!", stage.filename[stage.now]);  
       glRasterPos3f(5.0f, 0.0f, 1.0f);  
       DrawString(string);  
   }  
   else if (stage.blocks == 0 && stage.now == stage.last)  
   {  
       if (ship.newrecoad && ship.deadcount)  
       {  
           savehighscore();  
       }  
       exit(0);  
   }  

   glMaterialfv(GL_FRONT, GL_EMISSION, v[1]);  
}


/* ブロック描画 */
void blockput(GLfloat x, GLfloat y, GLint block, GLfloat roll)
{
	GLfloat v[2][4],w[2][4]={{1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,1.0f}};
	v[0][0]=x/10.0f;
	v[0][1]=y/10.0f;
	v[0][2]=1.0f;
	v[0][3]=1.0f;
	v[1][0]=1.0f;
	v[1][1]=1.0f;
	v[1][2]=1.0f;
	v[1][3]=1.0f;

	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v[0]);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v[1]);
	glMaterialf(GL_FRONT,GL_SHININESS,64.0f);
	glPushMatrix();
	glTranslatef(x,y,0);
	
    if(block<=10){
		  //auxSolidBox(1.8,0.8,(GLfloat)(block/2.0));
		  glutSolidBox(1.8f, 0.8f, (GLfloat)(block / 2.0f));
	}
	else if(block>=11){
		switch(block){
			case 11: glPushMatrix();
			           glRotatef(roll,0.0f,1.0f,0.0f);
					   //auxSolidCube(1.3);
					   glutSolidCube(1.3f);
			         glPopMatrix();
			         break;
			case 12: glPushMatrix();
			           glRotatef(roll,0.0f,0.0f,1.0f);
					   //auxSolidTetrahedron(1.0);
					   glutSolidTetrahedron();
					   
			         glPopMatrix();
			         break;
			case 13: //auxSolidCone(1.0,2.0);
					 glutSolidCone(1.0f,2.0f, 16, 16);
			         break;
		}
		if(block>=14 && block<=20){
			glPushMatrix();
			  glRotatef(51.4f*(float)(block-13)+roll,1.0f,0.0f,0.0f);
			  //auxWireBox(1.8, 0.8, 0.8);
			  glutWireBox(1.8f,0.8f,0.8f);
			glPopMatrix();
			if(stage.demotime==0){
				glMaterialfv(GL_FRONT,GL_EMISSION, w[0]);
				glRasterPos3f(x/8.0f-2.0f,y/8.0f,-1.0f);
				Drawnumber((block-13)*10);
				glMaterialfv(GL_FRONT,GL_EMISSION, w[1]);
			}
		}
	}
	glPopMatrix();
}

/* パドル（自機）描画 */
void paddleput(void)
{
	GLfloat v[4]={1.0f,1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,ship.material.diffuse_rgba);
	glMaterialfv(GL_FRONT,GL_SPECULAR,ship.material.specular_rgba);
	glMaterialf(GL_FRONT,GL_SHININESS,ship.material.shininess_rate);
	if(ship.gamemode==0){
		glPushMatrix();
		glTranslatef(ship.position.vector[_X],ship.position.vector[_Y],ship.position.vector[_Z]);
		//auxSolidBox(ship.hitwidth, ship.hitheight, ship.hitdepth);
		glutSolidBox(ship.hitwidth,ship.hitheight,ship.hitdepth);
		glPopMatrix();
	}
	else if(ship.gamemode==1){
		glPushMatrix();
		glTranslatef(ship.position.vector[_X],ship.position.vector[_Y],ship.position.vector[_Z]);
		//auxSolidBox(ship.hitwidth, ship.hitheight, ship.hitdepth);
		glutSolidBox(ship.hitwidth,ship.hitheight,ship.hitdepth);
		glPopMatrix();

		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v);
		glMaterialfv(GL_FRONT,GL_SPECULAR,v);
		glMaterialf(GL_FRONT,GL_SHININESS,0.0f);
		glPushMatrix();
		glTranslatef(stage.right/2.0f,stage.bottom-1.5f,0.0f);
		//auxSolidBox(stage.right + 3.0, 1.0, 0.5);
		glutSolidBox(stage.right+3.0f,1.0f,0.5f);
		glPopMatrix();


	}

}

/* 枠（壁）描画 */
void wakuput(void)
{
	GLfloat v[4]={1.0f,1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,v);
	glMaterialfv(GL_FRONT,GL_SPECULAR,v);
	glMaterialf(GL_FRONT,GL_SHININESS,60.0f);
	glPushMatrix();
	  glTranslatef(stage.left-1.0f,0.0f,0.0f);
	  //auxSolidBox(1.0, 30.0, 1.0);
	  glutSolidBox(1.0f,30.0f,1.0f);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(stage.right+1.0f,0.0f,0.0f);
	  //auxSolidBox(1.0, 30.0, 1.0);
	  glutSolidBox(1.0f,30.0f,1.0f);
	glPopMatrix();
	glPushMatrix();
	  glTranslatef(stage.right/2.0f,stage.top,0.0f);
	  //auxSolidBox(stage.right + 3.0, 1.0, 1.0);
	  glutSolidBox(stage.right+3.0f,1.0f,1.0f);
	glPopMatrix();
}

/* ボール描画 */
void ballput(void)
{
	GLint i;
	for(i=0;i<10;i++){
		if(tama[i].tamaflag == 1){
			glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,tama[i].material.diffuse_rgba);
			glMaterialfv(GL_FRONT,GL_SPECULAR,tama[i].material.specular_rgba);
			glMaterialf(GL_FRONT,GL_SHININESS,tama[i].material.shininess_rate);
			glPushMatrix();
			glTranslatef(tama[i].position.vector[_X],tama[i].position.vector[_Y],tama[i].position.vector[_Z]);
			//auxSolidIcosahedron(0.5);
			glutSolidIcosahedron();
			glPopMatrix();
		}
	}
}

/* メイン描画処理 */
void CALLBACK display(void)
{
	GLint i,j;

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(stage.block[i][j] > 0){
				stage.blockroll[i][j] += stage.blockrollspeed[stage.block[i][j]];
				if(stage.blockroll[i][j] > 360.0f){
					stage.blockroll[i][j] -= 360.0f;
				}
				else if(stage.blockroll[i][j] < -360.0f){
					stage.blockroll[i][j] += 360.0f;
				}
				blockput((GLfloat)j*2+1.0f, (GLfloat)i, stage.block[i][j], stage.blockroll[i][j]);
			}
		}
	}

	if(ship.life > -10){
		paddleput();
	}
	ballput();

	wakuput();
  
	textsput();
   
#if 1
  {
    float tt = (float)timeGetTime();
    float ts = tt / (1000.0f/(float)FPS);
    int tn = (int)((int)(ts+1) * (1000.0f/(float)FPS));
    if(tn > (int)tt){
      Sleep(tn-(int)tt);
    }
  }
#endif
	glFlush();

	//auxSwapBuffers();
	glutSwapBuffers();

}
/* メイン描画処理ここまで */


/* ウィンドウリサイズ時の処理 */
void CALLBACK Reshape(int w, int h)
{
	static GLfloat v[4]={1.0f,-1.0f,2.0f,0.0f}; /* ｸｻ､ﾎｸ､ｭ */
	//auxInitPosition(0, 0, 800, 800);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800,800);
 
	

	if(stage.demotime==0){
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		gluPerspective(120,(GLfloat)w/(GLfloat)h,1.0,40.0);
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		gluLookAt(9.5f,-5.0f,10.0f, 9.5f,0.0f,0.0f, 0.0f,1.0f,0.0f);
	}
	else if(stage.blocks>0 && stage.demotime>0){
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		gluPerspective(120,1.0f,1.0f,(GLfloat)(stage.demotime)+50.0f);
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		gluLookAt(9.5f,-5.0f,10.0f+(GLfloat)(stage.demotime), 9.5f,0.0f,0.0f, 0.0f,1.0f,0.0f);
	}
	glLightfv(GL_LIGHT0,GL_POSITION,v);      /* ﾊｿｹﾔｸ */
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	display();
}

/* テキスト移動用の初期化 */
void init_movetext(Tama moji[],GLint i)
{
	moji[i].tamaflag = 1;
	moji[i].position.vector[_X] = tama[i].position.vector[_X];
	moji[i].position.vector[_Y] = tama[i].position.vector[_Y];
	moji[i].position.vector[_Z] = 0.0f;
	moji[i].speed.vector[_X] = 0.0f;
	moji[i].speed.vector[_Y] = 0.01f;
	moji[i].speed.vector[_Z] = 0.0f;
}


/* ボール発射処理 */
int ball_shot(GLfloat vector1[3],GLfloat vector2[3]){
	GLint i,j,flag=0;
	for(i=0;i<10;i++){
		if(tama[i].tamaflag == 0){
			for(j=0;j<10;j++){
				tama[i].hitflag[i][j] = 1;
				tama[i].hitflag[j][i] = 1;
			}
			tama[i].time=0;
			tama[i].tamaflag = 1;
			tama[i].position.vector[_X] = vector1[_X];
			tama[i].position.vector[_Y] = vector1[_Y];
			tama[i].position.vector[_Z] = vector1[_Z];
			tama[i].speed.vector[_X] = vector2[_X];
			tama[i].speed.vector[_Y] = vector2[_Y];
			tama[i].speed.vector[_Z] = 0.0;
			i = 10;
			flag=1;
		}
	}
	return flag;
}

//マウス移動時の処理
void mouse_move(int x, int y) {
	mouse.mouseX = x;
	mouse.mouseY = y;
}

/* マウス左ボタン押下時の処理 */
//void  CALLBACK mouse_left_down(AUX_EVENTREC *event)
void CALLBACK mouse_button(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mouse.Down[_LEFT] = 1;
		mouse.Up[_LEFT] = 0;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mouse.Down[_LEFT] = 0;
		mouse.Up[_LEFT] = 1;
		return; //マウス左ボタンを離したときはここで早期リターン
	}

	GLint i,j,no=0;
	GLfloat rr,dx,dy;
	GLfloat vector[3],position[3];
	if(stage.blocks && stage.demotime==0){
		for(j=0;j<10;j++){
			if(tama[j].tamaflag){
				dx = ship.position.vector[_X] - tama[j].position.vector[_X];
				dy = ship.position.vector[_Y]+1.0f - tama[j].position.vector[_Y];
				rr = dx*dx+dy*dy;
				if(rr<=1.0f){
					no = 1;
				}
			}
		}
		vector[_X] = stage.vmax[ship.gamemode];
		vector[_Y] = stage.vmax[ship.gamemode];
		vector[_Z] = 0.0f;


		if(ship.balls>0 && no == 0){
			for(i=_X;i<=_Z;i++){
				position[i]=ship.position.vector[i];
			}
			position[_Y] += 1.0f;
			ship.balls --;
			ball_shot(position,vector);
		}
	}
	else if(stage.blocks && stage.demotime>0){
		mouse.Down[_LEFT] = 1;
		Reshape(1, 1);
	}
	else if(stage.blocks==0 && stage.now!=stage.last){
		mouse.Down[_LEFT] = 1;
		/*
		stage.now++;
		Initcharacter();
		Initstage();
		*/
	}
	else if(stage.blocks==0 && stage.now==stage.last){
		exit(0);
	}
}

///* マウス左ボタン離した時の処理 */
//void CALLBACK mouse_left_up(AUX_EVENTREC *event)
//{
//	mouse.Down[_LEFT]=0;
//}

///* マウス中ボタン押下時の処理（ゲームモード切替） */
//void CALLBACK mouse_middle_down(AUX_EVENTREC *event)
//{
//	if(ship.gamemode == 0){
//	    ship.hitwidth = ship.hitwidth0[1];
//	}
//	else{
//		ship.hitwidth = ship.hitwidth0[0];
//	}
//	ship.gamemode = !ship.gamemode;
//}

/* ブロックを消す */
void block_delete(GLint i, GLint x, GLint y)
{
	stage.blocks --;
	stage.block[y][x] = 0;
}
/* ヒットしたブロックの得点計算 */
void hit_score_make(GLint i,GLint x, GLint y,GLint pos)
{
	tama[i].hitcount ++;
	tama[i].point = (tama[i].hitcount+i) * pos;
	ship.score += tama[i].point;
	hit[i].tamaflag = 0;
	init_movetext(hit,i);
}

/* ブロックにヒットした時の処理 */
void block_hit(GLfloat fnx, GLfloat fny, GLfloat fbx, GLfloat fby, GLint i)
{
	GLint blo,blox,bloy,j,k,nx,ny,bx,by,hitflag=0;
	GLint bloM[3][3];
	GLint Mx, My;// , Mxs, Mxe, Mys, Mye;
	GLint flag[]={0,0,0,0,0,0,0,0,0};
	static GLfloat vector[3];
	GLfloat v[3];
	nx = (GLint)(fnx/2.0f);
	ny = (GLint)fny;
	bx = (GLint)(fbx/2.0f);
	by = (GLint)fby;
	stage.debugflag=0;
	if(nx>=0 && nx<10 && ny>=0 && ny<10){

		v[_X]=tama[i].speed.vector[_X];
		v[_Y]=tama[i].speed.vector[_Y];
		v[_Z]=tama[i].speed.vector[_Z];

		if(hit[i].tamaflag == 2){
			tama[i].hitcount = 0;
		}
		
		blo = stage.block[ny][nx];
		if(by != ny && bx != nx && blo>0){
			for(Mx=nx-1;Mx<=nx+1;Mx++){
				for(My=ny-1;My<=ny+1;My++){
					if(Mx>=0 && Mx<10 && My>=0 && My<10){
						bloM[My-ny+1][Mx-nx+1] = stage.block[My][Mx];
					}
					else if(Mx<0 || Mx>=10){
						bloM[My-ny+1][Mx-nx+1] = 11;
					}
					else if(My<0 || My>=10){
						bloM[My-ny+1][Mx-nx+1] = 0;
					}
				}
			}
			
			if(blo<=11 || blo>=14 && blo<=20){
				
				/* ブロックの周囲のブロックを調べる */
				/*
				flag[0] = flag[0] || (bloM[1][0]>0 && bloM[1][2]>0 && bloM[0][1]>0 && bloM[2][1]>0);				
				*/
				flag[1] = ((bloM[0][1]>0 && bloM[1][2]>0) && bx>nx && by<ny);
				flag[1] = flag[1] || ((bloM[2][1]>0 && bloM[1][0]>0) && bx<nx && by>ny);
				flag[1] = flag[1] || ((bloM[2][1]>0 && bloM[1][2]>0) && bx>nx && by>ny);
				flag[1] = flag[1] || ((bloM[0][1]>0 && bloM[1][0]>0) && bx<nx && by<ny);
			    flag[2] = bloM[0][1]==0 && bloM[1][2]==0 && bx>nx && by<ny;
				flag[2] = flag[2] || (bloM[2][1]==0 && bloM[1][0]==0 && bx<nx && by>ny);
				flag[3] = bloM[2][1]==0 && bloM[1][2]==0 && bx>nx && by>ny;
				flag[3] = flag[3] || (bloM[0][1]==0 && bloM[1][0]==0 && bx<nx && by<ny);
				flag[4] = bloM[1][0]>0 && bx<nx;
				flag[4] = flag[4] || bloM[1][2]>0 && bx>nx;
				flag[4] = flag[4] && (bloM[0][1]==0 || bloM[2][1]==0);
				flag[5] = bloM[0][1]>0 && by<ny;
				flag[5] = flag[5] || bloM[2][1]>0 && by>ny;
				flag[5] = flag[5] && (bloM[1][0]==0 || bloM[1][2]==0);


				if(flag[1]){
					tama[i].speed.vector[_X] = -v[_X];
					tama[i].position.vector[_X] = fbx;		
					tama[i].speed.vector[_Y] = -v[_Y];
					tama[i].position.vector[_Y] = fby;		
					stage.debugflag=1;
				}
				else if(flag[2]){
					tama[i].speed.vector[_Y] = v[_X];
					tama[i].position.vector[_Y] = fby;
					tama[i].speed.vector[_X] = v[_Y];
					tama[i].position.vector[_X] = fbx;
				}				
				else if(flag[3]){
					tama[i].speed.vector[_Y] = -v[_X];
					tama[i].position.vector[_Y] = fby;
					tama[i].speed.vector[_X] = -v[_Y];
					tama[i].position.vector[_X] = fbx;
				}				
				else if(flag[4]){
					tama[i].speed.vector[_Y] = -v[_Y];
					tama[i].position.vector[_Y] = fby;		
				}
				else if(flag[5]){
					tama[i].speed.vector[_X] = -v[_X];
					tama[i].position.vector[_X] = fbx;		
				}
				
				if(blo<=10){
					hitflag=1;
					stage.block[ny][nx] -= (i/2+1);
					if(stage.block[ny][nx]<=0){
						block_delete(i,nx,ny);
					}
					hit_score_make(i,nx,ny,10);
				}
				else if(blo>=14 && blo<=20){
					hit_score_make(i,nx,ny,(blo-13)*10);
					stage.block[ny][nx] ++;
					if(stage.block[ny][nx] == 21){stage.block[ny][nx] = 17;}	
				}
			
			}		
			else {
				switch(blo){
					case 12 :
					if(ship.life>0){
						ship.life += ball_shot(tama[i].position.vector,tama[i].speed.vector);
						
						if(ship.life>(10+ship.balls)){
							ship.life = 10+ship.balls;
						}
					}				
					block_delete(i,nx,ny);
					hit_score_make(i,nx,ny,10);
					break;

					case 13 :
					if(ship.life>0){
						for(j=0;j<9;j++){
							vector[_X] = (GLfloat)cosf((float)j*6.28f/9.0f);
							vector[_Y] = (GLfloat)sinf((float)j*6.28f/9.0f);
							ship.life += ball_shot(tama[i].position.vector,vector);
							if(ship.life>(10+ship.balls)){
								ship.life = 10+ship.balls;
							}
						}
					}
					block_delete(i,nx,ny);
					hit_score_make(i,nx,ny,10);
					break;
				}
			}
		
		}
		else {
		
		bloy = stage.block[ny][bx];
		
			if(by != ny && bloy>0 && bx>=0 && bx<10){
				if(bloy<=11 || bloy>=14 && bloy<=20){		
					tama[i].speed.vector[_Y] = -v[_Y];
					if(bloy<=10){
						hitflag=1;
						stage.block[ny][bx] -= (i/2+1);
						if(stage.block[ny][bx]<=0){
							block_delete(i,bx,ny);
						}
						hit_score_make(i,bx,ny,10);
					}
					else if(bloy>=14 && bloy<=20){
						hit_score_make(i,bx,ny,(bloy-13)*10);
						stage.block[ny][bx] ++;
						if(stage.block[ny][bx] == 21){stage.block[ny][bx] = 17;}	
					}
					
				}		
				else {
					switch(bloy){
						case 12 :
						if(ship.life>0){
							ship.life += ball_shot(tama[i].position.vector,tama[i].speed.vector);
							
							if(ship.life>(10+ship.balls)){
								ship.life = 10+ship.balls;
							}
						}				
						block_delete(i,bx,ny);
						hit_score_make(i,bx,ny,10);
						break;
						
						case 13 :
						if(ship.life>0){
							for(j=0;j<9;j++){
								vector[_X] = (GLfloat)cosf((float)j*6.28f/9.0f);
								vector[_Y] = (GLfloat)sinf((float)j*6.28f/9.0f);
								ship.life += ball_shot(tama[i].position.vector,vector);
								
								if(ship.life>(10+ship.balls)){
									ship.life = 10+ship.balls;
								}
							}
						}
						block_delete(i,bx,ny);
						hit_score_make(i,bx,ny,10);
						break;
					}
				}
				tama[i].position.vector[_Y] = fby;
			}
			
			blox = stage.block[by][nx];	
			if(bx != nx && blox>0 && by>=0 && by<10){
				if(blox<=11 || blox>=14 && blox<=20){
					tama[i].speed.vector[_X] = -v[_X];
					tama[i].hitcount ++;
					if(blox<=10){
						hitflag=1;
						stage.block[by][nx] -= (i/2+1);
						if(stage.block[by][nx] <= 0){
							block_delete(i,nx,by);
						}
						hit_score_make(i,nx,by,10);
					}
					else if(blox>=14 && blox<=20){
						hit_score_make(i,nx,by,(blox-13)*10);
						stage.block[by][nx] ++;
						if(stage.block[by][nx] == 21){stage.block[by][nx] = 17;}
					}
				}
				else {
					switch(blox){
						case 12 :
						if(ship.life>0){
							ship.life += ball_shot(tama[i].position.vector,tama[i].speed.vector);
							
							if(ship.life>(10+ship.balls)){
								ship.life = 10+ship.balls;
							}
						}				
						block_delete(i,nx,by);
						hit_score_make(i,nx,by,10);
						
						break;
						
						case 13 :
						if(ship.life>0){
							for(j=0;j<9;j++){
								vector[_X] = (GLfloat)cosf((float)j*6.28f/9.0f);
								vector[_Y] = (GLfloat)sinf((float)j*6.28f/9.0f);
								ship.life += ball_shot(tama[i].position.vector,vector);
								
								if(ship.life>(10+ship.balls)){
									ship.life = 10+ship.balls;
								}
							}
						}
						block_delete(i,nx,by);
						hit_score_make(i,nx,by,10);
						break;
					}
				}
				tama[i].position.vector[_X] = fbx;
			}
		
	    }
		for(k=_X;k<=_Y;k++){
			if(tama[i].speed.vector[k]>stage.vmax[ship.gamemode]){
				tama[i].speed.vector[k]=stage.vmax[ship.gamemode];
			}
			else if(tama[i].speed.vector[k]<-stage.vmax[ship.gamemode]){
				tama[i].speed.vector[k]=-stage.vmax[ship.gamemode];
			}
		}
		for(k=_X;k<=_Y;k++){
			if(tama[i].speed.vector[k]>stage.vmax[ship.gamemode]){
				tama[i].speed.vector[k]=stage.vmax[ship.gamemode];
			}
			else if(tama[i].speed.vector[k]<-stage.vmax[ship.gamemode]){
				tama[i].speed.vector[k]=-stage.vmax[ship.gamemode];
			}
		}
	}
}

/* 重力処理 */
void gravity()
{
	GLint i,j;
	GLfloat g_const=0.1f;
	GLfloat rr,g,dx,dy,gx,gy;
	for(i=0;i<10;i++){
		tama[i].addspeed.vector[_X] = 0;
		tama[i].addspeed.vector[_Y] = 0;
	}
	for(i=0;i<9;i++){
		for(j=i+1;j<10;j++){
			if((i != j) && (tama[i].tamaflag == 1) && (tama[j].tamaflag == 1)){
				dx = tama[i].position.vector[_X] - tama[j].position.vector[_X];
				dy = tama[i].position.vector[_Y] - tama[j].position.vector[_Y];
				rr = dx*dx + dy*dy;
				if(rr>0.5){
					g = g_const/(rr+1.0f);
					gx = g*((dx<0)-(dx>0));
					gy = g*((dy<0)-(dy>0));
					tama[i].addspeed.vector[_X] += gx;
					tama[i].addspeed.vector[_Y] += gy;
					tama[j].addspeed.vector[_X] += -gx;
					tama[j].addspeed.vector[_Y] += -gy;
				}
			}
		}
	}
}

/* 速度ベクトルの進行方向成分を計算 */
void pass_speed(GLfloat vp[],GLfloat dx, GLfloat dy, GLfloat r, GLint index)
{
	GLfloat vx, vy, si, co, vbase;
	si = dy / r;
	co = dx / r;
	vx = tama[index].speed.vector[_X];
	vy = tama[index].speed.vector[_Y];
	vbase = vx*co + vy*si;
	vp[_X] = co * vbase;
	vp[_Y] = si * vbase;
}

/* 速度ベクトルの法線方向成分を計算 */
void leave_speed(GLfloat vl[],GLfloat dx, GLfloat dy, GLfloat r, GLint index)
{
	GLfloat vx, vy, si, co, vbase;
	si = dy / r;
	co = dx / r;
	vx = tama[index].speed.vector[_X];
	vy = tama[index].speed.vector[_Y];
	vbase = vx*si - vy*co;
	vl[_X] = si * vbase;
	vl[_Y] = -co * vbase;
}

/* ボール同士の衝突判定と反射処理 */
void ballhit(void)
{
	GLfloat r, rr, dx, dy, vp1[2], vp2[2], vl1[2], vl2[2];
	GLint i,j,k;
	static GLint hitflag[10][10];

	for(i=0;i<9;i++){
		for(j=i+1;j<10;j++){
			dx = tama[i].position.vector[_X] - tama[j].position.vector[_X];
			dy = tama[i].position.vector[_Y] - tama[j].position.vector[_Y];
			rr = dx*dx+dy*dy;
			if(rr<=1.0 && rr!=0 && (tama[i].hitflag[i][j])==0 && tama[i].tamaflag==1 && tama[j].tamaflag==1){
				init_movetext(collision,i);
				init_movetext(collision,j);
				collision[i].hitcount += 5;
				collision[j].hitcount += 5;

				tama[i].point = (collision[i].hitcount + collision[j].hitcount)*(i+1);
				ship.score += tama[i].point;

				tama[i].hitflag[i][j] = 1;
				
				r = sqrtf(rr);
				pass_speed(vp1, dx, dy, r, i);
				pass_speed(vp2, dx, dy, r, j);

				leave_speed(vl1, dx, dy, r, i);
				leave_speed(vl2, dx, dy, r, j);
				
				tama[i].speed.vector[_X] = vl1[_X] + vp2[_X];
				tama[i].speed.vector[_Y] = vl1[_Y] + vp2[_Y];
				/*
				tama[i].position.vector[_X] += tama[i].speed.vector[_X];
				tama[i].position.vector[_Y] += tama[i].speed.vector[_Y];
				*/
				tama[j].speed.vector[_X] = vl2[_X] + vp1[_X];
				tama[j].speed.vector[_Y] = vl2[_Y] + vp1[_Y];
				/*
				tama[j].position.vector[_X] += tama[j].speed.vector[_X];
				tama[j].position.vector[_Y] += tama[j].speed.vector[_Y];
				*/
				for(k=_X;k<=_Y;k++){
					if(tama[i].speed.vector[k]>stage.vmax[ship.gamemode]){
						tama[i].speed.vector[k]=stage.vmax[ship.gamemode];
					}
					else if(tama[i].speed.vector[k]<-stage.vmax[ship.gamemode]){
						tama[i].speed.vector[k]=-stage.vmax[ship.gamemode];
					}
				}
			}
			else if(rr>1.0){
				tama[i].hitflag[i][j] = 0;
			}
		}
	}
}

/* ボールの移動処理 */
void ballmove(void)
{
	GLint i,flag=0;
	GLfloat x,y,dx,bx,by;
	if(ship.life>0){
		ballhit();
		gravity();
	}
	
	for(i=0;i<10;i++){
		if(tama[i].tamaflag == 1){
			/* ボールの速度変化（時間経過による加速・減速） */
			tama[i].time ++;
			if(tama[i].time==500){
				tama[i].speed.vector[_X]/=2.0f;
				tama[i].speed.vector[_Y]*=2.0f;
			}
			else if(tama[i].time==1000){
				tama[i].speed.vector[_X]*=2.0f;
				tama[i].speed.vector[_Y]/=2.0f;
				tama[i].time=0;
			}				
			x = tama[i].position.vector[_X];
			y = tama[i].position.vector[_Y];
			bx = x;
			by = y;
			if(stage.gravity_flag != 0){
				tama[i].speed.vector[_X] += tama[i].addspeed.vector[_X] * stage.gravity_flag;
				tama[i].speed.vector[_Y] += tama[i].addspeed.vector[_Y] * stage.gravity_flag;
			}
			if(ship.life <= -10 && ship.gamemode == 0){
				tama[i].speed.vector[_Y] -= 0.02f;
			}
			else if(ship.life > 0 && ship.gamemode == 0){
				tama[i].speed.vector[_Y] -= 0.02f;
			}

			/* X方向の壁判定 */
			x += tama[i].speed.vector[_X];
			if(x<0.5f || x>19.5f){
				tama[i].speed.vector[_X] *= -1;
				x = tama[i].position.vector[_X];
			}
			else{
				tama[i].position.vector[_X] = x;
			}
			
			/* Y方向の壁判定・パドル判定 */
			y += tama[i].speed.vector[_Y];
			dx = x-ship.position.vector[_X];
			if(y>14){
				tama[i].speed.vector[_Y] *= -1;
				y = tama[i].position.vector[_Y];
			}
			/* パドルに当たった場合の処理（ノーマルモード） */
			else if(ship.gamemode==0 && ship.life>0  && fabs(dx)<ship.hitwidth/1.7 && y<ship.position.vector[_Y]+1.0 && y>ship.position.vector[_Y]-2.0){
				tama[i].speed.vector[_Y] = 1.0f;
				tama[i].speed.vector[_X] = 2.0f * dx/ship.hitwidth;
				tama[i].position.vector[_Y] = ship.position.vector[_Y]+1.0f;
				tama[i].point = tama[i].hitcount*tama[i].hitcount*(10+i);
				ship.score += tama[i].point;
				hit[i].tamaflag = 2;
				/* tama[i].hitcount = 0; */
				collision[i].tamaflag = 0;
				collision[i].hitcount = 0;
			}
			/* パドルに当たった場合の処理（弾避けモード） */
			else if(ship.gamemode==1 && ship.life>0 && y<stage.bottom-1.0f){
				tama[i].speed.vector[_Y] = 0.5f;
				tama[i].position.vector[_Y] = stage.bottom-1.0f;
				tama[i].point = tama[i].hitcount*(tama[i].hitcount+10)*(i+1);
				ship.score += tama[i].point;
				hit[i].tamaflag = 2;
				/* tama[i].hitcount = 0; */
				collision[i].tamaflag = 0;
				collision[i].hitcount = 0;
			}
			/* パドルに直撃した場合（弾避けモード） */
			else if(ship.gamemode==1 && ship.life>0  && fabs(dx)<0.5f && y<ship.position.vector[_Y]+0.25f && y>ship.position.vector[_Y]-0.25f){
				ship.life=0;
			}
			/* ボールが下に落ちた場合の処理 */
			else if(y<-17){
				tama[i].tamaflag = 0;
				ship.life--;
				tama[i].hitcount = 0;
				hit[i].tamaflag = 0;
				collision[i].tamaflag = 0;
				collision[i].hitcount = 0;
			}
			else{
				tama[i].position.vector[_Y] = y;
			}
			/* ブロックとの当たり判定 */
			block_hit(x,y,bx,by,i);
		}
	}
}
/* 爆発エフェクトの初期化 */
void initexplotion(void)
{
	GLint i;
	for(i=0;i<10;i++){
		tama[i].speed.vector[_X] = (GLfloat)cosf(i*0.62f)*(sinf(i*1.5f)+0.5f);
		tama[i].speed.vector[_Y] = (GLfloat)(sinf(i*0.62f)*(cosf(i*1.5f)+0.5f)+0.4f);
		tama[i].position.vector[_X] = ship.position.vector[_X];
		tama[i].position.vector[_Y] = ship.position.vector[_Y];
		tama[i].material.diffuse_rgba[0] = 1.0f;
		tama[i].material.diffuse_rgba[1] = 0.3f;
		tama[i].material.diffuse_rgba[2] = 0.0f;
		tama[i].material.diffuse_rgba[3] = 1.0f;	
		tama[i].tamaflag = 1;
	}
}

/* アイドル時の処理（マウス位置による自機移動など） */
void  CALLBACK  idle_process(void)
{
	//GLint x = 0, y = 0;
	GLfloat fx= 0,fy = 0;
	//fx = (GLfloat)x;
	//auxGetMouseLoc(&x,&y);
	
	fx = (GLfloat)mouse.mouseX/40.0f;
	fy = (GLfloat)(mouse.mouseY -200.0f)/40.0f;
	ship.position.vector[_X] = fx;
	if(ship.gamemode) ship.position.vector[_Y] = -fy;

	if(ship.position.vector[_X] < ship.hitwidth/2.0f - 0.5f){
		ship.position.vector[_X] = ship.hitwidth/2.0f - 0.5f;
	}
	else if(ship.position.vector[_X] > 20.5f - ship.hitwidth/2.0f){
		ship.position.vector[_X] = 20.5f - ship.hitwidth/2.0f;
	}
	if(ship.position.vector[_Y] < stage.bottom-0.5f){
		ship.position.vector[_Y] = stage.bottom-0.5f;
	}
	else if(ship.position.vector[_Y] >-1.0f){
		ship.position.vector[_Y] = -1.0f;
	}

	
	if(stage.blocks && stage.demotime==0){
	
		ballmove();
		if(ship.life<=0 && ship.life > -10){
			ship.material.diffuse_rgba[0] = -(GLfloat)ship.life;
			ship.life --;
			if(ship.life == -10){
				initexplotion();
				ship.deadcount++;
				if(ship.newrecoad){
					savehighscore();
				}
			}
		}
		if(ship.life == -20 || ship.life<=-10 && ship.gamemode==1){
			ship.score = 0;
			ship.newrecoad = 0;
			Initcharacter();
			Initstage();
			light_setting();
		}
		

		
		if(ship.newrecoad){
			ship.highscore = ship.score;
		}
		else if(ship.score>ship.highscore && ship.newrecoad==0){
			ship.newrecoad=1;
			ship.highscore = ship.score;
		}
	}	
	else if(stage.blocks>0 && stage.demotime>0){
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		gluPerspective(120,1.0,1.0,(GLfloat)(stage.demotime)+50.0);
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		gluLookAt(9.5,-5.0,10.0+(GLfloat)(stage.demotime), 9.5,0.0,0.0, 0.0,1.0,0.0);
		stage.demotime -= (1 + mouse.Down[_LEFT]*3);
		if(stage.demotime<=0){
			stage.demotime = 0;
			glMatrixMode(GL_PROJECTION); glLoadIdentity();
			gluPerspective(120,1.0,1.0,40.0);
			glMatrixMode(GL_MODELVIEW); glLoadIdentity();
			gluLookAt(9.5,-5.0,10.0, 9.5,0.0,0.0, 0.0,1.0,0.0);
		}
	}
	else if(stage.blocks==0 && stage.demotime<200){
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		gluPerspective(120,1.0,1.0,(GLfloat)(stage.demotime)+50.0);
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		gluLookAt(9.5,-5.0,10.0+(GLfloat)(stage.demotime), 9.5,0.0,0.0, 0.0,1.0,0.0);
		stage.demotime += (1 + mouse.Down[_LEFT]*3);
		if(stage.demotime>=200){
			stage.demotime = 200;
			stage.now++;
			Initcharacter();
			Initstage();
		}
	}

	display();
}
/* メイン関数 */
void main(void)
{
	printf("Select Game Mode\n");
	printf("0.Normal Game\n");
	printf("1.Tameyoke Game\n");
	do{
		scanf_s("%d",&ship.gamemode);
	}while(ship.gamemode>1 || ship.gamemode<0);

	ship.score = 0;
	stage.now = 0;
	stage.display = 0;
	ship.deadcount = 0;
	window();
	Initcharacter();
	Initfile();
	ship.hitwidth = ship.hitwidth0[ship.gamemode];
	loadhighscore();

	Initstage();
	light_setting();

	glColor4f(1.0,1.0,1.0,1.0);

	stage.display = 1;

	MakeBitMapFont();	
	//auxReshapeFunc(Reshape);	

	/* display(); */

	//auxMouseFunc(AUX_LEFTBUTTON,AUX_MOUSEDOWN,mouse_left_down);
	//auxMouseFunc(AUX_LEFTBUTTON,AUX_MOUSEUP,mouse_left_up);
	//auxIdleFunc(idle_process);
	//auxMainLoop(display);
	
	// freeglut

	glutMouseFunc(mouse_button); // コールバックの引数や処理内容をGLUT用に修正
	glutPassiveMotionFunc(mouse_move); // マウス移動時
	//glutMotionFunc(mouse_move);        // ドラッグ時
	//glutMouseFunc(mouse_left_up);


	glutReshapeFunc(Reshape);
	glutIdleFunc(idle_process);
	glutDisplayFunc(display);
	glutMainLoop();
}
