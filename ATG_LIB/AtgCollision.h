//-----------------------------------------------------------------------------
// AtgCollision.h
//
// A VMX-128 optimized collision library.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once
#ifndef ATG_COLLISION_H
#define ATG_COLLISION_H

#include <xboxmath.h>

namespace ATG
{

//-----------------------------------------------------------------------------
// Bounding volumes structures.
//
// The bounding volume structures are setup for near minimum size because there
// are likely to be many of them, and memory bandwidth and space will be at a
// premium relative to CPU cycles on Xbox 360.
//-----------------------------------------------------------------------------

struct Sphere
{
    XMFLOAT3 Center;            // Center of the sphere.
    FLOAT Radius;               // Radius of the sphere.
};

struct AxisAlignedBox
{
    XMFLOAT3 Center;            // Center of the box.
    XMFLOAT3 Extents;           // Distance from the center to each side.
};

struct OrientedBox
{
    XMFLOAT3 Center;            // Center of the box.
    XMFLOAT3 Extents;           // Distance from the center to each side.
    XMFLOAT4 Orientation;       // Unit quaternion representing rotation (box -> world).
};

struct Frustum
{
    XMFLOAT3 Origin;            // Origin of the frustum (and projection).
    XMFLOAT4 Orientation;       // Unit quaternion representing rotation.

    FLOAT RightSlope;           // Positive X slope (X/Z).
    FLOAT LeftSlope;            // Negative X slope.
    FLOAT TopSlope;             // Positive Y slope (Y/Z).
    FLOAT BottomSlope;          // Negative Y slope.
    FLOAT Near, Far;            // Z of the near plane and far plane.
};


//-----------------------------------------------------------------------------
// Bounding volume construction.
//-----------------------------------------------------------------------------
VOID ComputeBoundingSphereFromPoints( Sphere* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );
VOID ComputeBoundingOrientedBoxFromPoints( OrientedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );
VOID ComputeFrustumFromProjection( Frustum* pOut, XMMATRIX* pProjection );
VOID ComputePlanesFromFrustum( const Frustum* pVolume, XMVECTOR* pPlane0, XMVECTOR* pPlane1, XMVECTOR* pPlane2, XMVECTOR* pPlane3, XMVECTOR* pPlane4, XMVECTOR* pPlane5 );


//-----------------------------------------------------------------------------
// Bounding volume transforms.
//-----------------------------------------------------------------------------
VOID TransformSphere( Sphere* pOut, const Sphere *pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation );
VOID TransformAxisAlignedBox( AxisAlignedBox* pOut, const AxisAlignedBox* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation );
VOID TransformOrientedBox( OrientedBox* pOut, const OrientedBox* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation );
VOID TransformFrustum( Frustum* pOut, const Frustum* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation );


//-----------------------------------------------------------------------------
// Intersection testing routines.
//-----------------------------------------------------------------------------
BOOL IntersectPointSphere( XMVECTOR Point, const Sphere* pVolume );
BOOL IntersectPointAxisAlignedBox( XMVECTOR Point, const AxisAlignedBox* pVolume );
BOOL IntersectPointOrientedBox( XMVECTOR Point, const OrientedBox* pVolume );
BOOL IntersectPointFrustum( XMVECTOR Point, const Frustum* pVolume );
BOOL IntersectRayTriangle( XMVECTOR Origin, XMVECTOR Direction, XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, FLOAT* pDist );
BOOL IntersectRaySphere( XMVECTOR Origin, XMVECTOR Direction, const Sphere* pVolume, FLOAT* pDist );
BOOL IntersectRayAxisAlignedBox( XMVECTOR Origin, XMVECTOR Direction, const AxisAlignedBox* pVolume, FLOAT* pDist );
BOOL IntersectRayOrientedBox( XMVECTOR Origin, XMVECTOR Direction, const OrientedBox* pVolume, FLOAT* pDist );
BOOL IntersectTriangleTriangle( XMVECTOR A0, XMVECTOR A1, XMVECTOR A2, XMVECTOR B0, XMVECTOR B1, XMVECTOR B2 );
BOOL IntersectTriangleSphere( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const Sphere* pVolume );
BOOL IntersectTriangleAxisAlignedBox( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const AxisAlignedBox* pVolume );
BOOL IntersectTriangleOrientedBox( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const OrientedBox* pVolume );
BOOL IntersectSphereSphere( const Sphere* pVolumeA, const Sphere* pVolumeB );
BOOL IntersectSphereAxisAlignedBox( const Sphere* pVolumeA, const AxisAlignedBox* pVolumeB );
BOOL IntersectSphereOrientedBox( const Sphere* pVolumeA, const OrientedBox* pVolumeB );
BOOL IntersectAxisAlignedBoxAxisAlignedBox( const AxisAlignedBox* pVolumeA, const AxisAlignedBox* pVolumeB );
BOOL IntersectAxisAlignedBoxOrientedBox( const AxisAlignedBox* pVolumeA, const OrientedBox* pVolumeB );
BOOL IntersectOrientedBoxOrientedBox( const OrientedBox* pVolumeA, const OrientedBox* pVolumeB );


//-----------------------------------------------------------------------------
// Frustum intersection testing routines.
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = A is completely inside B
//-----------------------------------------------------------------------------
INT IntersectTriangleFrustum( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const Frustum* pVolume );
INT IntersectSphereFrustum( const Sphere* pVolumeA, const Frustum* pVolumeB );
INT IntersectAxisAlignedBoxFrustum( const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB );
INT IntersectOrientedBoxFrustum( const OrientedBox* pVolumeA, const Frustum* pVolumeB );
INT IntersectFrustumFrustum( const Frustum* pVolumeA, const Frustum* pVolumeB );



//-----------------------------------------------------------------------------
// Test vs six planes (usually forming a frustum) intersection routines.
// The intended use for these routines is for fast culling to a view frustum.  
// When the volume being tested against a view frustum is small relative to the
// view frustum it is usually either inside all six planes of the frustum or 
// outside one of the planes of the frustum. If neither of these cases is true
// then it may or may not be intersecting the frustum. Outside a plane is 
// defined as being on the positive side of the plane (and inside negative).
// Return values: 0 = volume is outside one of the planes (no intersection),
//                1 = not completely inside or completely outside (intersecting),
//                2 = volume is inside all the planes (completely inside)
//-----------------------------------------------------------------------------
INT IntersectTriangle6Planes( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 );
INT IntersectSphere6Planes( const Sphere* pVolume, const XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 );
INT IntersectAxisAlignedBox6Planes( const AxisAlignedBox* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 );
INT IntersectOrientedBox6Planes( const OrientedBox* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 );
INT IntersectFrustum6Planes( const Frustum* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 );


//-----------------------------------------------------------------------------
// Volume vs plane intersection testing routines.
// Return values: 0 = volume is outside the plane (on the positive sideof the plane),
//                1 = volume intersects the plane,
//                2 = volume is inside the plane (on the negative side of the plane) 
//-----------------------------------------------------------------------------
INT IntersectTrianglePlane( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, XMVECTOR Plane );
INT IntersectSpherePlane( const Sphere* pVolume, const XMVECTOR Plane );
INT IntersectAxisAlignedBoxPlane( const AxisAlignedBox* pVolume, XMVECTOR Plane );
INT IntersectOrientedBoxPlane( const OrientedBox* pVolume, XMVECTOR Plane );
INT IntersectFrustumPlane( const Frustum* pVolume, XMVECTOR Plane );

} // namespace ATG

#endif