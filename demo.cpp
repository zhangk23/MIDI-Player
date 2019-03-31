#include <stdio.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#define PI 3.14159265
#include "yssimplesound.h"
#include "fssimplewindow.h"
#include <cstdlib>
#include <string>

int window_W, window_H;
double moveV=0.5;
double keyboardR=300;
YsSoundPlayer player;
// ------------------string parsing functinos from lecture---------------
int ParseString(
                int &nWord,
                int wordTop[],
                int wordLength[],
                char str[],
                int maxNumWords)
{
    int i,state;
    
    state=0;
    nWord=0;
    for(i=0; str[i]!=0; i++)
    {
        switch(state)
        {
            case 0:
                if(str[i]!=' ' &&
                   str[i]!='\t' &&
                   str[i]!=',')
                {
                    state=1;
                    wordTop[nWord]=i;
                    wordLength[nWord]=1;
                    nWord++;
                }
                break;
            case 1:
                if(str[i]!=' ' &&
                   str[i]!='\t' &&
                   str[i]!=',')
                {
                    wordLength[nWord-1]++;
                }
                else
                {
                    state=0;
                    if(nWord==maxNumWords)
                    {
                        goto LOOPOUT;
                    }
                }
                break;
        }
    }
LOOPOUT:
    return nWord;
}


void SafeStrCpy(char dst[],char src[],int nLetters,int nLimit)
{
    int i,stopper;
    if(nLetters<nLimit)
    {
        stopper=nLetters;
    }
    else
    {
        stopper=nLimit;
    }
    
    for(i=0; i<stopper; i++)
    {
        dst[i]=src[i];
    }
    dst[stopper]=0;
}
// ----------------------------note class------------------------------------------
class Note{
public:
    int tPlay;
    int len;
    double ang;
    double x,y;
    bool playNote,drawNote;
    bool played;
    int midiNumber;
    int velocity;
    int colorR,colorG,colorB;
    
    Note();
    ~Note();
    void Create(double t_s, double l, double angle, int midiNum, int velocity);
    void SetCoord(double x, double y);
};

Note::Note(){
    tPlay=0;
    len=0;
    ang=0;
    x=0;
    y=0;
    midiNumber=0;
    playNote=false;
    drawNote=false;
    played=false;
}

Note::~Note(){
}

void Note::Create(double start_time, double length, double angle, int midiNum, int vel){
    tPlay=start_time;
    len=length;
    ang=angle;
    midiNumber=midiNum;
    velocity=vel;
}

void Note::SetCoord(double coordX, double coordY){
    x=coordX;
    y=coordY;
}

// --------------------beat class------------------------------
class Beat{
public:
    int r;
    int t_appear;
    bool drawBeat=false;
    int colorR,colorG,colorB;
    bool reverseColorChange;
    int rPrev;
};

// ------------------drawing functions---------------------------
void DrawBackground(){
    glColor3ub(0,0,0);
    
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(window_W,0);
    glVertex2i(window_W,window_H);
    glVertex2i(0,window_H);
    glEnd();
}

void DrawCircleKeyboard()
{
    int cx = window_W/2;
    int cy = window_H/2;
    int length=30;
    int num_keys=88;
    glShadeModel(GL_SMOOTH);
    
    glBegin(GL_LINES);
    for(int i=0;i<88;i++)
    {
        double theta=2.0f*PI*double(i)/double(num_keys);
        double x=keyboardR*cos(theta);
        double y=keyboardR*sin(theta);
        
        glColor3ub(0,0,0);
        glVertex2f(cx, cy);
        glColor3ub(25,25,25);
        glVertex2f(x+cx, y+cy);
        
        glColor3ub(30,30,30);
        glVertex2f(x+cx, y+cy);
        glColor3ub(40,40,40);
        glVertex2f(x+cx+length*cos(theta), y+cy+length*sin(theta));
        
        glColor3ub(30,30,30);
        glVertex2f(x+cx+length*cos(theta), y+cy+length*sin(theta));
        glColor3ub(20,20,20);
        glVertex2f(x+cx+(length+40)*cos(theta), y+cy+(length+40)*sin(theta));
        
        glColor3ub(10,10,10);
        glVertex2f(x+cx+(length+40)*cos(theta), y+cy+(length+40)*sin(theta));
        glColor3ub(0,0,0);
        glVertex2f(x+cx+(length+70)*cos(theta), y+cy+(length+70)*sin(theta));
    }
    
    glEnd();
    
}

void DrawBeat(Beat beat){
    int cx = window_W/2;
    int cy = window_H/2;
    int num_segments=500;
    
    if(beat.drawBeat==true && beat.r<=window_W/2-100){
        glColor3ub(beat.colorR,beat.colorG,beat.colorB);
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<num_segments;i++)
        {
            double theta=2.0*PI*double(i)/double(num_segments);
            double x=beat.r*cosf(theta);
            double y=beat.r*sinf(theta);
            glVertex2f(x+cx, y+cy);
        }
        glEnd();
    }
}

