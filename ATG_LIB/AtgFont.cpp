//--------------------------------------------------------------------------------------
// AtgFont.cpp
//
// Font class for samples. For details, see header.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include <xgraphics.h>
#include "AtgDevice.h"
#include "AtgFont.h"
#include "AtgUtil.h"

namespace ATG
{


//--------------------------------------------------------------------------------------
// Access to the global device
//--------------------------------------------------------------------------------------
extern D3DDevice* g_pd3dDevice;


//--------------------------------------------------------------------------------------
// Vertex and pixel shaders for font rendering
//--------------------------------------------------------------------------------------
static const CHAR* g_strFontShader =
"struct VS_IN                                                 \n"
"{                                                            \n"
"   float2   Pos             : POSITION;                      \n"
"   float2   Tex             : TEXCOORD0;                     \n"
"   float4   ChannelSelector : TEXCOORD1;                     \n"
"};                                                           \n"
"                                                             \n"
"struct VS_OUT                                                \n"
"{                                                            \n"
"   float4 Position        : POSITION;                        \n"
"   float4 Diffuse         : COLOR0_center;                   \n"
"   float2 TexCoord0       : TEXCOORD0;                       \n"
"   float4 ChannelSelector : TEXCOORD1;                       \n"
"};                                                           \n"
"                                                             \n"
"uniform float4 Color       : register(c1);                   \n"
"uniform float2 TexScale    : register(c2);                   \n"
"                                                             \n"
"sampler        FontTexture : register(s0);                   \n"
"                                                             \n"
"VS_OUT FontVertexShader( VS_IN In )                          \n"
"{                                                            \n"
"   VS_OUT Out;                                               \n"
"   Out.Position.x  = (In.Pos.x-0.5);                         \n"
"   Out.Position.y  = (In.Pos.y-0.5);                         \n"
"   Out.Position.z  = ( 0.0 );                                \n"
"   Out.Position.w  = ( 1.0 );                                \n"
"   Out.Diffuse     = Color;                                  \n"
"   Out.TexCoord0.x = In.Tex.x * TexScale.x;                  \n"
"   Out.TexCoord0.y = In.Tex.y * TexScale.y;                  \n"
"   Out.ChannelSelector = In.ChannelSelector;                 \n"
"   return Out;                                               \n"
"}                                                            \n"
"                                                             \n"
"float4 FontPixelShader( VS_OUT In ) : COLOR0                 \n"
"{                                                            \n"
"   // Fetch a texel from the font texture                    \n"
"   float4 FontTexel = tex2D( FontTexture, In.TexCoord0 );    \n"
"                                                             \n"
"   if( dot( In.ChannelSelector, float4(1,1,1,1) ) )          \n"
"   {                                                         \n"
"      // Select the color from the channel                   \n"
"      float value = dot( FontTexel, In.ChannelSelector );    \n"
"                                                             \n"
"      // For white pixels, the high bit is 1 and the low     \n"
"      // bits are luminance, so r0.a will be > 0.5. For the  \n"
"      // RGB channel, we want to lop off the msb and shift   \n"
"      // the lower bits up one bit. This is simple to do     \n"
"      // with the _bx2 modifier. Since these pixels are      \n"
"      // opaque, we emit a 1 for the alpha channel (which    \n"
"      // is 0.5 x2 ).                                        \n"
"                                                             \n"
"      // For black pixels, the high bit is 0 and the low     \n"
"      // bits are alpha, so r0.a will be < 0.5. For the RGB  \n"
"      // channel, we emit zero. For the alpha channel, we    \n"
"      // just use the x2 modifier to scale up the low bits   \n"
"      // of the alpha.                                       \n"
"      float4 Color;                                          \n"
"      Color.rgb = ( value > 0.5f ? 2*value-1 : 0.0f );       \n"
"      Color.a   = 2 * ( value > 0.5f ? 1.0f : value );       \n"
"                                                             \n"
"      // Return the texture color modulated with the vertex  \n"
"      // color                                               \n"
"      return Color * In.Diffuse;                             \n"
"   }                                                         \n"
"   else                                                      \n"
"   {                                                         \n"
"      return FontTexel * In.Diffuse;                         \n"
"   }                                                         \n"
"}                                                            \n";


static D3DVertexDeclaration* g_pFontVertexDecl   = NULL;
static D3DVertexShader*      g_pFontVertexShader = NULL;
static D3DPixelShader*       g_pFontPixelShader  = NULL;


//--------------------------------------------------------------------------------------
// Name: CreateFontShaders()
// Desc: Creates the global font shaders
//--------------------------------------------------------------------------------------
HRESULT CreateFontShaders()
{
    // Create vertex declaration
    if( NULL == g_pFontVertexDecl )
    {   
        D3DVERTEXELEMENT9 decl[] = 
        {
            { 0,  0, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
            { 0,  8, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
            { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
            D3DDECL_END()
        };

        if( FAILED( g_pd3dDevice->CreateVertexDeclaration( decl, &g_pFontVertexDecl ) ) )
            return E_FAIL;
    }
    else
    {
        g_pFontVertexDecl->AddRef();
    }

    // Create vertex shader
    ID3DXBuffer* pShaderCode;
    if( NULL == g_pFontVertexShader )
    {
        if( FAILED( D3DXCompileShader( g_strFontShader, strlen(g_strFontShader),
                                       NULL, NULL, "FontVertexShader", "vs.2.0", 0,
                                       &pShaderCode, NULL, NULL ) ) )
            return E_FAIL;

        if( FAILED( g_pd3dDevice->CreateVertexShader( (DWORD*)pShaderCode->GetBufferPointer(),
                                                      &g_pFontVertexShader ) ) )
            return E_FAIL;
            
        pShaderCode->Release();
    }
    else
    {
        g_pFontVertexShader->AddRef();
    }
    
    // Create pixel shader.
    if( NULL == g_pFontPixelShader )
    {
        if( FAILED( D3DXCompileShader( g_strFontShader, strlen(g_strFontShader),
                                       NULL, NULL, "FontPixelShader", "ps.2.0", 0,
                                       &pShaderCode, NULL, NULL ) ) )
            return E_FAIL;

        if( FAILED( g_pd3dDevice->CreatePixelShader( (DWORD*)pShaderCode->GetBufferPointer(),
                                                     &g_pFontPixelShader ) ) )
            return E_FAIL;
            
        pShaderCode->Release();
    }
    else
    {
        g_pFontPixelShader->AddRef();
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Font()
// Desc: Constructor
//--------------------------------------------------------------------------------------
Font::Font()
{
    m_pFontTexture       = NULL;

    m_dwNumGlyphs        = 0L;
    m_Glyphs             = NULL;

    m_fCursorX           = 0.0f;
    m_fCursorY           = 0.0f;

    m_fXScaleFactor      = 1.0f;
    m_fYScaleFactor      = 1.0f;
    m_fSlantFactor       = 0.0f;

    m_cMaxGlyph          = 0;
    m_TranslatorTable    = NULL;
    
    m_dwNestedBeginCount = 0L;
}


//--------------------------------------------------------------------------------------
// Name: ~Font()
// Desc: Destructor
//--------------------------------------------------------------------------------------
Font::~Font()
{
    Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Create()
// Desc: Create the font's internal objects (texture and array of glyph info)
//       using the XPR packed resource file
//--------------------------------------------------------------------------------------
HRESULT Font::Create( const CHAR* strFontFileName )
{
    // Create the font
    if( FAILED( m_xprResource.Create( strFontFileName ) ) )
        return E_FAIL;

    return Create( m_xprResource.GetTexture("FontTexture"),
                   m_xprResource.GetData("FontData") );
}


//--------------------------------------------------------------------------------------
// Name: Create()
// Desc: Create the font's internal objects (texture and array of glyph info)
//--------------------------------------------------------------------------------------
HRESULT Font::Create( D3DTexture* pFontTexture, const VOID* pFontData )
{
    // Save a copy of the texture
    m_pFontTexture = pFontTexture;

    // Check version of file (to make sure it matches up with the FontMaker tool)
    const BYTE* pData = (BYTE*)pFontData;
    DWORD dwFileVersion = *((DWORD*)pData); pData += sizeof(DWORD);
    
    if( dwFileVersion == 0x00000005 )
    {
        // Parse the font data
        m_fFontHeight        = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontTopPadding    = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontBottomPadding = *((FLOAT*)pData); pData += sizeof(FLOAT);
        m_fFontYAdvance      = *((FLOAT*)pData); pData += sizeof(FLOAT);

        // Point to the translator string
        m_cMaxGlyph       = ((WORD*)pData)[0];   pData += sizeof(WORD);
        m_TranslatorTable = (SHORT*)pData;       pData += sizeof(WCHAR)*(m_cMaxGlyph+1);

        // Read the glyph attributes from the file
        m_dwNumGlyphs = ((DWORD*)pData)[0];  pData += sizeof(DWORD);
        m_Glyphs      = (GLYPH_ATTR*)pData;
    }
    else
    {
        ATG_PrintError( "Incorrect version number on font file!\n" );
        return E_FAIL;
    }

    // Create the vertex and pixel shaders for rendering the font
    if( FAILED( CreateFontShaders() ) )
    {
        ATG_PrintError( "Could not create font shaders!\n" );
        return E_FAIL;
    }

    // Initialize the window
    D3DDISPLAYMODE DisplayMode;
    g_pd3dDevice->GetDisplayMode( 0, &DisplayMode );
    m_rcWindow.x1 = 0;
    m_rcWindow.y1 = 0;
    m_rcWindow.x2 = DisplayMode.Width;
    m_rcWindow.y2 = DisplayMode.Height;

    // Determine whether we should save/restore state
    m_bSaveState = TRUE;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Destroy the font object
//--------------------------------------------------------------------------------------
VOID Font::Destroy()
{
    m_pFontTexture       = NULL;
    m_dwNumGlyphs        = 0L;
    m_Glyphs             = NULL;
    m_cMaxGlyph          = 0;
    m_TranslatorTable    = NULL;
    m_dwNestedBeginCount = 0L;

    // Safely release shaders
    if( ( g_pFontVertexDecl != NULL ) && ( g_pFontVertexDecl->Release() == 0 ) )
        g_pFontVertexDecl = NULL;
    if( ( g_pFontVertexShader != NULL ) && ( g_pFontVertexShader->Release() == 0 ) )
        g_pFontVertexShader = NULL;
    if( ( g_pFontPixelShader != NULL ) && ( g_pFontPixelShader->Release() == 0 ) )
        g_pFontPixelShader = NULL;
}


//--------------------------------------------------------------------------------------
// Name: SetWindow()
// Desc: Sets the cursor position for drawing text
//--------------------------------------------------------------------------------------
VOID Font::SetWindow( D3DRECT rcWindow )
{
    SetWindow( rcWindow.x1, rcWindow.y1, rcWindow.x2, rcWindow.y2 );
}


//--------------------------------------------------------------------------------------
// Name: SetWindow()
// Desc: Sets the cursor position for drawing text
//--------------------------------------------------------------------------------------
VOID Font::SetWindow( LONG x1, LONG y1, LONG x2, LONG y2 )
{
    m_rcWindow.x1 = x1;
    m_rcWindow.y1 = y1;
    m_rcWindow.x2 = x2;
    m_rcWindow.y2 = y2;

    m_fCursorX    = 0.0f;
    m_fCursorY    = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: SetCursorPosition()
// Desc: Sets the cursor position for drawing text
//--------------------------------------------------------------------------------------
VOID Font::SetCursorPosition( FLOAT fCursorX, FLOAT fCursorY )
{
    m_fCursorX = floorf( fCursorX );
    m_fCursorY = floorf( fCursorY );
}


//--------------------------------------------------------------------------------------
// Name: SetScaleFactors()
// Desc: Sets X and Y scale factor to make rendered text bigger or smaller.
//       Note that since text is pre-anti-aliased and therefore point-filtered,
//       any scale factors besides 1.0f will degrade the quality.
//--------------------------------------------------------------------------------------
VOID Font::SetScaleFactors( FLOAT fXScaleFactor, FLOAT fYScaleFactor )
{
    m_fXScaleFactor = fXScaleFactor;
    m_fYScaleFactor = fYScaleFactor;
}


//--------------------------------------------------------------------------------------
// Name: SetSlantFactor()
// Desc: Sets the slant factor for rendering slanted text.
//--------------------------------------------------------------------------------------
VOID Font::SetSlantFactor( FLOAT fSlantFactor )
{
    m_fSlantFactor = fSlantFactor;
}


//--------------------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//--------------------------------------------------------------------------------------
VOID Font::GetTextExtent( const WCHAR* strText, FLOAT* pWidth, 
                          FLOAT* pHeight, BOOL bFirstLineOnly ) const
{
    assert( pWidth != NULL );
    assert( pHeight != NULL );

    // Set default text extent in output parameters
    *pWidth   = 0.0f;
    *pHeight  = 0.0f;

    if( strText )
    {
        // Initialize counters that keep track of text extent
        FLOAT sx = 0.0f;
        FLOAT sy = m_fFontHeight;

        // Loop through each character and update text extent
        while( *strText )
        {
            WCHAR letter = *strText++;
        
            // Handle newline character
            if( letter == L'\n' )
            {
                if( bFirstLineOnly )
                    break;
                sx  = 0.0f;
                sy += m_fFontYAdvance;
            }

            // Handle carriage return characters by ignoring them. This helps when
            // displaying text from a file.
            if( letter == L'\r' )
                continue;

            // Translate unprintable characters
            GLYPH_ATTR* pGlyph;
            if( letter > m_cMaxGlyph || m_TranslatorTable[letter] == 0 )
                pGlyph = &m_Glyphs[0];
            else
                pGlyph = &m_Glyphs[m_TranslatorTable[letter]];

            // Get text extent for this character's glyph
            sx += pGlyph->wOffset;
            sx += pGlyph->wAdvance;

            // Store text extent of string in output parameters
            if( sx > (*pWidth) )
                *pWidth = sx;
            if( sy > (*pHeight) )
                *pHeight = sy;
        }
    }

    // Apply the scale factor to the result
    (*pWidth)  *= m_fXScaleFactor;
    (*pHeight) *= m_fYScaleFactor;
}


//--------------------------------------------------------------------------------------
// Name: GetTextWidth()
// Desc: Returns the width in pixels of a text string
//--------------------------------------------------------------------------------------
FLOAT Font::GetTextWidth( const WCHAR* strText ) const
{
    FLOAT fTextWidth  = 0.0f;
    FLOAT fTextHeight = 0.0f;
    GetTextExtent( strText, &fTextWidth, &fTextHeight );
    return fTextWidth;
}


//--------------------------------------------------------------------------------------
// Name: Begin()
// Desc: Prepares the font vertex buffers for rendering.
//--------------------------------------------------------------------------------------
VOID Font::Begin()
{
    // Set state on the first call
    if( 0 == m_dwNestedBeginCount )
    {
        // Save state
        if( m_bSaveState )
        {
            // Note, we are not saving the texture, vertex, or pixel shader,
            //       since it's not worth the performance. We're more interested
            //       in saving state that would cause hard to find problems.
            g_pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHABLENDENABLE ] );
            g_pd3dDevice->GetRenderState( D3DRS_SRCBLEND,         &m_dwSavedState[ SAVEDSTATE_D3DRS_SRCBLEND ] );
            g_pd3dDevice->GetRenderState( D3DRS_DESTBLEND,        &m_dwSavedState[ SAVEDSTATE_D3DRS_DESTBLEND ] );
            g_pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,  &m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHATESTENABLE ] );
            g_pd3dDevice->GetRenderState( D3DRS_ALPHAREF,         &m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHAREF ] );
            g_pd3dDevice->GetRenderState( D3DRS_ALPHAFUNC,        &m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHAFUNC ] );
            g_pd3dDevice->GetRenderState( D3DRS_FILLMODE,         &m_dwSavedState[ SAVEDSTATE_D3DRS_FILLMODE ] );
            g_pd3dDevice->GetRenderState( D3DRS_CULLMODE,         &m_dwSavedState[ SAVEDSTATE_D3DRS_CULLMODE ] );
            g_pd3dDevice->GetRenderState( D3DRS_ZENABLE,          &m_dwSavedState[ SAVEDSTATE_D3DRS_ZENABLE ] );
            g_pd3dDevice->GetRenderState( D3DRS_STENCILENABLE,    &m_dwSavedState[ SAVEDSTATE_D3DRS_STENCILENABLE ] );
            g_pd3dDevice->GetRenderState( D3DRS_VIEWPORTENABLE,   &m_dwSavedState[ SAVEDSTATE_D3DRS_VIEWPORTENABLE ] );
            g_pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,  &m_dwSavedState[ SAVEDSTATE_D3DSAMP_MINFILTER ] );
            g_pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER,  &m_dwSavedState[ SAVEDSTATE_D3DSAMP_MAGFILTER ] );
            g_pd3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSU,   &m_dwSavedState[ SAVEDSTATE_D3DSAMP_ADDRESSU ] );
            g_pd3dDevice->GetSamplerState( 0, D3DSAMP_ADDRESSV,   &m_dwSavedState[ SAVEDSTATE_D3DSAMP_ADDRESSV ] );
        }

        // Set render state
        g_pd3dDevice->SetTexture( 0, m_pFontTexture );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
        g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
        g_pd3dDevice->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
        g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE,   FALSE );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        g_pd3dDevice->SetVertexDeclaration( g_pFontVertexDecl );
        g_pd3dDevice->SetVertexShader( g_pFontVertexShader );
        g_pd3dDevice->SetPixelShader( g_pFontPixelShader );

