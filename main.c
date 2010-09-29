#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

static SDL_Surface *gScreen;

static int globalTime = 0;
static int fadeOutStartTime = 0;
static int fadeOutTime = 0;
static int totalTime = 60000;
static int fadeOutDuration = 5000;

static int width = 640;
static int height = 480;

static Mix_Music *music = NULL;

static TTF_Font *font = NULL;

static void initFont ()
{
	font = TTF_OpenFont("ccdemo.app/Contents/Resources/FreeSansBold.ttf", 48);
	if (!font) {
		printf("\nError opening font file: %s\n", TTF_GetError());
	}
}

static void renderText( const char* text, const unsigned char r, const unsigned	char g, const unsigned char b, float x, float y, float z)
{
	SDL_Color color = {r, g, b};
	SDL_Surface	*message = TTF_RenderText_Blended(font, text, color);
	
	unsigned int texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, message->w, message->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, message->pixels);
	glBegin(GL_QUADS);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glTexCoord2d(0, 0); glVertex3d(x, y, z);
	glTexCoord2d(1, 0); glVertex3d(x+message->w, y, z);
	glTexCoord2d(1, 1); glVertex3d(x+message->w, y+message->h, z);
	glTexCoord2d(0, 1); glVertex3d(x, y+message->h, z);
	glEnd();
	glDeleteTextures(1, &texture);
	SDL_FreeSurface(message);
}

static void initAttributes ()
{
    int value;
    
    value = 16;
    SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, value);
    
    value = 1;
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, value);
}

static void printAttributes ()
{
    int nAttr;
    int i;
    
    int  attr[] = { SDL_GL_RED_SIZE, SDL_GL_BLUE_SIZE, SDL_GL_GREEN_SIZE,
                    SDL_GL_ALPHA_SIZE, SDL_GL_BUFFER_SIZE, SDL_GL_DEPTH_SIZE };
                    
    char *desc[] = { "Red size: %d bits\n", "Blue size: %d bits\n", "Green size: %d bits\n",
                     "Alpha size: %d bits\n", "Color buffer size: %d bits\n", 
                     "Depth bufer size: %d bits\n" };

    nAttr = sizeof(attr) / sizeof(int);
    
    for (i = 0; i < nAttr; i++) {
    
        int value;
        SDL_GL_GetAttribute (attr[i], &value);
        printf (desc[i], value);
    } 
}

static void createSurface (int fullscreen)
{
    Uint32 flags = 0;
    
    flags = SDL_OPENGL;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;
    
    gScreen = SDL_SetVideoMode (width, height, 0, flags);
    if (gScreen == NULL) {
		
        fprintf (stderr, "Couldn't set 640x480 OpenGL video mode: %s\n",
                 SDL_GetError());
		SDL_Quit();
		exit(2);
	}
}

