//--------------------------------------------------------------------------------------
// AtgDevice.h
//
// Over-loaded device to trap and optimize certain calls to D3D
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
#ifndef ATGDEVICE_H
#define ATGDEVICE_H

namespace ATG
{

//--------------------------------------------------------------------------------------
// struct ATG::D3DDevice
// Over-loaded device to trap and optimize certain calls to D3D
//--------------------------------------------------------------------------------------
struct D3DDevice : public ::D3DDevice
{
    D3DVOID WINAPI SetVertexShader( D3DVertexShader *pShader );
    D3DVOID WINAPI SetPixelShader( D3DPixelShader* pShader );
    D3DVOID WINAPI SetVertexDeclaration( D3DVertexDeclaration *pDecl);
    HRESULT WINAPI CreateVertexDeclaration( CONST D3DVERTEXELEMENT9* pVertexElements,
                                            D3DVertexDeclaration **ppVertexDeclaration );
};


} // namespace ATG

#endif // ATGDEVICE_H
