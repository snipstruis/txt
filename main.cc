#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <cstring>
#include <cstdio>
#include <chrono>
#include "mesh.h"

int readFile(char const * filename, unsigned char** out){
    FILE* fp = fopen(filename,"r");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    *out = (unsigned char*)malloc(size + 1);
    fread(*out, size, 1, fp);
    (*out)[size]=0;
    fclose(fp);
    return size;
}

int main(int argc, char** argv){
    char const* font_file = argc>1 ? argv[1] : "fonts/LibreBaskerville-Italic.ttf";
    char const* text_file = argc>2 ? argv[2] : "text/80days.txt";

    // load textfile
    unsigned char* text;
    readFile(text_file,&text);
    
    // init window
    SDL_Window *win = SDL_CreateWindow("font rendering test", 0, 0, 640, 480, 
                                       SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GL_CreateContext(win);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1); //unaligned RGB values
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // load font
    unsigned char* ttf;
    readFile(font_file,&ttf);
    stbtt_fontinfo font;
    stbtt_InitFont(&font, ttf, stbtt_GetFontOffsetForIndex(ttf,0));

    unsigned char* glyph = (unsigned char*)malloc(1<<12);


    // run for 240 frames
    for(int t=0; t<240; t++){
        auto t_begin = std::chrono::high_resolution_clock::now();
        // handle events
        for(SDL_Event e;SDL_PollEvent(&e);){ if(e.type==SDL_QUIT) return 0; }

        // clear
        int w,h; SDL_GL_GetDrawableSize(win,&w,&h);
        glClearColor(1,1,0.8,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, w, h);

        // get font properties at current size
        float px = 45.0-(t/6);
        float scale = stbtt_ScaleForPixelHeight(&font,px);
        int ascent,descent,linegap; stbtt_GetFontVMetrics(&font,&ascent,&descent,&linegap);
        float baseline = (float)ascent*scale;
        float x=-0.4*w, y=0.4*h-baseline;

#if 0   // draw base line
        glBegin(GL_LINES);
        glColor3f(1,0,0);
        for(int i=0;i<20;i++){
            glVertex2f(      x/(w/2.f),(y-(float)i*(px+linegap))/(h/2.f));
            glVertex2f((0.4*w)/(w/2.f),(y-(float)i*(px+linegap))/(h/2.f));
        }
        glEnd();
#endif
        auto d_bitmap=std::chrono::duration<long,std::nano>(0);
        auto d_screen=std::chrono::duration<long,std::nano>(0);
        // loop through characters
        for(unsigned char const *c=text;*c;c++){
            // bounding box (in pixels) of the glyph
            float x_shift=x-floor(x), y_shift=floor(y)-y;
            int x0,y0,x1,y1;
            stbtt_GetCodepointBitmapBoxSubpixel(&font,*c,scale,scale,
                    x_shift,y_shift,&x0,&y0,&x1,&y1);

            // calculate GL-coordinates of the bounding box of the glyph
            // remember: the subpixel offset is already baked into the bitmap
            float ax=(floor(x)+(float)x0)/(w/2.f), // upper left
                  ay=(floor(y)-(float)y0)/(h/2.f),
                  bx=(floor(x)+(float)x1)/(w/2.f), // lower right
                  by=(floor(y)-(float)y1)/(h/2.f);

            // don't draw on bottom margin
            if((y/(h/2.f))<-0.8) break;

            // don't draw on right margin
            if(bx>0.8){
                x=-0.4*w; 
                y-=px+linegap;
                c--;
                continue;
            }

            // handle newline
            if(*c=='\n'){
                x=-0.4*w; 
                y-=1.5*(px+linegap);
                continue;
            }

#if 1       // draw bounding boxes
            glBegin(GL_QUADS);
            glColor4f(0,1,1,0.5);
            glVertex2f(ax,ay);
            glVertex2f(bx,ay);
            glVertex2f(bx,by);
            glVertex2f(ax,by);
            glEnd();
#endif

            auto t_a = std::chrono::high_resolution_clock::now();

            // render glyph to bitmap
            int box_w=x1-x0, box_h=y1-y0;
            memset(glyph,0,box_w*box_h);
            stbtt_MakeCodepointBitmapSubpixel(&font,glyph,box_w,box_h,box_w,
                    scale,scale, x_shift,y_shift,*c);

            auto t_b = std::chrono::high_resolution_clock::now();

            // draw bitmap to screen
            glRasterPos2f(ax,ay);
            glPixelZoom(1,-1);
            glDrawPixels(box_w,box_h,GL_ALPHA,GL_UNSIGNED_BYTE,glyph);

            auto t_c = std::chrono::high_resolution_clock::now();
            d_bitmap += t_b-t_a;
            d_screen += t_c-t_b;

            // advance the x position with the correct ammount
            int advance; 
            stbtt_GetCodepointHMetrics(&font, *c, &advance,NULL);
            x+=advance*scale;
            if(c[1]) x+=scale*stbtt_GetCodepointKernAdvance(&font,c[0],c[1]);
        }

        auto t_end = std::chrono::high_resolution_clock::now();
        long frametime = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end-t_begin).count();

        printf("%3d:\tb:%ld\ts:%ld\tt:%ld\tbt:%f\tst:%f\n",
                t, d_bitmap.count(), d_screen.count(), frametime,
                double(d_bitmap.count())/double(frametime), double(d_screen.count())/double(frametime));
        
        SDL_GL_SwapWindow(win);
    }
}
