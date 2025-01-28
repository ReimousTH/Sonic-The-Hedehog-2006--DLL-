//--------------------------------------------------------------------------------------
// AtgXime.cpp
//
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "stdafx.h"
#include <malloc.h>
#include "AtgXime.h"
#include "AtgUtil.h"


namespace ATG
{

//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------

    WCHAR  Xime::m_StringBuffer[ NUM_STRINGBUFFER ];
    WCHAR  Xime::m_CandidateListBuffer[ NUM_STRINGBUFFER ];

    // Dictionaries
    const int   Xime::iNumDics = 3;
    const char  Xime::szMainDicPath[] =  "game:\\media\\ximejpmain.dic";
    const char  Xime::szSubDicPath[] =   "game:\\media\\ximejpsub.dic";
    const char  Xime::szTitleDicPath[] = "game:\\media\\ximejpnameaddr.dic";

    XIME_KEYMAPPING Xime::m_InputKeymap[] = {
    { MODE_INPUT | MODE_JP, VK_RETURN,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },        //for Japanese, Bopomofo
    { MODE_INPUT | MODE_JP, VK_SEPARATOR,XINPUT_KEYSTROKE_KEYDOWN,0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_BACK,    XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_BACKSPACE, NULL },                     //for Japanese, Hangul, Bopomofo
    { MODE_INPUT | MODE_JP, VK_DELETE,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_DELETE, NULL },                                //for Japanese
    { MODE_INPUT | MODE_JP, VK_HOME,    XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_HOME, NULL },                                  //for Japanese
    { MODE_INPUT | MODE_JP, VK_END,     XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_END, NULL },                                   //for Japanese
    { MODE_INPUT | MODE_JP, VK_RIGHT,   XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_RIGHT, NULL },                         //for Japanese, Hangul
    { MODE_INPUT | MODE_JP, VK_LEFT,    XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_LEFT, NULL },                          //for Japanese, Hangul
    { MODE_INPUT | MODE_JP, VK_SPACE,   XINPUT_KEYSTROKE_KEYDOWN, XINPUT_KEYSTROKE_SHIFT | XINPUT_KEYSTROKE_CTRL | XINPUT_KEYSTROKE_ALT,   XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_INPUT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN,  XINPUT_KEYSTROKE_CTRL | XINPUT_KEYSTROKE_ALT,   XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_INPUT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT , XINPUT_KEYSTROKE_CTRL | XINPUT_KEYSTROKE_ALT,   XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_INPUT | MODE_JP, VK_ESCAPE,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_FLUSH, NULL },                                 //for Japanese, Bopomofo
    { MODE_INPUT | MODE_JP, VK_F6,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_HIRAGANA, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_F7,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_KATAKANA, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_F9,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_ALPHANUMERIC_FULL_WIDTH, (IMECALLBACK)Xime::AllConvert },           //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_F10,     XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_ALPHANUMERIC_HALF_WIDTH, (IMECALLBACK)Xime::AllConvert },           //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, L'U',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_HIRAGANA, (IMECALLBACK)Xime::AllConvert },   //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, L'I',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_KATAKANA, (IMECALLBACK)Xime::AllConvert },   //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, L'P',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_ALPHANUMERIC_FULL_WIDTH, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, L'O',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_ALPHANUMERIC_HALF_WIDTH, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD0, XINPUT_KEYSTROKE_KEYDOWN, 0, L'0', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD1, XINPUT_KEYSTROKE_KEYDOWN, 0, L'1', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD2, XINPUT_KEYSTROKE_KEYDOWN, 0, L'2', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD3, XINPUT_KEYSTROKE_KEYDOWN, 0, L'3', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD4, XINPUT_KEYSTROKE_KEYDOWN, 0, L'4', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD5, XINPUT_KEYSTROKE_KEYDOWN, 0, L'5', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD6, XINPUT_KEYSTROKE_KEYDOWN, 0, L'6', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD7, XINPUT_KEYSTROKE_KEYDOWN, 0, L'7', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD8, XINPUT_KEYSTROKE_KEYDOWN, 0, L'8', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_NUMPAD9, XINPUT_KEYSTROKE_KEYDOWN, 0, L'9', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_MULTIPLY,XINPUT_KEYSTROKE_KEYDOWN, 0, L'*', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_ADD,     XINPUT_KEYSTROKE_KEYDOWN, 0, L'+', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_SUBTRACT,XINPUT_KEYSTROKE_KEYDOWN, 0, L'-', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_DECIMAL, XINPUT_KEYSTROKE_KEYDOWN, 0, L'.', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_DIVIDE,  XINPUT_KEYSTROKE_KEYDOWN, 0, L'/', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_OEM_NEC_EQUAL,  XINPUT_KEYSTROKE_KEYDOWN, 0, L'=', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_KANA,    XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_MODE_JP_ROMAJI_HIRAGANA, (IMECALLBACK)Xime::ChangeInputMode },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_KANA,    XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0, XIME_MODE_JP_ROMAJI_KATAKANA, (IMECALLBACK)Xime::ChangeInputMode },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_KANA,    XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_ALT, 0, XIME_MODE_JP_KANAINPUT_WITH_KEYSTROKE_SHIFTFLAG_OFF, (IMECALLBACK)Xime::ToggleKeyboardLayout },          //for Japanese, special conversion
    { MODE_INPUT | MODE_JP, VK_KANJI,   XINPUT_KEYSTROKE_KEYDOWN, 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to Japanese
    { MODE_INPUT | MODE_JP, VK_OEM_3,   XINPUT_KEYSTROKE_KEYDOWN |XINPUT_KEYSTROKE_ALT , 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to English

    // Convert mode
    { MODE_CONVERT | MODE_JP, VK_RETURN,XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },      //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_SEPARATOR, XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },      //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_RIGHT, XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_RIGHT, (IMECALLBACK)Xime::ChangeFocus },      //for Japanese, Hangul
    { MODE_CONVERT | MODE_JP, VK_LEFT,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_LEFT, (IMECALLBACK)Xime::ChangeFocus },       //for Japanese, Hangul
    { MODE_CONVERT | MODE_JP, VK_RIGHT, XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0,   XIME_KEY_RIGHT, (IMECALLBACK)Xime::ChangeClauseLength },          //for Japanese, Hangul
    { MODE_CONVERT | MODE_JP, VK_LEFT,  XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0,   XIME_KEY_LEFT, (IMECALLBACK)Xime::ChangeClauseLength },           //for Japanese, Hangul
    { MODE_CONVERT | MODE_JP, VK_ESCAPE,XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_FLUSH, (IMECALLBACK)Xime::Revert },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_BACK,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_FLUSH, (IMECALLBACK)Xime::Revert },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_SPACE, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_NEXT, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_NEXT, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0, (XIMEKEY)XIME_INDEX_PREV, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_UP,    XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_PREV, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_SPACE, XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0,   (XIMEKEY)XIME_INDEX_PREV, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_DOWN,  XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_NEXT, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_PRIOR, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_TOP_OF_PREV_PAGE, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_NEXT,  XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_TOP_OF_NEXT_PAGE, (IMECALLBACK)Xime::SetCandidate },           //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_HOME,  XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_TOP, (IMECALLBACK)Xime::SetCandidate },            //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_END,   XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_END, (IMECALLBACK)Xime::SetCandidate },            //for Japanese, Bopomofo
    { MODE_CONVERT | MODE_JP, VK_NONCONVERT, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_HIRAGANA_KATAKANA_TOGGLE, (IMECALLBACK)Xime::SetCandidate },            //for Japanese
    { MODE_CONVERT | MODE_JP, VK_F6,    XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_HIRAGANA_CANDIDATE, (IMECALLBACK)Xime::SetCandidate },            //for Japanese
    { MODE_CONVERT | MODE_JP, VK_F7,    XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_KATAKANA_CANDIDATE, (IMECALLBACK)Xime::SetCandidate },            //for Japanese
    { MODE_CONVERT | MODE_JP, L'U',     XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   (XIMEKEY)XIME_INDEX_HIRAGANA_CANDIDATE, (IMECALLBACK)Xime::SetCandidate },            //for Japanese
    { MODE_CONVERT | MODE_JP, L'I',     XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   (XIMEKEY)XIME_INDEX_KATAKANA_CANDIDATE, (IMECALLBACK)Xime::SetCandidate },            //for Japanese
    { MODE_CONVERT | MODE_JP, VK_NUMPAD0, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD1, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD2, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD3, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD4, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD5, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD6, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD7, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD8, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_NUMPAD9, XINPUT_KEYSTROKE_KEYDOWN, 0, (XIMEKEY)XIME_INDEX_CURRENT_PAGE_OFFSET, (IMECALLBACK)Xime::HandleNumpadConversion },          //for Japanese, special conversion
    { MODE_CONVERT | MODE_JP, VK_KANJI, XINPUT_KEYSTROKE_KEYDOWN, 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to Japanese
    { MODE_CONVERT | MODE_JP, VK_OEM_3, XINPUT_KEYSTROKE_KEYDOWN |XINPUT_KEYSTROKE_ALT , 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to English

    // All conversion mode
    { MODE_ALLCONVERT | MODE_JP, VK_RETURN,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },      //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_SEPARATOR,XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_COMPLETE, (IMECALLBACK)Xime::Complete },      //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_ESCAPE,  XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_FLUSH, (IMECALLBACK)Xime::AllRevert },           //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_BACK,    XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_FLUSH, (IMECALLBACK)Xime::AllRevert },           //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_F6,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_HIRAGANA, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_F7,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_KATAKANA, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_F9,      XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_ALPHANUMERIC_FULL_WIDTH, (IMECALLBACK)Xime::AllConvert },           //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_F10,     XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_ALL_ALPHANUMERIC_HALF_WIDTH, (IMECALLBACK)Xime::AllConvert },           //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, L'U',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_HIRAGANA, (IMECALLBACK)Xime::AllConvert },   //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, L'I',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_KATAKANA, (IMECALLBACK)Xime::AllConvert },   //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, L'P',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_ALPHANUMERIC_FULL_WIDTH, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, L'O',       XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_CTRL, 0,   XIME_KEY_ALL_ALPHANUMERIC_HALF_WIDTH, (IMECALLBACK)Xime::AllConvert },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_SPACE,   XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN, 0, XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_CONVERT, XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_SHIFT, 0, XIME_KEY_CONVERT, (IMECALLBACK)Xime::Convert },         //for Japanese, Bopomofo
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD0, XINPUT_KEYSTROKE_KEYDOWN, 0, L'0', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD1, XINPUT_KEYSTROKE_KEYDOWN, 0, L'1', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD2, XINPUT_KEYSTROKE_KEYDOWN, 0, L'2', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD3, XINPUT_KEYSTROKE_KEYDOWN, 0, L'3', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD4, XINPUT_KEYSTROKE_KEYDOWN, 0, L'4', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD5, XINPUT_KEYSTROKE_KEYDOWN, 0, L'5', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD6, XINPUT_KEYSTROKE_KEYDOWN, 0, L'6', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD7, XINPUT_KEYSTROKE_KEYDOWN, 0, L'7', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD8, XINPUT_KEYSTROKE_KEYDOWN, 0, L'8', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_NUMPAD9, XINPUT_KEYSTROKE_KEYDOWN, 0, L'9', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_MULTIPLY,XINPUT_KEYSTROKE_KEYDOWN, 0, L'*', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_ADD,     XINPUT_KEYSTROKE_KEYDOWN, 0, L'+', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_SUBTRACT,XINPUT_KEYSTROKE_KEYDOWN, 0, L'-', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_DECIMAL, XINPUT_KEYSTROKE_KEYDOWN, 0, L'.', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_DIVIDE,  XINPUT_KEYSTROKE_KEYDOWN, 0, L'/', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_OEM_NEC_EQUAL,  XINPUT_KEYSTROKE_KEYDOWN, 0, L'=', (IMECALLBACK)Xime::HandleNumpadInput },          //for Japanese, special conversion
    { MODE_ALLCONVERT | MODE_JP, VK_KANJI,   XINPUT_KEYSTROKE_KEYDOWN, 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to Japanese
    { MODE_ALLCONVERT | MODE_JP, VK_OEM_3,   XINPUT_KEYSTROKE_KEYDOWN |XINPUT_KEYSTROKE_ALT , 0, 0, (IMECALLBACK)Xime::ToggleIME },          //for Japanese, special conversion, when dashboard is set to English
    
};
static const DWORD NUM_INPUT_KEYMAP = sizeof(Xime::m_InputKeymap)/sizeof(Xime::m_InputKeymap[0]);

//---------------------------------------------------------------
//   Lookup Table For Japanese (Hiragana character) Non-alphabet characters
//---------------------------------------------------------------
HID_LOOKUP_TABLE JapaneseHiraganaNonAlphabetic[] = {
                                          { 0x1e,   0x38,   0,      0 },
/*HID_USAGE_INDEX_KEYBOARD_ONE*/          { 0x306C, 0x306C, 0x30CC, 0x30CC }, // nu, nu
/*HID_USAGE_INDEX_KEYBOARD_TWO*/          { 0x3075, 0x3075, 0x30D5, 0x30D5 }, // fu, fu
/*HID_USAGE_INDEX_KEYBOARD_THREE*/        { 0x3042, 0x3041, 0x30A2, 0x30A1 }, // a, a (small)
/*HID_USAGE_INDEX_KEYBOARD_FOUR*/         { 0x3046, 0x3045, 0x30A6, 0x30A5 }, // u, u (small)
/*HID_USAGE_INDEX_KEYBOARD_FIVE*/         { 0x3048, 0x3047, 0x30A8, 0x30A7 }, // e, e (small)
/*HID_USAGE_INDEX_KEYBOARD_SIX*/          { 0x304A, 0x3049, 0x30AA, 0x30A9 }, // o, o (small)
/*HID_USAGE_INDEX_KEYBOARD_SEVEN*/        { 0x3084, 0x3083, 0x30E4, 0x30E3 }, // ya, ya (small)
/*HID_USAGE_INDEX_KEYBOARD_EIGHT*/        { 0x3086, 0x3085, 0x30E6, 0x30E5 }, // yu, yu (small)
/*HID_USAGE_INDEX_KEYBOARD_NINE*/         { 0x3088, 0x3087, 0x30E8, 0x30E7 }, // yo, yo (small)
/*HID_USAGE_INDEX_KEYBOARD_ZERO*/         { 0x308F, 0x3092, 0x30EF, 0x30F2 }, // wa, o
/*HID_USAGE_INDEX_KEYBOARD_RETURN*/       { 0x000A, 0x000A, 0x000A, 0x000A }, //LF,LF
/*HID_USAGE_INDEX_KEYBOARD_ESCAPE*/       { 0x001B, 0x001B, 0x001B, 0x001B }, //ESC,ESC
/*HID_USAGE_INDEX_KEYBOARD_BACKSPACE*/    { 0x0008, 0x0008, 0x0008, 0x0008 }, //BS
/*HID_USAGE_INDEX_KEYBOARD_TAB*/          { 0x0000, 0x0000, 0x0000, 0x0000 }, //TAB is not supported
/*HID_USAGE_INDEX_KEYBOARD_SPACEBAR*/     { 0x0020, 0x0020, 0x0020, 0x0020 }, //SPACE,SPACE
/*HID_USAGE_INDEX_KEYBOARD_MINUS*/        { 0x307B, 0x307B, 0x30DB, 0x30DB }, // ho, ho
/*HID_USAGE_INDEX_KEYBOARD_EQUALS*/       { 0x3078, 0x3078, 0x30D8, 0x30D8 }, // he, he
/*HID_USAGE_INDEX_KEYBOARD_OPEN_BRACE*/   { 0x309B, 0x309B, 0x309B, 0x309B }, // voiced sound mark, voiced sound mark
/*HID_USAGE_INDEX_KEYBOARD_CLOSE_BRACE*/  { 0x309C, 0x300C, 0x309C, 0x300C }, // semi-voiced sound mark, left corner bracket
/*HID_USAGE_INDEX_KEYBOARD_BACKSLASH*/    { 0x0000, 0x0000, 0x0000, 0x0000 }, // no character, no character
/*HID_USAGE_INDEX_KEYBOARD_NON_US_TILDE*/ { 0x3080, 0x300D, 0x30E0, 0x300D }, // mu, right corner bracket
/*HID_USAGE_INDEX_KEYBOARD_COLON*/        { 0x308C, 0x308C, 0x30EC, 0x30EC }, // re, re
/*HID_USAGE_INDEX_KEYBOARD_QUOTE*/        { 0x3051, 0x3051, 0x30B1, 0x30B1 }, // ke, ke
/*HID_USAGE_INDEX_KEYBOARD_TILDE*/        { 0x0000, 0x0000, 0x0000, 0x0000 }, // no character, no character (Japanese/English toggle)
/*HID_USAGE_INDEX_KEYBOARD_COMMA*/        { 0x306D, 0x3001, 0x30CD, 0x3001 }, // ne, ideographic comma
/*HID_USAGE_INDEX_KEYBOARD_PERIOD*/       { 0x308B, 0x3002, 0x30EB, 0x3002 }, // ru,ideographic full stop
/*HID_USAGE_INDEX_KEYBOARD_QUESTION*/     { 0x3081, 0x30FB, 0x30E1, 0x30FB }  // me, middle dot
};

//---------------------------------------------------------------
//   Lookup Table For Japanese (Hiragana character) alphabet characters
//---------------------------------------------------------------
HID_LOOKUP_TABLE JapaneseHiraganaAlphabetic[] = {
                                          { 0x04,   0x1d,   0,      0 },
/*HID_USAGE_INDEX_KEYBOARD_aA*/           { 0x3061, 0x3061, 0x30C1, 0x30C1 },  // chi, chi
/*HID_USAGE_INDEX_KEYBOARD_bB*/           { 0x3053, 0x3053, 0x30B3, 0x30B3 },  // ko, ko
/*HID_USAGE_INDEX_KEYBOARD_cC*/           { 0x305D, 0x305D, 0x30BD, 0x30BD },  // so, so
/*HID_USAGE_INDEX_KEYBOARD_dD*/           { 0x3057, 0x3057, 0x30B7, 0x30B7 },  // shi, shi
/*HID_USAGE_INDEX_KEYBOARD_eE*/           { 0x3044, 0x3043, 0x30A4, 0x30A3 },  // i, i (small)
/*HID_USAGE_INDEX_KEYBOARD_fF*/           { 0x306F, 0x306F, 0x30CF, 0x30CF },  // ha, ha
/*HID_USAGE_INDEX_KEYBOARD_gG*/           { 0x304D, 0x304D, 0x30AD, 0x30AD },  // ki, ki
/*HID_USAGE_INDEX_KEYBOARD_hH*/           { 0x304F, 0x304F, 0x30AF, 0x30AF },  // ku, ku
/*HID_USAGE_INDEX_KEYBOARD_iI*/           { 0x306B, 0x306B, 0x30CB, 0x30CB },  // ni, ni
/*HID_USAGE_INDEX_KEYBOARD_jJ*/           { 0x307E, 0x307E, 0x30DE, 0x30DE },  // ma, ma
/*HID_USAGE_INDEX_KEYBOARD_kK*/           { 0x306E, 0x306E, 0x30CE, 0x30CE }, // no, no
/*HID_USAGE_INDEX_KEYBOARD_lL*/           { 0x308A, 0x308A, 0x30EA, 0x30EA }, // ri, ri
/*HID_USAGE_INDEX_KEYBOARD_mM*/           { 0x3082, 0x3082, 0x30E2, 0x30E2 }, // mo, mo
/*HID_USAGE_INDEX_KEYBOARD_nN*/           { 0x307F, 0x307F, 0x30DF, 0x30DF }, // mi, mi
/*HID_USAGE_INDEX_KEYBOARD_oO*/           { 0x3089, 0x3089, 0x30E9, 0x30E9 }, // ra,ra
/*HID_USAGE_INDEX_KEYBOARD_pP*/           { 0x305B, 0x305B, 0x30BB, 0x30BB }, // se, se
/*HID_USAGE_INDEX_KEYBOARD_qQ*/           { 0x305F, 0x305F, 0x30BF, 0x30BF }, // ta, ta
/*HID_USAGE_INDEX_KEYBOARD_rR*/           { 0x3059, 0x3059, 0x30B9, 0x30B9 }, // su, su
/*HID_USAGE_INDEX_KEYBOARD_sS*/           { 0x3068, 0x3068, 0x30C8, 0x30C8 }, // to, to
/*HID_USAGE_INDEX_KEYBOARD_tT*/           { 0x304B, 0x304B, 0x30AB, 0x30AB }, // ka, ka
/*HID_USAGE_INDEX_KEYBOARD_uU*/           { 0x306A, 0x306A, 0x30CA, 0x30CA }, // na, na
/*HID_USAGE_INDEX_KEYBOARD_vV*/           { 0x3072, 0x3072, 0x30D2, 0x30D2 }, // hi, hi
/*HID_USAGE_INDEX_KEYBOARD_wW*/           { 0x3066, 0x3066, 0x30C6, 0x30C6 }, // te, te
/*HID_USAGE_INDEX_KEYBOARD_xX*/           { 0x3055, 0x3055, 0x30B5, 0x30B5 }, // sa, sa
/*HID_USAGE_INDEX_KEYBOARD_yY*/           { 0x3093, 0x3093, 0x30F3, 0x30F3 }, // n, n
/*HID_USAGE_INDEX_KEYBOARD_zZ*/           { 0x3064, 0x3063, 0x30C4, 0x30C3 }  // tsu, tsu (small)
};


//--------------------------------------------------------------------------------------
// Name: Init
// Desc: Initialize the XIME
//--------------------------------------------------------------------------------------
HRESULT Xime::Init( IMEMODE eLanguage, DWORD dwCharLength, D3DPRESENT_PARAMETERS * pD3DParam )
{
    assert( pD3DParam != NULL );

    switch ( eLanguage )
    {
    case MODE_JP:
        {
        //Initialize parameters
        XIME_DICTIONARY    XimeDic[ iNumDics ];
        ZeroMemory( XimeDic, sizeof(XimeDic) );
        XimeDic[ 0 ].eDicType    = XIME_DIC_JP_MAIN;
        XimeDic[ 0 ].pcszDicFile = szMainDicPath;
        XimeDic[ 1 ].eDicType    = XIME_DIC_JP_SUB;
        XimeDic[ 1 ].pcszDicFile = szSubDicPath;
        XimeDic[ 2 ].eDicType    = XIME_DIC_JP_TITLE;
        XimeDic[ 2 ].pcszDicFile = szTitleDicPath;

        //Initialzie IME
        XIME_CREATE    XimeJpCreate = { 0 };
        XimeJpCreate.dwNumberOfDictionaries = iNumDics;
        XimeJpCreate.pXimeDictionary = XimeDic;
        XimeJpCreate.dwNumberOfCandidateListInPage = NUM_CANDIATEINPAGE;

        if( XIMECreate( XIMEJPInit, &XimeJpCreate ) != ERROR_SUCCESS )
            return E_FAIL;
        m_CurrentLanguage = MODE_JP;
        }
        break;
    default:
        return E_FAIL;
    }

    // Work around for API issue
    //if( XIMESetCharacterLimit( dwCharLimit ) != ERROR_SUCCESS )
    //        return E_FAIL;

    SetCurrentMode( MODE_INPUT );
    m_bNeedBlocking = false;
    m_dwCharsInXime = 0;
    m_iInputMode = XIME_MODE_JP_ROMAJI_HIRAGANA;
    m_iKeyboardLayout = XIME_LAYOUT_ALPHANUMERIC;

    m_BackBufferWidth = (float)pD3DParam->BackBufferWidth;
    m_BackBufferHeight = (float)pD3DParam->BackBufferHeight;

    m_bIMEOn = true;
    m_bCompleted = false;

    if( !dwCharLength )
        m_dwInputCharacterLength = DEFAULT_CHARACTERLENGTH;
    else
        m_dwInputCharacterLength = dwCharLength;
    XIMESetCharacterLimit( m_dwInputCharacterLength );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Update
// Desc: Update Xime
//--------------------------------------------------------------------------------------
HRESULT Xime::Update()
{
    HRESULT hr = S_OK;
 
       //Check request cue status
    if( !m_RequestList.empty() )
    {
        if( XHasOverlappedIoCompleted( &m_RequestList.front().ov ) )
        {
            // Check error code.
            DWORD dwResult;
            XGetOverlappedResult( &m_RequestList.front().ov, &dwResult, false );
            if( dwResult != ERROR_SUCCESS )
                ATG::DebugSpew( "XIME call failed with Error %x.\n", dwResult );
            m_RequestList.pop_front();
        }
        if( m_bNeedBlocking )
            return S_OK;
    }
    else
        m_bNeedBlocking = false;

    hr = XInputGetKeystroke( XUSER_INDEX_ANY, XINPUT_FLAG_KEYBOARD, &m_Key );
    if( hr != ERROR_SUCCESS )
        return hr;

    if( !m_bIMEOn )
    {
        // Now that IME is OFF
        // Just returns input chars to the application
        if ( XGetLanguage() == XC_LANGUAGE_JAPANESE )
        {
            if( m_Key.VirtualKey == VK_KANJI  && (m_Key.Flags & XINPUT_KEYSTROKE_KEYDOWN) )
            {
                m_bIMEOn = true;
                return S_OK;
            }
        }
        else
        {
            if( (m_Key.VirtualKey == VK_OEM_3) && (m_Key.Flags & XINPUT_KEYSTROKE_KEYDOWN) && (m_Key.Flags & XINPUT_KEYSTROKE_ALT) )
            {
                m_bIMEOn = true;
                return S_OK;
            }

        }
        return ERROR_SERVICE_DISABLED;
    }

    //Just ingnore released key
    if( m_Key.Flags & XINPUT_KEYSTROKE_KEYUP ) return S_OK;

    // Ignore some flags
    m_Key.Flags &= ~( XINPUT_KEYSTROKE_VALIDUNICODE |
                    XINPUT_KEYSTROKE_CAPSLOCK |
                    XINPUT_KEYSTROKE_NUMLOCK |
                    XINPUT_KEYSTROKE_ROMAJI |
                    XINPUT_KEYSTROKE_HIRAGANA |
                    XINPUT_KEYSTROKE_HANGUL |
                    XINPUT_KEYSTROKE_BOPOMOFO |
                    XINPUT_KEYSTROKE_CHAJEI |
                    XINPUT_KEYSTROKE_REMOTE |
                    XINPUT_KEYSTROKE_REPEAT );

    DWORD         dwFlags = m_CurrentImeMode | m_CurrentLanguage; 
    XIMEKEY       eControlKey = XIME_KEY_UNICODE;
    ATG::IMECALLBACK  CallBack = NULL;
    int i;

    // Look up a table...
    for( i = 0; i < NUM_INPUT_KEYMAP; i++ )
    {
        if( (dwFlags & m_InputKeymap[ i ].dwFlags) == dwFlags
            && m_Key.VirtualKey == m_InputKeymap[ i ].wVK
            && ( ( m_Key.Flags & ~m_InputKeymap[ i ].wIgnoredXinputFlags)
                                     == m_InputKeymap[ i ].wXinputFlags ) )
        {
            eControlKey = m_InputKeymap[ i ].eControlKey;
            CallBack = m_InputKeymap[ i ].function;
            break;
        }
    }

    // Look up Unicode from HID
    INT iInputMode = m_iInputMode;
    LayoutConversion( &m_Key, &iInputMode );

    if( CallBack )
    {
        // Then, invoke callback
        hr = CallBack( this, &m_InputKeymap[ i ] );
    }
    else if( m_Key.Unicode || eControlKey != XIME_KEY_UNICODE )
    {
        if( m_CurrentImeMode == MODE_CONVERT || m_CurrentImeMode == MODE_ALLCONVERT )
        {
            Complete( this, &m_InputKeymap[ 0 ] );
            // Need to block since input mode has been changed
            SetCurrentMode( MODE_INPUT );
        }
        //Input some letters
        XIMEREQUEST request = { 0 };
        m_RequestList.push_back( request );

        if( eControlKey != XIME_KEY_UNICODE )
        {
            // API does not accept Unicode value except when using XIME_KEY_UNICODEt
            m_Key.Unicode = 0x0;
        }
        if( !m_dwCharsInXime )
        {
            // Reset IME string length
            XIMESetCharacterLimit( GetInputCharacterLength() ); 
        }
        
        DWORD dwRet = XIMEInsertCharacter( m_Key.Unicode,
                                           eControlKey,
                                           iInputMode,
                                           &m_RequestList.back().ov );

        if( dwRet != ERROR_IO_INCOMPLETE )
            m_RequestList.pop_back();

        m_dwCharsInXime++;

    }
    if( m_bCompleted )
    {
        // Input sequence completed
        // reset the counter, the conversion could be completed by non return key
        m_dwCharsInXime = 0;
    }
    return hr;
}


//--------------------------------------------------------------------------------------
// Name: GetString
// Desc: RetrieveString
//--------------------------------------------------------------------------------------
HRESULT Xime::GetString( std::wstring & stringOut )
{
    if( !m_RequestList.empty() ) return ERROR_NOT_READY;
    stringOut = m_CompletedString;
    m_bCompleted = false;
    m_CompletedString.clear();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: IsStringReady
// Desc:
//--------------------------------------------------------------------------------------
BOOL    Xime::IsStringReady( void )
{
    if( m_bCompleted )
    {
        // Block until all requets done
        while ( m_RequestList.size() > 0 )
        {
            if( XHasOverlappedIoCompleted( &m_RequestList.front().ov ) )
                m_RequestList.pop_front();
            else
                return false;
        } 
        return true;
    }
    else
        return false;
}
 

//--------------------------------------------------------------------------------------
// Name: Render
// Desc: Render Xime
//--------------------------------------------------------------------------------------
HRESULT Xime::Render( HXUIDC hDC, HXUIFONT hFont, float fPosX, float fPosY )
{
    assert( hDC != NULL );
    assert( hFont != NULL );

    DWORD   dwRet;
    DWORD   dwFocus;
    DWORD   dwCursor;
    DWORD   dwClauses;
    DWORD   dwDeterminedClause;
    
    DWORD   dwNumCandidate = 0;
    DWORD   dwCandidateIndex = 0;
    DWORD   dwHighLightIndexInlist;
    DWORD   dwNumberOfCandidateStringsInList;
    bool    bListAvailable = false;
    
    if( !m_bIMEOn )
    {
        return ERROR_SERVICE_DISABLED;
    }
    // Retrieve current clause information of IME
    dwRet = XIMEGetClauseInfo( &dwFocus,
                               &dwClauses,
                               &dwCursor,
                               &dwDeterminedClause,
                               m_StringBuffer,
                               sizeof( m_StringBuffer ) / sizeof( WCHAR ),
                               NULL,
                               NULL, 
                               NULL,
                               0 );
    if( dwRet != ERROR_SUCCESS ) return E_FAIL;

    // Retrieve candidate list if IME is in conversion mode
    if( m_CurrentImeMode == MODE_CONVERT )
    {
        dwRet = XIMEGetCandidateListInfo( m_CandidateListBuffer,
                                          sizeof( m_CandidateListBuffer ) / sizeof( WCHAR ),
                                          NULL,
                                          &dwCandidateIndex,
                                          &dwNumCandidate,
                                          &dwHighLightIndexInlist,
                                          &dwNumberOfCandidateStringsInList );
        if( dwRet == ERROR_SUCCESS ) bListAvailable = true;
    }


    // Begin Xui rendering
    XuiRenderBegin( hDC, D3DCOLOR_ARGB( 255, 0, 0, 0 ) );
 
    // Set the view
    D3DXMATRIX matView;
    D3DXMatrixIdentity( &matView );
    XuiRenderSetViewTransform( hDC, &matView );

    WCHAR * pString = m_StringBuffer;
    D3DCOLOR color;

    m_CurrentString = L"";

    //Show typed string
    color = COLOR_NORMAL;
    
    float fX = fPosX;
    // Render clause, candidate list
    for( DWORD i = 0; i < dwClauses; i++ )
    {
        if( i == dwFocus )
        {
            // Draw focus string
            color = COLOR_FOCUS;
            float fWidth, fHeight;
            DrawText( hDC, hFont, color, fX, fPosY, pString, &fWidth, &fHeight );
            if( bListAvailable && m_bShowCandidateList )
            {
                float fY = fPosY + fHeight;
                WCHAR * pCandidateString = m_CandidateListBuffer;

                // draw candidate list
                for( DWORD j = 0; j < dwNumCandidate; j++ )
                {
                    if( j == dwCandidateIndex )
                        color = COLOR_FOCUS;
                    else
                        color = COLOR_LIST;
                
                    DrawText( hDC, hFont, color, fX, fY, pCandidateString, NULL, &fHeight );
                    INT iStrLen = wcslen( pCandidateString );
                    pCandidateString += iStrLen + 1;
                    fY += fHeight;
                }
            }
            fX += fWidth;
        }
        else
        {
            // Draw other clause strings in IME
            color = COLOR_INIME;
            float fWidth;
            DrawText( hDC, hFont, color, fX, fPosY, pString, &fWidth );
            fX += fWidth;
        }
        m_CurrentString += pString;

        INT iStrLen = wcslen( pString );
        pString += iStrLen + 1;
    }
 
    //Show cursor
    if( dwCursor >= 0 && dwCursor <= (DWORD)m_CurrentString.length() )
    {
        // Measure the cursor position
        pString = (WCHAR * )m_CurrentString.data();
        XUIRect clipRect( 0, 0, m_BackBufferWidth, m_BackBufferHeight );
        if( dwCursor )
            XuiMeasureText( hFont, pString, dwCursor,
                            XUI_FONT_STYLE_NORMAL | XUI_FONT_STYLE_SINGLE_LINE,
                            0, &clipRect );
        else
            clipRect.right = 0; //Work around for XuiMeasureText behaivor

        // Draw the cursor if it's inside a screen
        color = COLOR_NORMAL;
        if( fPosX + clipRect.right < m_BackBufferWidth )
            DrawText( hDC, hFont, color, fPosX + clipRect.right, fPosY, L"|" );
    }

    // Complete Xui rendering
    XuiRenderEnd( hDC );
    XuiRenderPresent( hDC, NULL, NULL, NULL );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: DrawText
// Desc: Draw text at the given coordinates with the given color.
//--------------------------------------------------------------------------------------
void Xime::DrawText( HXUIDC hdc, HXUIFONT hFont, D3DCOLOR color, float x, float y,
                     LPCWSTR text, float * pfX, float * pfY )
{
    assert( hdc != NULL );
    assert( hFont != NULL );
    assert( text );

    // Measure the text
    XUIRect clipRect( 0, 0, m_BackBufferWidth - x, m_BackBufferHeight - y );
    XuiMeasureText( hFont, text, -1, XUI_FONT_STYLE_NORMAL | XUI_FONT_STYLE_SINGLE_LINE,
                    0, &clipRect );
 
    // Set the text position in the device context
    D3DXMATRIX matXForm;
    D3DXMatrixIdentity( &matXForm );
    matXForm._41 = x;
    matXForm._42 = y;
    XuiRenderSetTransform( hdc, &matXForm );
    
    // Select the font and color into the device context
    XuiSelectFont( hdc, hFont );
    XuiSetColorFactor( hdc, (DWORD)color );
    
    // Draw the text
    XuiDrawText( hdc, text, XUI_FONT_STYLE_NORMAL, 0, &clipRect );

    if( pfX != NULL ) *pfX = clipRect.GetWidth();
    if( pfY != NULL ) *pfY = clipRect.GetHeight();
    return;
}


//--------------------------------------------------------------------------------------
// Callback functions
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Name: Convert
// Desc: Start a conversion process
//--------------------------------------------------------------------------------------
HRESULT Xime::Convert( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );

    if( !pXIME->m_dwCharsInXime )
    {
        // Just returns white space
        pXIME->m_bCompleted = TRUE;
        pXIME->m_CompletedString = L" ";
        return S_OK;
    }

    pXIME->SetCurrentMode( MODE_CONVERT );
    //Input some letters
    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMEInsertCharacter( 0,
                                       XIME_KEY_CONVERT,
                                       pXIME->m_iInputMode,
                                       &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    // Build candidate list
    BuildCandidateList( pXIME, pKey );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: ChangeFocus
// Desc: Chang clause focus
//--------------------------------------------------------------------------------------
HRESULT Xime::ChangeFocus( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_CONVERT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    bool bMoveToRight = ( pKey->wVK == VK_RIGHT ? true : false );
    DWORD dwRet = XIMEMoveClauseFocus( bMoveToRight, &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    BuildCandidateList( pXIME, pKey );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: ChangeClauseLength
// Desc: Change clause length in focus
//--------------------------------------------------------------------------------------
HRESULT Xime::ChangeClauseLength( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_CONVERT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    bool bExpand = ( pKey->wVK == VK_RIGHT ? true : false );
    DWORD dwRet = XIMEChangeClauseLength( bExpand, &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    BuildCandidateList( pXIME, pKey );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Revert
// Desc: Revert conversion
//--------------------------------------------------------------------------------------
HRESULT Xime::Revert( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_CONVERT );

    pXIME->SetCurrentMode( MODE_INPUT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMERevertString( &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: SetCandidate
// Desc: Notify a candidate to IME
//--------------------------------------------------------------------------------------
HRESULT Xime::SetCandidate( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_CONVERT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMESetCandidateIndex( (XIMEINDEX)pKey->eControlKey, 0,
                                         true, &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    pXIME->m_bShowCandidateList = true;
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: BuildCandidateList
// Desc: Request candidate list to IME
//--------------------------------------------------------------------------------------
HRESULT Xime::BuildCandidateList( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    DWORD dwRet = XIMEBuildCandidateList( &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    pXIME->m_bShowCandidateList = false;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: Complete
// Desc: Complete IME conversion
//--------------------------------------------------------------------------------------
HRESULT Xime::Complete( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
        
    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );

    DWORD dwRet = XIMEInsertCharacter( 0,
                                       XIME_KEY_COMPLETE,
                                       pXIME->m_iInputMode,
                                       &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
    {
        pXIME->m_RequestList.pop_back();
    }
    else
    {
        pXIME->SetCurrentMode( MODE_INPUT );
        pXIME->m_bCompleted = TRUE;
        pXIME->m_CompletedString = pXIME->m_CurrentString;
        pXIME->m_dwCharsInXime = 0;

    }
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: HandleNumpadInput
// Desc: Handle numpad inputs
//--------------------------------------------------------------------------------------
HRESULT Xime::HandleNumpadInput( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
        
    if( pXIME->m_CurrentImeMode == MODE_CONVERT || pXIME->m_CurrentImeMode == MODE_ALLCONVERT )
    {
        Complete( pXIME, &m_InputKeymap[ 0 ] );
        // Need to block since input mode has been changed
        pXIME->SetCurrentMode( MODE_INPUT );
    }
    //Input some letters
    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );

    DWORD dwRet = XIMEInsertCharacter(  (WCHAR)pKey->dwUnicode,
                                        XIME_KEY_UNICODE,
                                        XIME_MODE_JP_HALFWIDTH_ALPHANUMERIC,
                                        &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
    {
        pXIME->m_RequestList.pop_back();
    }
    pXIME->m_dwCharsInXime++;
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: HandleNumpadConversion
// Desc: Handle numpad inputs
//--------------------------------------------------------------------------------------
HRESULT Xime::HandleNumpadConversion( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_CONVERT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMESetCandidateIndex( XIME_INDEX_CURRENT_PAGE_OFFSET,
                                         pKey->wVK - VK_NUMPAD1, true,
                                         &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    pXIME->m_bShowCandidateList = true;
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: AllConvert
// Desc: Start a conversion process
//--------------------------------------------------------------------------------------
HRESULT Xime::AllConvert( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );

    pXIME->SetCurrentMode( MODE_ALLCONVERT );
    //Input some letters
    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMEInsertCharacter( 0,
                                       pKey->eControlKey,
                                       pXIME->m_iInputMode,
                                       &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: AllRevert
// Desc: Revert all conversion
//--------------------------------------------------------------------------------------
HRESULT Xime::AllRevert( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_ALLCONVERT );

    pXIME->SetCurrentMode( MODE_INPUT );

    XIMEREQUEST request = { 0 };
    pXIME->m_RequestList.push_back( request );
    
    DWORD dwRet = XIMEInsertCharacter(  0,
                                        XIME_KEY_END,
                                        XIME_MODE_JP_HALFWIDTH_ALPHANUMERIC,
                                        &pXIME->m_RequestList.back().ov );

    if( dwRet != ERROR_IO_INCOMPLETE )
        pXIME->m_RequestList.pop_back();

    return S_OK;
}

//--------------------------------------------------------------------------------------
// Name: ChangeInputMode
// Desc: Change IME input mode
//--------------------------------------------------------------------------------------
HRESULT Xime::ChangeInputMode( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );
    assert( pXIME->m_CurrentImeMode == MODE_INPUT );

    pXIME->m_iInputMode = pKey->eControlKey;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: SetCurrentMode
// Desc: Switch current IME mode
//--------------------------------------------------------------------------------------
void    Xime::SetCurrentMode( IMEMODE mode )
{
    assert( mode == MODE_CONVERT || mode == MODE_INPUT || mode == MODE_ALLCONVERT );

    if( m_CurrentImeMode != mode )
    {
        m_bNeedBlocking = true;
    }
    m_CurrentImeMode = mode;
}


//--------------------------------------------------------------------------------------
// Name: ToggleIME
// Desc: Toggle IME
//--------------------------------------------------------------------------------------
HRESULT Xime::ToggleIME( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );

    if( pXIME->m_bIMEOn )
    {
        pXIME->Complete( pXIME, &pXIME->m_InputKeymap[ 0 ] );
        // Need to block since input mode has been changed
        pXIME->SetCurrentMode( MODE_INPUT );
        pXIME->m_bIMEOn = false;
    }
    else
        pXIME->m_bIMEOn = true;
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: ToggleKeyboardLayout
// Desc: Toggle Keyboard layout
//--------------------------------------------------------------------------------------
HRESULT Xime::ToggleKeyboardLayout( Xime * pXIME, XIME_KEYMAPPING * pKey )
{
    assert( pXIME != NULL );
    assert( pKey != NULL );

    if( pXIME->m_CurrentLanguage == MODE_JP )
    {
        if( pXIME->m_iKeyboardLayout == XIME_LAYOUT_ALPHANUMERIC )
        {
            pXIME->m_iKeyboardLayout = XIME_LAYOUT_KANA;
        }
        else
        {
            pXIME->m_iKeyboardLayout = XIME_LAYOUT_ALPHANUMERIC;
        }
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: LayoutConversion
// Desc: Keyboard layout conversion routine. Language specific
//--------------------------------------------------------------------------------------
HRESULT Xime::LayoutConversion( XINPUT_KEYSTROKE* pKey, INT* pInputMode )
{
    if( m_CurrentLanguage == MODE_JP )
    {
        if( m_iKeyboardLayout == XIME_LAYOUT_KANA )
        {
            pKey->Unicode = HidUnicodeLookUpJP( pKey->HidCode,
                                                pKey->Flags & XINPUT_KEYSTROKE_SHIFT,
                                                m_iInputMode == XIME_MODE_JP_ROMAJI_KATAKANA,
                                                JapaneseHiraganaNonAlphabetic,
                                                JapaneseHiraganaAlphabetic );
            *pInputMode = XIME_LANGUAGE_JAPANESE | XIME_LAYOUT_KANA |
                          (pKey->Flags & XINPUT_KEYSTROKE_SHIFT ? XIME_COMBINE_KANA_SHIFT : 0);
        }
    }

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: HidUnicodeLookUp
// Desc: Look up Kana characters
//--------------------------------------------------------------------------------------
WCHAR Xime::HidUnicodeLookUpJP( DWORD dwHidCode, BOOL fShifted, BOOL fKana,
                                HID_LOOKUP_TABLE * pTable, HID_LOOKUP_TABLE * pTable2 )
{
    HID_LOOKUP_TABLE *pLookupTable;
    if( ( dwHidCode >= pTable[ 0 ].wHiragana ) && ( dwHidCode <= pTable[ 0 ].wHiraganaShifted ) )
        pLookupTable = pTable;
    else if( ( dwHidCode >= pTable2[ 0 ].wHiragana ) &&
             ( dwHidCode <= pTable2[ 0 ].wHiraganaShifted ) )
        pLookupTable = pTable2;
    else if ( dwHidCode == 0x87 )
    {
        //
        // Handle the special Japanese international keycode for the prolonged sound mark
        //
        return fKana ? 0x30ED : 0x308D; // ro character
    }
    else if( dwHidCode == 0x89 )
    {
        return 0x30FC; // Prolonged sound mark
    } else return 0;

    dwHidCode -= pLookupTable[ 0 ].wHiragana-1;
    if( fKana )
        return fShifted ? pLookupTable[ dwHidCode ].wKatakanaShifted :
                          pLookupTable[ dwHidCode ].wKatakana;
    else
        return fShifted ? pLookupTable[ dwHidCode ].wHiraganaShifted :
                          pLookupTable[ dwHidCode ].wHiragana;
}


}; // namespace ATG
