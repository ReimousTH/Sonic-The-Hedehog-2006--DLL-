//--------------------------------------------------------------------------------------
// AtgXime.h
//
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ATGXIME_H
#define ATGXIME_H

#include <xui.h>
#include <xime.h>
#include <list>
#include <string>

namespace ATG
{

//--------------------------------------------------------------------------------------
// Definitions
//--------------------------------------------------------------------------------------

    const D3DCOLOR COLOR_NORMAL = D3DCOLOR_ARGB( 255, 255, 255, 255 );
    const D3DCOLOR COLOR_INIME  = D3DCOLOR_ARGB( 255, 128, 128, 128 );
    const D3DCOLOR COLOR_LIST   = D3DCOLOR_ARGB( 255, 128, 128, 128 );
    const D3DCOLOR COLOR_FOCUS  = D3DCOLOR_ARGB( 255, 255, 255, 0 );

    enum IMEMODE {
        MODE_INPUT      = 0x1,
        MODE_CONVERT    = 0x2,
        MODE_ALLCONVERT = 0x3,
        MODE_JP         = 0x10,
        MODE_KR         = 0x20,
        MODE_CH         = 0x30,
    };

    class   Xime;
    struct  XIME_KEYMAPPING;
    typedef HRESULT (*IMECALLBACK)( Xime *, XIME_KEYMAPPING * pKey ) ;

    struct XIME_KEYMAPPING{
        DWORD   dwFlags;
        WORD    wVK;
        WORD    wXinputFlags;
        WORD    wIgnoredXinputFlags;
        union {
            DWORD dwUnicode;
            XIMEKEY eControlKey;
        };
        IMECALLBACK     function;
    };

    struct XIMEREQUEST{
        DWORD dwRequestId;
        XOVERLAPPED ov;
    };

typedef std::list< ATG::XIMEREQUEST > RequestList;

struct HID_LOOKUP_TABLE {
        WCHAR wHiragana;
        WCHAR wHiraganaShifted;
        WCHAR wKatakana;
        WCHAR wKatakanaShifted;
};

class Xime
{
public:
    static XIME_KEYMAPPING m_InputKeymap[];

    // XIME methods
    BOOL    IsIMEOn( void ) { return m_bIMEOn; };
    HRESULT Init( IMEMODE eLanguage, DWORD dwCharLength, D3DPRESENT_PARAMETERS * pD3DParam );
    HRESULT Update();
    HRESULT Render( HXUIDC hDC, HXUIFONT hFont, float fPosX, float fPofY );
    BOOL    IsStringReady( void );
    HRESULT GetString( std::wstring & stringOut );
    // Get IME modes
    INT     GetInputMode( void ) { return m_iInputMode; };
    INT     GetKeyboardLayout( void ) { return m_iKeyboardLayout; };
    IMEMODE GetCurrentLanguage( void ) { return m_CurrentLanguage; };
    HRESULT GetLastKey( XINPUT_KEYSTROKE * pKey ) { memcpy( pKey, &m_Key, sizeof( m_Key ) );
                                                    return S_OK;
                                                  };
    DWORD   GetInputCharacterLength( void ) { return m_dwInputCharacterLength; };
    VOID    SetInputCharacterLength( DWORD dwLength ) { m_dwInputCharacterLength = dwLength; };
private:
    // Dictionaries
    static const int   iNumDics;
    static const char  szMainDicPath[];
    static const char  szSubDicPath[];
    static const char  szTitleDicPath[];
    
    static const INT NUM_STRINGBUFFER =512;
    static const INT NUM_CANDIATEINPAGE = 5;
    static const DWORD DEFAULT_CHARACTERLENGTH = 32;

    static WCHAR  m_StringBuffer[ NUM_STRINGBUFFER ];
    static WCHAR  m_CandidateListBuffer[ NUM_STRINGBUFFER ];

    //Callbacks
    static HRESULT Convert( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT ChangeFocus( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT ChangeClauseLength( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT Revert( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT BuildCandidateList( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT SetCandidate( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT Complete( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT HandleNumpadInput( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT HandleNumpadConversion( Xime * pXime, XIME_KEYMAPPING * pKey );

    static HRESULT AllConvert( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT AllRevert( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT ChangeInputMode( Xime * pXime, XIME_KEYMAPPING * pKey );
    static HRESULT ToggleIME( Xime * pXime, XIME_KEYMAPPING * pKey );

    //Japanese specific routines
    static HRESULT ToggleKeyboardLayout( Xime * pXime, XIME_KEYMAPPING * pKey );
    HRESULT LayoutConversion( XINPUT_KEYSTROKE* pKey, INT* pIntputMode );

    void    DrawText( HXUIDC hdc, HXUIFONT hFont, D3DCOLOR color,
                      float x, float y, LPCWSTR text,
                      float * fX = NULL, float * fY = NULL );
    void    SetCurrentMode( IMEMODE );

    // Language specific routine for Japanese
    WCHAR   HidUnicodeLookUpJP( DWORD dwHidCode, BOOL fShifted, BOOL fKana,
                                HID_LOOKUP_TABLE * pTable, HID_LOOKUP_TABLE * pTable2 );
    
    RequestList         m_RequestList;
    IMEMODE             m_CurrentImeMode;
    IMEMODE             m_CurrentLanguage;
    bool                m_bShowCandidateList;
    bool                m_bNeedBlocking;
    
    float               m_BackBufferWidth;
    float               m_BackBufferHeight;
    bool                m_bCompleted;
    DWORD               m_dwCharsInXime;
    INT                 m_iInputMode;
    INT                 m_iKeyboardLayout;
    BOOL                m_bIMEOn;
    DWORD               m_dwInputCharacterLength;

    XINPUT_KEYSTROKE    m_Key;

    std::wstring        m_CurrentString;
    std::wstring        m_CompletedString;
};


}; // namespace ATG

#endif
