//**************************************************************
//*            OpenGLide - Glide to OpenGL Wrapper
//*             http://openglide.sourceforge.net
//*
//*     Linux specific functions for library initialisation
//*
//*         OpenGLide is OpenSource under LGPL license
//*              Originaly made by Fabio Barros
//*      Modified by Paul for Glidos (http://www.glidos.net)
//**************************************************************


#include <stdlib.h>

class InitLibrary
{
public:
    InitLibrary( )
    {
        if ( !ClearAndGenerateLogFile( ) )
        {
            exit( 0 );
        }
        InitMainVariables( );            
    }
    ~InitLibrary( )
    {
        grGlideShutdown( );
        CloseLogFile( );
    }
};

static InitLibrary initLibrary;

