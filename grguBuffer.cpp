//**************************************************************
//*            OpenGLide - Glide to OpenGL Wrapper
//*             http://openglide.sourceforge.net
//*
//*                    Buffer functions
//*
//*         OpenGLide is OpenSource under LGPL license
//*              Originaly made by Fabio Barros
//*      Modified by Paul for Glidos (http://www.glidos.net)
//**************************************************************

#include <stdio.h>

#include "glogl.h"

// extern variables
extern __int64          InitialTick,
                        FinalTick;
extern DWORD            Frame;
extern double           Fps, 
                        FpsAux, 
                        ClockFreq;
extern HDC              hDC;


// extern functions
void ConvertColorF( GrColor_t GlideColor, float &R, float &G, float &B, float &A );


//*************************************************
//* Clear all the Buffers
//*************************************************
DLLEXPORT void __stdcall
grBufferClear( GrColor_t color, GrAlpha_t alpha, FxU16 depth )
{
#ifdef CRITICAL
    GlideMsg( "grBufferClear( %d, %d, %d )\n", color, alpha, depth );
#endif
    static float        BR, 
                        BG, 
                        BB, 
                        BA;
    static unsigned int Bits;

    Bits = 0;
    
    RenderDrawTriangles( );

    if ( OpenGL.ColorMask )
    {
        Bits = GL_COLOR_BUFFER_BIT;
        ConvertColorF( color, BR, BG, BB, BA );
        glClearColor( BR, BG, BB, BA );
    }

    if ( Glide.State.DepthBufferWritting )
    {
        glClearDepth( depth * D1OVER65536 );
        Bits |= GL_DEPTH_BUFFER_BIT;
    }

    glClear( Bits );

#ifdef OPENGL_DEBUG
    GLErro( "grBufferClear" );
#endif
}

//*************************************************
//* Swaps Front and Back Buffers
//*************************************************
DLLEXPORT void __stdcall
grBufferSwap( int swap_interval )
{

    static float    Temp = 1.0f;

#ifdef CRITICAL
    GlideMsg( "grBufferSwap( %d )\n", swap_interval );
#endif

    RenderDrawTriangles( );
    glFlush( );

#ifdef DEBUG
    if ( OGLRender.FrameTriangles > OGLRender.MaxTriangles )
    {
        OGLRender.MaxTriangles = OGLRender.FrameTriangles;
    }
    OGLRender.FrameTriangles = 0;
#endif


//  if ( swap_interval > 0 )
//  {
//      Sleep( swap_interval * OpenGL.WaitSignal );
//  }

    SwapBuffers( hDC );
/*
    RDTSC( FinalTick );
    Temp = (float)(FinalTick - InitialTick);
    FpsAux += Temp;
    Frame++;
    RDTSC( InitialTick );
*/

#ifdef OPENGL_DEBUG
    GLErro( "grBufferSwap" );
#endif
}

//*************************************************
//* Return the number of queued buffer swap requests
//* Always 0, never pending
//*************************************************
DLLEXPORT int __stdcall
grBufferNumPending( void )
{
#ifdef DONE
    GlideMsg("grBufferNumPending()\n");
#endif

    return 0; 
}

//*************************************************
//* Defines the Buffer to Render
//*************************************************
DLLEXPORT void __stdcall
grRenderBuffer(GrBuffer_t dwBuffer)
{
#ifdef DONE
    GlideMsg("grRenderBuffer( %d )\n", dwBuffer);
#endif

    RenderDrawTriangles();

    Glide.State.RenderBuffer = dwBuffer;

    switch ( dwBuffer )
    {
    case GR_BUFFER_FRONTBUFFER:
        OpenGL.RenderBuffer = GL_FRONT;
        break;
    case GR_BUFFER_BACKBUFFER:
        OpenGL.RenderBuffer = GL_BACK;
        break;
    case GR_BUFFER_AUXBUFFER:
    case GR_BUFFER_DEPTHBUFFER:
    case GR_BUFFER_ALPHABUFFER:
    case GR_BUFFER_TRIPLEBUFFER:
        Error("Unsupported parameter in grRenderBuffer();\n");
        return;
    }

    glDrawBuffer( OpenGL.RenderBuffer );

#ifdef OPENGL_DEBUG
    GLErro( "grRenderBuffer" );
#endif
}