        // Set the texture scaling factor as a vertex shader constant
        D3DSURFACE_DESC TextureDesc;
        m_pFontTexture->GetLevelDesc( 0, &TextureDesc );
        FLOAT vTexScale[4];
        vTexScale[0] = 1.0f/TextureDesc.Width;
        vTexScale[1] = 1.0f/TextureDesc.Height;
        vTexScale[2] = 0.0f;
        vTexScale[3] = 0.0f;
        g_pd3dDevice->SetVertexShaderConstantF( 2, vTexScale, 1 );
    }

    // Keep track of the nested begin/end calls.
    m_dwNestedBeginCount++;
}


//--------------------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws text as textured polygons
//--------------------------------------------------------------------------------------
VOID Font::DrawText( DWORD dwColor, const WCHAR* strText, DWORD dwFlags,
                     FLOAT fMaxPixelWidth )
{
    DrawText( m_fCursorX, m_fCursorY, dwColor, strText, dwFlags, fMaxPixelWidth );
}


//--------------------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws text as textured polygons
//       TODO: This function should use the Begin/SetVertexData/End() API when it
//       becomes available.
//--------------------------------------------------------------------------------------
VOID Font::DrawText( FLOAT fOriginX, FLOAT fOriginY, DWORD dwColor,
                     const WCHAR* strText, DWORD dwFlags, FLOAT fMaxPixelWidth )
{
    if( NULL  == strText )    return;
    if( L'\0' == strText[0] ) return;

    // Create a PIX user-defined event that encapsulates all of the text draw calls.
    // This makes DrawText calls easier to recognize in PIX captures, and it makes
    // them take up fewer entries in the event list.
    PIXBeginNamedEvent( dwColor, "DrawText: %S", strText );

    // Set up stuff to prepare for drawing text
    Begin();

    // Set the color as a vertex shader constant
    FLOAT vColor[4];
    vColor[0] = ((dwColor&0x00ff0000)>>16L)/255.0f;
    vColor[1] = ((dwColor&0x0000ff00)>> 8L)/255.0f;
    vColor[2] = ((dwColor&0x000000ff)>> 0L)/255.0f;
    vColor[3] = ((dwColor&0xff000000)>>24L)/255.0f;
    g_pd3dDevice->SetVertexShaderConstantF( 1, vColor, 1 );

    // Set the starting screen position
    if( ( fOriginX < 0.0f ) || ( ( dwFlags & ATGFONT_RIGHT ) && ( fOriginX <= 0.0f ) ) )
    {
        fOriginX += (m_rcWindow.x2-m_rcWindow.x1);
    }
    if( fOriginY < 0.0f )
    {
        fOriginY += (m_rcWindow.y2-m_rcWindow.y1);
    }

    m_fCursorX = floorf( fOriginX );
    m_fCursorY = floorf( fOriginY );

    // Adjust for padding
    fOriginY -= m_fFontTopPadding;

    FLOAT fEllipsesPixelWidth = m_fXScaleFactor * 3.0f * (m_Glyphs[m_TranslatorTable[L'.']].wOffset + m_Glyphs[m_TranslatorTable[L'.']].wAdvance);

    if( dwFlags & ATGFONT_TRUNCATED )
    {
        // Check if we will really need to truncate the string
        if( fMaxPixelWidth <= 0.0f )
        {
            dwFlags &= (~ATGFONT_TRUNCATED);
        }
        else
        {
            FLOAT w, h;
            GetTextExtent( strText, &w, &h, TRUE );
    
            // If not, then clear the flag
            if( w <= fMaxPixelWidth )
                dwFlags &= (~ATGFONT_TRUNCATED);
        }
    }

    // If vertically centered, offset the starting m_fCursorY value
    if( dwFlags & ATGFONT_CENTER_Y )
    {
        FLOAT w, h;
        GetTextExtent( strText, &w, &h );
        m_fCursorY = floorf( m_fCursorY - h/2 );
    }

    // Add window offsets
    fOriginX   += m_rcWindow.x1;
    fOriginY   += m_rcWindow.y1;
    m_fCursorX += m_rcWindow.x1;
    m_fCursorY += m_rcWindow.y1;

    // Set a flag so we can determine initial justification effects
    BOOL bStartingNewLine = TRUE;

    DWORD dwNumEllipsesToDraw = 0;

    // Begin drawing the vertices
    FLOAT* v;
    DWORD  dwNumChars = wcslen( strText ) + ( dwFlags & ATGFONT_TRUNCATED ? 3 : 0 );
    g_pd3dDevice->BeginVertices( D3DPT_QUADLIST, 4*dwNumChars, sizeof(XMFLOAT4)+sizeof(DWORD), (VOID**)&v );

    bStartingNewLine = TRUE;

    // Draw four vertices for each glyph
    while( *strText )
    {
        WCHAR letter;

        if( dwNumEllipsesToDraw )
        {
            letter = L'.';
        }
        else
        {
            // If starting text on a new line, determine justification effects
            if( bStartingNewLine )
            {
                if( dwFlags & (ATGFONT_RIGHT|ATGFONT_CENTER_X) )
                {
                    // Get the extent of this line
                    FLOAT w, h;
                    GetTextExtent( strText, &w, &h, TRUE );

                    // Offset this line's starting m_fCursorX value
                    if( dwFlags & ATGFONT_RIGHT )
                        m_fCursorX = floorf( fOriginX - w );
                    if( dwFlags & ATGFONT_CENTER_X )
                        m_fCursorX = floorf( fOriginX - w/2 );
                }
                bStartingNewLine = FALSE;
            }

            // Get the current letter in the string
            letter = *strText++;

            // Handle the newline character
            if( letter == L'\n' )
            {
                m_fCursorX  = fOriginX;
                m_fCursorY += m_fFontYAdvance * m_fYScaleFactor;
                bStartingNewLine = TRUE;
                continue;
            }

            // Handle carriage return characters by ignoring them. This helps when
            // displaying text from a file.
            if( letter == L'\r' )
                continue;
        }

        // Translate unprintable characters
        GLYPH_ATTR* pGlyph = &m_Glyphs[ (letter<=m_cMaxGlyph) ? m_TranslatorTable[letter] : 0 ];

        FLOAT fOffset  = m_fXScaleFactor * (FLOAT)pGlyph->wOffset;
        FLOAT fAdvance = m_fXScaleFactor * (FLOAT)pGlyph->wAdvance;
        FLOAT fWidth   = m_fXScaleFactor * (FLOAT)pGlyph->wWidth;
        FLOAT fHeight  = m_fYScaleFactor * m_fFontHeight;

        if( 0 == dwNumEllipsesToDraw )
        {
            if( dwFlags & ATGFONT_TRUNCATED )
            {
                // Check if we will be exceeded the max allowed width
                if( m_fCursorX + fOffset + fWidth + fEllipsesPixelWidth + m_fSlantFactor > fOriginX + fMaxPixelWidth )
                {
                    // Yup, draw the three ellipses dots instead
                    dwNumEllipsesToDraw = 3;
                    continue;
                }
            }
        }

        // Setup the screen coordinates
        m_fCursorX  += fOffset;
        FLOAT left1  = m_fCursorX;
        FLOAT left2  = left1 + m_fSlantFactor;
        FLOAT right1 = left1 + fWidth;
        FLOAT right2 = left2 + fWidth;
        FLOAT top    = m_fCursorY;
        FLOAT bottom = top + fHeight;
        m_fCursorX  += fAdvance;

        // Select the RGBA channel that the compressed glyph is stored in
        DWORD dwChannelSelector = (pGlyph->wMask&0x0f00?0x00ff0000:0) | 
                                  (pGlyph->wMask&0x00f0?0x0000ff00:0) |
                                  (pGlyph->wMask&0x000f?0x000000ff:0) |
                                  (pGlyph->wMask&0xf000?0xff000000:0);
        FLOAT fChannelSelector = *(FLOAT*)&dwChannelSelector;

        // Add the vertices to draw this glyph
        *v++ = left2,  *v++ = top,    *v++ = pGlyph->tu1, *v++ = pGlyph->tv1, *v++ = fChannelSelector;
        *v++ = right2, *v++ = top,    *v++ = pGlyph->tu2, *v++ = pGlyph->tv1, *v++ = fChannelSelector;
        *v++ = right1, *v++ = bottom, *v++ = pGlyph->tu2, *v++ = pGlyph->tv2, *v++ = fChannelSelector;
        *v++ = left1,  *v++ = bottom, *v++ = pGlyph->tu1, *v++ = pGlyph->tv2, *v++ = fChannelSelector;

        // If drawing ellipses, exit when they're all drawn
        if( dwNumEllipsesToDraw )
        {
            if( --dwNumEllipsesToDraw == 0 )
                break;
        }

        dwNumChars--;
    }

    // Since we allocated vertex data space based on the string length, we now need to
    // add some dummy verts for any skipped characters (like newlines, etc.)
    while( dwNumChars )
    {
        *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0;
        *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0;
        *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0;
        *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0, *v++ = 0;
        dwNumChars--;
    }

    // Stop drawing vertices
    g_pd3dDevice->EndVertices();

    // Undo window offsets
    m_fCursorX -= m_rcWindow.x1;
    m_fCursorY -= m_rcWindow.y1;

    // Call End() to complete the begin/end pair for drawing text
    End();

    // Close off the user-defined event opened with PIXBeginNamedEvent.
    PIXEndNamedEvent();
}


