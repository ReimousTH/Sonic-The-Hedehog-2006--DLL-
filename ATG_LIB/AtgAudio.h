//--------------------------------------------------------------------------------------
// AtgAudio.h
//
// Simple WAV file reader, XMA file writer and other audio utilities.  This file is 
// based off of the AtgAudio.h file in the samples common framework.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ATGAUDIO_H
#define ATGAUDIO_H

#include <XAudDefs.h>

namespace ATG
{

//--------------------------------------------------------------------------------------
// Misc type definitions
//--------------------------------------------------------------------------------------
typedef DWORD FOURCC, *PFOURCC, *LPFOURCC;


//--------------------------------------------------------------------------------------
// Format tags
//--------------------------------------------------------------------------------------
#define WAVE_FORMAT_PCM                     1
#define WAVE_FORMAT_EXTENSIBLE              0xFFFE


//--------------------------------------------------------------------------------------
// For initializing XAudio voices
//--------------------------------------------------------------------------------------
extern XAUDIOVOICEOUTPUT DefaultStereoVoiceOutput;
extern XAUDIOVOICEOUTPUT DefaultSurroundVoiceOutput;



//--------------------------------------------------------------------------------------
// For parsing WAV files
//--------------------------------------------------------------------------------------
#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_

typedef struct 
{
    WAVEFORMATEX    Format;                 // WAVEFORMATEX data

    union 
    {
        WORD        wValidBitsPerSample;    // Bits of precision
        WORD        wSamplesPerBlock;       // Samples per block of audio data
        WORD        wReserved;              // Unused -- must be 0
    } Samples;

    DWORD           dwChannelMask;          // Channel usage bitmask
    GUID            SubFormat;              // Sub-format identifier
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE, *LPWAVEFORMATEXTENSIBLE;

typedef const WAVEFORMATEXTENSIBLE *LPCWAVEFORMATEXTENSIBLE;

#endif // _WAVEFORMATEXTENSIBLE_


//--------------------------------------------------------------------------------------
// For parsing WAV files
//--------------------------------------------------------------------------------------
struct RIFFHEADER
{
    FOURCC  fccChunkId;
    DWORD   dwDataSize;
};

#define RIFFCHUNK_FLAGS_VALID   0x00000001


//--------------------------------------------------------------------------------------
// Name: class RiffChunk
// Desc: RIFF chunk utility class
//--------------------------------------------------------------------------------------
class RiffChunk
{
    FOURCC            m_fccChunkId;       // Chunk identifier
    const RiffChunk* m_pParentChunk;     // Parent chunk
    HANDLE            m_hFile;
    DWORD             m_dwDataOffset;     // Chunk data offset
    DWORD             m_dwDataSize;       // Chunk data size
    DWORD             m_dwFlags;          // Chunk flags

public:
    RiffChunk();

    // Initialization
    VOID    Initialize( FOURCC fccChunkId, const RiffChunk* pParentChunk,
                        HANDLE hFile );
    HRESULT Open();
    BOOL    IsValid() const { return !!(m_dwFlags & RIFFCHUNK_FLAGS_VALID); }

    // Data
    HRESULT ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize, OVERLAPPED* pOL ) const;

    // Chunk information
    FOURCC  GetChunkId() const  { return m_fccChunkId; }
    DWORD   GetDataSize() const { return m_dwDataSize; }

private:
    // prevent copying so that we don't have to duplicate file handles
    RiffChunk( const RiffChunk& );
    RiffChunk& operator =( const RiffChunk& );
};

//--------------------------------------------------------------------------------------
// Name: class WaveFile
// Desc: Wave file utility class
//--------------------------------------------------------------------------------------
class WaveFile
{
    HANDLE     m_hFile;            // File handle
    RiffChunk  m_RiffChunk;        // RIFF chunk
    RiffChunk  m_FormatChunk;      // Format chunk
    RiffChunk  m_DataChunk;        // Data chunk
    RiffChunk  m_WaveSampleChunk;  // Wave Sample chunk
    RiffChunk  m_SamplerChunk;     // Sampler chunk
    
public:
    WaveFile();
    ~WaveFile();

