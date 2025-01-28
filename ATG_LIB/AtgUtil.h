//--------------------------------------------------------------------------------------
// AtgUtil.h
//
// Helper functions and typing shortcuts for samples
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ATGUTIL_H
#define ATGUTIL_H

#include <xtl.h>
#include <xboxmath.h>
#include <stdio.h>
#include <assert.h>

namespace ATG
{

//--------------------------------------------------------------------------------------
// Debug spew and error handling routines
//--------------------------------------------------------------------------------------
VOID CDECL DebugSpew( const CHAR*, ... );  // Un-modified debug spew
VOID CDECL FatalError( const CHAR*, ... ); // Debug spew with a forced break and optional exit

// Macros for printing warnings/errors with prepended file and line numbers
#define ATG_PrintWarning ATG::DebugSpew( "%s(%d): warning: ", __FILE__, __LINE__ ), ATG::DebugSpew
#define ATG_PrintError   ATG::DebugSpew( "%s(%d): error: ",   __FILE__, __LINE__ ), ATG::DebugSpew

// Avoid compiler warnings for unused variables
#define ATG_Unused( x )   ((VOID)(x))

// Assert in debug but still execute code in release
// Useful for validating expected return values from functions
#ifdef _DEBUG
    #define ATG_Verify( e ) assert( e )
#else
    #define ATG_Verify( e ) ATG_Unused( e )
#endif    

//--------------------------------------------------------------------------------------
// Miscellaneous helper functions
//--------------------------------------------------------------------------------------

// For converting a FLOAT to a DWORD (useful for SetRenderState() calls)
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


//--------------------------------------------------------------------------------------
// Name: class Timer
// Desc: Helper class to perform timer operations
//       For stop-watch timer functionality, use:
//          Start()           - To start the timer
//          Stop()            - To stop (or pause) the timer
//          Reset()           - To reset the timer
//          GetTime()         - Returns current time or last stopped time
//
//       For app-timing and per-frame updates, use:
//          GetAbsoluteTime() - To get the absolute system time
//          GetAppTime()      - To get the running time since construction
//                              (which is usually the start of the app)
//          GetElapsedTime()  - To get the time that elapsed since the previous call
//                              GetElapsedTime() call
//          SingleStep()      - To advance the timer by a time delta
//
//       For framerate computation, use the following functions:
//          MarkFrame()       - Increments an internal frame counter
//          GetFrameRate()    - Returns a string with the current frame rate
//--------------------------------------------------------------------------------------
class Timer
{
public:
    DOUBLE   m_fLastElapsedAbsoluteTime;
    DOUBLE   m_fBaseAbsoluteTime;

    DOUBLE   m_fLastElapsedTime;
    DOUBLE   m_fBaseTime;
    DOUBLE   m_fStopTime;
    BOOL     m_bTimerStopped;

    WCHAR    m_strFrameRate[16];
    DWORD    m_dwNumFrames;
    DOUBLE   m_fLastFPSTime;

    LARGE_INTEGER m_PerfFreq;
    
    Timer()
    {
        QueryPerformanceFrequency( &m_PerfFreq );
        DOUBLE fTime = GetAbsoluteTime();

        m_fBaseAbsoluteTime         = fTime;
        m_fLastElapsedAbsoluteTime  = fTime;

        m_fBaseTime         = fTime;
        m_fStopTime         = 0.0;
        m_fLastElapsedTime  = fTime;
        m_bTimerStopped     = FALSE;
        
        m_strFrameRate[0]   = L'\0';
        m_dwNumFrames       = 0;
        m_fLastFPSTime      = fTime;
    }

    DOUBLE GetAbsoluteTime()
    {
        LARGE_INTEGER Time;
        QueryPerformanceCounter( &Time );
        DOUBLE fTime = (DOUBLE)Time.QuadPart / (DOUBLE)m_PerfFreq.QuadPart;
        return fTime;
    }

    DOUBLE GetTime()
    {
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        return ( m_fStopTime != 0.0 ) ? m_fStopTime : GetAbsoluteTime();
    }

    DOUBLE GetElapsedTime()
    {
        DOUBLE fTime = GetAbsoluteTime();

        DOUBLE fElapsedAbsoluteTime = (DOUBLE) (fTime - m_fLastElapsedAbsoluteTime);
        m_fLastElapsedAbsoluteTime = fTime;
        return fElapsedAbsoluteTime;
    }

    // Return the current time
    DOUBLE GetAppTime()
    {
        return GetTime() - m_fBaseTime;
    }

    // Reset the timer
    DOUBLE Reset()
    {
        DOUBLE fTime = GetTime();

        m_fBaseTime         = fTime;
        m_fLastElapsedTime  = fTime;
        m_fStopTime         = 0;
        m_bTimerStopped     = FALSE;
        return 0.0;
    }

    // Start the timer
    VOID Start()
    {
        DOUBLE fTime = GetAbsoluteTime();

        if( m_bTimerStopped )
            m_fBaseTime += fTime - m_fStopTime;
        m_fStopTime         = 0.0;
        m_fLastElapsedTime  = fTime;
        m_bTimerStopped     = FALSE;
    }

    // Stop the timer
    VOID Stop()
    {
        DOUBLE fTime = GetTime();

        if( !m_bTimerStopped )
        {
            m_fStopTime         = fTime;
            m_fLastElapsedTime  = fTime;
            m_bTimerStopped     = TRUE;
        }
    }