//--------------------------------------------------------------------------------------
// Name: End()
// Desc: Paired call that restores state set in the Begin() call.
//--------------------------------------------------------------------------------------
VOID Font::End()
{
    assert( m_dwNestedBeginCount > 0 );
    if( --m_dwNestedBeginCount > 0 )
        return;

    // Restore state
    if( m_bSaveState )
    {
        g_pd3dDevice->SetTexture( 0, NULL );
        g_pd3dDevice->SetVertexDeclaration( NULL );
        g_pd3dDevice->SetVertexShader( NULL );
        g_pd3dDevice->SetPixelShader( NULL );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHABLENDENABLE ] );
        g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         m_dwSavedState[ SAVEDSTATE_D3DRS_SRCBLEND ] );
        g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        m_dwSavedState[ SAVEDSTATE_D3DRS_DESTBLEND ] );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHATESTENABLE ] );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHAREF ] );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        m_dwSavedState[ SAVEDSTATE_D3DRS_ALPHAFUNC ] );
        g_pd3dDevice->SetRenderState( D3DRS_FILLMODE,         m_dwSavedState[ SAVEDSTATE_D3DRS_FILLMODE ] );
        g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         m_dwSavedState[ SAVEDSTATE_D3DRS_CULLMODE ] );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          m_dwSavedState[ SAVEDSTATE_D3DRS_ZENABLE ] );
        g_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    m_dwSavedState[ SAVEDSTATE_D3DRS_STENCILENABLE ] );
        g_pd3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE,   m_dwSavedState[ SAVEDSTATE_D3DRS_VIEWPORTENABLE ] );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,  m_dwSavedState[ SAVEDSTATE_D3DSAMP_MINFILTER ] );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,  m_dwSavedState[ SAVEDSTATE_D3DSAMP_MAGFILTER ] );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,   m_dwSavedState[ SAVEDSTATE_D3DSAMP_ADDRESSU ] );
        g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,   m_dwSavedState[ SAVEDSTATE_D3DSAMP_ADDRESSV ] );
    }
}


