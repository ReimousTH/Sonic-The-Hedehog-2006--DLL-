//--------------------------------------------------------------------------------------
// AtgAudio.cpp
//
// Simple WAV file reader, XMA file writer and other audio utilities.  This file is 
// based off of the AtgAudio.h file in the samples common framework.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "AtgAudio.h"
#include "AtgUtil.h"

namespace ATG
{

XAUDIOCHANNELMAPENTRY DefaultChannelMapEntries[] =
{
    { 0, 0, 1.0f },
    { 1, 1, 1.0f },
    { 2, 2, 1.0f },
    { 3, 3, 1.0f },
    { 4, 4, 1.0f },
    { 5, 5, 1.0f }
};
XAUDIOCHANNELMAP DefaultStereoChannelMap = { 2, DefaultChannelMapEntries };
XAUDIOCHANNELMAP DefaultSurroundChannelMap = { 6, DefaultChannelMapEntries };

XAUDIOVOICEOUTPUTENTRY DefaultStereoVoiceOutputEntry = { NULL, &DefaultStereoChannelMap };
XAUDIOVOICEOUTPUTENTRY DefaultSurroundVoiceOutputEntry = { NULL, &DefaultSurroundChannelMap };

XAUDIOVOICEOUTPUT DefaultStereoVoiceOutput = { 1, &DefaultStereoVoiceOutputEntry };
XAUDIOVOICEOUTPUT DefaultSurroundVoiceOutput = { 1, &DefaultSurroundVoiceOutputEntry };


//--------------------------------------------------------------------------------------
// FourCC definitions
//--------------------------------------------------------------------------------------
const DWORD ATG_FOURCC_RIFF   = 'RIFF';
const DWORD ATG_FOURCC_WAVE   = 'WAVE';
const DWORD ATG_FOURCC_FORMAT = 'fmt ';
const DWORD ATG_FOURCC_DATA   = 'data';
const DWORD ATG_FOURCC_WSMP   = 'wsmp';
const DWORD ATG_FOURCC_SMPL   = 'lsmp';
const DWORD ATG_FOURCC_SEEK   = 'seek';
const DWORD ATG_FOURCC_XMA2   = 'XMA2';


//--------------------------------------------------------------------------------------
// RIFF chunk type that contains loop point information
//--------------------------------------------------------------------------------------
struct WAVESAMPLE
{
    DWORD   dwSize;
    WORD    UnityNote;
    SHORT   FineTune;
    LONG    Gain;
    DWORD   dwOptions;
    DWORD   dwSampleLoops;
};


//--------------------------------------------------------------------------------------
// Loop point (contained in WSMP chunk)
//--------------------------------------------------------------------------------------
struct WAVESAMPLE_LOOP
{
    DWORD dwSize;
    DWORD dwLoopType;
    DWORD dwLoopStart;
    DWORD dwLoopLength;
};


//--------------------------------------------------------------------------------------
// RIFF chunk that may contain loop point information
//--------------------------------------------------------------------------------------
struct SAMPLER
{
    DWORD dwManufacturer;
    DWORD dwProduct;
    DWORD dwSamplePeriod;
    DWORD dwMIDIUnityNote;
    DWORD dwMIDIPitchFraction;
    DWORD dwSMPTEFormat;
    DWORD dwSMPTEOffset;
    DWORD dwNumSampleLoops;
    DWORD dwSamplerData;
};


//--------------------------------------------------------------------------------------
// Loop point contained in SMPL chunk
//--------------------------------------------------------------------------------------
struct SAMPLER_LOOP
{
    DWORD dwCuePointID;
    DWORD dwType;
    DWORD dwStart;
    DWORD dwEnd;
    DWORD dwFraction;
    DWORD dwPlayCount;
};


//--------------------------------------------------------------------------------------
// Name: RiffChunk()
// Desc: Constructor
//--------------------------------------------------------------------------------------
RiffChunk::RiffChunk()
{
    // Initialize defaults
    m_fccChunkId   = 0;
    m_pParentChunk = NULL;
    m_hFile        = INVALID_HANDLE_VALUE;
    m_dwDataOffset = 0;
    m_dwDataSize   = 0;
    m_dwFlags      = 0;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initializes the Riff chunk for use
//--------------------------------------------------------------------------------------
VOID RiffChunk::Initialize( FOURCC fccChunkId, const RiffChunk* pParentChunk, 
                             HANDLE hFile )
{
    m_fccChunkId   = fccChunkId;
    m_pParentChunk = pParentChunk;
    m_hFile        = hFile;
}


//--------------------------------------------------------------------------------------
// Name: Open()
// Desc: Opens an existing chunk
//--------------------------------------------------------------------------------------
HRESULT RiffChunk::Open()
{
    LONG lOffset = 0;

    // Seek to the first byte of the parent chunk's data section
    if( m_pParentChunk )
    {
        lOffset = m_pParentChunk->m_dwDataOffset;

        // Special case the RIFF chunk
        if( ATG_FOURCC_RIFF == m_pParentChunk->m_fccChunkId )
            lOffset += sizeof(FOURCC);
    }

    // Read each child chunk header until we find the one we're looking for
    for( ;; )
    {
        if( INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, lOffset, NULL, FILE_BEGIN ) )
            return HRESULT_FROM_WIN32( GetLastError() );

        RIFFHEADER rhRiffHeader;
        DWORD dwRead;
        if( 0 == ReadFile( m_hFile, &rhRiffHeader, sizeof(rhRiffHeader), &dwRead, NULL ) )
            return HRESULT_FROM_WIN32( GetLastError() );
        rhRiffHeader.dwDataSize = __loadwordbytereverse( 0, &rhRiffHeader.dwDataSize);

        // Hit EOF without finding it
        if( 0 == dwRead )
            return E_FAIL;

        // Check if we found the one we're looking for
        if( m_fccChunkId == rhRiffHeader.fccChunkId )
        {
            // Save the chunk size and data offset
            m_dwDataOffset = lOffset + sizeof(rhRiffHeader);
            m_dwDataSize   = rhRiffHeader.dwDataSize;

            // Success
            m_dwFlags |= RIFFCHUNK_FLAGS_VALID;

            return S_OK;
        }

        lOffset += sizeof(rhRiffHeader) + rhRiffHeader.dwDataSize;
    }
}


//--------------------------------------------------------------------------------------
// Name: ReadData()
// Desc: Reads from the file
//--------------------------------------------------------------------------------------
HRESULT RiffChunk::ReadData( LONG lOffset, VOID* pData, DWORD dwDataSize, OVERLAPPED* pOL ) const
{
    HRESULT hr = S_OK;

    OVERLAPPED defaultOL = {0};
    OVERLAPPED* pOverlapped = pOL;
    if( !pOL )
    {
        pOverlapped = &defaultOL;
    } 

    // Seek to the offset
    pOverlapped->Offset = m_dwDataOffset + lOffset;

    // Read from the file
    DWORD dwRead;
    if( SUCCEEDED(hr) && 0 == ReadFile( m_hFile, pData, dwDataSize, &dwRead, pOverlapped ) )
        hr = HRESULT_FROM_WIN32( GetLastError() );

    if( SUCCEEDED(hr) && !pOL )
    {
        // we're using the default overlapped structure, which means that even if the
        // read was async, we need to act like it was synchronous.
        if( !GetOverlappedResult( m_hFile, pOverlapped, &dwRead, TRUE ) )
            hr = HRESULT_FROM_WIN32( GetLastError() );
    }
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: WaveFile()
// Desc: Constructor
//--------------------------------------------------------------------------------------
WaveFile::WaveFile()
{
    m_hFile = INVALID_HANDLE_VALUE;
}


//--------------------------------------------------------------------------------------
// Name: ~WaveFile()
// Desc: Denstructor
//--------------------------------------------------------------------------------------
WaveFile::~WaveFile()
{
    Close();
}


//--------------------------------------------------------------------------------------
// Name: Open()
// Desc: Initializes the object
//--------------------------------------------------------------------------------------
HRESULT WaveFile::Open( const CHAR* strFileName )
{
    // If we're already open, close
    Close();

    // Open the file
    m_hFile = CreateFile( 
        strFileName, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL );

    if( INVALID_HANDLE_VALUE == m_hFile )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Initialize the chunk objects
    m_RiffChunk.Initialize( ATG_FOURCC_RIFF, NULL, m_hFile );
    m_FormatChunk.Initialize( ATG_FOURCC_FORMAT, &m_RiffChunk, m_hFile );
    m_DataChunk.Initialize( ATG_FOURCC_DATA, &m_RiffChunk, m_hFile );
    m_WaveSampleChunk.Initialize( ATG_FOURCC_WSMP, &m_RiffChunk, m_hFile );
    m_SamplerChunk.Initialize( ATG_FOURCC_SMPL, &m_RiffChunk, m_hFile );

    HRESULT hr = m_RiffChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_FormatChunk.Open();
    if( FAILED(hr) )
        return hr;

    hr = m_DataChunk.Open();
    if( FAILED(hr) )
        return hr;

    // Wave Sample and Sampler chunks are not required
    m_WaveSampleChunk.Open();
    m_SamplerChunk.Open();

    // Validate the file type
    FOURCC fccType;
    hr = m_RiffChunk.ReadData( 0, &fccType, sizeof(fccType), NULL );
    if( FAILED(hr) )
        return hr;

    if( ATG_FOURCC_WAVE != fccType )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: GetFormat()
// Desc: Gets the wave file format.  Since Xbox only supports WAVE_FORMAT_PCM,
//       WAVE_FORMAT_XBOX_ADPCM, and WAVE_FORMAT_EXTENSIBLE, we know any
//       valid format will fit into a WAVEFORMATEXTENSIBLE struct
//--------------------------------------------------------------------------------------
HRESULT WaveFile::GetFormat( WAVEFORMATEXTENSIBLE* pwfxFormat ) const
{
    assert( pwfxFormat );
    DWORD dwValidSize = m_FormatChunk.GetDataSize();

    // Anything larger than WAVEFORMATEXTENSIBLE is not a valid Xbox WAV file
    assert( dwValidSize <= sizeof(WAVEFORMATEXTENSIBLE) );

    // Read the format chunk into the buffer
    HRESULT hr = m_FormatChunk.ReadData( 0, pwfxFormat, dwValidSize, NULL );
    if( FAILED(hr) )
        return hr;

    // Endianness conversion
    pwfxFormat->Format.wFormatTag       = __loadshortbytereverse( 0, &pwfxFormat->Format.wFormatTag );
    pwfxFormat->Format.nChannels        = __loadshortbytereverse( 0, &pwfxFormat->Format.nChannels );
    pwfxFormat->Format.nSamplesPerSec   = __loadwordbytereverse( 0, &pwfxFormat->Format.nSamplesPerSec );
    pwfxFormat->Format.nAvgBytesPerSec  = __loadwordbytereverse( 0, &pwfxFormat->Format.nAvgBytesPerSec );
    pwfxFormat->Format.nBlockAlign      = __loadshortbytereverse( 0, &pwfxFormat->Format.nBlockAlign );
    pwfxFormat->Format.wBitsPerSample   = __loadshortbytereverse( 0, &pwfxFormat->Format.wBitsPerSample );
    pwfxFormat->Format.cbSize           = __loadshortbytereverse( 0, &pwfxFormat->Format.cbSize );
    pwfxFormat->Samples.wReserved       = __loadshortbytereverse( 0, &pwfxFormat->Samples.wReserved );
    pwfxFormat->dwChannelMask           = __loadwordbytereverse( 0, &pwfxFormat->dwChannelMask );
    pwfxFormat->SubFormat.Data1         = __loadwordbytereverse( 0, &pwfxFormat->SubFormat.Data1 );
    pwfxFormat->SubFormat.Data2         = __loadshortbytereverse( 0, &pwfxFormat->SubFormat.Data2 );
    pwfxFormat->SubFormat.Data3         = __loadshortbytereverse( 0, &pwfxFormat->SubFormat.Data3 );
    // Data4 is a array of char, not needed to convert

    // Zero out remaining bytes, in case enough bytes were not read
    if( dwValidSize < sizeof(WAVEFORMATEXTENSIBLE) )
        ZeroMemory( (BYTE*)pwfxFormat + dwValidSize, sizeof(WAVEFORMATEXTENSIBLE) - dwValidSize );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: ReadSample()
// Desc: Reads data from the audio file.
//--------------------------------------------------------------------------------------
HRESULT WaveFile::ReadSample( DWORD dwPosition, VOID* pBuffer, 
                               DWORD dwBufferSize, DWORD* pdwRead ) const
{
    // Don't read past the end of the data chunk
    DWORD dwDuration;
    GetDuration( &dwDuration );

    // Check bit size for endinaness conversion.
    WAVEFORMATEXTENSIBLE wfxFormat;
    GetFormat( &wfxFormat );

    if( dwPosition + dwBufferSize > dwDuration )
        dwBufferSize = dwDuration - dwPosition;

    HRESULT hr = S_OK;
    if( dwBufferSize )
        hr = m_DataChunk.ReadData( (LONG)dwPosition, pBuffer, dwBufferSize, NULL );

    //Endianness conversion
    if( wfxFormat.Format.wFormatTag == WAVE_FORMAT_PCM && wfxFormat.Format.wBitsPerSample == 16 )
    {
        SHORT* pBufferShort = (SHORT*)pBuffer;
        for( DWORD i=0; i< dwBufferSize / sizeof(SHORT); i++ )
            pBufferShort[i]  = __loadshortbytereverse( 0, &pBufferShort[i] );
    }

    if( pdwRead )
        *pdwRead = dwBufferSize;

    return hr;
}


//--------------------------------------------------------------------------------------
// Name: GetLoopRegion()
// Desc: Gets the loop region, in terms of samples
//--------------------------------------------------------------------------------------
HRESULT WaveFile::GetLoopRegion( DWORD* pdwStart, DWORD* pdwLength ) const
{
    assert( pdwStart != NULL );
    assert( pdwLength != NULL );
    HRESULT hr = S_OK;

    *pdwStart  = 0;
    *pdwLength = 0;

    // First, look for a MIDI-style SMPL chunk, then for a DLS-style WSMP chunk.
    BOOL bGotLoopRegion = FALSE;
    if( !bGotLoopRegion && m_SamplerChunk.IsValid() )
    {
        // Read the SAMPLER struct from the chunk
        SAMPLER smpl;
        hr = m_SamplerChunk.ReadData( 0, &smpl, sizeof(SAMPLER), NULL );
        if( FAILED( hr ) )
            return hr;

        //Endianness conversion
        LONG* l = (LONG*)&smpl;
        for( INT i=0; i < sizeof(SAMPLER)/sizeof(LONG); i++ )
            *l++ = __loadwordbytereverse( i, &smpl );


        // Check if the chunk contains any loop regions
        if( smpl.dwNumSampleLoops > 0 )
        {
            SAMPLER_LOOP smpl_loop;
            hr = m_SamplerChunk.ReadData( sizeof(SAMPLER), &smpl_loop, sizeof(SAMPLER_LOOP), NULL );
            if( FAILED( hr ) )
                return E_FAIL;

            //Endianness conversion
            LONG* l = (LONG*)&smpl_loop;
            for( INT i=0; i < sizeof(SAMPLER_LOOP)/sizeof(LONG); i++ )
                *l++ = __loadwordbytereverse( i, &smpl_loop );

            // Documentation on the SMPL chunk indicates that dwStart and
            // dwEnd are stored as byte-offsets, rather than sample counts,
            // but SoundForge stores sample counts, so we'll go with that.
            *pdwStart  = smpl_loop.dwStart;
            *pdwLength = smpl_loop.dwEnd - smpl_loop.dwStart + 1;
            bGotLoopRegion = TRUE;
        }
    }

    if( !bGotLoopRegion && m_WaveSampleChunk.IsValid() )
    {
        // Read the WAVESAMPLE struct from the chunk
        WAVESAMPLE ws;
        hr = m_WaveSampleChunk.ReadData( 0, &ws, sizeof(WAVESAMPLE), NULL );
        if( FAILED( hr ) )
            return hr;

        // Endianness conversion
        ws.dwSize        = __loadwordbytereverse( 0, &ws.dwSize );
        ws.UnityNote     = __loadshortbytereverse( 0, &ws.UnityNote );
        ws.FineTune      = __loadshortbytereverse( 0, &ws.FineTune );
        ws.Gain          = __loadwordbytereverse( 0, &ws.Gain );
        ws.dwOptions     = __loadwordbytereverse( 0, &ws.dwOptions );
        ws.dwSampleLoops = __loadwordbytereverse( 0, &ws.dwSampleLoops );

        // Check if the chunk contains any loop regions
        if( ws.dwSampleLoops > 0 )
        {
            // Read the loop region
            WAVESAMPLE_LOOP wsl;
            hr = m_WaveSampleChunk.ReadData( ws.dwSize, &wsl, sizeof(WAVESAMPLE_LOOP), NULL );
            if( FAILED( hr ) )
                return hr;

            //Endianness conversion
            LONG* l = (LONG*)&wsl; 
            for( INT i=0; i < sizeof(WAVESAMPLE_LOOP)/sizeof(LONG); i++ )
                *l++ = __loadwordbytereverse( i, &wsl );

            // Fill output vars with the loop region
            *pdwStart = wsl.dwLoopStart;
            *pdwLength = wsl.dwLoopLength;
            bGotLoopRegion = TRUE;
        }
    }

    // Couldn't find either chunk, or they didn't contain loop points
    if( !bGotLoopRegion )
        return E_FAIL;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: GetLoopRegionBytes()
// Desc: Gets the loop region, in terms of bytes
//--------------------------------------------------------------------------------------
HRESULT WaveFile::GetLoopRegionBytes( DWORD* pdwStart, DWORD* pdwLength ) const
{
    assert( pdwStart != NULL );
    assert( pdwLength != NULL );

    // If no loop region is explicitly specified, loop the entire file
    *pdwStart  = 0;
    GetDuration( pdwLength );

    // We'll need the wave format to convert from samples to bytes
    WAVEFORMATEXTENSIBLE wfx;
    if( FAILED( GetFormat( &wfx ) ) )
        return E_FAIL;

    // See if the file contains an embedded loop region
    DWORD dwLoopStartSamples;
    DWORD dwLoopLengthSamples;
    if( FAILED( GetLoopRegion( &dwLoopStartSamples, &dwLoopLengthSamples ) ) )
        return S_FALSE;

    // For PCM, multiply by bytes per sample
    DWORD cbBytesPerSample = wfx.Format.nChannels * wfx.Format.wBitsPerSample / 8;
    *pdwStart  = dwLoopStartSamples  * cbBytesPerSample;
    *pdwLength = dwLoopLengthSamples * cbBytesPerSample;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Close()
// Desc: Closes the object
//--------------------------------------------------------------------------------------
VOID WaveFile::Close()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
    }
}

//--------------------------------------------------------------------------------------
// Name: XMAFile()
// Desc: Constructor
//--------------------------------------------------------------------------------------
XMAFile::XMAFile()
: m_pFormat(0)
, m_pFormat2(0)
, m_pSeekTable(0)
{
    m_hFile = INVALID_HANDLE_VALUE;
}


//--------------------------------------------------------------------------------------
// Name: ~XMAFile()
// Desc: Destructor
//--------------------------------------------------------------------------------------
XMAFile::~XMAFile()
{
    Close();
}


//--------------------------------------------------------------------------------------
// Name: Open()
// Desc: Opens the file and initializes the object
//--------------------------------------------------------------------------------------
HRESULT XMAFile::Open( const CHAR* strFileName, bool async /* = false */ )
{
    // If we're already open, close
    Close();

    // Open the file
    m_hFile = CreateFile( 
        strFileName, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        async ? FILE_FLAG_OVERLAPPED : 0, 
        NULL );

    if( m_hFile == INVALID_HANDLE_VALUE )
        return HRESULT_FROM_WIN32( GetLastError() );

    // Initialize the chunk objects
    m_RiffChunk.Initialize( ATG_FOURCC_RIFF, NULL, m_hFile );
    m_FormatChunk.Initialize( ATG_FOURCC_FORMAT, &m_RiffChunk, m_hFile );
    m_DataChunk.Initialize( ATG_FOURCC_DATA, &m_RiffChunk, m_hFile );
    m_WaveChunk.Initialize( ATG_FOURCC_WAVE, &m_RiffChunk, m_hFile );
    m_SeekTableChunk.Initialize( ATG_FOURCC_SEEK, &m_RiffChunk, m_hFile );
    m_Xma2FormatChunk.Initialize( ATG_FOURCC_XMA2, &m_RiffChunk, m_hFile );

    // Open the chunks
    //
    HRESULT hr = m_RiffChunk.Open();
    hr = FAILED(hr) ? hr : m_FormatChunk.Open();
    hr = FAILED(hr) ? hr : m_DataChunk.Open();
    hr = FAILED(hr) ? hr : m_SeekTableChunk.Open();

    // Read the format
    if( SUCCEEDED(hr) )
    {
        m_pFormat = (XMAWAVEFORMAT*)(new BYTE[m_FormatChunk.GetDataSize()]);
        hr = FAILED(hr) ? hr : m_FormatChunk.ReadData( 0, m_pFormat, m_FormatChunk.GetDataSize(), NULL );
    }

    // byteswap the format
    if( SUCCEEDED( hr ) )
    {
        SwapXmaFormat( m_pFormat );
    }
    
    // read the seek table
    if( SUCCEEDED(hr) )
    {
        m_pSeekTable = (DWORD*)(new BYTE[m_SeekTableChunk.GetDataSize()]);
        hr = FAILED(hr) ? hr : m_SeekTableChunk.ReadData( 0, m_pSeekTable, m_SeekTableChunk.GetDataSize(), NULL );
    }

    // Don't store this hresult because this is optional.
    if( SUCCEEDED( m_Xma2FormatChunk.Open() ) )
    {
        m_pFormat2 = (XMA2WAVEFORMAT*)(new BYTE[m_Xma2FormatChunk.GetDataSize()]);
        hr = FAILED(hr) ? hr : m_Xma2FormatChunk.ReadData( 0, m_pFormat2, m_Xma2FormatChunk.GetDataSize(), NULL );
    }

    // Validate the file type
    FOURCC fccType = 0;
    hr = FAILED(hr) ? hr : m_RiffChunk.ReadData( 0, &fccType, sizeof(fccType), NULL );

    if( ATG_FOURCC_WAVE != fccType )
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );

    return S_OK;
}




//--------------------------------------------------------------------------------------
// Name: ReadSample()
// Desc: Reads data from the audio file.
//--------------------------------------------------------------------------------------
HRESULT XMAFile::ReadSample( DWORD dwPosition, VOID* pBuffer, 
                               DWORD dwBufferSize, DWORD* pdwRead,
                               OVERLAPPED* pOL /* = NULL */) const
{                                   
    // Don't read past the end of the data chunk
    DWORD dwDuration;
    GetDuration( &dwDuration );

    if( dwPosition + dwBufferSize > dwDuration )
        dwBufferSize = dwDuration - dwPosition;

    HRESULT hr = S_OK;
    if( dwBufferSize )
        hr = m_DataChunk.ReadData( (LONG)dwPosition, pBuffer, dwBufferSize, pOL );

    if( pdwRead )
        *pdwRead = dwBufferSize;

    return hr;
}

//--------------------------------------------------------------------------------------
// Name: GetAsyncResult()
// Desc: checks pending async I/O result
//--------------------------------------------------------------------------------------
HRESULT XMAFile::GetAsyncResult( OVERLAPPED* pOvl, DWORD* pdwBytesRead ) const
{
    BOOL result = GetOverlappedResult( m_hFile, pOvl, pdwBytesRead, FALSE );
    
    HRESULT hr = result ? S_OK : HRESULT_FROM_WIN32( GetLastError() );
    return hr;
}

//--------------------------------------------------------------------------------------
// Name: Close()
// Desc: Closes the object
//--------------------------------------------------------------------------------------
VOID XMAFile::Close()
{
    if( m_hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_hFile );
        m_hFile = INVALID_HANDLE_VALUE;
    }

    delete[] m_pFormat;
    m_pFormat = NULL;
    
    delete[] m_pFormat2;
    m_pFormat2 = NULL;

    delete[] m_pSeekTable;
    m_pSeekTable = NULL;
}

//--------------------------------------------------------------------------------------
// Name: CreateSourceVoiceInit
// Desc: Fill 
//--------------------------------------------------------------------------------------
HRESULT CreateSourceVoiceInit( const XMAFile* pXmaFile, XAUDIOSOURCEVOICEINIT* pSourceVoiceInit )
{
    assert( pSourceVoiceInit );
    assert( pXmaFile );

    HRESULT hr = S_OK;

    //
    // Retrieve XMA format
    //
    XMAWAVEFORMAT *pfmt = pXmaFile->GetFormat();
    if( pfmt->FormatTag != WAVE_FORMAT_XMA )
        ATG::FatalError( "Invalid format tag\n" );

    //
    // Set up the basic defaults for the source voice
    //
    memset( pSourceVoiceInit, 0, sizeof( XAUDIOSOURCEVOICEINIT ) );
    pSourceVoiceInit->MaxPacketCount = 1;
    pSourceVoiceInit->pVoiceOutput = &ATG::DefaultSurroundVoiceOutput;

    //
    // Set up the source voice init values that are dependent on this particular
    // XmaFile
    //
    pSourceVoiceInit->Format.SampleType = XAUDIOSAMPLETYPE_XMA;
    pSourceVoiceInit->Format.NumStreams = (XAUDIOXMASTREAMCOUNT)pfmt->NumStreams;

    assert( pfmt->NumStreams <= XAUDIOXMASTREAMCOUNT_MAX );
    for( INT i = 0; i < pfmt->NumStreams; ++i )
    {
        pSourceVoiceInit->Format.Stream[ i ].SampleRate = pfmt->XmaStreams[ i ].SampleRate;
        pSourceVoiceInit->Format.Stream[ i ].ChannelCount = (XAUDIOCHANNEL) pfmt->XmaStreams[ i ].Channels;
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// SampleToBlockOffset
//
// Description: converts from a sample index to a block index + the number of samples
//              to offset from the beginning of the block.
//
// Parameters:
//      dwSampleIndex:      sample index to convert
//      pdwSeekTable:       pointer to the file's XMA2 seek table
//      nEntries:           number of DWORD entries in the seek table
//      out_pBlockIndex:    index of block where the desired sample lives
//      out_pOffset:        number of samples in the block before the desired sample
//--------------------------------------------------------------------------------------
HRESULT SampleToBlockOffset( DWORD dwSampleIndex, const DWORD* pdwSeekTable, DWORD nEntries, DWORD* out_pBlockIndex, DWORD* out_pOffset )
{
    assert( out_pBlockIndex );
    assert( out_pOffset );

    // Run through the seek table to find the block closest to the desired sample. 
    // Each seek table entry is the index (counting from the beginning of the file) 
    // of the first sample in the corresponding block, but there's no entry for the 
    // first block (since the index would always be zero).
    bool found = false;
    for( DWORD i = 0; !found && i < nEntries; ++i )
    {
        if( dwSampleIndex < pdwSeekTable[i] )
        {
            *out_pBlockIndex = i;
            found = true;
        }
    }

    // Calculate the sample offset by figuring out what the sample index of the first sample
    // in the block is, then subtracting that from dwSampleIndex.
    if( found )
    {
        DWORD dwStartOfBlock = (*out_pBlockIndex == 0) ? 0 : pdwSeekTable[*out_pBlockIndex - 1];
        *out_pOffset = dwSampleIndex - dwStartOfBlock;
    }

    return found ? S_OK : E_FAIL;
}

//--------------------------------------------------------------------------------------
// Name: GetFormat()
// Desc: Gets the wave file format.  Since Xbox 360 only supports WAVE_FORMAT_PCM,
//       and WAVE_FORMAT_EXTENSIBLE, we know any
//       valid format will fit into a WAVEFORMATEXTENSIBLE struct
//--------------------------------------------------------------------------------------
HRESULT XMAFile::GetFormat( XMAWAVEFORMAT* pxmaFormat ) const
{
    assert( pxmaFormat );
    DWORD dwValidSize = m_FormatChunk.GetDataSize();

    // Read the format chunk into the buffer
    HRESULT hr = m_FormatChunk.ReadData( 0, pxmaFormat, dwValidSize, NULL );
    if( FAILED(hr) )
        return hr;

    SwapXmaFormat( pxmaFormat );
    return S_OK;
}



//--------------------------------------------------------------------------------------
// Name: SwapXmaFormat)
// Desc: endian-swaps an XMAWAVEFORMAT structure.
//--------------------------------------------------------------------------------------
void SwapXmaFormat( XMAWAVEFORMAT* pxmaFormat )
{
    pxmaFormat->FormatTag = __loadshortbytereverse( 0, &pxmaFormat->FormatTag );
    pxmaFormat->BitsPerSample = __loadshortbytereverse( 0, &pxmaFormat->BitsPerSample );
    pxmaFormat->EncodeOptions = __loadshortbytereverse( 0, &pxmaFormat->EncodeOptions );
    pxmaFormat->LargestSkip = __loadshortbytereverse( 0, &pxmaFormat->LargestSkip );
    pxmaFormat->NumStreams = __loadshortbytereverse( 0, &pxmaFormat->NumStreams );

    for( INT i = 0; i < pxmaFormat->NumStreams; ++i )
    {
        pxmaFormat->XmaStreams[ i ].PsuedoBytesPerSec =
            __loadwordbytereverse( 0,&pxmaFormat->XmaStreams[ i ].PsuedoBytesPerSec );
        pxmaFormat->XmaStreams[ i ].SampleRate =
            __loadwordbytereverse( 0,&pxmaFormat->XmaStreams[ i ].SampleRate );
        pxmaFormat->XmaStreams[ i ].LoopStart =
            __loadwordbytereverse( 0,&pxmaFormat->XmaStreams[ i ].LoopStart );
        pxmaFormat->XmaStreams[ i ].LoopEnd =
            __loadwordbytereverse( 0,&pxmaFormat->XmaStreams[ i ].LoopEnd );
        pxmaFormat->XmaStreams[ i ].ChannelMask =
            __loadshortbytereverse( 0,&pxmaFormat->XmaStreams[ i ].ChannelMask );
    }
}

} // namespace ATG