static void initGL ()
{
	float aspectratio = width / height;
	glViewport(0, 0, width, height);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    // glOrtho(0, 640, 480, 0, 1, -1);
	gluPerspective(45.0, aspectratio, 0.2, 255.0);
	
    glMatrixMode(GL_MODELVIEW);
	
    glEnable(GL_TEXTURE_2D);
	
    glLoadIdentity();
	
	//glEnable(GL_DEPTH_TEST);
	
	//glShadeModel(GL_FLAT);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	float ambientLight[] = { 0.2, 0.2, 0.2, 1.0 };
	float diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };
	float specularLight[] = { 0.5, 0.5, 0.5, 1.0 };
	float position[] = { -1.5, 1.0, -4.0, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

double distort(double x) {
	return x > 0 ? 0 : x;
}

static void drawGL ()
{
	float t = globalTime / 1000.0;
	
	glClearColor(1.0, 1.0, 1.0, ((float)fadeOutTime/fadeOutDuration) );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
	
	
	int m = 12, n = 8;
	float cw = 1.0 / m;
	float ch = 1.0 / n;
	
	int i, j;
	
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);
	
	glPushMatrix();
	glTranslated(220-10.0*t, -30.0, -100.0);
	glScaled(0.11, 0.12, 0.1);
	glRotated(180.0, 1.0, 0.0, 0.0);
	//glScaled(0.1, -0.1, 0.1);
	//glOrtho(0, 640, 480, 0, 1, -1);
	renderText("Hello, demoscene! This is first cornercut demo! Code by Platosha, music by Lyzzard. Greetings: SandS, Quite, 7dump, Mahyem, Skrju, Valet2.", 255, 255, 255, 0.0, 0.0, 0.0);
	glPopMatrix();
	
	
	glPushMatrix();
	glTranslated(sin(t*.78) / 2, sin(t*.56) / 2, -4.5 + 4 * pow(sin(t*0.43), 2));
	//glRotated(12.0*t, 1.0, 0, 0);
	//glRotated(12.0*t, 1.0, 0, 0);
	
	glPushMatrix();
	glScaled(3.0, 3.0, 3.0);
	glBegin(GL_QUADS);
	for (i=-n; i<n; i++) {
		for (j=-m; j<m; j++) {
			// float s = sin((float) j / (float) m * 20 + t) * .3333 + 1;
			int ii, jj;
			float w = cw;
			float h = ch;
			for (ii=0; ii<=1; ii++) {
				for (jj=ii; jj>=0 && jj <= 1; ii ? jj-- : jj++) {
					float x = w * (j + jj);
					float y = h * (i + ii);
					//float z = distort(sin((x+t*0.45)*10))*.1 + distort(sin((y+t*0.245)*10))*.1;
					float r, g, b;
					r = sin( ((x)/1 + sin(t*0.78)) * (sin(t*0.84)/2+.5) * m / 2 ) / 4;
					r+= sin( ((y)/1 + sin(t*0.64)) * (sin(t*0.53)/2+.5) * n / 2 ) / 4;
					r+= .5;
					g = sin( ((x)/1 + sin(t*0.32)) * (sin(t*0.57)/2+.5) * m / 2 ) / 4;
					g+= sin( ((y)/1 + sin(t*0.23)) * (sin(t*0.69)/2+.5) * n / 2 ) / 4;
					g+- .5;
					b = sin( ((x)/1 + sin(t*0.43)) * (sin(t*0.15)/2+.5) * m / 2 ) / 4;
					b+= sin( ((y)/1 + sin(t*0.71)) * (sin(t*0.23)/2+.5) * n / 2 ) / 4;
					b+= .5;
					r = 1 - r;
					g = 1 - g;
					b = 1 - b;
					glColor3f(r, g, b);
					glVertex3f(x, y, 0.0);					
				}
			}
		}
	}
	glEnd();
	glPopMatrix();
	
	glRotated(180.0, 0, 1.0, 0);
	glRotated(45.0*t, 0, 0, 1.0);
	glRotated(22.0*t, 1.0, 0, 0);
	
	float di, dj;
	float step = 1.0/20;
	int dk, dn = 100;
	for (dk=0; dk < dn; dk++) {
		glPointSize(10.0* (sin(t*2.42 + 20*(float)dk/dn)+1));
		glLineWidth(10.0* (sin(t*2.42 + 20*(float)dk/dn)+1));
		glBegin(GL_POINTS);
		for (di=0; di < M_PI * 2; di+=step) {
			for (dj=di; dj <= di+step; dj+=step) {
				float x = sin(dj);
				float y = cos(dj);
				float z = (0.5 - (float) dk/dn) * 4.0;
				float dx = sin(z*20) * (1 - cos(t*.43));
				float dy = cos(z*20) * (1 - cos(t*.43));
				x = x + sin(z*20);
				y = y + cos(z*20);
				float r = 1 - cos(dj * 10 * sin((t-1.8*dk/dn)*.11));
				float cr = r;
				glColor4f(0.33, 0.15, 0.03, 1 - pow((cr+0.5), 2)*.5 );
				r = r * sin((z + t*.5) * 10 * (sin(t*0.25) + 1));
				glVertex3f(x * r + dx, y * r + dy, z);
			}
		}
		glEnd();
	}
	
	glPopMatrix();
	
	
	
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
}

static void mainLoop ()
{
    SDL_Event event;
    int done = 0;
    int fps = 0;
	int delay = 0;
    int thenTicks = -1;
    int nowTicks;
	int frames = 0;
    
    while ( !done ) {
		
		globalTime = SDL_GetTicks();
		if (fadeOutStartTime==0 && globalTime > (totalTime - fadeOutDuration)) {
			fadeOutStartTime = globalTime;
		}
		if (fadeOutStartTime) {
			fadeOutTime = globalTime - fadeOutStartTime;
			if (fadeOutTime > fadeOutDuration) {
				done = 1;
				break;
			}
		}

		/* Check for events */
		while ( SDL_PollEvent (&event) ) {
			switch (event.type) {
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						done = 1;
					}
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
    
        drawGL ();
        SDL_GL_SwapBuffers ();

        if (thenTicks > 0) {
			frames++;
            nowTicks = SDL_GetTicks ();
            delay = nowTicks - thenTicks;
			if (delay >= 100) {
				fps = frames / (delay / 1000.0);
				frames = 0;
				thenTicks = nowTicks;
				printf ("FPS: %d\n", fps);
			}
        }
        else {
            thenTicks = SDL_GetTicks ();
        }
	}
}

int main(int argc, char *argv[])
{
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
	int audio_channels = 2;
	int audio_buffers = 4096;
	
	// Init SDL video and audio subsystem
	if ( SDL_Init (SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0 ) {
		
        fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		exit(1);
	}
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		printf("Unable to open audio!\n");
		exit(1);
	}
	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	
	if(TTF_Init()==-1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	initFont();

    // Set GL context attributes
    initAttributes ();
    
    // Create GL context
    createSurface (argc > 1);
    
    // Get GL context attributes
    printAttributes ();
    
    // Init GL state
    initGL ();
	
	music = Mix_LoadMUS("ccdemo.app/Contents/Resources/07.06.mp3");
	Mix_PlayMusic(music, 0);
    
    // Draw, get events...
    mainLoop ();
    
	TTF_Quit();
	
    // Cleanup
	SDL_Quit();
	
    return 0;
}