//--------------------------------------------------------------------------------------
// Name: CreateTexture()
// Desc: Creates a texture and renders a text string into it.
//--------------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 Font::CreateTexture( const WCHAR* strText, D3DCOLOR dwBackgroundColor, 
                                        D3DCOLOR dwTextColor, D3DFORMAT d3dFormat )
{
    // Make sure the format is tiled (otherwise the Resolve will fail)
    if( FALSE == XGIsTiledFormat( d3dFormat ) )
    {
        ATG_PrintError( "Format must be tiled!\n" );
        return NULL;
    }

    // Calculate texture dimensions
    FLOAT fTexWidth, fTexHeight;
    GetTextExtent( strText, &fTexWidth, &fTexHeight );
    DWORD dwWidth  = XGNextMultiple( (DWORD)fTexWidth,  32 );
    DWORD dwHeight = XGNextMultiple( (DWORD)fTexHeight, 32 );

    // Create a render target
    LPDIRECT3DSURFACE9 pNewRenderTarget;
    if( FAILED( g_pd3dDevice->CreateRenderTarget( dwWidth, dwHeight, d3dFormat, D3DMULTISAMPLE_NONE,
                                                  0L, FALSE, &pNewRenderTarget, NULL ) ) )
    {
        ATG_PrintError( "Could not create a render target for the font texture!\n" );
        return NULL;
    }

    // Create the texture
    LPDIRECT3DTEXTURE9 pNewTexture;
    if( FAILED( g_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0L, d3dFormat, 
                                             D3DPOOL_DEFAULT, &pNewTexture, NULL ) ) )
    {
        ATG_PrintError( "Could not create a font texture!\n" );
        pNewRenderTarget->Release();
        return NULL;
    }

    // Get the current backbuffer and zbuffer
    LPDIRECT3DSURFACE9 pOldRenderTarget;
    g_pd3dDevice->GetRenderTarget( 0, &pOldRenderTarget );

    // Set the new texture as the render target
    g_pd3dDevice->SetRenderTarget( 0, pNewRenderTarget );
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, dwBackgroundColor, 1.0f, 0L );

    // Render the text
    D3DRECT rcSaved = m_rcWindow;
    SetWindow( 0, 0, dwWidth, dwHeight );
    DrawText( dwWidth/2.0f, dwHeight/2.0f, dwTextColor, strText, ATGFONT_CENTER_X|ATGFONT_CENTER_Y );

    // Resolve to the texture
    g_pd3dDevice->Resolve( 0L, NULL, pNewTexture,  NULL, 0, 0, NULL, 0, 0, NULL );

    // Restore the render target
    g_pd3dDevice->SetRenderTarget( 0, pOldRenderTarget );
    pOldRenderTarget->Release();
    pNewRenderTarget->Release();

    m_rcWindow = rcSaved;

    // Return the new texture
    return pNewTexture;
}

} // namespace ATG