    // Advance the timer by 1/10th second
    VOID SingleStep( DOUBLE fTimeAdvance )
    {
        m_fStopTime += fTimeAdvance;
    }

    VOID MarkFrame()
    {
        m_dwNumFrames++;
    }

    WCHAR* GetFrameRate()
    {
        DOUBLE fTime = GetAbsoluteTime();

        // Only re-compute the FPS (frames per second) once per second
        if( fTime - m_fLastFPSTime > 1.0 )
        {
            DOUBLE fFPS    = m_dwNumFrames / ( fTime - m_fLastFPSTime );
            m_fLastFPSTime = fTime;
            m_dwNumFrames  = 0L;
            swprintf_s( m_strFrameRate, L"%0.02f fps", (FLOAT)fFPS );
        }
        return m_strFrameRate;
    }
};


//--------------------------------------------------------------------------------------
// Name: VectorToRGBA()
// Desc: Converts a normal into an RGBA vector
//--------------------------------------------------------------------------------------
inline D3DCOLOR VectorToRGBA( const XMVECTOR vec, FLOAT fHeight = 1.0f )
{
    D3DCOLOR r = (D3DCOLOR)( ( vec.x + 1.0f ) * 127.5f );
    D3DCOLOR g = (D3DCOLOR)( ( vec.y + 1.0f ) * 127.5f );
    D3DCOLOR b = (D3DCOLOR)( ( vec.z + 1.0f ) * 127.5f );
    D3DCOLOR a = (D3DCOLOR)( 255.0f * fHeight );
    return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
}


//--------------------------------------------------------------------------------------
// Name: VectorToQWVU()
// Desc: Converts a normal into a signed QWVU vector
//--------------------------------------------------------------------------------------
inline D3DCOLOR VectorToQWVU( const XMVECTOR vec, FLOAT fHeight = 1.0f )
{
    LONG u = LONG( vec.x * 127.5f ) & 0xFF;
    LONG v = LONG( vec.y * 127.5f ) & 0xFF;
    LONG w = LONG( vec.z * 127.5f ) & 0xFF;
    LONG q = LONG( 127.5f * fHeight ) & 0xFF;
    return( (q<<24L) | (w<<16L) | (v<<8L) | (u<<0L) );
}


//--------------------------------------------------------------------------------------
// Name: GetVideoSettings()
// Desc: Return various display settings
//--------------------------------------------------------------------------------------
VOID GetVideoSettings( UINT* pdwDisplayWidth=NULL, UINT* pdwDisplayHeight=NULL,
                       BOOL* pbWidescreen=NULL );


//--------------------------------------------------------------------------------------
// Name: GetTitleSafeArea()
// Desc: Returns the title safe area for the given display mode
//--------------------------------------------------------------------------------------
D3DRECT GetTitleSafeArea();


//--------------------------------------------------------------------------------------
// Name: CreateNormalizationCubeMap()
// Desc: Creates a cubemap and fills it with normalized RGBA vectors
//--------------------------------------------------------------------------------------
HRESULT CreateNormalizationCubeMap( DWORD dwSize, D3DCubeTexture** ppCubeMap );


//--------------------------------------------------------------------------------------
// Load/save binary files
//--------------------------------------------------------------------------------------
HRESULT SaveFile( const CHAR* strFileName, VOID* pFileData, DWORD dwFileSize );
HRESULT LoadFile( const CHAR* strFileName, VOID** ppFileData,
                          DWORD* pdwFileSize = NULL );
VOID    UnloadFile( VOID* pFileData );

HRESULT LoadFilePhysicalMemory( const CHAR* strFileName, VOID** ppFileData,
                          DWORD* pdwFileSize = NULL, DWORD dwAlignment = 0 );
VOID    UnloadFilePhysicalMemory( VOID* pFileData );


//--------------------------------------------------------------------------------------
// Name: BuildVertexDeclFromFVF()
// Desc: Returns a vertex declaration element array for an FVF code
//--------------------------------------------------------------------------------------
VOID BuildVertexDeclFromFVF( DWORD dwFVF, D3DVERTEXELEMENT9* pDecl );


//--------------------------------------------------------------------------------------
// Name: RenderBackground()
// Desc: Draws a gradient filled background
//--------------------------------------------------------------------------------------
VOID RenderBackground( DWORD dwTopColor, DWORD dwBottomColor );


//--------------------------------------------------------------------------------------
// Name: Load*Shader()
// Desc: Loads and creates shaders from file
//--------------------------------------------------------------------------------------
HRESULT LoadVertexShader( const CHAR* strFileName, LPDIRECT3DVERTEXSHADER9* ppVS );
HRESULT LoadPixelShader( const CHAR* strFileName, LPDIRECT3DPIXELSHADER9* ppPS );


//--------------------------------------------------------------------------------------
// Name: AppendVertexElements()
// Desc: Helper function for building an array of vertex elements.
//--------------------------------------------------------------------------------------
VOID AppendVertexElements( D3DVERTEXELEMENT9* pDstElements, DWORD dwSrcStream, 
                           D3DVERTEXELEMENT9* pSrcElements, DWORD dwSrcUsageIndex,
                           DWORD dwSrcOffset=0 );

//--------------------------------------------------------------------------------------
// Name: SetThreadName()
// Desc: Set the name of the given thread so that it will show up in the Threads Window
//       in Visual Studio
//--------------------------------------------------------------------------------------
VOID SetThreadName( DWORD dwThreadID, LPCSTR strThreadName );

} // namespace ATG

#endif // ATGUTIL_H
