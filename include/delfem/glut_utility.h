/*
 *  glut_utility.h
 *  dfm_core
 *
 *  Created by Nobuyuki Umetani on 1/20/11.
 *  Copyright 2011 The University of Tokyo. All rights reserved.
 *
 */

void RenderBitmapString(float x, float y, void *font,char *string)
{
  char *c;
  ::glRasterPos2f(x, y);
  for (c=string; *c != '\0'; c++) {
	  ::glutBitmapCharacter(font, *c);
  }
}

void ShowBackGround(){  
	::glMatrixMode(GL_MODELVIEW);
  ::glPushMatrix();
	::glLoadIdentity();
	::glMatrixMode(GL_PROJECTION);
  ::glPushMatrix();
	::glLoadIdentity();
  ::glDisable(GL_DEPTH_TEST);
  ::glBegin(GL_QUADS);
  ::glColor3d(0.2,0.7,0.7);
  ::glVertex3d(-1,-1,0);
  ::glVertex3d( 1,-1,0);
  ::glColor3d(1,1,1);
  ::glVertex3d( 1, 1,0);
  ::glVertex3d(-1, 1,0);
  ::glEnd();
  ::glEnable(GL_DEPTH_TEST);
	::glMatrixMode(GL_PROJECTION);
  ::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
  ::glPopMatrix();
}

void ShowFPS(){
	static char s_fps[32];
	int* font=(int*)GLUT_BITMAP_8_BY_13;
	{
		static int frame, timebase;
		int time;
		frame++;
		time=glutGet(GLUT_ELAPSED_TIME);
		if (time - timebase > 500) {
			sprintf(s_fps,"FPS:%4.2f",frame*1000.0/(time-timebase));
			timebase = time;
			frame = 0;
		}
	}
	char s_tmp[30];
  
	GLint viewport[4];
	::glGetIntegerv(GL_VIEWPORT,viewport);
	const int win_w = viewport[2];
	const int win_h = viewport[3];
  
	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();
	::gluOrtho2D(0, win_w, 0, win_h);
	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();
	::glScalef(1, -1, 1);
	::glTranslatef(0, -win_h, 0);
	::glDisable(GL_LIGHTING);
  //	::glDisable(GL_DEPTH_TEST);
  //	::glColor3d(1.0, 1.0, 0.0);
	::glColor3d(1.0, 0.0, 0.0);
	strcpy(s_tmp,"DelFEM demo");
	RenderBitmapString(10,15, (void*)font, s_tmp);
	::glColor3d(0.0, 0.0, 1.0);
	strcpy(s_tmp,"Press \"space\" key!");
	RenderBitmapString(120,15, (void*)font, s_tmp);
  //	::glColor3d(1.0, 0.0, 0.0);
	::glColor3d(0.0, 0.0, 0.0);
	RenderBitmapString(10,30, (void*)font, s_fps);
  //	::glEnable(GL_LIGHTING);
	::glEnable(GL_DEPTH_TEST);
	::glPopMatrix();
	::glMatrixMode(GL_PROJECTION);
	::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
}


void DrawBackGround()
{
  // îwåiï`âÊ
  ::glMatrixMode(GL_MODELVIEW);  // ModelViewçsóÒÇÉNÉäÉA
  ::glPushMatrix();
  ::glLoadIdentity();
  ::glMatrixMode(GL_PROJECTION); // ProjectionçsóÒÇÉNÉäÉA
  ::glPushMatrix();
  ::glLoadIdentity();
  ::glDisable(GL_DEPTH_TEST);    // DepthTestÇñ≥å¯Ç…Ç∑ÇÈ
  ::glBegin(GL_QUADS);
  ::glColor3d(0.2,0.7,0.7);   // â∫ÇÃêFÇê›íË
  ::glVertex3d(-1,-1,0);  // ç∂â∫ÇÃì_
  ::glVertex3d( 1,-1,0);  // âEâ∫ÇÃì_
  ::glColor3d(1,1,1);         // è„ÇÃêFÇê›íË
  ::glVertex3d( 1, 1,0);  // âEè„ÇÃì_
  ::glVertex3d(-1, 1,0);  // ç∂è„ÇÃì_
  ::glEnd();
  ::glEnable(GL_DEPTH_TEST);     // DepthTestÇóLå¯Ç…Ç∑ÇÈ
  ::glMatrixMode(GL_PROJECTION); // ProjectionçsóÒÇå≥Ç…ñﬂÇ∑
  ::glPopMatrix();
  ::glMatrixMode(GL_MODELVIEW);  // ModelViewçsóÒÇå≥Ç…ñﬂÇ∑
  ::glPopMatrix();
}