static GLint display_num;

GLubyte fontdata[95][12]; /* フォントデータ配列 */

void load_bitmapdata(void)
{
	int i, j;
	FILE* fp = NULL;
	unsigned int data[95][12];
	errno_t err;
	if ((err = fopen_s(&fp, "./font/bitmap_font.dat", "r")) != 0 || fp == NULL) {
		printf("Can't open font data file\n");
		exit(1);
	}

	for (i = 0; i < 95; i++) {
		for (j = 0; j < 12; j++) {
			if (feof(fp)) {
				printf("Unexpected end of file in font data\n");
				if (fp != NULL) {
					fclose(fp);
				}
				exit(1);
			}
			fscanf_s(fp, "%x", &data[i][j]);
			fontdata[i][j] = (GLubyte)data[i][j];
		}
	}
	if (fp != NULL) {
		fclose(fp);
	}
}

void MakeBitMapFont(void)
{
	int i;
	load_bitmapdata();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	display_num = glGenLists (128);
	for(i=32; i<127; i++){
		glNewList(display_num+i, GL_COMPILE);
		   glBitmap(8, 12, 0.0, 1.0, 10.0, 0.0, fontdata[i-32]);
		glEndList();
	}
}

void DrawString(char *str)
{
	int len;
	len = (int)strlen(str);
	glListBase(display_num);
	glCallLists(len, GL_BYTE, str);
}

void DrawChar(char c)
{
	glListBase(display_num);
	glCallLists(1,GL_BYTE,&c);
}