void DrawNote(Note &note){
    double x,y,ang;
    x=note.x;
    y=note.y;
    ang=note.ang;
    
    double wid=8;
    if(note.drawNote==true && note.x>=0 && note.x<=window_W && note.y>=0 && note.y<=window_H){
        glShadeModel(GL_SMOOTH);
        
        glPushMatrix();
        glTranslated(x, y, 0.0);
        glRotated(-ang*180/PI, 0.0, 0.0, 1.0);
        glTranslated(-x, -y, 0 );
        
        // note body
        glBegin(GL_QUADS);
        glColor3ub(note.colorR,note.colorG,note.colorB);
        glVertex2d(x+note.len*2/10,y-wid/2);
        glColor3ub(0,0,0);
        glVertex2d(x,y-wid/2);
        glColor3ub(0,0,0);
        glVertex2d(x,y+wid/2);
        glColor3ub(note.colorR,note.colorG,note.colorB);
        glVertex2d(x+note.len*2/10,y+wid/2);
        glEnd();
        
        // note head
        int cx = x+note.len*2/10;
        int cy = y;
        int num_segments=500;
        glBegin(GL_POLYGON);
        for(int i=0;i<num_segments;i++)
        {
            glColor3ub(note.colorR,note.colorG,note.colorB);
            double theta=2*PI*double(i)/double(num_segments);
            x=wid/2*cosf(theta);
            y=wid/2*sinf(theta);
            glVertex2f(x+cx, y+cy);
        }
        glEnd();
        glPopMatrix();
    }
}

double CalcAngle(double midiKeyNum){
    double angle;
    angle=2.0*PI*(midiKeyNum+1)/88;
    return angle;
}

// --------------------------------------------------------------------
double noteX, noteY, noteLen, noteAng;
int num_notes=4077;
int num_beats=4077;
Note notes[4077];
Beat beats[4077];

void Render(void *)
{
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    DrawBackground();
    DrawCircleKeyboard();
    
    for(int i=0;i<num_beats;i++){
        DrawBeat(beats[i]);
    }
    
    for(int i=0;i<num_notes;i++){
        DrawNote(notes[i]);
    }
    
    FsSwapBuffers();
}

int main(void)
{
    printf("24-780 Engineering Computation Demo\n");
    FsChangeToProgramDir();
    window_W = 1200;
    window_H = 900;
    
    FsOpenWindow(32,32,window_W,window_H,1);
    FsRegisterOnPaintCallBack(Render,nullptr);
    
    // load music
    YsSoundPlayer::SoundData theme;
    //    theme.LoadWav("lacamp.wav");
    
    if (YSOK != theme.LoadWav("lacamp.wav"))
    {
        printf("Error!  Cannot load lacamp.wav!\n");
    }
    
    FILE *fp;
    fp=fopen("lacamp.txt","r");
    
    int nWord,wordTop[16],wordLength[16];
    double noteInfo[4];
    double k=1;
    int wait_time=0;
    
    if (fp!=nullptr){
        char str[256],parsed[256];
        int counter=0;
        while(fgets(str,255,fp)!=NULL){
            str[255]=0;
            if(counter!=0){
                ParseString(nWord,wordTop,wordLength,str,16);
                for(int i=0; i<nWord; i++)
                {
                    SafeStrCpy(parsed,str+wordTop[i],wordLength[i],255);
                    if(i>0){
                        noteInfo[i-1]=std::stod(parsed);
                    }
                }
                notes[counter-1].Create(wait_time+k*int(1000*noteInfo[1]), int(8000*noteInfo[3]), CalcAngle(noteInfo[0]), noteInfo[0], int(1000*noteInfo[2]));
                notes[counter-1].SetCoord(window_W/2,window_H/2);
                if (notes[counter-1].midiNumber<=68){
                    notes[counter-1].colorR=102;
                    notes[counter-1].colorG=0;
                    notes[counter-1].colorB=204;
                }
                else{
                    notes[counter-1].colorR=240;
                    notes[counter-1].colorG=50;
                    notes[counter-1].colorB=230;
                }
            }
            counter++;
        }
    }
    fclose(fp);
    
    for(int i=0;i<num_beats;i++){
        beats[i].r=0;
        beats[i].rPrev= beats[i].r;
        beats[i].colorR=0;
        beats[i].colorG=0;
        beats[i].colorB=0;
        beats[i].t_appear=notes[i].tPlay;
        beats[i].reverseColorChange=false;
    }
    
    bool terminate=false;
    int time_counter=0;
    int loop_counter=0;
    int dt=0;
    
    player.Start();
    player.PlayOneShot(theme);
    
    FsPassedTime();
    while(terminate!=true && time_counter<222000)
    {
        dt = int(FsPassedTime());
        auto key=FsInkey();
        if(FSKEY_ESC==key)
        {
            terminate=true;
        }
        
        for(int i=0;i<num_notes;i++){
            if(time_counter>notes[i].tPlay){
                notes[i].drawNote=true;
            }
            
            if(notes[i].drawNote==true){
                notes[i].x+=moveV*cos(notes[i].ang)*dt;
                notes[i].y+=-moveV*sin(notes[i].ang)*dt;
            }
        }
        
        // change beat color
        int colorStep=10;
        for(int i=0;i<num_beats;i++){
            if(time_counter>beats[i].t_appear){
                beats[i].drawBeat=true;
            }
            if(beats[i].drawBeat==true){
                beats[i].r+=0.3*dt;
                if(beats[i].r-beats[i].rPrev>17){
                    if(beats[i].colorR<100 && beats[i].reverseColorChange==false){
                        beats[i].colorR+=colorStep+10;
                        beats[i].colorG+=colorStep+10;
                        beats[i].colorB+=colorStep+10;
                    }
                    else{
                        beats[i].reverseColorChange=true;
                        if(beats[i].colorR==0){
                            beats[i].colorR=0;
                            beats[i].colorG=0;
                            beats[i].colorB=0;
                        }
                        else{
                            beats[i].colorR-=colorStep;
                            beats[i].colorG-=colorStep;
                            beats[i].colorB-=colorStep;
                        }
                    }
                    beats[i].rPrev=beats[i].r;
                    
                }
            }
        }
        
        if(time_counter>217000){
            moveV=0.15;
        }
        
        FsPollDevice();
        FsPushOnPaintEvent();
        FsSleep(1);
        time_counter+=dt;
        loop_counter+=1;
    }
    return 0;
}

