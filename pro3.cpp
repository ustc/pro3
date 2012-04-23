#define GL_GLEXT_PROTOTYPES 1
#include <stdio.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
using std::cout; using std::endl; using std::vector;

#define JITTER 0.002
#define VPASSES 30

double genrand() {
  double symbol = random()%2;
  if(symbol == 0.0) symbol = -1.0; 
  return (((double)(random()+1))/2147483649)*symbol;
}

struct point {
  float x, y, z;
};

struct face {
	int f[4][3];
};

int index_tangent;
int index_bitangent;

int p;

float eye[] = {0.0, 2.5, 4.0};
float viewpt[] = {0.0, 0.0, 0.0};
float up[] = {0.0, 1.0, 0.0};

float light0_position[] = {5.0, 2.0-0.02, 5.0, 1.0};
float light0_position_r[] = {5.0, -2.0-0.02, 5.0, 1.0};

vector<struct point> vpos; 
vector<struct point> vn; 
vector<struct point> vx; 
vector<struct point> vy; 
vector<struct point> vt; 
vector<struct face> pface; 

//open file
void load_obj(const char* filename) {
  char c[4][20];
  struct point p;
  struct face temp;

  FILE* file = fopen(filename,"r");
  char buffer[100];

  // read header 
  while (  strncmp( "mtllib", buffer,strlen("mtllib")) != 0  ) {	
    fgets(buffer,100,file);		
  }
  
  while ( fgets(buffer, 100, file) ) {
    // read vertex position
    if (buffer[0]=='v' && buffer[1]==' ') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vpos.push_back(p);
    }
    // read vertex normal
    else if (buffer[0]=='v' && buffer[1]=='n') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vn.push_back(p);
    }
    // read vertex tangents
    else if (buffer[0]=='v' && buffer[1]=='x') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vx.push_back(p);
    }
    // read vertex bitangent
    else if (buffer[0]=='v' && buffer[1]=='y') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vy.push_back(p);
    }
    // read vertex texture coordinates. 
    else if (buffer[0]=='v' && buffer[1]=='t') {
      sscanf(buffer, "%*s%f%f", &p.x, &p.y);
      vt.push_back(p);
    }
    // read polygonal face. 
    else if (buffer[0]=='f' && buffer[1]==' ') {
      sscanf(buffer, "%*s%s%s%s%s", c[0], c[1], c[2], c[3]);
      for (int i=0; i<4; i++) {
        sscanf(c[i], "%i/%i/%i", &temp.f[i][0], &temp.f[i][1], &temp.f[i][2]);
      }
      pface.push_back(temp);
    }
  }//while
}

char  *read_shader_program(const char* filename) {
	FILE* fp;
	char *content = NULL;
	int fd, count;
	fd = open(filename, O_RDONLY);
	//cout<<fd<<endl;
	count = lseek(fd,0,SEEK_END);
	close(fd);
	content = (char*) calloc(1, (count+1));
	fp = fopen(filename,"r");
	count = fread(content, sizeof(char), count, fp);
	content[count]='\0';
	fclose(fp);
	return content;
}

void load_texture(char* filename, int tid, int p=0) {
	FILE* fptr;
	char buf[512], *parse;
	int im_size, im_width, im_height, max_color;
	unsigned char* texture_bytes;
	fptr=fopen(filename, "r");
	if (fptr==NULL) cout<<"fptr_NULL"<<endl;
	fgets(buf,512,fptr);
	do {    	
		fgets(buf,512,fptr);
	} while (buf[0]=='#');
	if (buf==NULL) cout<<"buf_NULL"<<endl;
	parse = strtok(buf, " \t");
	im_width = atoi(parse);

	parse = strtok(NULL, " \n");
	im_height = atoi(parse);

	fgets(buf, 512, fptr);
	parse = strtok(buf, " \n");
	max_color = atoi(parse);
	
	im_size = im_width*im_height;
	texture_bytes = (unsigned char*) calloc(3, im_size);
	fread(texture_bytes,3,im_size, fptr);
	fclose(fptr);

	glBindTexture(GL_TEXTURE_2D, tid);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, im_width, im_height,0, GL_RGB,
			GL_UNSIGNED_BYTE, texture_bytes);
	if(p==1){
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	free(texture_bytes);
}


void view_volume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,1.0,10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