    // Initialization
    HRESULT Open( const CHAR* strFileName );
    VOID    Close();

    // File format
    HRESULT GetFormat( WAVEFORMATEXTENSIBLE* pwfxFormat ) const;

    // File data
    HRESULT ReadSample( DWORD dwPosition, VOID* pBuffer, DWORD dwBufferSize, 
                        DWORD* pdwRead ) const;

    // Loop region
    HRESULT GetLoopRegion( DWORD* pdwStart, DWORD* pdwLength ) const;
    HRESULT GetLoopRegionBytes( DWORD *pdwStart, DWORD* pdwLength ) const;

    // File properties
    VOID    GetDuration( DWORD* pdwDuration ) const { *pdwDuration = m_DataChunk.GetDataSize(); }
    DWORD   Duration() const { return m_DataChunk.GetDataSize(); }

private:
    // prevent copying so that we don't have to duplicate file handles
    WaveFile( const WaveFile& );
    WaveFile& operator =( const WaveFile& );
};

//--------------------------------------------------------------------------------------
// Name: class XMAFile
// Desc: XMA file utility class (for use on the PC)
//--------------------------------------------------------------------------------------
class XMAFile
{
    HANDLE     m_hFile;            // File handle
    RiffChunk  m_RiffChunk;        // RIFF chunk
    RiffChunk  m_FormatChunk;      // Format chunk
    RiffChunk  m_DataChunk;        // Data chunk
    RiffChunk  m_WaveChunk;        // Wave chunk
    RiffChunk  m_SeekTableChunk;   // Seek table chunk
    RiffChunk  m_Xma2FormatChunk;  // XMA2 format (if available)

public:
    XMAFile();
    ~XMAFile();

    // Initialization
    HRESULT Open( const CHAR* strFileName, bool async = false ); 

    // File format
    XMAWAVEFORMAT*  GetFormat() const { return m_pFormat; }
    XMA2WAVEFORMAT* GetXma2Format() const { return m_pFormat2; }
    DWORD*          GetSeekTable() const { return m_pSeekTable; }

    // legacy version of GetFormat() -- not as safe as new version
     HRESULT GetFormat( XMAWAVEFORMAT* ) const;


    // File data
    HRESULT ReadSample( DWORD dwPosition, VOID* pBuffer, DWORD dwBufferSize, 
                        DWORD* pdwRead, OVERLAPPED* pOL = NULL ) const;
    HRESULT GetAsyncResult( OVERLAPPED* pOvl, DWORD* pdwBytesRead ) const;

    // File properties
    VOID    GetDuration( DWORD* pdwDuration ) const { *pdwDuration = m_DataChunk.GetDataSize(); }
    DWORD   Duration() const { return m_DataChunk.GetDataSize(); }

    VOID Close();

private:
    // prevent copying so that we don't have to duplicate file handles
    XMAFile( const XMAFile& );
    XMAFile& operator =( const XMAFile& );
    XMAWAVEFORMAT*  m_pFormat;
    XMA2WAVEFORMAT* m_pFormat2;
    DWORD*          m_pSeekTable;
};

//--------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------
HRESULT CreateSourceVoiceInit( const XMAFile* pXmaFile, XAUDIOSOURCEVOICEINIT* ppSourceVoice );
HRESULT SampleToBlockOffset( 
                            DWORD dwSampleIndex, 
                            const DWORD* pdwSeekTable, 
                            DWORD nEntries, 
                            DWORD* out_pBlockIndex, 
                            DWORD* out_pOffset );
void SwapXmaFormat( XMAWAVEFORMAT* pxmaFormat );

} // namespace ATG



#endif // ATGAUDIO_H
