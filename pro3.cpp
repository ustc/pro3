#define GL_GLEXT_PROTOTYPES 1
#include <stdio.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
using std::cout; using std::endl; using std::vector;

double genrand() {
  return (((double)(random()+1))/2147483649);
}

struct point {
  float x, y, z;
};

int index_tangent;
int index_bitangent;

float eye[] = {4.0, 2.0, 0.0};
float viewpt[] = {0.0, 0.0, 0.0};
float up[] = {0.0, 1.0, 0.0};
float light0_position[] = {3.0, 3.0, 3.0, 1.0};

vector<struct point> vpos; 
vector<struct point> vn; 
vector<struct point> vx; 
vector<struct point> vy; 
vector<struct point> vt; 
vector< vector< vector<int> > > pface; 

//open file
void load_obj(const char* filename) {
  int a[3];
  char c[4][20];
  struct point p;

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
    if (buffer[0]=='v' && buffer[1]=='n') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vn.push_back(p);
    }
    // read vertex tangents
    if (buffer[0]=='v' && buffer[1]=='x') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vx.push_back(p);
    }
    // read vertex bitangent
    if (buffer[0]=='v' && buffer[1]=='y') {
      sscanf(buffer, "%*s%f%f%f", &p.x, &p.y, &p.z);
      vy.push_back(p);
    }
    // read vertex texture coordinates. 
    if (buffer[0]=='v' && buffer[1]=='t') {
      sscanf(buffer, "%*s%f%f", &p.x, &p.y);
      vt.push_back(p);
    }
    // read polygonal face. 
    if (buffer[0]=='f' && buffer[1]==' ') {
      vector< vector<int> > vec;
      sscanf(buffer, "%*s%s%s%s%s", c[0], c[1], c[2], c[3]);
      for (int i=0; i<4; i++) {
   	vector<int> va;
        sscanf(c[i], "%i/%i/%i", &a[0], &a[1], &a[2]);
        va.push_back(a[0]);va.push_back(a[1]);va.push_back(a[2]);
        vec.push_back(va);
      }
      pface.push_back(vec);
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

void draw() { 
  int v, t, n;
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.35,0.35,0.35,0.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D,1);

  glBegin(GL_QUADS);
  for (unsigned int i=0; i<pface.size(); i++) {
    for (unsigned int j=0; j<4; j++) {
      v = pface[i][j][0] - 1;
      t = pface[i][j][1] - 1;
      n = pface[i][j][2] - 1;
      glNormal3f(vn[n].x, vn[n].y, vn[n].z);
      glTexCoord2f(vt[t].x, vt[t].y); 
      glVertexAttrib3f(index_tangent, vx[v].x, vx[v].y, vx[v].z);
      glVertexAttrib3f(index_bitangent, vy[v].x, vy[v].y, vy[v].z);
      glVertex3f(vpos[v].x, vpos[v].y, vpos[v].z);
    }
  }
  glEnd(); 
	glutSwapBuffers();
}

void renderscene()
{
	glClearColor(0.5,0.4,0.3,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,1);
	glRotatef(-0.0,1.0,0.0,0.0);
	glutSolidTeapot(1.0);
	glutSwapBuffers();
}

void view_volume()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,1.0,20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0],eye[1],eye[2],viewpt[0],viewpt[1],viewpt[2],up[0],up[1],up[2]);
}

void do_viewvolume() {
	struct point eye, view, up;

 	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0,1.0,0.1,20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	eye.x = 3.0;eye.y = 3.0; eye.z = 3.0;
	view.x = 0.0;view.y = 0.0; view.z = 0.0;
	up.x = 0.0; up.y = 1.0; up.z = 0.0;
	gluLookAt(eye.x, eye.y, eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

unsigned int set_shaders() {
	//GLint vertCompiled, fragCompiled;
	char *vs, *fs;
	GLuint v,f,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
 	vs = read_shader_program("phong_teatex.vert");
 	fs = read_shader_program("phong_teapot.frag");
 	//fs = read_shader_program("phong_teatex.frag");
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

void do_lights() {
	float light0_ambient[] = {0.0,0.0,0.0,0.0};
	float light0_diffuse[] = {2.0,2.0,2.0,0.0};
	float light0_specular[] = {2.25,2.25,2.25,0.0};
	float light0_position[] = {1.5,2.0,2.0,1.0};
	float light0_direction[] = {-1.5,-2.0,-2.0,1.0};

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void set_light()
{
	glLightfv(GL_LIGHT0,GL_POSITION, light0_position);
}

void do_material() {
  float mat_ambient[] = {0.00, 0.00, 0.00};
  float mat_diffuse[] = {1.00, 0.80, 0.70};
  float mat_specular[] = {0.15, 0.15, 0.15};
  float mat_shininess[] = {60.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void set_material()
{ 
  float mat_ambient[] = {0.00, 0.00, 0.00};
  float mat_diffuse[] = {1.00, 0.80, 0.70};
  float mat_specular[] = {0.15, 0.15, 0.15};
  float mat_shininess[] = {60.0};
/*  
	float mat_diffuse[] = {0.5,0.5,0.5,1.0};
	float mat_specular[] = {0.6,0.6,0.6,1.0};
	float mat_shininess[] = {20.0};
*/
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void load_texture(char* filename, int tid) {
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
	cout<<"pp"<<endl;

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
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//free(texture_bytes);
}

void set_uniform(int p)
{
	int location;
	location = glGetUniformLocation(p,"mytexture");
	glUniform1i(location,0);
	location = glGetUniformLocation(p,"mynormalmap");
	glUniform1i(location,1);
}

int main (int argc, char**argv) {
  int p;
  load_obj("teapot.605.obj");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	//glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,50);
	glutCreateWindow("my_cool_teapot");        
	glActiveTexture(GL_TEXTURE0);
	load_texture("bubble_color.ppm", 1);
	glActiveTexture(GL_TEXTURE1);
	load_texture("fieldstoneN.ppm", 2);
 	//glEnable(GL_DEPTH_TEST);
	//do_viewvolume();
	//do_lights();
	//do_material();

        view_volume();
 	set_light();
        set_material();
 	p = set_shaders();
	set_uniform(p);
  index_tangent = glGetAttribLocation(p, "tangent");
  index_bitangent = glGetAttribLocation(p, "bitangent");
	glutDisplayFunc(draw);
	//glutDisplayFunc(renderscene);
	glutMainLoop();

  return 0;
}