void set_light()
{
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	float light0_ambient[] = {1.0,1.0,1.0,1.0};
    float light0_diffuse[] = {1.0,1.0,1.0,1.0};
    float light0_specular[] = {1.0,1.0,1.0,1.0};

    glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


void set_material()
{ 
	//~ float mat_diffuse[] = {0.3,0.3,0.3,1.0};
	//~ float mat_specular[] = {0.5,0.5,0.5,1.0};
	//~ PEARL
	float mat_ambient[] = {0.25,0.20725,0.20725,0.922};
	float mat_diffuse[] = {0.8, 0.829, 0.829, 0.922};
	float mat_specular[] = { 0.296648, 0.296648, 0.296648, 0.922};
	float mat_shininess[] = {11.264};
	
	//~ float mat_ambient[] = {0.24725,0.2245,0.0645,1.0};
	//~ float mat_diffuse[] = {0.34615,0.3143,0.0903,1.0};
	//~ float mat_specular[] = { 0.797357,0.723991,0.208006,1.0};
	//~ float mat_shininess[] = {83.2};

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void jitter_model()
{
    glTranslatef(JITTER*genrand(),JITTER*genrand(),JITTER*genrand());

}

void draw_backgroud()
{
	glUseProgram(0);
	
	glPushMatrix();
	glDisable(GL_LIGHTING);
	GLUquadric *gpt = gluNewQuadric();
  	gluQuadricTexture(gpt, 1);
	gluQuadricOrientation(gpt,GLU_INSIDE);
	glActiveTexture(GL_TEXTURE0);
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);
	glRotatef(90,1.0,1.0,0.0);
	gluSphere(gpt, 4.0, 64, 64);
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	glFlush();

}

void draw_teapot()
{
	int v, t, n;
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glUseProgram(p);
	glRotatef(-90,0.0,1.0,0.0);
	glTranslatef(0.0,-0.02,0.0);


	glBegin(GL_QUADS);
		for(unsigned int i=0; i<pface.size(); i++) {
			for (unsigned int j=0; j<4; j++) {
				v = pface[i].f[j][0] - 1;
				t = pface[i].f[j][1] - 1;
				n = pface[i].f[j][2] - 1;
				glNormal3f(vn[n].x, vn[n].y, vn[n].z);
				glTexCoord2f(vt[t].x, vt[t].y); 
				glVertexAttrib3f(index_tangent, vx[v].x, vx[v].y, vx[v].z);
				glVertexAttrib3f(index_bitangent, vy[v].x, vy[v].y, vy[v].z);
				glVertex3f(vpos[v].x, vpos[v].y, vpos[v].z);
			}
		}
	glEnd(); 
	glPopMatrix();
}

void draw_plane()
{
	glStencilFunc(GL_ALWAYS,1,0xff);  
    glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);  
	glDepthMask(GL_FALSE);  
	glDisable(GL_LIGHTING);
	float mytexcoords[4][2] = {{0.0,1.0},{1.0,1.0},{1.0,0.0},{0.0,0.0}};
        
        float surface[4][3]= {
		{-2.0, -0.0, -2.0},
		{2.0, -0.0, -2.0},
		{2.0, -0.0, 3.0},
		{-2.0, -0.0, 3.0},
	};
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glUseProgram(0);
	
	glBegin(GL_QUADS);
	glNormal3f(0.0, 1.0, 0.0);
	for (int k=0; k<4; k++) {
		glTexCoord2fv(mytexcoords[k]);
		glVertex3fv(surface[k]);
	}
	glEnd();
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE); 
	glStencilFunc(GL_EQUAL,1,0xff);  
    	glStencilFunc(GL_KEEP,GL_KEEP,GL_KEEP); 
	
	glPushMatrix();  
    	glScalef(1.0,-1.0,1.0);  

    	//glutSolidTeapot(1);  
        glActiveTexture(GL_TEXTURE1);
        load_texture("textures/skyenv_r.ppm", 2);
        glLightfv(GL_LIGHT0,GL_POSITION,light0_position_r);	
        draw_teapot();
    	glPopMatrix();  
		glLightfv(GL_LIGHT0,GL_POSITION,light0_position);	
    	glDisable(GL_STENCIL_TEST);  	
}

void draw() { 

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.35,0.35,0.35,0.0);

	glClear(GL_ACCUM_BUFFER_BIT);
	for(int i=0;i<VPASSES;i++){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	draw_backgroud();
	glPushMatrix();
	jitter_model();
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
    load_texture("textures/skyenv.ppm", 2);
        
	draw_teapot();
	
	draw_plane();
	glAccum(GL_ACCUM,1.0/(float)(VPASSES));
	glPopMatrix();
	}
	glAccum(GL_RETURN,1.0);
	glPopMatrix();
	
	glutSwapBuffers();
}

unsigned int set_shaders(const char* vert,const char* frag) {
	char *vs, *fs;
	GLuint v,f,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	vs = read_shader_program(vert);
	fs = read_shader_program(frag);
	glShaderSource(v,1, (const char**)&vs, NULL);	
	glShaderSource(f,1, (const char**)&fs, NULL);	
	free(vs);
	free(fs);
	glCompileShader(v);
	glCompileShader(f);
  	glGetError();
	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);
	glLinkProgram(p);
	glUseProgram(p);
	return (p);
}





void set_uniform(int p)
{
	int location;
	location = glGetUniformLocation(p,"background");
	glUniform1i(location,0);
	cout<<"background: "<<location<<endl;
	location = glGetUniformLocation(p,"myenvmap");
	glUniform1i(location,1);
	cout<<"myenvmap: "<<location<<endl;
	location = glGetUniformLocation(p,"mytexture");
	glUniform1i(location,2);
	cout<<"mytexture: "<<location<<endl;
	location = glGetUniformLocation(p,"mynormalmap");
	glUniform1i(location,3);
	cout<<"mynormalmap: "<<location<<endl;
}

void set_textures()
{
	glActiveTexture(GL_TEXTURE0);
	load_texture("textures/sky.ppm", 1);

	glActiveTexture(GL_TEXTURE1);
	load_texture("textures/skyenv.ppm", 2);

	glActiveTexture(GL_TEXTURE2);
	load_texture("textures/material.ppm", 3);
	
	glActiveTexture(GL_TEXTURE3);
	load_texture("textures/f_n.ppm", 4,1);
}

int main (int argc, char**argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_ACCUM|GLUT_STENCIL);
	glutInitWindowSize(720,720);
	glutInitWindowPosition(100,50);
	glutCreateWindow("my_cool_teapot");        

	load_obj("teapot.605.obj");
	set_light();
    set_material();
	set_textures();
    view_volume();
 	p = set_shaders("phong_teatex.vert","phong.frag");
	set_uniform(p);
    index_tangent = glGetAttribLocation(p, "tangent");
    index_bitangent = glGetAttribLocation(p, "bitangent");
	glutDisplayFunc(draw);
	glutMainLoop();

	return 0;
}
