//-----------------------------------------------------------------------------
// Collision.cpp
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include <xtl.h>
#include <xboxmath.h>
#include "AtgCollision.h"

namespace ATG
{


static const XMVECTOR g_UnitQuaternionEpsilon = { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f };
static const XMVECTOR g_UnitVectorEpsilon = { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f };
static const XMVECTOR g_UnitPlaneEpsilon = { 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f };



//-----------------------------------------------------------------------------
// Return TRUE if any of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline BOOL XMVector3AnyTrue( XMVECTOR V )
{
    XMVECTOR C;
    
    // Duplicate the fourth element from the first element.
    C = XMVectorSwizzle( V, 0, 1, 2, 0 );
    
    return XMComparisonAnyTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}



//-----------------------------------------------------------------------------
// Return TRUE if all of the elements of a 3 vector are equal to 0xffffffff.
// Slightly more efficient than using XMVector3EqualInt.
//-----------------------------------------------------------------------------
static inline BOOL XMVector3AllTrue( XMVECTOR V )
{
    XMVECTOR C;
    
    // Duplicate the fourth element from the first element.
    C = XMVectorSwizzle( V, 0, 1, 2, 0 );
    
    return XMComparisonAllTrue( XMVector4EqualIntR( C, XMVectorTrueInt() ) );
}



//-----------------------------------------------------------------------------
// Return TRUE if the vector is a unit vector (length == 1).
//-----------------------------------------------------------------------------
static inline BOOL XMVector3IsUnit( XMVECTOR V )
{
    XMVECTOR Difference = XMVector3Length( V ) - XMVectorSplatOne();
    
    return XMVector4Less( XMVectorAbs( Difference ), g_UnitVectorEpsilon );
}



//-----------------------------------------------------------------------------
// Return TRUE if the quaterion is a unit quaternion.
//-----------------------------------------------------------------------------
static inline BOOL XMQuaternionIsUnit( XMVECTOR Q )
{
    XMVECTOR Difference = XMVector4Length( Q ) - XMVectorSplatOne();
    
    return XMVector4Less( XMVectorAbs( Difference ), g_UnitQuaternionEpsilon );
}



//-----------------------------------------------------------------------------
// Return TRUE if the plane is a unit plane.
//-----------------------------------------------------------------------------
static inline BOOL XMPlaneIsUnit( XMVECTOR Plane )
{
    XMVECTOR Difference = XMVector3Length( Plane ) - XMVectorSplatOne();
    
    return XMVector4Less( XMVectorAbs( Difference ), g_UnitPlaneEpsilon );
}



//-----------------------------------------------------------------------------
// Transform a plane by a rotation and translation.
//-----------------------------------------------------------------------------
static inline XMVECTOR TransformPlane( XMVECTOR Plane, XMVECTOR Rotation, XMVECTOR Translation )
{
    XMVECTOR Normal = XMVector3Rotate( Plane, Rotation );
    XMVECTOR D = XMVectorSplatW( Plane ) - XMVector3Dot( Normal, Translation );
    
    return XMVectorInsert( Normal, D, 0, 0, 0, 0, 1 );
}



//-----------------------------------------------------------------------------
// Return the point on the line segement (S1, S2) nearest the point P.
//-----------------------------------------------------------------------------
static inline XMVECTOR PointOnLineSegmentNearestPoint( XMVECTOR S1, XMVECTOR S2, XMVECTOR P )
{
    XMVECTOR Dir = S2 - S1;
    XMVECTOR Projection = ( XMVector3Dot( P, Dir ) - XMVector3Dot( S1, Dir ) );
    XMVECTOR LengthSq = XMVector3Dot( Dir, Dir );

    XMVECTOR t = Projection * XMVectorReciprocal( LengthSq );
    XMVECTOR Point = S1 + t * Dir;
    
    // t < 0
    XMVECTOR SelectS1 = XMVectorLess( Projection, XMVectorZero() );
    Point = XMVectorSelect( Point, S1, SelectS1 );

    // t > 1
    XMVECTOR SelectS2 = XMVectorGreater( Projection, LengthSq );
    Point = XMVectorSelect( Point, S2, SelectS2 );

    return Point;
}



//-----------------------------------------------------------------------------
// Test if the point (P) on the plane of the triangle is inside the triangle 
// (V0, V1, V2).
//-----------------------------------------------------------------------------
static inline XMVECTOR PointOnPlaneInsideTriangle( XMVECTOR P, XMVECTOR V0, XMVECTOR V1, XMVECTOR V2 )
{
    // Compute the triangle normal.
    XMVECTOR N = XMVector3Cross( V2 - V0, V1- V0 );
    
    // Compute the cross products of the vector from the base of each edge to 
    // the point with each edge vector.
    XMVECTOR C0 = XMVector3Cross( P - V0, V1 - V0 );
    XMVECTOR C1 = XMVector3Cross( P - V1, V2 - V1 );
    XMVECTOR C2 = XMVector3Cross( P - V2, V0 - V2 );

    // If the cross product points in the same direction as the normal the the
    // point is inside the edge (it is zero if is on the edge).
    XMVECTOR Zero = XMVectorZero();
    XMVECTOR Inside0 = XMVectorGreaterOrEqual( XMVector3Dot( C0, N ), Zero );
    XMVECTOR Inside1 = XMVectorGreaterOrEqual( XMVector3Dot( C1, N ), Zero );
    XMVECTOR Inside2 = XMVectorGreaterOrEqual( XMVector3Dot( C2, N ), Zero );
    
    // If the point inside all of the edges it is inside.
    return XMVectorAndInt( XMVectorAndInt( Inside0, Inside1 ), Inside2 );
}



//-----------------------------------------------------------------------------
// Find the approximate smallest enclosing bounding sphere for a set of 
// points. Exact computation of the smallest enclosing bounding sphere is 
// possible but is slower and requires a more complex algorithm.
// The algorithm is based on  Jack Ritter, "An Efficient Bounding Sphere", 
// Graphics Gems.
//-----------------------------------------------------------------------------
VOID ComputeBoundingSphereFromPoints( Sphere* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride )
{
    XMASSERT( pOut );
    XMASSERT( Count > 0 );
    XMASSERT( pPoints );

    // Find the points with minimum and maximum x, y, and z
    XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
    
    MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3(pPoints);
    
    for ( UINT i = 1; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( (XMFLOAT3*)((BYTE*)pPoints + i*Stride) );
        
        if ( Point.x < MinX.x )
            MinX = Point;

        if ( Point.x > MaxX.x )
            MaxX = Point;
        
        if ( Point.y < MinY.y )
            MinY = Point;

        if ( Point.y > MaxY.y )
            MaxY = Point;

        if ( Point.z < MinZ.z )
            MinZ = Point;

        if ( Point.z > MaxZ.z )
            MaxZ = Point;
    }
    
    // Use the min/max pair that are farthest apart to form the initial sphere.
    XMVECTOR DeltaX = MaxX - MinX;
    XMVECTOR DistX = XMVector3Length(DeltaX);

    XMVECTOR DeltaY = MaxY - MinY;
    XMVECTOR DistY = XMVector3Length(DeltaY);

    XMVECTOR DeltaZ = MaxZ - MinZ;
    XMVECTOR DistZ = XMVector3Length(DeltaZ);
    
    XMVECTOR Center;
    XMVECTOR Radius;

    if ( XMVector3Greater( DistX, DistY ) )
    {
        if ( XMVector3Greater( DistX, DistZ ) )
        {
            // Use min/max x.
            Center = (MaxX + MinX) * 0.5f;
            Radius = DistX * 0.5f;
        }
        else
        {
            // Use min/max z.
            Center = (MaxZ + MinZ) * 0.5f;
            Radius = DistZ * 0.5f;
        }
    }
    else // Y >= X
    {
        if ( XMVector3Greater( DistY, DistZ ) )
        {
            // Use min/max y.
            Center = (MaxY + MinY) * 0.5f;
            Radius = DistY * 0.5f;
        }
        else
        {
            // Use min/max z.
            Center = (MaxZ + MinZ) * 0.5f;
            Radius = DistZ * 0.5f;
        }
    }
    
    // Add any points not inside the sphere.
    for ( UINT i = 0; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( (XMFLOAT3*)((BYTE*)pPoints + i*Stride) );

        XMVECTOR Delta = Point - Center;

        XMVECTOR Dist = XMVector3Length(Delta);
        
        if ( XMVector3Greater(Dist, Radius) )
        {
            // Adjust sphere to include the new point.
            Radius = (Radius + Dist) * 0.5f;
            Center += (XMVectorReplicate(1.0f) - Radius * XMVectorReciprocal(Dist)) * Delta;
        }
    }
    
    XMStoreFloat3( &pOut->Center, Center );
    pOut->Radius = Radius.x;
    
    return;
}



//-----------------------------------------------------------------------------
// Find the minimum axis aligned bounding box containing a set of points.
//-----------------------------------------------------------------------------
VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride )
{
    XMASSERT( pOut );
    XMASSERT( Count > 0 );
    XMASSERT( pPoints );

    // Find the minimum and maximum x, y, and z
    XMVECTOR vMin, vMax;
    
    vMin = vMax = XMLoadFloat3(pPoints);
    
    for ( UINT i = 1; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( (XMFLOAT3*)((BYTE*)pPoints + i*Stride) );
 
        vMin = XMVectorMinimize( vMin, Point );
        vMax = XMVectorMaximize( vMax, Point );
    }

    // Store center and extents.
    XMStoreFloat3( &pOut->Center,  (vMin + vMax) * 0.5f  );
    XMStoreFloat3( &pOut->Extents, (vMax - vMin) * 0.5f );
    
    return;
}



//-----------------------------------------------------------------------------
static inline BOOL SolveCubic( FLOAT e, FLOAT f, FLOAT g, FLOAT *t, FLOAT *u, FLOAT *v )
{
    FLOAT p, q, h, rc, d, theta, costh3, sinth3;

    p = f - e * e / 3.0f;
    q = g - e * f / 3.0f + e * e * e * 2.0f / 27.0f;
    h = q * q / 4.0f + p * p * p / 27.0f;
    
    if ( h > 0.0 ) 
    {
        return FALSE; // only one real root
    }

    if ( (h == 0.0) && (q == 0.0) ) // all the same root
    {
        *t = - e / 3;
        *u = - e / 3;
        *v = - e / 3;
        
        return TRUE;
    }

    d = sqrtf(q * q / 4.0f - h);
    if ( d < 0 )
        rc = -powf(-d, 1.0f/3.0f);
    else
        rc = powf(d, 1.0f/3.0f);
    
    theta = acosf(-q / (2.0f * d));
    costh3 = cosf(theta / 3.0f);
    sinth3 = sqrtf(3.0f) * sinf(theta / 3.0f);
    *t = 2.0f * rc * costh3 - e / 3.0f;  
    *u = -rc * (costh3 + sinth3) - e / 3.0f;
    *v = -rc * (costh3 - sinth3) - e / 3.0f;
    
    return TRUE;
}



//-----------------------------------------------------------------------------
static inline XMVECTOR CalculateEigenVector( FLOAT m11, FLOAT m12, FLOAT m13, 
                                             FLOAT m22, FLOAT m23, FLOAT m33, FLOAT e )
{
    XMVECTOR vTmp;
    FLOAT f1, f2, f3;

    vTmp.x = (FLOAT)(m12 * m23 - m13 * (m22 - e));
    vTmp.y = (FLOAT)(m13 * m12 - m23 * (m11 - e));
    vTmp.z = (FLOAT)((m11 - e) * (m22 - e) - m12 * m12);

    if ( (vTmp.x == 0.0) && (vTmp.y == 0.0) && (vTmp.z == 0.0) ) // planar or linear
    {
        // we only have one equation - find a valid one
        if ( (m11 - e != 0.0) || (m12 != 0.0) || (m13 != 0.0) )
        {
            f1 = m11 - e; f2 = m12; f3 = m13;
        }
        else if ( (m12 != 0.0) || (m22 - e != 0.0) || (m23 != 0.0) )
        {
            f1 = m12; f2 = m22 - e; f3 = m23;   
        }
        else if ( (m13 != 0.0) || (m23 != 0.0) || (m33 - e != 0.0) )
        {
            f1 = m13; f2 = m23; f3 = m33 - e;
        }
        else
        {
            // error, we'll just make something up - we have NO context
            f1 = 1.0; f2 = 0.0; f3 = 0.0;
        }
    
        if ( f1 == 0.0 ) 
            vTmp.x = 0.0; 
        else
            vTmp.x = 1.0;

        if ( f2 == 0.0 )
            vTmp.y = 0.0;
        else
            vTmp.y = 1.0;

        if ( f3 == 0.0 )
        {   
            vTmp.z = 0.0;
            // recalculate y to make equation work
            if (m12 != 0.0)
                vTmp.y = (FLOAT)(-f1 / f2);
        }
        else
        {
            vTmp.z = (FLOAT)((f2 - f1) / f3);
        }
    }
    
    if ( XMVector3LengthSq( vTmp ).x > 1e-5f )
    {
        return XMVector3Normalize( vTmp );
    }
    else
    {
        // Multiply by a value large enough to make the vector non-zero.
        vTmp *= 1e5f;
        return XMVector3Normalize( vTmp );
    }
}



//-----------------------------------------------------------------------------
static inline BOOL CalculateEigenVectors( FLOAT m11, FLOAT m12, FLOAT m13, 
                                          FLOAT m22, FLOAT m23, FLOAT m33, 
                                          FLOAT e1, FLOAT e2, FLOAT e3, 
                                          XMVECTOR *pV1, XMVECTOR *pV2, XMVECTOR *pV3 )
{
    XMVECTOR vTmp, vUp, vRight;

    BOOL v1z, v2z, v3z, e12, e13, e23;

    vUp.x = vUp.z = 0.0f; vUp.y = 1.0f;
    vRight.x = 1.0f;  vRight.y = vRight.z = 0.0f;

    *pV1 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e1 );
    *pV2 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e2 );
    *pV3 = CalculateEigenVector( m11, m12, m13, m22, m23, m33, e3 );

    v1z = v2z = v3z = FALSE;

    if ( (pV1->x == 0.0) && (pV1->y == 0.0) && (pV1->z == 0.0) ) v1z = TRUE;
    if ( (pV2->x == 0.0) && (pV2->y == 0.0) && (pV2->z == 0.0) ) v2z = TRUE;
    if ( (pV3->x == 0.0) && (pV3->y == 0.0) && (pV3->z == 0.0 )) v3z = TRUE;

    e12 = (fabsf(XMVector3Dot( *pV1, *pV2 ).x) >  0.1f); // check for non-orthogonal vectors
    e13 = (fabsf(XMVector3Dot( *pV1, *pV3 ).x) >  0.1f);
    e23 = (fabsf(XMVector3Dot( *pV2, *pV3 ).x) >  0.1f);

    if ( (v1z && v2z && v3z) || (e12 && e13 && e23) ||
         (e12 && v3z) || (e13 && v2z) || (e23 && v1z) ) // all eigenvectors are 0- any basis set
    {
        pV1->x = pV2->y = pV3->z = 1.0f;
        pV1->y = pV1->z = pV2->x = pV2->z = pV3->x = pV3->y = 0.0f;
        return TRUE;
    }

    if ( v1z && v2z )
    {
        vTmp = XMVector3Cross( vUp, *pV3 );
        if ( XMVector3LengthSq( vTmp ).x < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV3 );
        }
        *pV1 = XMVector3Normalize( vTmp );
        *pV2 = XMVector3Cross( *pV3, *pV1 );
        return TRUE;
    }

    if ( v3z && v1z )
    {
        vTmp = XMVector3Cross( vUp, *pV2 );
        if ( XMVector3LengthSq( vTmp ).x < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV2 );
        }
        *pV3 = XMVector3Normalize( vTmp );
        *pV1 = XMVector3Cross( *pV2, *pV3 );
        return TRUE;
    }

    if ( v2z && v3z )
    {
        vTmp = XMVector3Cross( vUp, *pV1 );
        if ( XMVector3LengthSq( vTmp ).x < 1e-5f )
        {
            vTmp = XMVector3Cross( vRight, *pV1 );
        }
        *pV2 = XMVector3Normalize( vTmp );
        *pV3 = XMVector3Cross( *pV1, *pV2 );
        return TRUE;
    }

    if ( (v1z) || e12 )
    {   
        *pV1 = XMVector3Cross( *pV2, *pV3 );
        return TRUE;
    }

    if ( (v2z) || e23 )
    {   
        *pV2 = XMVector3Cross( *pV3, *pV1 );
        return TRUE;
    }

    if ( (v3z) || e13 )
    {   
        *pV3 = XMVector3Cross( *pV1, *pV2 );
        return TRUE;
    }
    
    return TRUE;
}



//-----------------------------------------------------------------------------
static inline BOOL CalculateEigenVectorsFromCovarianceMatrix( FLOAT Cxx, FLOAT Cyy, FLOAT Czz, 
                                                              FLOAT Cxy, FLOAT Cxz, FLOAT Cyz, 
                                                              XMVECTOR *pV1, XMVECTOR *pV2, XMVECTOR *pV3 )
{
    FLOAT e, f, g, ev1, ev2, ev3;

    // Calculate the eigenvalues by solving a cubic equation.
    e = -(Cxx + Cyy + Czz);
    f = Cxx * Cyy + Cyy * Czz + Czz * Cxx - Cxy * Cxy - Cxz * Cxz - Cyz * Cyz;
    g = Cxy * Cxy * Czz + Cxz * Cxz * Cyy + Cyz * Cyz * Cxx - Cxy * Cyz * Cxz * 2.0f - Cxx * Cyy * Czz;

    if ( !SolveCubic( e, f, g, &ev1, &ev2, &ev3 ) ) 
    {
        // set them to arbitrary orthonormal basis set
        pV1->x = pV2->y = pV3->z = 1.0f;
        pV1->y = pV1->z = pV2->x = pV2->z = pV3->x = pV3->y = 0.0f;
        return FALSE;
    }
    
    return CalculateEigenVectors( Cxx, Cxy, Cxz, Cyy, Cyz, Czz, ev1, ev2, ev3, pV1, pV2, pV3 );
}



//-----------------------------------------------------------------------------
// Find the approximate minimum oriented bounding box containing a set of 
// points.  Exact computation of minimum oriented bounding box is possible but 
// is slower and requires a more complex algorithm.
// The algorithm works by computing the inertia tensor of the points and then
// using the eigenvectors of the intertia tensor as the axes of the box.
// Computing the intertia tensor of the convex hull of the points will usually 
// result in better bounding box but the computation is more complex. 
// Exact computation of the minimum oriented bounding box is possible but the
// best know algorithm is O(N^3) and is significanly more complex to implement.
//-----------------------------------------------------------------------------
VOID ComputeBoundingOrientedBoxFromPoints( OrientedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride )
{
    static CONST XMVECTORI PermuteXXY = { XM_PERMUTE_0X, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W };
    static CONST XMVECTORI PermuteYZZ = { XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0Z, XM_PERMUTE_0W };

    XMASSERT( pOut );
    XMASSERT( Count > 0 );
    XMASSERT( pPoints );
    
    XMVECTOR CenterOfMass = XMVectorZero();
    
    // Compute the center of mass and inertia tensor of the points.
    for ( UINT i = 0; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3( (XMFLOAT3*)((BYTE*)pPoints + i*Stride) );
        
        CenterOfMass += Point;
    }
    
    CenterOfMass *= XMVectorReciprocal( XMVectorReplicate( FLOAT(Count) ) );

    // Compute the inertia tensor of the points around the center of mass.
    // Using the center of mass is not strictly necessary, but will hopefully
    // improve the stability of finding the eigenvectors.
    XMVECTOR XX_YY_ZZ = XMVectorZero();
    XMVECTOR XY_XZ_YZ = XMVectorZero();

    for ( UINT i = 0; i < Count; i++ )
    {
        XMVECTOR Point = XMLoadFloat3((XMFLOAT3*)((BYTE*)pPoints + i*Stride)) - CenterOfMass;
        
        XX_YY_ZZ += Point * Point;
        
        XMVECTOR XXY = XMVectorPermute( Point, Point, *(XMVECTOR*)PermuteXXY );
        XMVECTOR YZZ = XMVectorPermute( Point, Point, *(XMVECTOR*)PermuteYZZ );
        
        XY_XZ_YZ += XXY * YZZ;
    }
    
    XMVECTOR v1, v2, v3;
    
    // Compute the eigenvectors of the inertia tensor.
    CalculateEigenVectorsFromCovarianceMatrix( XX_YY_ZZ.x, XX_YY_ZZ.y, XX_YY_ZZ.z,
                                               XY_XZ_YZ.x, XY_XZ_YZ.y, XY_XZ_YZ.z,
                                               &v1, &v2, &v3 );
    
    // Put them in a matrix.
    XMMATRIX R;
    
    R.r[0] = v1;    R.r[0].w = 0.0f;
    R.r[1] = v2;    R.r[1].w = 0.0f;
    R.r[2] = v3;    R.r[2].w = 0.0f;
    R.r[3] = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );

    // Multiply by -1 to convert the matrix into a right handed coordinate 
    // system (Det ~= 1) in case the eigenvectors form a left handed 
    // coordinate system (Det ~= -1) because XMQuaternionRotationMatrix only 
    // works on right handed matrices.
    XMVECTOR Det = XMMatrixDeterminant( R );
  
    if ( XMVector4Less( Det, XMVectorZero() ) )
    {
        const XMVECTOR VectorNegativeOne = { -1.0f, -1.0f, -1.0f, -1.0f };
        
        R.r[0] *= VectorNegativeOne;
        R.r[1] *= VectorNegativeOne;
        R.r[2] *= VectorNegativeOne;
    }
    
    // Get the rotation quaternion from the matrix.
    XMVECTOR Orientation = XMQuaternionRotationMatrix( R );
    
    // Make sure it is normal (in case the vectors are slightly non-orthogonal).
    Orientation = XMQuaternionNormalize( Orientation );

    // Rebuild the rotation matrix from the quaternion.
    R = XMMatrixRotationQuaternion( Orientation );

    // Build the rotation into the rotated space.
    XMMATRIX InverseR = XMMatrixTranspose( R );
    
    // Find the minimum OBB using the eigenvectors as the axes.
    XMVECTOR vMin, vMax;
    
    vMin = vMax = XMVector3TransformNormal( XMLoadFloat3(pPoints), InverseR );
    
    for ( UINT i = 1; i < Count; i++ )
    {
        XMVECTOR Point = XMVector3TransformNormal( XMLoadFloat3((XMFLOAT3*)((BYTE*)pPoints + i*Stride)), InverseR );
        
        vMin = XMVectorMinimize( vMin, Point );
        vMax = XMVectorMaximize( vMax, Point );
    }
    
    // Rotate the center into world space.
    XMVECTOR Center = (vMin + vMax) * 0.5f;
    Center = XMVector3TransformNormal( Center, R );

    // Store center, extents, and orientation.
    XMStoreFloat3( &pOut->Center, Center );
    XMStoreFloat3( &pOut->Extents, (vMax - vMin) * 0.5f );
    XMStoreFloat4( &pOut->Orientation, Orientation );

    return;
}



//-----------------------------------------------------------------------------
// Build a frustum from a persepective projection matrix.  The matrix may only
// contain a projection; any rotation, translation or scale will cause the
// constructed frustum to be incorrect.
//-----------------------------------------------------------------------------
VOID ComputeFrustumFromProjection( Frustum* pOut, XMMATRIX* pProjection )
{
    XMASSERT( pOut );
    XMASSERT( pProjection );

    // Corners of the projection frustum in homogenous space.
    static XMVECTOR HomogenousPoints[6] =
    {
        {  1.0f,  0.0f, 1.0f, 1.0f },   // right (at far plane)
        { -1.0f,  0.0f, 1.0f, 1.0f },   // left
        {  0.0f,  1.0f, 1.0f, 1.0f },   // top
        {  0.0f, -1.0f, 1.0f, 1.0f },   // bottom
        
        { 0.0f, 0.0f, 0.0f, 1.0f },     // near
        { 0.0f, 0.0f, 1.0f, 1.0f }      // far
    };

    XMVECTOR Determinant;
    XMMATRIX matInverse = XMMatrixInverse( &Determinant, *pProjection );
    
    // Compute the frustum corners in world space.
    XMVECTOR Points[6];

    for ( INT i = 0; i < 6; i++ )
    {
        // Transform point.
        Points[i] = XMVector4Transform( HomogenousPoints[i], matInverse );
    }
    
    pOut->Origin = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    pOut->Orientation = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Compute the slopes.
    Points[0] = Points[0] * XMVectorReciprocal( XMVectorSplatZ( Points[0] ) );
    Points[1] = Points[1] * XMVectorReciprocal( XMVectorSplatZ( Points[1] ) );
    Points[2] = Points[2] * XMVectorReciprocal( XMVectorSplatZ( Points[2] ) );
    Points[3] = Points[3] * XMVectorReciprocal( XMVectorSplatZ( Points[3] ) );
    
    pOut->RightSlope  = Points[0].x;
    pOut->LeftSlope   = Points[1].x;
    pOut->TopSlope    = Points[2].y;
    pOut->BottomSlope = Points[3].y;

    // Compute near and far.
    Points[4] = Points[4] * XMVectorReciprocal( XMVectorSplatW( Points[4] ) );
    Points[5] = Points[5] * XMVectorReciprocal( XMVectorSplatW( Points[5] ) );
    
    pOut->Near = Points[4].z;
    pOut->Far  = Points[5].z;
    
    return;
}



//-----------------------------------------------------------------------------
// Build the 6 frustum planes from a frustum.
//-----------------------------------------------------------------------------
VOID ComputePlanesFromFrustum( const Frustum* pVolume, XMVECTOR* pPlane0, XMVECTOR* pPlane1, XMVECTOR* pPlane2, 
                                                       XMVECTOR* pPlane3, XMVECTOR* pPlane4, XMVECTOR* pPlane5 )
{
    XMASSERT( pVolume );
    XMASSERT( pPlane0 );
    XMASSERT( pPlane1 );
    XMASSERT( pPlane2 );
    XMASSERT( pPlane3 );
    XMASSERT( pPlane4 );
    XMASSERT( pPlane5 );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolume->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    // Build the frustum planes.
    XMVECTOR Plane0 = XMVectorSet(  0.0f,  0.0f, -1.0f,                  pVolume->Near );
    XMVECTOR Plane1 = XMVectorSet(  0.0f,  0.0f,  1.0f,                 -pVolume->Far );
    XMVECTOR Plane2 = XMVectorSet(  1.0f,  0.0f, -pVolume->RightSlope,   0.0f );
    XMVECTOR Plane3 = XMVectorSet( -1.0f,  0.0f,  pVolume->LeftSlope,    0.0f );
    XMVECTOR Plane4 = XMVectorSet(  0.0f,  1.0f, -pVolume->TopSlope,     0.0f );
    XMVECTOR Plane5 = XMVectorSet(  0.0f, -1.0f,  pVolume->BottomSlope,  0.0f );

    Plane0 = TransformPlane( Plane0, Orientation, Origin );
    Plane1 = TransformPlane( Plane1, Orientation, Origin );
    Plane2 = TransformPlane( Plane2, Orientation, Origin );
    Plane3 = TransformPlane( Plane3, Orientation, Origin );
    Plane4 = TransformPlane( Plane4, Orientation, Origin );
    Plane5 = TransformPlane( Plane5, Orientation, Origin );
    
    *pPlane0 = XMPlaneNormalize( Plane0 );
    *pPlane1 = XMPlaneNormalize( Plane1 );
    *pPlane2 = XMPlaneNormalize( Plane2 );
    *pPlane3 = XMPlaneNormalize( Plane3 );
    *pPlane4 = XMPlaneNormalize( Plane4 );
    *pPlane5 = XMPlaneNormalize( Plane5 );
}



//-----------------------------------------------------------------------------
// Transform a sphere by an angle preserving transform.
//-----------------------------------------------------------------------------
VOID TransformSphere( Sphere* pOut, const Sphere *pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation )
{
    XMASSERT( pOut );
    XMASSERT( pIn );
    XMASSERT( XMQuaternionIsUnit( Rotation ) );

    // Load the center of the sphere.
    XMVECTOR Center = XMLoadFloat3( &pIn->Center );
    
    // Transform the center of the sphere.
    Center = XMVector3Rotate( Center * XMVectorReplicate( Scale ), Rotation ) + Translation;
    
    // Store the center sphere.
    XMStoreFloat3( &pOut->Center, Center );
    
    // Scale the radius of the pshere.
    pOut->Radius = pIn->Radius * Scale;
    
    return;
}



//-----------------------------------------------------------------------------
// Transform an axis aligned box by an angle preserving transform.
//-----------------------------------------------------------------------------
VOID TransformAxisAlignedBox( AxisAlignedBox* pOut, const AxisAlignedBox* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation )
{
    XMASSERT( pOut );
    XMASSERT( pIn );
    XMASSERT( XMQuaternionIsUnit( Rotation ) );

    static XMVECTOR Offset[8] = { { -1.0f, -1.0f, -1.0f, 0.0f },
                                  { -1.0f, -1.0f,  1.0f, 0.0f },
                                  { -1.0f,  1.0f, -1.0f, 0.0f },
                                  { -1.0f,  1.0f,  1.0f, 0.0f },
                                  {  1.0f, -1.0f, -1.0f, 0.0f },
                                  {  1.0f, -1.0f,  1.0f, 0.0f },
                                  {  1.0f,  1.0f, -1.0f, 0.0f },
                                  {  1.0f,  1.0f,  1.0f, 0.0f } };

    // Load center and extents.
    XMVECTOR Center = XMLoadFloat3( &pIn->Center );
    XMVECTOR Extents = XMLoadFloat3( &pIn->Extents );

    XMVECTOR VectorScale = XMVectorReplicate( Scale );
    
    // Compute and transform the corners and find new min/max bounds.
    XMVECTOR Corner = Center + Extents * Offset[0];
    Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;
 
    XMVECTOR Min, Max;
    Min = Max = Corner;
    
    for ( INT i = 1; i < 8; i++ )
    {
        Corner = Center + Extents * Offset[i];
        Corner = XMVector3Rotate( Corner * VectorScale, Rotation ) + Translation;

        Min = XMVectorMinimize( Min, Corner );
        Max = XMVectorMaximize( Max, Corner );
    }

    // Store center and extents.
    XMStoreFloat3( &pOut->Center, (Min + Max) * 0.5f );
    XMStoreFloat3( &pOut->Extents, (Max - Min) * 0.5f );

    return;
}



//-----------------------------------------------------------------------------
// Transform an oriented box by an angle preserving transform.
//-----------------------------------------------------------------------------
VOID TransformOrientedBox( OrientedBox* pOut, const OrientedBox* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation )
{
    XMASSERT( pOut );
    XMASSERT( pIn );
    XMASSERT( XMQuaternionIsUnit( Rotation ) );

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pIn->Center );
    XMVECTOR Extents = XMLoadFloat3( &pIn->Extents );
    XMVECTOR Orientation = XMLoadFloat4( &pIn->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Composite the box rotation and the transform rotation.
    Orientation = XMQuaternionMultiply( Orientation, Rotation );

    // Transform the center.
    XMVECTOR VectorScale = XMVectorReplicate( Scale );
    Center = XMVector3Rotate( Center * VectorScale, Rotation ) + Translation;

    // Scale the box extents.
    Extents = Extents * VectorScale;

    // Store the box.
    XMStoreFloat3( &pOut->Center, Center );
    XMStoreFloat3( &pOut->Extents, Extents );
    XMStoreFloat4( &pOut->Orientation, Orientation );
    
    return;
}



//-----------------------------------------------------------------------------
// Transform a frustum by an angle preserving transform.
//-----------------------------------------------------------------------------
VOID TransformFrustum( Frustum* pOut, const Frustum* pIn, FLOAT Scale, XMVECTOR Rotation, XMVECTOR Translation )
{
    XMASSERT( pOut );
    XMASSERT( pIn );
    XMASSERT( XMQuaternionIsUnit( Rotation ) );
    
    // Load the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pIn->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pIn->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );
    
    // Composite the frustum rotation and the transform rotation.
    Orientation = XMQuaternionMultiply( Orientation, Rotation );

    // Transform the origin.
    Origin = XMVector3Rotate( Origin * XMVectorReplicate( Scale ), Rotation ) + Translation;
    
    // Store the frustum.
    XMStoreFloat3( &pOut->Origin, Origin );
    XMStoreFloat4( &pOut->Orientation, Orientation );

    // Scale the near and far distances (the slopes remain the same).
    pOut->Near = pIn->Near * Scale;
    pOut->Far  = pIn->Far  * Scale;
    
    // Copy the slopes.
    pOut->RightSlope  = pIn->RightSlope;
    pOut->LeftSlope   = pIn->LeftSlope;
    pOut->TopSlope    = pIn->TopSlope;
    pOut->BottomSlope = pIn->BottomSlope;

    return;
}



//-----------------------------------------------------------------------------
// Point in sphere test.
//-----------------------------------------------------------------------------
BOOL IntersectPointSphere( XMVECTOR Point, const Sphere* pVolume )
{
    XMASSERT( pVolume );

    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolume->Radius );

    XMVECTOR DistanceSquared = XMVector3LengthSq( Point - Center );
    XMVECTOR RadiusSquared = Radius * Radius;
    
    return XMVector4LessOrEqual( DistanceSquared, RadiusSquared );
}



//-----------------------------------------------------------------------------
// Point in axis aligned box test.
//-----------------------------------------------------------------------------
BOOL IntersectPointAxisAlignedBox( XMVECTOR Point, const AxisAlignedBox* pVolume )
{
    XMASSERT( pVolume );

    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );

    return XMVector3InBounds( Point - Center, Extents );
}



//-----------------------------------------------------------------------------
// Point in oriented box test.
//-----------------------------------------------------------------------------
BOOL IntersectPointOrientedBox( XMVECTOR Point, const OrientedBox* pVolume )
{
    XMASSERT( pVolume );

    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Transform the point to be local to the box.
    Point = XMVector3InverseRotate( Point - Center, Orientation );

    return XMVector3InBounds( Point, Extents );
}



//-----------------------------------------------------------------------------
// Point in frustum test.
//-----------------------------------------------------------------------------
BOOL IntersectPointFrustum( XMVECTOR Point, const Frustum* pVolume )
{
    static const XMVECTORI SelectW = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_0, XM_SELECT_1};
    static const XMVECTORI SelectZ = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};

    static const XMVECTOR BasePlanes[6] = 
    {
        {  0.0f,  0.0f, -1.0f, 0.0f },
        {  0.0f,  0.0f,  1.0f, 0.0f },
        {  1.0f,  0.0f,  0.0f, 0.0f },
        { -1.0f,  0.0f,  0.0f, 0.0f },
        {  0.0f,  1.0f,  0.0f, 0.0f },
        {  0.0f, -1.0f,  0.0f, 0.0f },
    };

    XMASSERT( pVolume );

    // Build frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSelect( BasePlanes[0], XMVectorSplatX(  XMLoadScalar( &pVolume->Near ) ),
                                *(XMVECTOR*)SelectW );
                                
    Planes[1] = XMVectorSelect( BasePlanes[1], XMVectorSplatX( -XMLoadScalar( &pVolume->Far ) ),
                                *(XMVECTOR*)SelectW );
                                
    Planes[2] = XMVectorSelect( BasePlanes[2], XMVectorSplatX( -XMLoadScalar( &pVolume->RightSlope ) ),
                                *(XMVECTOR*)SelectZ );
                                
    Planes[3] = XMVectorSelect( BasePlanes[3], XMVectorSplatX(  XMLoadScalar( &pVolume->LeftSlope ) ),
                                *(XMVECTOR*)SelectZ );
                                
    Planes[4] = XMVectorSelect( BasePlanes[4], XMVectorSplatX( -XMLoadScalar( &pVolume->TopSlope ) ),
                                *(XMVECTOR*)SelectZ );
                                
    Planes[5] = XMVectorSelect( BasePlanes[5], XMVectorSplatX(  XMLoadScalar( &pVolume->BottomSlope ) ),
                                *(XMVECTOR*)SelectZ );

    // Load origin and orientation.
    XMVECTOR Origin = XMLoadFloat3( &pVolume->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Transform point into local space of frustum.
    Point = XMVector3InverseRotate( Point - Origin, Orientation );
    
    // Set w to one.
    Point = XMVectorInsert( Point, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    XMVECTOR Zero = XMVectorZero();
    XMVECTOR Outside = Zero;
    
    // Test point against each plane of the frustum.
    for ( INT i = 0; i < 6; i++ )
    {
        XMVECTOR Dot = XMVector4Dot( Point, Planes[i] );
        Outside = XMVectorOrInt( Outside, XMVectorGreater( Dot, Zero ) );
    }
    
    return XMVector4NotEqualInt( Outside, XMVectorTrueInt() );
}



//-----------------------------------------------------------------------------
// Compute the intersecion of a ray (Origin, Direction) with a triangle 
// (V0, V1, V2).  Return TRUE if there is an interseciton and also set *pDist 
// to the distance along the ray to the intersection.
// 
// The algorithm is based on Moller, Tomas and Trumbore, "Fast, Minimum Storage 
// Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1, 
// pp 21-28, 1997.
//-----------------------------------------------------------------------------
BOOL IntersectRayTriangle( XMVECTOR Origin, XMVECTOR Direction, XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, FLOAT* pDist )
{
    XMASSERT( pDist );
    XMASSERT( XMVector3IsUnit( Direction ) );

    static const XMVECTOR Epsilon = { 1e-20f, 1e-20f, 1e-20f, 1e-20f };
    
    XMVECTOR Zero = XMVectorZero();

    XMVECTOR e1 = V1 - V0;
    XMVECTOR e2 = V2 - V0;

    // p = Direction ^ e2;
    XMVECTOR p = XMVector3Cross( Direction, e2 );

    // det = e1 * p;
    XMVECTOR det = XMVector3Dot( e1, p );
    
    XMVECTOR u, v, t;

    if ( XMVector3GreaterOrEqual( det, Epsilon ) )
    {
        // Determinate is positive (front side of the triangle).
        XMVECTOR s = Origin - V0;

        // u = s * p;
        u = XMVector3Dot( s, p );

        XMVECTOR NoIntersection = XMVectorLess( u, Zero );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u, det ) );

        // q = s ^ e1;
        XMVECTOR q = XMVector3Cross( s, e1 );

        // v = Direction * q;
        v = XMVector3Dot( Direction, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( v, Zero ) );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( u + v, det ) );

        // t = e2 * q;
        t = XMVector3Dot( e2, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( t, Zero ) );
            
        if( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
            return FALSE;
    }
    else if ( XMVector3LessOrEqual( det, -Epsilon ) )
    {
        // Determinate is negative (back side of the triangle).
        XMVECTOR s = Origin - V0;

        // u = s * p;
        u = XMVector3Dot( s, p );

        XMVECTOR NoIntersection = XMVectorGreater( u, Zero );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u, det ) );

        // q = s ^ e1;
        XMVECTOR q = XMVector3Cross( s, e1 );

        // v = Direction * q;
        v = XMVector3Dot( Direction, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( v, Zero ) );
        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( u + v, det ) );

        // t = e2 * q;
        t = XMVector3Dot( e2, q );

        NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( t, Zero ) );

        if ( XMVector4EqualInt( NoIntersection, XMVectorTrueInt() ) )
            return FALSE;
    }
    else
    {
        // Parallel ray.
        return FALSE;
    }

    XMVECTOR inv_det = XMVectorReciprocal( det );

    t *= inv_det;
    
    // u * inv_det and v * inv_det are the barycentric cooridinates of the intersection.

    *pDist = t.x;

    return TRUE;
}



//-----------------------------------------------------------------------------
// Compute the intersecion of a ray (Origin, Direction) with a sphere.
//-----------------------------------------------------------------------------
BOOL IntersectRaySphere( XMVECTOR Origin, XMVECTOR Direction, const Sphere* pVolume, FLOAT* pDist )
{
    XMASSERT( pVolume );
    XMASSERT( pDist );
    XMASSERT( XMVector3IsUnit( Direction ) );

    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolume->Radius );

    // l is the vector from the ray origin to the center of the sphere.
    XMVECTOR l = Center - Origin;
    
    // s is the projection of the l onto the ray direction.
    XMVECTOR s = XMVector3Dot( l, Direction );
    
    XMVECTOR l2 = XMVector3Dot( l, l );
    
    XMVECTOR r2 = Radius * Radius;
    
    // m2 is squared distance from the center of the sphere to the projection.
    XMVECTOR m2 = l2 - s * s;
    
    XMVECTOR NoIntersection;
    
    // If the ray origin is outside the sphere and the center of the sphere is 
    // behind the ray origin there is no intersection.
    NoIntersection = XMVectorAndInt( XMVectorLess( s, XMVectorZero() ), XMVectorGreater( l2, r2 ) );

    // If the squared distance from the center of the sphere to the projection
    // is greater than the radius squared the ray will miss the sphere.
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( m2, r2 ) );

    // The ray hits the sphere, compute the nearest intersection point.
    XMVECTOR q = XMVectorSqrt( r2 - m2 );
    XMVECTOR t1 = s - q;
    XMVECTOR t2 = s + q;
    
    XMVECTOR OriginInside = XMVectorLessOrEqual( l2, r2 );
    XMVECTOR t = XMVectorSelect( t1, t2, OriginInside );

    if ( XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() ) )
    {
        *pDist = t.x;
        return TRUE;
    }
    
    return FALSE;
}



//-----------------------------------------------------------------------------
// Compute the intersecion of a ray (Origin, Direction) with an axis aligned 
// box using the slabs method.
//-----------------------------------------------------------------------------
BOOL IntersectRayAxisAlignedBox( XMVECTOR Origin, XMVECTOR Direction, const AxisAlignedBox* pVolume, FLOAT* pDist )
{
    XMASSERT( pVolume );
    XMASSERT( pDist );
    XMASSERT( XMVector3IsUnit( Direction ) );

    static const XMVECTOR Epsilon = { 1e-20f, 1e-20f, 1e-20f, 1e-20f };
    static const XMVECTOR FltMin = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
    static const XMVECTOR FltMax = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );

    // Adjust ray origin to be relative to center of the box.
    Origin = Center - Origin;
    
    // Compute the dot product againt each axis of the box.
    // Since the axii are (1,0,0), (0,1,0), (0,0,1) no computation is necessary.
    XMVECTOR AxisDotOrigin = Origin;
    XMVECTOR AxisDotDirection = Direction;

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

    // Test against all three axii simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
    XMVECTOR t1 = (AxisDotOrigin - Extents) * InverseAxisDotDirection;
    XMVECTOR t2 = (AxisDotOrigin + Extents) * InverseAxisDotDirection;

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect( XMVectorMinimize(t1, t2), FltMin, IsParallel );
    XMVECTOR t_max = XMVectorSelect( XMVectorMaximize(t1, t2), FltMax, IsParallel );

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMaximize( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
    t_min = XMVectorMaximize( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
    t_max = XMVectorMinimize( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
    t_max = XMVectorMinimize( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)

    // if ( t_min > t_max ) return FALSE;
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

    // if ( t_max < 0.0f ) return FALSE;
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );

    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
    XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

    if ( !XMVector3AnyTrue( NoIntersection ) )
    {
        *pDist = t_min.x;
        return TRUE;
    }

    return FALSE;
}



//-----------------------------------------------------------------------------
// Compute the intersecion of a ray (Origin, Direction) with an oriented box
// using the slabs method.
//-----------------------------------------------------------------------------
BOOL IntersectRayOrientedBox( XMVECTOR Origin, XMVECTOR Direction, const OrientedBox* pVolume, FLOAT* pDist )
{
    XMASSERT( pVolume );
    XMASSERT( pDist );
    XMASSERT( XMVector3IsUnit( Direction ) );

    static const XMVECTOR Epsilon = { 1e-20f, 1e-20f, 1e-20f, 1e-20f };
    static const XMVECTOR FltMin = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
    static const XMVECTOR FltMax = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
    static const XMVECTORI SelectY = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
    static const XMVECTORI SelectZ = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Get the boxes normalized side directions.
    XMMATRIX R = XMMatrixRotationQuaternion( Orientation );
    
    // Adjust ray origin to be relative to center of the box.
    Origin = Center - Origin;
    
    // Compute the dot product againt each axis of the box.
    XMVECTOR AxisDotOrigin = XMVector3Dot( R.r[0], Origin );
    AxisDotOrigin = XMVectorSelect( AxisDotOrigin, XMVector3Dot( R.r[1], Origin ), *(XMVECTOR*)SelectY );
    AxisDotOrigin = XMVectorSelect( AxisDotOrigin, XMVector3Dot( R.r[2], Origin ), *(XMVECTOR*)SelectZ );
    
    XMVECTOR AxisDotDirection = XMVector3Dot( R.r[0], Direction );
    AxisDotDirection = XMVectorSelect( AxisDotDirection, XMVector3Dot( R.r[1], Direction ), *(XMVECTOR*)SelectY );
    AxisDotDirection = XMVectorSelect( AxisDotDirection, XMVector3Dot( R.r[2], Direction ), *(XMVECTOR*)SelectZ );

    // if (fabs(AxisDotDirection) <= Epsilon) the ray is nearly parallel to the slab.
    XMVECTOR IsParallel = XMVectorLessOrEqual( XMVectorAbs( AxisDotDirection ), Epsilon );

    // Test against all three axes simultaneously.
    XMVECTOR InverseAxisDotDirection = XMVectorReciprocal( AxisDotDirection );
    XMVECTOR t1 = (AxisDotOrigin - Extents) * InverseAxisDotDirection;
    XMVECTOR t2 = (AxisDotOrigin + Extents) * InverseAxisDotDirection;

    // Compute the max of min(t1,t2) and the min of max(t1,t2) ensuring we don't
    // use the results from any directions parallel to the slab.
    XMVECTOR t_min = XMVectorSelect( XMVectorMinimize(t1, t2), FltMin, IsParallel );
    XMVECTOR t_max = XMVectorSelect( XMVectorMaximize(t1, t2), FltMax, IsParallel );

    // t_min.x = maximum( t_min.x, t_min.y, t_min.z );
    // t_max.x = minimum( t_max.x, t_max.y, t_max.z );
    t_min = XMVectorMaximize( t_min, XMVectorSplatY( t_min ) );  // x = max(x,y)
    t_min = XMVectorMaximize( t_min, XMVectorSplatZ( t_min ) );  // x = max(max(x,y),z)
    t_max = XMVectorMinimize( t_max, XMVectorSplatY( t_max ) );  // x = min(x,y)
    t_max = XMVectorMinimize( t_max, XMVectorSplatZ( t_max ) );  // x = min(min(x,y),z)
    
    // if ( t_min > t_max ) return FALSE;
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorSplatX( t_min ), XMVectorSplatX( t_max ) );

    // if ( t_max < 0.0f ) return FALSE;
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( XMVectorSplatX( t_max ), XMVectorZero() ) );
    
    // if (IsParallel && (-Extents > AxisDotOrigin || Extents < AxisDotOrigin)) return FALSE;
    XMVECTOR ParallelOverlap = XMVectorInBounds( AxisDotOrigin, Extents );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorAndCInt( IsParallel, ParallelOverlap ) );

    if ( !XMVector3AnyTrue( NoIntersection ) )
    {
        *pDist = t_min.x;
        return TRUE;
    }
    
    return FALSE;
}



//-----------------------------------------------------------------------------
// Test if two triangles intersect.
//
// The final test of algorithm is based on Shen, Heng, and Tang, "A Fast 
// Triangle-Triangle Overlap Test Using Signed Distances", Journal of Graphics 
// Tools, vol. 8, no. 1, pp 17-23, 2003 and Guigue and Devillers, "Fast and 
// Robust Triangle-Triangle Overlap Test Using Orientation Predicates", Journal 
// of Graphics Tools, vol. 8, no. 1, pp 25-32, 2003.
//
// The final test could be considered an edge-edge separating plane test with
// the 9 possible cases narrowed down to the only two pairs of edges that can 
// actaully result in a seperation.
//-----------------------------------------------------------------------------
BOOL IntersectTriangleTriangle( XMVECTOR A0, XMVECTOR A1, XMVECTOR A2, XMVECTOR B0, XMVECTOR B1, XMVECTOR B2 )
{
    static const XMVECTOR Epsilon = { 1e-20f, 1e-20f, 1e-20f, 1e-20f };
    static const XMVECTORI SelectY = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
    static const XMVECTORI SelectZ = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};
    static const XMVECTORI Select0111 = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_1, XM_SELECT_1};
    static const XMVECTORI Select1011 = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_1};
    static const XMVECTORI Select1101 = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};
    
    XMVECTOR Zero = XMVectorZero();
    
    // Compute the normal of triangle A.
    XMVECTOR N1 = XMVector3Cross( A1 - A0, A2 - A0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N1, XMVectorZero() ) );
    
    // Test points of B against the plane of A.
    XMVECTOR BDist = XMVector3Dot( N1, B0 - A0 );
    BDist = XMVectorSelect( BDist, XMVector3Dot( N1, B1 - A0 ), *(XMVECTOR*)SelectY );
    BDist = XMVectorSelect( BDist, XMVector3Dot( N1, B2 - A0 ), *(XMVECTOR*)SelectZ );

    // Ensure robustness with co-planar triangles by zeroing small distances.
    static UINT BDistIsZeroCR;
    XMVECTOR BDistIsZero = XMVectorGreaterR( &BDistIsZeroCR, Epsilon, XMVectorAbs( BDist ) );
    BDist = XMVectorSelect( BDist, Zero, BDistIsZero );
    
    static UINT BDistIsLessCR;
    XMVECTOR BDistIsLess = XMVectorGreaterR( &BDistIsLessCR, Zero, BDist );

    static UINT BDistIsGreaterCR;
    XMVECTOR BDistIsGreater = XMVectorGreaterR( &BDistIsGreaterCR, BDist, Zero );
    
    // If all the points are on the same side we don't intersect.
    if ( XMComparisonAllTrue(BDistIsLessCR) || XMComparisonAllTrue(BDistIsGreaterCR) )
         return FALSE;
    
    // Compute the normal of triangle B.
    XMVECTOR N2 = XMVector3Cross( B1 - B0, B2 - B0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N2, Zero ) );

    // Test points of A against the plane of B.
    XMVECTOR ADist = XMVector3Dot( N2, A0 - B0 );
    ADist = XMVectorSelect( ADist, XMVector3Dot( N2, A1 - B0 ), *(XMVECTOR*)SelectY );
    ADist = XMVectorSelect( ADist, XMVector3Dot( N2, A2 - B0 ), *(XMVECTOR*)SelectZ );

    // Ensure robustness with co-planar triangles by zeroing small distances.
    static UINT ADistIsZeroCR;
    XMVECTOR ADistIsZero = XMVectorGreaterR( &ADistIsZeroCR, Epsilon, XMVectorAbs( BDist ) );
    ADist = XMVectorSelect( ADist, Zero, ADistIsZero );

    static UINT ADistIsLessCR;
    XMVECTOR ADistIsLess = XMVectorGreaterR( &ADistIsLessCR, Zero, ADist );

    static UINT ADistIsGreaterCR;
    XMVECTOR ADistIsGreater = XMVectorGreaterR( &ADistIsGreaterCR, ADist, Zero );
    
    // If all the points are on the same side we don't intersect.
    if ( XMComparisonAllTrue(ADistIsLessCR) || XMComparisonAllTrue(ADistIsGreaterCR) )
         return FALSE;
    
    // Special case for co-planar triangles.
    if ( XMComparisonAllTrue(ADistIsZeroCR) || XMComparisonAllTrue(BDistIsZeroCR) )
    {
        XMVECTOR Axis, Dist, MinDist;
        
        // Compute an axis perpindicular to the edge (points out).
        Axis = XMVector3Cross( N1, A1 - A0 );
        Dist = XMVector3Dot( Axis, A0 );
        
        // Test points of B against the axis.
        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (A1, A2)
        Axis = XMVector3Cross( N1, A2 - A1 );
        Dist = XMVector3Dot( Axis, A1 );

        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (A2, A0)
        Axis = XMVector3Cross( N1, A0 - A2 );
        Dist = XMVector3Dot( Axis, A2 );

        MinDist = XMVector3Dot( B0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( B2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (B0, B1)
        Axis = XMVector3Cross( N2, B1 - B0 );
        Dist = XMVector3Dot( Axis, B0 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (B1, B2)
        Axis = XMVector3Cross( N2, B2 - B1 );
        Dist = XMVector3Dot( Axis, B1 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        // Edge (B2,B0)
        Axis = XMVector3Cross( N2, B0 - B2 );
        Dist = XMVector3Dot( Axis, B2 );

        MinDist = XMVector3Dot( A0, Axis );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A1, Axis ) );
        MinDist = XMVectorMinimize( MinDist, XMVector3Dot( A2, Axis ) );
        if ( XMVector4GreaterOrEqual( MinDist, Dist ) )
            return FALSE;

        return TRUE;
    }
    
    //
    // Find the single vertex of A and B (ie the vertex on the opposite side
    // of the plane from the other two) and reorder the edges so we can compute 
    // the signed edge/edge distances.
    //
    // if ( (V0 >= 0 && V1 <  0 && V2 <  0) ||
    //      (V0 >  0 && V1 <= 0 && V2 <= 0) ||
    //      (V0 <= 0 && V1 >  0 && V2 >  0) ||
    //      (V0 <  0 && V1 >= 0 && V2 >= 0) ) then V0 is singular;
    //
    // If our singular vertex is not on the positive side of the plane we reverse
    // the triangle winding so that the overlap comparisons will compare the 
    // correct edges with the correct signs.
    //
    XMVECTOR ADistIsLessEqual = XMVectorOrInt( ADistIsLess, ADistIsZero );
    XMVECTOR ADistIsGreaterEqual = XMVectorOrInt( ADistIsGreater, ADistIsZero );
    
    XMVECTOR AA0, AA1, AA2;
    bool bPositiveA;

    if ( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess,         *(XMVECTOR*)Select0111 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsGreater,      ADistIsLessEqual,    *(XMVECTOR*)Select0111 ) ) )
    {
        // A0 is singular, crossing from positive to negative.
        AA0 = A0; AA1 = A1; AA2 = A2;
        bPositiveA = true;
    }   
    else 
    if ( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual,    ADistIsGreater,      *(XMVECTOR*)Select0111 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsLess,         ADistIsGreaterEqual, *(XMVECTOR*)Select0111 ) ) )
    {
        // A0 is singular, crossing from negative to positive.
        AA0 = A0; AA1 = A2; AA2 = A1;
        bPositiveA = false;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess,         *(XMVECTOR*)Select1011 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsGreater,      ADistIsLessEqual,    *(XMVECTOR*)Select1011 ) ) )
    {
        // A1 is singular, crossing from positive to negative.
        AA0 = A1; AA1 = A2; AA2 = A0;
        bPositiveA = true;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual,    ADistIsGreater,      *(XMVECTOR*)Select1011 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsLess,         ADistIsGreaterEqual, *(XMVECTOR*)Select1011 ) ) )
    {
        // A1 is singular, crossing from negative to positive.
        AA0 = A1; AA1 = A0; AA2 = A2;
        bPositiveA = false;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( ADistIsGreaterEqual, ADistIsLess,         *(XMVECTOR*)Select1101 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsGreater,      ADistIsLessEqual,    *(XMVECTOR*)Select1101 ) ) )
    {
        // A2 is singular, crossing from positive to negative.
        AA0 = A2; AA1 = A0; AA2 = A1;
        bPositiveA = true;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( ADistIsLessEqual,    ADistIsGreater,      *(XMVECTOR*)Select1101 ) ) ||
         XMVector3AllTrue( XMVectorSelect( ADistIsLess,         ADistIsGreaterEqual, *(XMVECTOR*)Select1101 ) ) )
    {
        // A2 is singular, crossing from negative to positive.
        AA0 = A2; AA1 = A1; AA2 = A0;
        bPositiveA = false;
    }
    else
    {
        XMASSERT( FALSE );
        return FALSE;
    }

    XMVECTOR BDistIsLessEqual = XMVectorOrInt( BDistIsLess, BDistIsZero );
    XMVECTOR BDistIsGreaterEqual = XMVectorOrInt( BDistIsGreater, BDistIsZero );

    XMVECTOR BB0, BB1, BB2;
    bool bPositiveB;
    
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess,         *(XMVECTOR*)Select0111 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsGreater,      BDistIsLessEqual,    *(XMVECTOR*)Select0111 ) ) )
    {
        // B0 is singular, crossing from positive to negative.
        BB0 = B0; BB1 = B1; BB2 = B2;
        bPositiveB = true;
    }   
    else 
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual,    BDistIsGreater,      *(XMVECTOR*)Select0111 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsLess,         BDistIsGreaterEqual, *(XMVECTOR*)Select0111 ) ) )
    {
        // B0 is singular, crossing from negative to positive.
        BB0 = B0; BB1 = B2; BB2 = B1;
        bPositiveB = false;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess,         *(XMVECTOR*)Select1011 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsGreater,      BDistIsLessEqual,    *(XMVECTOR*)Select1011 ) ) )
    {
        // B1 is singular, crossing from positive to negative.
        BB0 = B1; BB1 = B2; BB2 = B0;
        bPositiveB = true;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual,    BDistIsGreater,      *(XMVECTOR*)Select1011 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsLess,         BDistIsGreaterEqual, *(XMVECTOR*)Select1011 ) ) )
    {
        // B1 is singular, crossing from negative to positive.
        BB0 = B1; BB1 = B0; BB2 = B2;
        bPositiveB = false;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsGreaterEqual, BDistIsLess,         *(XMVECTOR*)Select1101 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsGreater,      BDistIsLessEqual,    *(XMVECTOR*)Select1101 ) ) )
    {
        // B2 is singular, crossing from positive to negative.
        BB0 = B2; BB1 = B0; BB2 = B1;
        bPositiveB = true;
    }
    else
    if ( XMVector3AllTrue( XMVectorSelect( BDistIsLessEqual,    BDistIsGreater,      *(XMVECTOR*)Select1101 ) ) ||
         XMVector3AllTrue( XMVectorSelect( BDistIsLess,         BDistIsGreaterEqual, *(XMVECTOR*)Select1101 ) ) )
    {
        // B2 is singular, crossing from negative to positive.
        BB0 = B2; BB1 = B1; BB2 = B0;
        bPositiveB = false;
    }
    else
    {
        XMASSERT( FALSE );
        return FALSE;
    }

    XMVECTOR Delta0, Delta1;
    
    // Reverse the direction of the test depending on whether the singular vertices are
    // the same sign or different signs.
    if ( bPositiveA ^ bPositiveB )
    {
        Delta0 = (BB0 - AA0);
        Delta1 = (AA0 - BB0);
    }
    else
    {
        Delta0 = (AA0 - BB0);
        Delta1 = (BB0 - AA0);
    }
       
    // Check if the triangles overlap on the line of intersection between the
    // planes of the two triangles by finding the signed line distances.
    XMVECTOR Dist0 = XMVector3Dot( Delta0, XMVector3Cross( (BB2 - BB0), (AA2 - AA0) ) );
    if ( XMVector4Greater( Dist0, Zero ) )
        return FALSE;
    
    XMVECTOR Dist1 = XMVector3Dot( Delta1, XMVector3Cross( (BB1 - BB0), (AA1 - AA0) ) );
    if ( XMVector4Greater( Dist1, Zero ) )
        return FALSE;
        
    return TRUE;
}



//-----------------------------------------------------------------------------
BOOL IntersectTriangleSphere( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const Sphere* pVolume )
{
    XMASSERT( pVolume );

    // Load the sphere.    
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolume->Radius );

    // Compute the plane of the triangle (has to be normalized).
    XMVECTOR N = XMVector3Normalize( XMVector3Cross( V1 - V0, V2 - V0 ) );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( N, XMVectorZero() ) );

    // Find the nearest feature on the triangle to the sphere.
    XMVECTOR Dist = XMVector3Dot( Center - V0, N );
    
    // If the center of the sphere is farther from the plane of the triangle than
    // the radius of the sphere, then there cannot be an intersection.
    XMVECTOR NoIntersection = XMVectorLess( Dist, -Radius );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Dist, Radius ) );

    // Project the center of the sphere onto the plane of the triangle.
    XMVECTOR Point = Center - (N * Dist);
    
    // Is it inside all the edges? If so we  intersect if the distance 
    // to the plane is less than the radius.
    XMVECTOR Intersection = PointOnPlaneInsideTriangle( Point, V0, V1, V2 );
    
    // Find the nearest point on each edge.
    XMVECTOR RadiusSq = Radius * Radius;

    // Edge 0,1
    Point = PointOnLineSegmentNearestPoint( V0, V1, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );
        
    // Edge 1,2
    Point = PointOnLineSegmentNearestPoint( V1, V2, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );

    // Edge 2,0
    Point = PointOnLineSegmentNearestPoint( V2, V0, Center );

    // If the distance to the center of the sphere to the point is less than 
    // the radius of the sphere then it must intersect.
    Intersection = XMVectorOrInt( Intersection, XMVectorLessOrEqual( XMVector3LengthSq( Center - Point ), RadiusSq ) );
    
    return XMVector4EqualInt( XMVectorAndCInt( Intersection, NoIntersection ), XMVectorTrueInt() );
}



//-----------------------------------------------------------------------------
BOOL IntersectTriangleAxisAlignedBox( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const AxisAlignedBox* pVolume )
{
    XMASSERT( pVolume );

    static CONST XMVECTORI Permute0W1Z0Y0X = { XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X };
    static CONST XMVECTORI Permute0Z0W1X0Y = { XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y };
    static CONST XMVECTORI Permute1Y0X0W0Z = { XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z };

    XMVECTOR Zero = XMVectorZero();

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );
    
    XMVECTOR BoxMin = Center - Extents;
    XMVECTOR BoxMax = Center + Extents;

    // Test the axes of the box (in effect test the AAB against the minimal AAB 
    // around the triangle).
    XMVECTOR TriMin = XMVectorMinimize( XMVectorMinimize( V0, V1 ), V2 );
    XMVECTOR TriMax = XMVectorMaximize( XMVectorMaximize( V0, V1 ), V2 );
    
    // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then disjoint
    XMVECTOR Disjoint = XMVectorOrInt( XMVectorGreater( TriMin, BoxMax ), XMVectorGreater( BoxMin, TriMax ) );
    if ( XMVector3AnyTrue( Disjoint ) )
        return FALSE;

    // Test the plane of the triangle.
    XMVECTOR Normal = XMVector3Cross( V1 - V0, V2 - V0 );
    XMVECTOR Dist = XMVector3Dot( Normal, V0 );

    // Assert that the triangle is not degenerate.
    XMASSERT( !XMVector3Equal( Normal, XMVectorZero() ) );

    // for each i in (x, y, z) if n(i) >= 0 then v_min(i)=b_min(i), v_max(i)=b_max(i)
    // else v_min(i)=b_max(i), v_max(i)=b_min(i)
    XMVECTOR NormalSelect = XMVectorGreater( Normal, Zero );
    XMVECTOR V_Min = XMVectorSelect( BoxMax, BoxMin, NormalSelect );
    XMVECTOR V_Max = XMVectorSelect( BoxMin, BoxMax, NormalSelect );
    
    // if n dot v_min + d > 0 || n dot v_max + d < 0 then disjoint
    XMVECTOR MinDist = XMVector3Dot( V_Min, Normal );
    XMVECTOR MaxDist = XMVector3Dot( V_Max, Normal );
    
    XMVECTOR NoIntersection = XMVectorGreater( MinDist, Dist );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( MaxDist, Dist ) );
    
    // Move the box center to zero to simplify the following tests.
    V0 = V0 - Center;
    V1 = V1 - Center;
    V2 = V2 - Center;
    
    // Test the edge/edge axes (3*3).
    XMVECTOR e0 = V1 - V0;
    XMVECTOR e1 = V2 - V1;
    XMVECTOR e2 = V0 - V2;
    
    // Make w zero.
    e0 = XMVectorInsert( e0, Zero, 0, 0, 0, 0, 1);
    e1 = XMVectorInsert( e1, Zero, 0, 0, 0, 0, 1);
    e2 = XMVectorInsert( e2, Zero, 0, 0, 0, 0, 1);
    
    XMVECTOR Axis;
    XMVECTOR p0, p1, p2;
    XMVECTOR Min, Max;
    XMVECTOR Radius;
    
    // Axis == (1,0,0) x e0 = (0, -e0.z, e0.y)
    Axis = XMVectorPermute( e0, -e0, *(XMVECTOR*)Permute0W1Z0Y0X );
    p0 = XMVector3Dot( V0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    p2 = XMVector3Dot( V2, Axis );
    Min = XMVectorMinimize( p0, p2 );
    Max = XMVectorMaximize( p0, p2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (1,0,0) x e1 = (0, -e1.z, e1.y)
    Axis = XMVectorPermute( e1, -e1, *(XMVECTOR*)Permute0W1Z0Y0X );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (1,0,0) x e2 = (0, -e2.z, e2.y)
    Axis = XMVectorPermute( e2, -e2, *(XMVECTOR*)Permute0W1Z0Y0X );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,1,0) x e0 = (e0.z, 0, -e0.x)
    Axis = XMVectorPermute( e0, -e0, *(XMVECTOR*)Permute0Z0W1X0Y );
    p0 = XMVector3Dot( V0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    p2 = XMVector3Dot( V2, Axis );
    Min = XMVectorMinimize( p0, p2 );
    Max = XMVectorMaximize( p0, p2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,1,0) x e1 = (e1.z, 0, -e1.x)
    Axis = XMVectorPermute( e1, -e1, *(XMVECTOR*)Permute0Z0W1X0Y );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e2 = (e2.z, 0, -e2.x)
    Axis = XMVectorPermute( e2, -e2, *(XMVECTOR*)Permute0Z0W1X0Y );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e0 = (-e0.y, e0.x, 0)
    Axis = XMVectorPermute( e0, -e0, *(XMVECTOR*)Permute1Y0X0W0Z );
    p0 = XMVector3Dot( V0, Axis );
    // p1 = XMVector3Dot( V1, Axis ); // p1 = p0;
    p2 = XMVector3Dot( V2, Axis );
    Min = XMVectorMinimize( p0, p2 );
    Max = XMVectorMaximize( p0, p2 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e1 = (-e1.y, e1.x, 0)
    Axis = XMVectorPermute( e1, -e1, *(XMVECTOR*)Permute1Y0X0W0Z );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p1;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    // Axis == (0,0,1) x e2 = (-e2.y, e2.x, 0)
    Axis = XMVectorPermute( e2, -e2, *(XMVECTOR*)Permute1Y0X0W0Z );
    p0 = XMVector3Dot( V0, Axis );
    p1 = XMVector3Dot( V1, Axis );
    // p2 = XMVector3Dot( V2, Axis ); // p2 = p0;
    Min = XMVectorMinimize( p0, p1 );
    Max = XMVectorMaximize( p0, p1 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Axis ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorGreater( Min, Radius ) );
    NoIntersection = XMVectorOrInt( NoIntersection, XMVectorLess( Max, -Radius ) );

    return XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() );
}



//-----------------------------------------------------------------------------
BOOL IntersectTriangleOrientedBox( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const OrientedBox* pVolume )
{
    XMASSERT( pVolume );

    // Load the box center & orientation.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Transform the triangle vertices into the space of the box.
    V0 = XMVector3InverseRotate( V0 - Center, Orientation );
    V1 = XMVector3InverseRotate( V1 - Center, Orientation );
    V2 = XMVector3InverseRotate( V2 - Center, Orientation );

    AxisAlignedBox Box;
    Box.Center = XMFLOAT3( 0.0f, 0.0f, 0.0f );
    Box.Extents = pVolume->Extents;

    // Use the triangle vs axis aligned box intersection routine.
    return IntersectTriangleAxisAlignedBox( V0, V1, V2, &Box );
}



//-----------------------------------------------------------------------------
BOOL IntersectSphereSphere( const Sphere* pVolumeA, const Sphere* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Load A.
    XMVECTOR CenterA = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR RadiusA = XMVectorReplicate( pVolumeA->Radius );

    // Load B.
    XMVECTOR CenterB = XMLoadFloat3( &pVolumeB->Center );
    XMVECTOR RadiusB = XMVectorReplicate( pVolumeB->Radius );

    // Distance squared between centers.    
    XMVECTOR Delta = CenterB - CenterA;
    XMVECTOR DistanceSquared = XMVector3LengthSq( Delta );

    // Sum of the radii sqaured.
    XMVECTOR RadiusSquared = RadiusA + RadiusB;
    RadiusSquared = RadiusSquared * RadiusSquared;
    
    return XMVector4LessOrEqual( DistanceSquared, RadiusSquared );
}



//-----------------------------------------------------------------------------
BOOL IntersectSphereAxisAlignedBox( const Sphere* pVolumeA, const AxisAlignedBox* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    XMVECTOR SphereCenter = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR SphereRadius = XMVectorReplicate( pVolumeA->Radius );

    XMVECTOR BoxCenter = XMLoadFloat3( &pVolumeB->Center );
    XMVECTOR BoxExtents = XMLoadFloat3( &pVolumeB->Extents );

    XMVECTOR BoxMin = BoxCenter - BoxExtents;
    XMVECTOR BoxMax = BoxCenter + BoxExtents;
    
    // Find the distance to the nearest point on the box.
    // for each i in (x, y, z)
    // if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
    // else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2
    
    XMVECTOR d = XMVectorZero();

    // Compute d for each dimension.
    XMVECTOR LessThanMin = XMVectorLess( SphereCenter, BoxMin );
    XMVECTOR GreaterThanMax = XMVectorGreater( SphereCenter, BoxMax );
    
    XMVECTOR MinDelta = SphereCenter - BoxMin;
    XMVECTOR MaxDelta = SphereCenter - BoxMax;
    
    // Choose value for each dimension based on the comparison.
    d = XMVectorSelect( d, MinDelta, LessThanMin );
    d = XMVectorSelect( d, MaxDelta, GreaterThanMax );
    
    // Use a dot-product to square them and sum them together.
    XMVECTOR d2 = XMVector3Dot( d, d );
    
    return XMVector4LessOrEqual( d2, XMVectorMultiply( SphereRadius, SphereRadius ) );
}



//-----------------------------------------------------------------------------
BOOL IntersectSphereOrientedBox( const Sphere* pVolumeA, const OrientedBox* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    XMVECTOR SphereCenter = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR SphereRadius = XMVectorReplicate( pVolumeA->Radius );

    XMVECTOR BoxCenter = XMLoadFloat3( &pVolumeB->Center );
    XMVECTOR BoxExtents = XMLoadFloat3( &pVolumeB->Extents );
    XMVECTOR BoxOrientation = XMLoadFloat4( &pVolumeB->Orientation );

    XMASSERT( XMQuaternionIsUnit( BoxOrientation ) );

    // Transform the center of the sphere to be local to the box.
    // BoxMin = -BoxExtents
    // BoxMax = +BoxExtents
    SphereCenter = XMVector3InverseRotate( SphereCenter - BoxCenter, BoxOrientation );
    
    // Find the distance to the nearest point on the box.
    // for each i in (x, y, z)
    // if (SphereCenter(i) < BoxMin(i)) d2 += (SphereCenter(i) - BoxMin(i)) ^ 2
    // else if (SphereCenter(i) > BoxMax(i)) d2 += (SphereCenter(i) - BoxMax(i)) ^ 2
    
    XMVECTOR d = XMVectorZero();

    // Compute d for each dimension.
    XMVECTOR LessThanMin = XMVectorLess( SphereCenter, -BoxExtents );
    XMVECTOR GreaterThanMax = XMVectorGreater( SphereCenter, BoxExtents );
    
    XMVECTOR MinDelta = SphereCenter + BoxExtents;
    XMVECTOR MaxDelta = SphereCenter - BoxExtents;
    
    // Choose value for each dimension based on the comparison.
    d = XMVectorSelect( d, MinDelta, LessThanMin );
    d = XMVectorSelect( d, MaxDelta, GreaterThanMax );
    
    // Use a dot-product to square them and sum them together.
    XMVECTOR d2 = XMVector3Dot( d, d );
    
    return XMVector4LessOrEqual( d2, XMVectorMultiply( SphereRadius, SphereRadius ) );
}



//-----------------------------------------------------------------------------
BOOL IntersectAxisAlignedBoxAxisAlignedBox( const AxisAlignedBox* pVolumeA, const AxisAlignedBox* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    XMVECTOR CenterA = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR ExtentsA = XMLoadFloat3( &pVolumeA->Extents );

    XMVECTOR CenterB = XMLoadFloat3( &pVolumeB->Center );
    XMVECTOR ExtentsB = XMLoadFloat3( &pVolumeB->Extents );
    
    XMVECTOR MinA = CenterA - ExtentsA;
    XMVECTOR MaxA = CenterA + ExtentsA;

    XMVECTOR MinB = CenterB - ExtentsB;
    XMVECTOR MaxB = CenterB + ExtentsB;
    
    // for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return FALSE
    XMVECTOR Disjoint = XMVectorOrInt( XMVectorGreater( MinA, MaxB ), XMVectorGreater( MinB, MaxA ) );

    return !XMVector3AnyTrue( Disjoint );
}



//-----------------------------------------------------------------------------
BOOL IntersectAxisAlignedBoxOrientedBox( const AxisAlignedBox* pVolumeA, const OrientedBox* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Make the axis aligned box oriented and do an OBB vs OBB test.
    OrientedBox BoxA;

    BoxA.Center = pVolumeA->Center;
    BoxA.Extents = pVolumeA->Extents;
    BoxA.Orientation.x = 0.0f;
    BoxA.Orientation.y = 0.0f;
    BoxA.Orientation.z = 0.0f;
    BoxA.Orientation.w = 1.0f;

    return IntersectOrientedBoxOrientedBox( &BoxA, pVolumeB );
}



//-----------------------------------------------------------------------------
// Fast oriented box / oriented box intersecion test using the separating axis 
// theorem.
//-----------------------------------------------------------------------------
BOOL IntersectOrientedBoxOrientedBox( const OrientedBox* pVolumeA, const OrientedBox* pVolumeB )
{
    static CONST XMVECTORI Permute0W1Z0Y0X = { XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_0Y, XM_PERMUTE_0X };
    static CONST XMVECTORI Permute0Z0W1X0Y = { XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1X, XM_PERMUTE_0Y };
    static CONST XMVECTORI Permute1Y0X0W0Z = { XM_PERMUTE_1Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z };
    static CONST XMVECTORI PermuteWZYX = { XM_PERMUTE_0W, XM_PERMUTE_0Z, XM_PERMUTE_0Y, XM_PERMUTE_0X };
    static CONST XMVECTORI PermuteZWXY = { XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_0X, XM_PERMUTE_0Y };
    static CONST XMVECTORI PermuteYXWZ = { XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0W, XM_PERMUTE_0Z };
    
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Build the 3x3 rotation matrix that defines the orientation of B relative to A.
    XMVECTOR A_quat = XMLoadFloat4( &pVolumeA->Orientation );
    XMVECTOR B_quat = XMLoadFloat4( &pVolumeB->Orientation );

    XMASSERT( XMQuaternionIsUnit( A_quat ) );
    XMASSERT( XMQuaternionIsUnit( B_quat ) );

    XMVECTOR Q = XMQuaternionMultiply( A_quat, XMQuaternionConjugate( B_quat ) );
    XMMATRIX R = XMMatrixRotationQuaternion( Q );
    
    // Compute the translation of B relative to A.
    XMVECTOR A_cent = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR B_cent = XMLoadFloat3( &pVolumeB->Center );
    XMVECTOR t = XMVector3InverseRotate( B_cent - A_cent, A_quat );

    //
    // h(A) = extents of A.
    // h(B) = extents of B.
    //
    // a(u) = axes of A = (1,0,0), (0,1,0), (0,0,1)
    // b(u) = axes of B relative to A = (r00,r10,r20), (r01,r11,r21), (r02,r12,r22)
    //  
    // For each possible separating axis l:
    //   d(A) = sum (for i = u,v,w) h(A)(i) * abs( a(i) dot l )
    //   d(B) = sum (for i = u,v,w) h(B)(i) * abs( b(i) dot l )
    //   if abs( t dot l ) > d(A) + d(B) then disjoint
    //
    
    // Load extents of A and B.
    XMVECTOR h_A = XMLoadFloat3( &pVolumeA->Extents );
    XMVECTOR h_B = XMLoadFloat3( &pVolumeB->Extents );
    
    // Rows. Note R[0,1,2]X.w = 0.
    XMVECTOR R0X = R.r[0];
    XMVECTOR R1X = R.r[1];
    XMVECTOR R2X = R.r[2];

    R = XMMatrixTranspose( R );

    // Columns. Note RX[0,1,2].w = 0.
    XMVECTOR RX0 = R.r[0];
    XMVECTOR RX1 = R.r[1];
    XMVECTOR RX2 = R.r[2];
    
    // Absolute value of rows.
    XMVECTOR AR0X = XMVectorAbs(R0X);
    XMVECTOR AR1X = XMVectorAbs(R1X);
    XMVECTOR AR2X = XMVectorAbs(R2X);
    
    // Absolute value of columns.
    XMVECTOR ARX0 = XMVectorAbs(RX0);
    XMVECTOR ARX1 = XMVectorAbs(RX1);
    XMVECTOR ARX2 = XMVectorAbs(RX2);
    
    // Test each of the 15 possible seperating axii.
    XMVECTOR d, d_A, d_B;
    
    // l = a(u) = (1, 0, 0)
    // t dot l = t.x
    // d(A) = h(A).x
    // d(B) = h(B) dot abs(r00, r01, r02)
    d = XMVectorSplatX( t );
    d_A = XMVectorSplatX( h_A );
    d_B = XMVector3Dot( h_B, AR0X );
    XMVECTOR NoIntersection = XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) );
    
    // l = a(v) = (0, 1, 0)
    // t dot l = t.y
    // d(A) = h(A).y
    // d(B) = h(B) dot abs(r10, r11, r12)
    d = XMVectorSplatY( t );
    d_A = XMVectorSplatY( h_A );
    d_B = XMVector3Dot( h_B, AR1X );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(w) = (0, 0, 1)
    // t dot l = t.z
    // d(A) = h(A).z
    // d(B) = h(B) dot abs(r20, r21, r22)
    d = XMVectorSplatZ( t );
    d_A = XMVectorSplatZ( h_A );
    d_B = XMVector3Dot( h_B, AR2X );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = b(u) = (r00, r10, r20)
    // d(A) = h(A) dot abs(r00, r10, r20)
    // d(B) = h(B).x
    d = XMVector3Dot( t, RX0 );
    d_A = XMVector3Dot( h_A, ARX0 );
    d_B = XMVectorSplatX( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = b(v) = (r01, r11, r21)
    // d(A) = h(A) dot abs(r01, r11, r21)
    // d(B) = h(B).y
    d = XMVector3Dot( t, RX1 );
    d_A = XMVector3Dot( h_A, ARX1 );
    d_B = XMVectorSplatY( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = b(w) = (r02, r12, r22)
    // d(A) = h(A) dot abs(r02, r12, r22)
    // d(B) = h(B).z
    d = XMVector3Dot( t, RX2 );
    d_A = XMVector3Dot( h_A, ARX2 );
    d_B = XMVectorSplatZ( h_B );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(u) x b(u) = (0, -r20, r10)
    // d(A) = h(A) dot abs(0, r20, r10)
    // d(B) = h(B) dot abs(0, r02, r01)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, *(XMVECTOR*)Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, *(XMVECTOR*)PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, *(XMVECTOR*)PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(u) x b(v) = (0, -r21, r11)
    // d(A) = h(A) dot abs(0, r21, r11)
    // d(B) = h(B) dot abs(r02, 0, r00)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, *(XMVECTOR*)Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, *(XMVECTOR*)PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, *(XMVECTOR*)PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(u) x b(w) = (0, -r22, r12)
    // d(A) = h(A) dot abs(0, r22, r12)
    // d(B) = h(B) dot abs(r01, r00, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, *(XMVECTOR*)Permute0W1Z0Y0X ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, *(XMVECTOR*)PermuteWZYX ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR0X, AR0X, *(XMVECTOR*)PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(v) x b(u) = (r20, 0, -r00)
    // d(A) = h(A) dot abs(r20, 0, r00)
    // d(B) = h(B) dot abs(0, r12, r11)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, *(XMVECTOR*)Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, *(XMVECTOR*)PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, *(XMVECTOR*)PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(v) x b(v) = (r21, 0, -r01)
    // d(A) = h(A) dot abs(r21, 0, r01)
    // d(B) = h(B) dot abs(r12, 0, r10)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, *(XMVECTOR*)Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, *(XMVECTOR*)PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, *(XMVECTOR*)PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(v) x b(w) = (r22, 0, -r02)
    // d(A) = h(A) dot abs(r22, 0, r02)
    // d(B) = h(B) dot abs(r11, r10, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, *(XMVECTOR*)Permute0Z0W1X0Y ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, *(XMVECTOR*)PermuteZWXY ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR1X, AR1X, *(XMVECTOR*)PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // l = a(w) x b(u) = (-r10, r00, 0)
    // d(A) = h(A) dot abs(r10, r00, 0)
    // d(B) = h(B) dot abs(0, r22, r21)
    d = XMVector3Dot( t, XMVectorPermute( RX0, -RX0, *(XMVECTOR*)Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX0, ARX0, *(XMVECTOR*)PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, *(XMVECTOR*)PermuteWZYX ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(w) x b(v) = (-r11, r01, 0)
    // d(A) = h(A) dot abs(r11, r01, 0)
    // d(B) = h(B) dot abs(r22, 0, r20)
    d = XMVector3Dot( t, XMVectorPermute( RX1, -RX1, *(XMVECTOR*)Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX1, ARX1, *(XMVECTOR*)PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, *(XMVECTOR*)PermuteZWXY ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );
    
    // l = a(w) x b(w) = (-r12, r02, 0)
    // d(A) = h(A) dot abs(r12, r02, 0)
    // d(B) = h(B) dot abs(r21, r20, 0)
    d = XMVector3Dot( t, XMVectorPermute( RX2, -RX2, *(XMVECTOR*)Permute1Y0X0W0Z ) );
    d_A = XMVector3Dot( h_A, XMVectorPermute( ARX2, ARX2, *(XMVECTOR*)PermuteYXWZ ) );
    d_B = XMVector3Dot( h_B, XMVectorPermute( AR2X, AR2X, *(XMVECTOR*)PermuteYXWZ ) );
    NoIntersection = XMVectorOrInt( NoIntersection, 
                                    XMVectorGreater( XMVectorAbs(d), XMVectorAdd( d_A, d_B ) ) );

    // No seperating axis found, boxes must intersect.
    return XMVector4NotEqualInt( NoIntersection, XMVectorTrueInt() );
}



//-----------------------------------------------------------------------------
// Exact triangle vs frustum test.
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = triangle is completely inside frustum
//-----------------------------------------------------------------------------
INT IntersectTriangleFrustum( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, const Frustum* pVolume )
{
    XMASSERT( pVolume );
    
    // Build the frustum planes (NOTE: D is negated from the usual).
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(  0.0f,  0.0f, -1.0f,                 -pVolume->Near );
    Planes[1] = XMVectorSet(  0.0f,  0.0f,  1.0f,                  pVolume->Far );
    Planes[2] = XMVectorSet(  1.0f,  0.0f, -pVolume->RightSlope,   0.0f );
    Planes[3] = XMVectorSet( -1.0f,  0.0f,  pVolume->LeftSlope,    0.0f );
    Planes[4] = XMVectorSet(  0.0f,  1.0f, -pVolume->TopSlope,     0.0f );
    Planes[5] = XMVectorSet(  0.0f, -1.0f,  pVolume->BottomSlope,  0.0f );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolume->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Transform triangle into the local space of frustum.
    V0 = XMVector3InverseRotate( V0 - Origin, Orientation );
    V1 = XMVector3InverseRotate( V1 - Origin, Orientation );
    V2 = XMVector3InverseRotate( V2 - Origin, Orientation );

    // Test each vertex of the triangle against the frustum planes.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();

    for ( INT i = 0; i < 6; i++ )
    {
        XMVECTOR Dist0 = XMVector3Dot( V0, Planes[i] );
        XMVECTOR Dist1 = XMVector3Dot( V1, Planes[i] );
        XMVECTOR Dist2 = XMVector3Dot( V2, Planes[i] );
        
        XMVECTOR MinDist = XMVectorMinimize( Dist0, Dist1 );
        MinDist = XMVectorMinimize( MinDist, Dist2 );
        XMVECTOR MaxDist = XMVectorMaximize( Dist0, Dist1 );
        MaxDist = XMVectorMaximize( MaxDist, Dist2 );
        
        XMVECTOR PlaneDist = XMVectorSplatW( Planes[i] );
        
        // Outside the plane?
        Outside = XMVectorOrInt( Outside, XMVectorGreater( MinDist, PlaneDist ) );

        // Fully inside the plane?
        InsideAll = XMVectorAndInt( InsideAll, XMVectorLessOrEqual( MaxDist, PlaneDist ) );
    }

    // If the triangle is outside any of the planes it is outside. 
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If the triangle is inside all planes it is fully inside.
    if ( XMVector4EqualInt( InsideAll, XMVectorTrueInt() ) )
        return 2;
    
    // Build the corners of the frustum.
    XMVECTOR RightTop = XMVectorSet( pVolume->RightSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottom = XMVectorSet( pVolume->RightSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTop = XMVectorSet( pVolume->LeftSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottom = XMVectorSet( pVolume->LeftSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR Near = XMVectorReplicate( pVolume->Near );
    XMVECTOR Far = XMVectorReplicate( pVolume->Far );
    
    XMVECTOR Corners[8];
    Corners[0] = RightTop * Near;
    Corners[1] = RightBottom * Near;
    Corners[2] = LeftTop * Near;
    Corners[3] = LeftBottom * Near;
    Corners[4] = RightTop * Far;
    Corners[5] = RightBottom * Far;
    Corners[6] = LeftTop * Far;
    Corners[7] = LeftBottom * Far;

    // Test the plane of the triangle.
    XMVECTOR Normal = XMVector3Cross( V1 - V0, V2 - V0 );
    XMVECTOR Dist = XMVector3Dot( Normal, V0 );

    XMVECTOR MinDist, MaxDist;
    MinDist = MaxDist = XMVector3Dot( Corners[0], Normal );
    for ( INT i = 1; i < 8; i++ )
    {
        XMVECTOR Temp = XMVector3Dot( Corners[i], Normal );
        MinDist = XMVectorMinimize( MinDist, Temp );
        MaxDist = XMVectorMaximize( MaxDist, Temp );
    }
     
    Outside = XMVectorOrInt( XMVectorGreater( MinDist, Dist ), XMVectorLess( MaxDist, Dist ) );   
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // Check the edge/edge axes (3*6).
    XMVECTOR TriangleEdgeAxis[3];
    TriangleEdgeAxis[0] = V1 - V0;
    TriangleEdgeAxis[1] = V2 - V1;
    TriangleEdgeAxis[2] = V0 - V2;

    XMVECTOR FrustumEdgeAxis[6];
    FrustumEdgeAxis[0] = RightTop;
    FrustumEdgeAxis[1] = RightBottom;
    FrustumEdgeAxis[2] = LeftTop;
    FrustumEdgeAxis[3] = LeftBottom;
    FrustumEdgeAxis[4] = RightTop - LeftTop;
    FrustumEdgeAxis[5] = LeftBottom - LeftTop;

    for ( INT i = 0; i < 3; i++ )
    {
        for ( INT j = 0; j < 6; j++ )
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross( TriangleEdgeAxis[i], FrustumEdgeAxis[j] );
            
            // Find the min/max of the projection of the triangle onto the axis.
            XMVECTOR MinA, MaxA;
            
            XMVECTOR Dist0 = XMVector3Dot( V0, Axis );
            XMVECTOR Dist1 = XMVector3Dot( V1, Axis );
            XMVECTOR Dist2 = XMVector3Dot( V2, Axis );
            
            MinA = XMVectorMinimize( Dist0, Dist1 );
            MinA = XMVectorMinimize( MinA, Dist2 );
            MaxA = XMVectorMaximize( Dist0, Dist1 );
            MaxA = XMVectorMaximize( MaxA, Dist2 );
            
            // Find the min/max of the projection of the frustum onto the axis.
            XMVECTOR MinB, MaxB;

            MinB = MaxB = XMVector3Dot( Axis, Corners[0] );
          
            for ( INT k = 1; k < 8; k++ )
            {
                XMVECTOR Temp = XMVector3Dot( Axis, Corners[k] );
                MinB = XMVectorMinimize( MinB, Temp );
                MaxB = XMVectorMaximize( MaxB, Temp );
            }
            
            // if (MinA > MaxB || MinB > MaxA) reject;
            Outside = XMVectorOrInt( Outside, XMVectorGreater( MinA, MaxB ) );
            Outside = XMVectorOrInt( Outside, XMVectorGreater( MinB, MaxA ) );
        }
    }

    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If we did not find a separating plane then the triangle must intersect the frustum.
    return 1;
}



//-----------------------------------------------------------------------------
// Exact sphere vs frustum test.  The algorithm first checks the sphere against
// the planes of the frustum, then if the plane checks were indeterminate finds
// the nearest feature (plane, line, point) on the frustum to the center of the
// sphere and compares the distance to the nearest feature to the radius of the 
// sphere (it is so cool that all the comment lines above are the same length).
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = sphere is completely inside frustum
//-----------------------------------------------------------------------------
INT IntersectSphereFrustum( const Sphere* pVolumeA, const Frustum* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );
    
    XMVECTOR Zero = XMVectorZero();

    // Build the frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(  0.0f,  0.0f, -1.0f,                   pVolumeB->Near );
    Planes[1] = XMVectorSet(  0.0f,  0.0f,  1.0f,                  -pVolumeB->Far );
    Planes[2] = XMVectorSet(  1.0f,  0.0f, -pVolumeB->RightSlope,   0.0f );
    Planes[3] = XMVectorSet( -1.0f,  0.0f,  pVolumeB->LeftSlope,    0.0f );
    Planes[4] = XMVectorSet(  0.0f,  1.0f, -pVolumeB->TopSlope,     0.0f );
    Planes[5] = XMVectorSet(  0.0f, -1.0f,  pVolumeB->BottomSlope,  0.0f );

    // Normalize the planes so we can compare to the sphere radius.
    Planes[2] = XMVector3Normalize( Planes[2] );
    Planes[3] = XMVector3Normalize( Planes[3] );
    Planes[4] = XMVector3Normalize( Planes[4] );
    Planes[5] = XMVector3Normalize( Planes[5] );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolumeB->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolumeB->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );
    
    // Load the sphere.
    XMVECTOR Center = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolumeA->Radius );

    // Transform the center of the sphere into the local space of frustum.
    Center = XMVector3InverseRotate( Center - Origin, Orientation );
    
    // Set w of the center to one so we can dot4 with the plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1);
   
    // Check against each plane of the frustum.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();
    XMVECTOR CenterInsideAll = XMVectorTrueInt();
    
    XMVECTOR Dist[6];
    
    for ( INT i = 0; i < 6; i++ )
    {
        Dist[i] = XMVector4Dot( Center, Planes[i] );
        
        // Outside the plane?
        Outside = XMVectorOrInt( Outside, XMVectorGreater( Dist[i], Radius ) );
        
        // Fully inside the plane?
        InsideAll = XMVectorAndInt( InsideAll, XMVectorLessOrEqual( Dist[i], -Radius ) );

        // Check if the center is inside the plane.
        CenterInsideAll = XMVectorAndInt( CenterInsideAll, XMVectorLessOrEqual( Dist[i], Zero ) );
    }
    
    // If the sphere is outside any of the planes it is outside. 
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If the sphere is inside all planes it is fully inside.
    if ( XMVector4EqualInt( InsideAll, XMVectorTrueInt() ) )
        return 2;
    
    // If the center of the sphere is inside all planes and the sphere intersects 
    // one or more planes then it must intersect.
    if ( XMVector4EqualInt( CenterInsideAll, XMVectorTrueInt() ) )
        return 1;
    
    // The sphere may be outside the frustum or intersecting the frustum.
    // Find the nearest feature (face, edge, or corner) on the frustum 
    // to the sphere.
    
    // The faces adjacent to each face are:
    INT adjacent_faces[6][4] = { { 2, 3, 4, 5 },    // 0
                                 { 2, 3, 4, 5 },    // 1
                                 { 0, 1, 4, 5 },    // 2
                                 { 0, 1, 4, 5 },    // 3
                                 { 0, 1, 2, 3 },    // 4
                                 { 0, 1, 2, 3 } };  // 5

    XMVECTOR Intersects = XMVectorFalseInt();

    // Check to see if the nearest feature is one of the planes.
    for ( INT i = 0; i < 6; i++ )
    {
        // Find the nearest point on the plane to the center of the sphere.
        XMVECTOR Point = Center - (Planes[i] * Dist[i]);

        // Set w of the point to one.
        Point = XMVectorInsert( Point, XMVectorSplatOne(), 0, 0, 0, 0, 1 );
        
        // If the point is inside the face (inside the adjacent planes) then
        // this plane is the nearest feature.
        XMVECTOR InsideFace = XMVectorTrueInt();
        
        for ( INT j = 0; j < 4; j++ )
        {
            INT plane_index = adjacent_faces[i][j];
            
            InsideFace = XMVectorAndInt( InsideFace,
                           XMVectorLessOrEqual( XMVector4Dot( Point, Planes[plane_index] ), Zero ) );
        }
        
        // Since we have already checked distance from the plane we know that the
        // sphere must intersect if this plane is the nearest feature.
        Intersects = XMVectorOrInt( Intersects, 
                                    XMVectorAndInt( XMVectorGreater( Dist[i], Zero ), InsideFace ) );
    }

    if ( XMVector4EqualInt( Intersects, XMVectorTrueInt() ) )
        return 1;

    // Build the corners of the frustum.
    XMVECTOR RightTop = XMVectorSet( pVolumeB->RightSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottom = XMVectorSet( pVolumeB->RightSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTop = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottom = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR Near = XMVectorReplicate( pVolumeB->Near );
    XMVECTOR Far = XMVectorReplicate( pVolumeB->Far );
    
    XMVECTOR Corners[8];
    Corners[0] = RightTop * Near;
    Corners[1] = RightBottom * Near;
    Corners[2] = LeftTop * Near;
    Corners[3] = LeftBottom * Near;
    Corners[4] = RightTop * Far;
    Corners[5] = RightBottom * Far;
    Corners[6] = LeftTop * Far;
    Corners[7] = LeftBottom * Far;
    
    // The Edges are:
    INT edges[12][2] = { { 0, 1 }, { 2, 3 }, { 0, 2 }, { 1, 3 },    // Near plane
                         { 4, 5 }, { 6, 7 }, { 4, 6 }, { 5, 7 },    // Far plane
                         { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }, }; // Near to far

    XMVECTOR RadiusSq = Radius * Radius;
    
    // Check to see if the nearest feature is one of the edges (or corners).
    for ( INT i = 0; i < 12; i++ )
    {
        INT ei0 = edges[i][0];
        INT ei1 = edges[i][1]; 
        
        // Find the nearest point on the edge to the center of the sphere.
        // The corners of the frustum are included as the endpoints of the edges.
        XMVECTOR Point = PointOnLineSegmentNearestPoint( Corners[ei0], Corners[ei1], Center );
        
        XMVECTOR Delta = Center - Point;
        
        XMVECTOR DistSq = XMVector3Dot( Delta, Delta );
        
        // If the distance to the center of the sphere to the point is less than 
        // the radius of the sphere then it must intersect.
        Intersects = XMVectorOrInt( Intersects, XMVectorLessOrEqual( DistSq, RadiusSq ) );
    }

    if ( XMVector4EqualInt( Intersects, XMVectorTrueInt() ) )
        return 1;

    // The sphere must be outside the frustum.
    return 0;
}



//-----------------------------------------------------------------------------
// Exact axis alinged box vs frustum test.  Constructs an oriented box and uses
// the oriented box vs frustum test.
//
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = box is completely inside frustum
//-----------------------------------------------------------------------------
INT IntersectAxisAlignedBoxFrustum( const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Make the axis aligned box oriented and do an OBB vs frustum test.
    OrientedBox BoxA;

    BoxA.Center = pVolumeA->Center;
    BoxA.Extents = pVolumeA->Extents;
    BoxA.Orientation.x = 0.0f;
    BoxA.Orientation.y = 0.0f;
    BoxA.Orientation.z = 0.0f;
    BoxA.Orientation.w = 1.0f;
    
    return IntersectOrientedBoxFrustum( &BoxA, pVolumeB );
}



//-----------------------------------------------------------------------------
// Exact oriented box vs frustum test.
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = box is completely inside frustum
//-----------------------------------------------------------------------------
INT IntersectOrientedBoxFrustum( const OrientedBox* pVolumeA, const Frustum* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    static const XMVECTORI SelectY = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
    static const XMVECTORI SelectZ = {XM_SELECT_0, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};
    
    XMVECTOR Zero = XMVectorZero();

    // Build the frustum planes.
    XMVECTOR Planes[6];
    Planes[0] = XMVectorSet(  0.0f,  0.0f, -1.0f,                   pVolumeB->Near );
    Planes[1] = XMVectorSet(  0.0f,  0.0f,  1.0f,                  -pVolumeB->Far );
    Planes[2] = XMVectorSet(  1.0f,  0.0f, -pVolumeB->RightSlope,   0.0f );
    Planes[3] = XMVectorSet( -1.0f,  0.0f,  pVolumeB->LeftSlope,    0.0f );
    Planes[4] = XMVectorSet(  0.0f,  1.0f, -pVolumeB->TopSlope,     0.0f );
    Planes[5] = XMVectorSet(  0.0f, -1.0f,  pVolumeB->BottomSlope,  0.0f );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolumeB->Origin );
    XMVECTOR FrustumOrientation = XMLoadFloat4( &pVolumeB->Orientation );

    XMASSERT( XMQuaternionIsUnit( FrustumOrientation ) );
    
    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolumeA->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolumeA->Extents );
    XMVECTOR BoxOrientation = XMLoadFloat4( &pVolumeA->Orientation );

    XMASSERT( XMQuaternionIsUnit( BoxOrientation ) );

    // Transform the oriented box into the space of the frustum in order to 
    // minimize the number of transforms we have to do.
    Center = XMVector3InverseRotate( Center - Origin, FrustumOrientation );
    BoxOrientation = XMQuaternionMultiply( BoxOrientation, XMQuaternionConjugate( FrustumOrientation ) );

    // Set w of the center to one so we can dot4 with the plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1);
    
    // Build the 3x3 rotation matrix that defines the box axes.
    XMMATRIX R = XMMatrixRotationQuaternion( BoxOrientation );

    // Check against each plane of the frustum.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();
    XMVECTOR CenterInsideAll = XMVectorTrueInt();
    
    for ( INT i = 0; i < 6; i++ )
    {
        // Compute the distance to the center of the box.
        XMVECTOR Dist = XMVector4Dot( Center, Planes[i] );
        
        // Project the axes of the box onto the normal of the plane.  Half the
        // length of the projection (sometime called the "radius") is equal to
        // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
        // where h(i) are extents of the box, n is the plane normal, and b(i) are the 
        // axes of the box.
        XMVECTOR Radius = XMVector3Dot( Planes[i], R.r[0] );
        Radius = XMVectorSelect( Radius, XMVector3Dot( Planes[i], R.r[1] ), *(XMVECTOR*)SelectY );
        Radius = XMVectorSelect( Radius, XMVector3Dot( Planes[i], R.r[2] ), *(XMVECTOR*)SelectZ );
        Radius = XMVector3Dot( Extents, XMVectorAbs( Radius ) );
        
        // Outside the plane?
        Outside = XMVectorOrInt( Outside, XMVectorGreater( Dist, Radius ) );
        
        // Fully inside the plane?
        InsideAll = XMVectorAndInt( InsideAll, XMVectorLessOrEqual( Dist, -Radius ) );

        // Check if the center is inside the plane.
        CenterInsideAll = XMVectorAndInt( CenterInsideAll, XMVectorLessOrEqual( Dist, Zero ) );
    }
    
    // If the box is outside any of the planes it is outside. 
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If the box is inside all planes it is fully inside.
    if ( XMVector4EqualInt( InsideAll, XMVectorTrueInt() ) )
        return 2;
    
    // If the center of the box is inside all planes and the box intersects 
    // one or more planes then it must intersect.
    if ( XMVector4EqualInt( CenterInsideAll, XMVectorTrueInt() ) )
        return 1;

    // Build the corners of the frustum.
    XMVECTOR RightTop = XMVectorSet( pVolumeB->RightSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottom = XMVectorSet( pVolumeB->RightSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTop = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottom = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR Near = XMVectorReplicate( pVolumeB->Near );
    XMVECTOR Far = XMVectorReplicate( pVolumeB->Far );
    
    XMVECTOR Corners[8];
    Corners[0] = RightTop * Near;
    Corners[1] = RightBottom * Near;
    Corners[2] = LeftTop * Near;
    Corners[3] = LeftBottom * Near;
    Corners[4] = RightTop * Far;
    Corners[5] = RightBottom * Far;
    Corners[6] = LeftTop * Far;
    Corners[7] = LeftBottom * Far;

    // Test against box axes (3)
    {
        // Find the min/max values of the projection of the frustum onto each axis.
        XMVECTOR FrustumMin, FrustumMax;
        
        FrustumMin = XMVector3Dot( Corners[0], R.r[0] );
        FrustumMin = XMVectorSelect( FrustumMin, XMVector3Dot( Corners[0], R.r[1] ), *(XMVECTOR*)SelectY );
        FrustumMin = XMVectorSelect( FrustumMin, XMVector3Dot( Corners[0], R.r[2] ), *(XMVECTOR*)SelectZ );
        FrustumMax = FrustumMin;
        
        for ( INT i = 1; i < 8; i++ )
        {
            XMVECTOR Temp = XMVector3Dot( Corners[i], R.r[0] );
            Temp = XMVectorSelect( Temp, XMVector3Dot( Corners[i], R.r[1] ), *(XMVECTOR*)SelectY );
            Temp = XMVectorSelect( Temp, XMVector3Dot( Corners[i], R.r[2] ), *(XMVECTOR*)SelectZ );
            
            FrustumMin = XMVectorMinimize( FrustumMin, Temp );
            FrustumMax = XMVectorMaximize( FrustumMax, Temp );
        }
        
        // Project the center of the box onto the axes.
        XMVECTOR BoxDist = XMVector3Dot( Center, R.r[0] );
        BoxDist = XMVectorSelect( BoxDist, XMVector3Dot( Center, R.r[1] ), *(XMVECTOR*)SelectY );
        BoxDist = XMVectorSelect( BoxDist, XMVector3Dot( Center, R.r[2] ), *(XMVECTOR*)SelectZ );
        
        // The projection of the box onto the axis is just its Center and Extents.
        // if (min > box_max || max < box_min) reject;
        XMVECTOR Outside = XMVectorOrInt( XMVectorGreater( FrustumMin, BoxDist + Extents ), 
                                          XMVectorLess(    FrustumMax, BoxDist - Extents ) );
        
        if ( XMVector3AnyTrue( Outside ) )
            return 0;
    }
        
    // Test against edge/edge axes (3*6).
    XMVECTOR FrustumEdgeAxis[6];
    
    FrustumEdgeAxis[0] = RightTop;
    FrustumEdgeAxis[1] = RightBottom;
    FrustumEdgeAxis[2] = LeftTop;
    FrustumEdgeAxis[3] = LeftBottom;
    FrustumEdgeAxis[4] = RightTop - LeftTop;
    FrustumEdgeAxis[5] = LeftBottom - LeftTop;
    
    for ( INT i = 0; i < 3; i++ )
    {
        for ( INT j = 0; j < 6; j++ )
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross( R.r[i], FrustumEdgeAxis[j] );
            
            // Find the min/max values of the projection of the frustum onto the axis.
            XMVECTOR FrustumMin, FrustumMax;

            FrustumMin = FrustumMax = XMVector3Dot( Axis, Corners[0] );
            
            for ( INT k = 1; k < 8; k++ )
            {
                XMVECTOR Temp = XMVector3Dot( Axis, Corners[k] );
                FrustumMin = XMVectorMinimize( FrustumMin, Temp );
                FrustumMax = XMVectorMaximize( FrustumMax, Temp );
            }
            
            // Project the center of the box onto the axis.
            XMVECTOR Dist = XMVector3Dot( Center, Axis );
            
            // Project the axes of the box onto the axis to find the "radius" of the box.
            XMVECTOR Radius = XMVector3Dot( Axis, R.r[0] );
            Radius = XMVectorSelect( Radius, XMVector3Dot( Axis, R.r[1] ), *(XMVECTOR*)SelectY );
            Radius = XMVectorSelect( Radius, XMVector3Dot( Axis, R.r[2] ), *(XMVECTOR*)SelectZ );
            Radius = XMVector3Dot( Extents, XMVectorAbs( Radius ) );

            // if (center > max + radius || center < min - radius) reject;
            Outside = XMVectorOrInt( Outside, XMVectorGreater( Dist, FrustumMax + Radius ) );
            Outside = XMVectorOrInt( Outside, XMVectorLess(    Dist, FrustumMin - Radius ) );
        }
    }
    
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If we did not find a separating plane then the box must intersect the frustum.
    return 1;
}



//-----------------------------------------------------------------------------
// Exact frustum vs frustum test.
// Return values: 0 = no intersection, 
//                1 = intersection, 
//                2 = frustum A is completely inside frustum B
//-----------------------------------------------------------------------------
INT IntersectFrustumFrustum( const Frustum* pVolumeA, const Frustum* pVolumeB )
{
    XMASSERT( pVolumeA );
    XMASSERT( pVolumeB );

    // Load origin and orientation of frustum B.
    XMVECTOR OriginB = XMLoadFloat3( &pVolumeB->Origin );
    XMVECTOR OrientationB = XMLoadFloat4( &pVolumeB->Orientation );

    XMASSERT( XMQuaternionIsUnit( OrientationB ) );

    // Build the planes of frustum B.
    XMVECTOR AxisB[6];
    AxisB[0] = XMVectorSet(  0.0f,  0.0f, -1.0f,                  0.0f );
    AxisB[1] = XMVectorSet(  0.0f,  0.0f,  1.0f,                  0.0f );
    AxisB[2] = XMVectorSet(  1.0f,  0.0f, -pVolumeB->RightSlope,  0.0f );
    AxisB[3] = XMVectorSet( -1.0f,  0.0f,  pVolumeB->LeftSlope,   0.0f );
    AxisB[4] = XMVectorSet(  0.0f,  1.0f, -pVolumeB->TopSlope,    0.0f );
    AxisB[5] = XMVectorSet(  0.0f, -1.0f,  pVolumeB->BottomSlope, 0.0f );
    
    XMVECTOR PlaneDistB[6];
    PlaneDistB[0] = -XMVectorReplicate( pVolumeB->Near );
    PlaneDistB[1] = XMVectorReplicate( pVolumeB->Far );
    PlaneDistB[2] = XMVectorZero();
    PlaneDistB[3] = XMVectorZero();
    PlaneDistB[4] = XMVectorZero();
    PlaneDistB[5] = XMVectorZero();

    // Load origin and orientation of frustum A.
    XMVECTOR OriginA = XMLoadFloat3( &pVolumeA->Origin );
    XMVECTOR OrientationA = XMLoadFloat4( &pVolumeA->Orientation );

    XMASSERT( XMQuaternionIsUnit( OrientationA ) );
    
    // Transform frustum A into the space of the frustum B in order to 
    // minimize the number of transforms we have to do.
    OriginA = XMVector3InverseRotate( OriginA - OriginB, OrientationB );
    OrientationA = XMQuaternionMultiply( OrientationA, XMQuaternionConjugate( OrientationB ) );

    // Build the corners of frustum A (in the local space of B).
    XMVECTOR RightTopA = XMVectorSet( pVolumeA->RightSlope, pVolumeA->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottomA = XMVectorSet( pVolumeA->RightSlope, pVolumeA->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTopA = XMVectorSet( pVolumeA->LeftSlope, pVolumeA->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottomA = XMVectorSet( pVolumeA->LeftSlope, pVolumeA->BottomSlope, 1.0f, 0.0f );
    XMVECTOR NearA = XMVectorReplicate( pVolumeA->Near );
    XMVECTOR FarA = XMVectorReplicate( pVolumeA->Far );
  
    RightTopA = XMVector3Rotate( RightTopA, OrientationA );
    RightBottomA = XMVector3Rotate( RightBottomA, OrientationA );
    LeftTopA = XMVector3Rotate( LeftTopA, OrientationA );
    LeftBottomA = XMVector3Rotate( LeftBottomA, OrientationA );
    
    XMVECTOR CornersA[8];
    CornersA[0] = OriginA + RightTopA * NearA;
    CornersA[1] = OriginA + RightBottomA * NearA;
    CornersA[2] = OriginA + LeftTopA * NearA;
    CornersA[3] = OriginA + LeftBottomA * NearA;
    CornersA[4] = OriginA + RightTopA * FarA;
    CornersA[5] = OriginA + RightBottomA * FarA;
    CornersA[6] = OriginA + LeftTopA * FarA;
    CornersA[7] = OriginA + LeftBottomA * FarA;

    // Check frustum A against each plane of frustum B.
    XMVECTOR Outside = XMVectorFalseInt();
    XMVECTOR InsideAll = XMVectorTrueInt();

    for ( INT i = 0; i < 6; i++ )
    {
        // Find the min/max projection of the frustum onto the plane normal.
        XMVECTOR Min, Max;

        Min = Max = XMVector3Dot( AxisB[i], CornersA[0] );
        
        for ( INT j = 1; j < 8; j++ )
        {
            XMVECTOR Temp = XMVector3Dot( AxisB[i], CornersA[j] );
            Min = XMVectorMinimize( Min, Temp );
            Max = XMVectorMaximize( Max, Temp );
        }
        
        // Outside the plane?
        Outside = XMVectorOrInt( Outside, XMVectorGreater( Min, PlaneDistB[i] ) );

        // Fully inside the plane?
        InsideAll = XMVectorAndInt( InsideAll, XMVectorLessOrEqual( Max, PlaneDistB[i] ) );
    }

    // If the frustum A is outside any of the planes of frustum B it is outside. 
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If frustum A is inside all planes of frustum B it is fully inside.
    if ( XMVector4EqualInt( InsideAll, XMVectorTrueInt() ) )
        return 2;
   
    // Build the corners of frustum B.
    XMVECTOR RightTopB = XMVectorSet( pVolumeB->RightSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottomB = XMVectorSet( pVolumeB->RightSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTopB = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottomB = XMVectorSet( pVolumeB->LeftSlope, pVolumeB->BottomSlope, 1.0f, 0.0f );
    XMVECTOR NearB = XMVectorReplicate( pVolumeB->Near );
    XMVECTOR FarB = XMVectorReplicate( pVolumeB->Far );
    
    XMVECTOR CornersB[8];
    CornersB[0] = RightTopB * NearB;
    CornersB[1] = RightBottomB * NearB;
    CornersB[2] = LeftTopB * NearB;
    CornersB[3] = LeftBottomB * NearB;
    CornersB[4] = RightTopB * FarB;
    CornersB[5] = RightBottomB * FarB;
    CornersB[6] = LeftTopB * FarB;
    CornersB[7] = LeftBottomB * FarB;
    
    // Build the planes of frustum A (in the local space of B).
    XMVECTOR AxisA[6];
    XMVECTOR PlaneDistA[6];
   
    AxisA[0] = XMVectorSet(  0.0f,  0.0f, -1.0f,                  0.0f );
    AxisA[1] = XMVectorSet(  0.0f,  0.0f,  1.0f,                  0.0f );
    AxisA[2] = XMVectorSet(  1.0f,  0.0f, -pVolumeA->RightSlope,  0.0f );
    AxisA[3] = XMVectorSet( -1.0f,  0.0f,  pVolumeA->LeftSlope,   0.0f );
    AxisA[4] = XMVectorSet(  0.0f,  1.0f, -pVolumeA->TopSlope,    0.0f );
    AxisA[5] = XMVectorSet(  0.0f, -1.0f,  pVolumeA->BottomSlope, 0.0f );
    
    AxisA[0] = XMVector3Rotate( AxisA[0], OrientationA );
    AxisA[1] = -AxisA[0];
    AxisA[2] = XMVector3Rotate( AxisA[2], OrientationA );
    AxisA[3] = XMVector3Rotate( AxisA[3], OrientationA );
    AxisA[4] = XMVector3Rotate( AxisA[4], OrientationA );
    AxisA[5] = XMVector3Rotate( AxisA[5], OrientationA );
    
    PlaneDistA[0] = XMVector3Dot( AxisA[0], CornersA[0] );  // Re-use corner on near plane.
    PlaneDistA[1] = XMVector3Dot( AxisA[1], CornersA[4] );  // Re-use corner on far plane.
    PlaneDistA[2] = XMVector3Dot( AxisA[2], OriginA );
    PlaneDistA[3] = XMVector3Dot( AxisA[3], OriginA );
    PlaneDistA[4] = XMVector3Dot( AxisA[4], OriginA );
    PlaneDistA[5] = XMVector3Dot( AxisA[5], OriginA );

    // Check each axis of frustum A for a seperating plane (5).
    for ( INT i = 0; i < 6; i++ )
    {
        // Find the minimum projection of the frustum onto the plane normal.
        XMVECTOR Min;

        Min = XMVector3Dot( AxisA[i], CornersB[0] );
        
        for ( INT j = 1; j < 8; j++ )
        {
            XMVECTOR Temp = XMVector3Dot( AxisA[i], CornersB[j] );
            Min = XMVectorMinimize( Min, Temp );
        }
        
        // Outside the plane?
        Outside = XMVectorOrInt( Outside, XMVectorGreater( Min, PlaneDistA[i] ) );
    }

    // If the frustum B is outside any of the planes of frustum A it is outside. 
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // Check edge/edge axes (6 * 6).
    XMVECTOR FrustumEdgeAxisA[6];
    FrustumEdgeAxisA[0] = RightTopA;
    FrustumEdgeAxisA[1] = RightBottomA;
    FrustumEdgeAxisA[2] = LeftTopA;
    FrustumEdgeAxisA[3] = LeftBottomA;
    FrustumEdgeAxisA[4] = RightTopA - LeftTopA;
    FrustumEdgeAxisA[5] = LeftBottomA - LeftTopA;

    XMVECTOR FrustumEdgeAxisB[6];
    FrustumEdgeAxisB[0] = RightTopB;
    FrustumEdgeAxisB[1] = RightBottomB;
    FrustumEdgeAxisB[2] = LeftTopB;
    FrustumEdgeAxisB[3] = LeftBottomB;
    FrustumEdgeAxisB[4] = RightTopB - LeftTopB;
    FrustumEdgeAxisB[5] = LeftBottomB - LeftTopB;

    for ( INT i = 0; i < 6; i++ )
    {
        for ( INT j = 0; j < 6; j++ )
        {
            // Compute the axis we are going to test.
            XMVECTOR Axis = XMVector3Cross( FrustumEdgeAxisA[i], FrustumEdgeAxisB[j] );
            
            // Find the min/max values of the projection of both frustums onto the axis.
            XMVECTOR MinA, MaxA;
            XMVECTOR MinB, MaxB;

            MinA = MaxA = XMVector3Dot( Axis, CornersA[0] );
            MinB = MaxB = XMVector3Dot( Axis, CornersB[0] );
            
            for ( INT k = 1; k < 8; k++ )
            {
                XMVECTOR TempA = XMVector3Dot( Axis, CornersA[k] );
                MinA = XMVectorMinimize( MinA, TempA );
                MaxA = XMVectorMaximize( MaxA, TempA );

                XMVECTOR TempB = XMVector3Dot( Axis, CornersB[k] );
                MinB = XMVectorMinimize( MinB, TempB );
                MaxB = XMVectorMaximize( MaxB, TempB );
            }
            
            // if (MinA > MaxB || MinB > MaxA) reject;
            Outside = XMVectorOrInt( Outside, XMVectorGreater( MinA, MaxB ) );
            Outside = XMVectorOrInt( Outside, XMVectorGreater( MinB, MaxA ) );
        }
    }

    // If there is a seperating plane, then the frustums do not intersect.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;

    // If we did not find a separating plane then the frustums intersect.
    return 1;
}



//-----------------------------------------------------------------------------
inline void FastIntersectTrianglePlane( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, XMVECTOR Plane,
                                        XMVECTOR& Outside, XMVECTOR& Inside )
{
    // Plane0
    XMVECTOR Dist0 = XMVector4Dot( V0, Plane );
    XMVECTOR Dist1 = XMVector4Dot( V1, Plane );
    XMVECTOR Dist2 = XMVector4Dot( V2, Plane );
    
    XMVECTOR MinDist = XMVectorMinimize( Dist0, Dist1 );
    MinDist = XMVectorMinimize( MinDist, Dist2 );
    
    XMVECTOR MaxDist = XMVectorMaximize( Dist0, Dist1 );
    MaxDist = XMVectorMaximize( MaxDist, Dist2 );
    
    XMVECTOR Zero = XMVectorZero();
    
    // Outside the plane?
    Outside = XMVectorGreater( MinDist, Zero );

    // Fully inside the plane?
    Inside = XMVectorLess( MaxDist, Zero );
}



//-----------------------------------------------------------------------------
// Test a triangle vs 6 planes (typically forming a frustum).
// Return values: 0 = no intersection, 
//                1 = may be intersecting, 
//                2 = triangle is inside all planes
//-----------------------------------------------------------------------------
INT IntersectTriangle6Planes( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 )
{
    XMVECTOR One = XMVectorSplatOne();

    // Set w of the points to one so we can dot4 with a plane.
    V0 = XMVectorInsert( V0, One, 0, 0, 0, 0, 1 );
    V1 = XMVectorInsert( V1, One, 0, 0, 0, 0, 1 );
    V2 = XMVectorInsert( V2, One, 0, 0, 0, 0, 1 );
    
    XMVECTOR Outside, Inside;

    // Test against each plane.
    FastIntersectTrianglePlane( V0, V1, V2, Plane0, Outside, Inside );
    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    FastIntersectTrianglePlane( V0, V1, V2, Plane1, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectTrianglePlane( V0, V1, V2, Plane2, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectTrianglePlane( V0, V1, V2, Plane3, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectTrianglePlane( V0, V1, V2, Plane4, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectTrianglePlane( V0, V1, V2, Plane5, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    // If the triangle is outside any plane it is outside.
    if ( XMVector4EqualInt( AnyOutside, XMVectorTrueInt() ) )
        return 0;
    
    // If the triangle is inside all planes it is inside.
    if ( XMVector4EqualInt( AllInside, XMVectorTrueInt() ) )
        return 2;

    // The triangle is not inside all planes or outside a plane, it may intersect.
    return 1;
}



//-----------------------------------------------------------------------------
inline void FastIntersectSpherePlane( XMVECTOR Center, XMVECTOR Radius, XMVECTOR Plane,
                                      XMVECTOR& Outside, XMVECTOR& Inside )
{
    XMVECTOR Dist = XMVector4Dot( Center, Plane );
    
    // Outside the plane?
    Outside = XMVectorGreater( Dist, Radius );

    // Fully inside the plane?
    Inside = XMVectorLess( Dist, -Radius );
}



//-----------------------------------------------------------------------------
// Test a sphere vs 6 planes (typically forming a frustum).
// Return values: 0 = no intersection, 
//                1 = may be intersecting, 
//                2 = sphere is inside all planes
//-----------------------------------------------------------------------------
INT IntersectSphere6Planes( const Sphere* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 )
{
    XMASSERT( pVolume );

    // Load the sphere.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolume->Radius );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1 );

    XMVECTOR Outside, Inside;

    // Test against each plane.
    FastIntersectSpherePlane( Center, Radius, Plane0, Outside, Inside );
    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    FastIntersectSpherePlane( Center, Radius, Plane1, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectSpherePlane( Center, Radius, Plane2, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectSpherePlane( Center, Radius, Plane3, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectSpherePlane( Center, Radius, Plane4, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectSpherePlane( Center, Radius, Plane5, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );
    
    // If the sphere is outside any plane it is outside.
    if ( XMVector4EqualInt( AnyOutside, XMVectorTrueInt() ) )
        return 0;
    
    // If the sphere is inside all planes it is inside.
    if ( XMVector4EqualInt( AllInside, XMVectorTrueInt() ) )
        return 2;

    // The sphere is not inside all planes or outside a plane, it may intersect.
    return 1;
}



//-----------------------------------------------------------------------------
inline void FastIntersectAxisAlignedBoxPlane( XMVECTOR Center, XMVECTOR Extents, XMVECTOR Plane,
                                              XMVECTOR& Outside, XMVECTOR& Inside )

{
    // Compute the distance to the center of the box.
    XMVECTOR Dist = XMVector4Dot( Center, Plane );
    
    // Project the axes of the box onto the normal of the plane.  Half the
    // length of the projection (sometime called the "radius") is equal to
    // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
    // where h(i) are extents of the box, n is the plane normal, and b(i) are the 
    // axes of the box. In this case b(i) = [(1,0,0), (0,1,0), (0,0,1)].
    XMVECTOR Radius = XMVector3Dot( Extents, XMVectorAbs( Plane ) );
    
    // Outside the plane?
    Outside = XMVectorGreater( Dist, Radius );

    // Fully inside the plane?
    Inside = XMVectorLess( Dist, -Radius );
}



//-----------------------------------------------------------------------------
// Test an axis alinged box vs 6 planes (typically forming a frustum).
// Return values: 0 = no intersection, 
//                1 = may be intersecting, 
//                2 = box is inside all planes
//-----------------------------------------------------------------------------
INT IntersectAxisAlignedBox6Planes( const AxisAlignedBox* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 )
{
    XMASSERT( pVolume );

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1 );

    XMVECTOR Outside, Inside;

    // Test against each plane.
    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane0, Outside, Inside );
    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane1, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane2, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane3, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane4, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane5, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );
    
    // If the box is outside any plane it is outside.
    if ( XMVector4EqualInt( AnyOutside, XMVectorTrueInt() ) )
        return 0;
    
    // If the box is inside all planes it is inside.
    if ( XMVector4EqualInt( AllInside, XMVectorTrueInt() ) )
        return 2;

    // The box is not inside all planes or outside a plane, it may intersect.
    return 1;
}



//-----------------------------------------------------------------------------
inline void FastIntersectOrientedBoxPlane( XMVECTOR Center, XMVECTOR Extents, XMVECTOR Axis0, XMVECTOR Axis1, XMVECTOR Axis2, XMVECTOR Plane,
                                           XMVECTOR& Outside, XMVECTOR& Inside )
{
    // Compute the distance to the center of the box.
    XMVECTOR Dist = XMVector4Dot( Center, Plane );
    
    // Project the axes of the box onto the normal of the plane.  Half the
    // length of the projection (sometime called the "radius") is equal to
    // h(u) * abs(n dot b(u))) + h(v) * abs(n dot b(v)) + h(w) * abs(n dot b(w))
    // where h(i) are extents of the box, n is the plane normal, and b(i) are the 
    // axes of the box.
    XMVECTOR Radius = XMVector3Dot( Plane, Axis0 );
    Radius = XMVectorInsert( Radius, XMVector3Dot( Plane, Axis1 ), 0, 0, 1, 0, 0 );
    Radius = XMVectorInsert( Radius, XMVector3Dot( Plane, Axis2 ), 0, 0, 0, 1, 0 );
    Radius = XMVector3Dot( Extents, XMVectorAbs( Radius ) );

    // Outside the plane?
    Outside = XMVectorGreater( Dist, Radius );

    // Fully inside the plane?
    Inside = XMVectorLess( Dist, -Radius );
}



//-----------------------------------------------------------------------------
// Test an oriented box vs 6 planes (typically forming a frustum).
// Return values: 0 = no intersection, 
//                1 = may be intersecting, 
//                2 = box is inside all planes
//-----------------------------------------------------------------------------
INT IntersectOrientedBox6Planes( const OrientedBox* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 )
{
    XMASSERT( pVolume );

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );
    XMVECTOR BoxOrientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( BoxOrientation ) );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    // Build the 3x3 rotation matrix that defines the box axes.
    XMMATRIX R = XMMatrixRotationQuaternion( BoxOrientation );

    XMVECTOR Outside, Inside;

    // Test against each plane.
    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane0, Outside, Inside );
    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane1, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane2, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane3, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane4, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane5, Outside, Inside );
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );
    
    // If the box is outside any plane it is outside.
    if ( XMVector4EqualInt( AnyOutside, XMVectorTrueInt() ) )
        return 0;
    
    // If the box is inside all planes it is inside.
    if ( XMVector4EqualInt( AllInside, XMVectorTrueInt() ) )
        return 2;

    // The box is not inside all planes or outside a plane, it may intersect.
    return 1;
}



//-----------------------------------------------------------------------------
inline void FastIntersectFrustumPlane( XMVECTOR Point0, XMVECTOR Point1, XMVECTOR Point2, XMVECTOR Point3, XMVECTOR Point4, XMVECTOR Point5, XMVECTOR Point6, XMVECTOR Point7, XMVECTOR Plane,
                                       XMVECTOR& Outside, XMVECTOR& Inside )
{
    // Find the min/max projection of the frustum onto the plane normal.
    XMVECTOR Min, Max, Dist;

    Min = Max = XMVector3Dot( Plane, Point0 );
    
    Dist = XMVector3Dot( Plane, Point1 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point2 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point3 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point4 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point5 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point6 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );

    Dist = XMVector3Dot( Plane, Point7 );
    Min = XMVectorMinimize( Min, Dist );
    Max = XMVectorMaximize( Max, Dist );
    
    XMVECTOR PlaneDist = -XMVectorSplatW( Plane );
    
    // Outside the plane?
    Outside = XMVectorGreater( Min, PlaneDist );

    // Fully inside the plane?
    Inside = XMVectorLess( Max, PlaneDist );
}



//-----------------------------------------------------------------------------
// Test a frustum vs 6 planes (typically forming another frustum).
// Return values: 0 = no intersection, 
//                1 = may be intersecting, 
//                2 = frustum is inside all planes
//-----------------------------------------------------------------------------
INT IntersectFrustum6Planes( const Frustum* pVolume, XMVECTOR Plane0, XMVECTOR Plane1, XMVECTOR Plane2, XMVECTOR Plane3, XMVECTOR Plane4, XMVECTOR Plane5 )
{
    XMASSERT( pVolume );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolume->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Set w of the origin to one so we can dot4 with a plane.
    Origin = XMVectorInsert( Origin, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    // Build the corners of the frustum (in world space).
    XMVECTOR RightTop = XMVectorSet( pVolume->RightSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottom = XMVectorSet( pVolume->RightSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTop = XMVectorSet( pVolume->LeftSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottom = XMVectorSet( pVolume->LeftSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR Near = XMVectorSet( pVolume->Near, pVolume->Near, pVolume->Near, 0.0f );
    XMVECTOR Far = XMVectorSet( pVolume->Far, pVolume->Far, pVolume->Far, 0.0f );
  
    RightTop = XMVector3Rotate( RightTop, Orientation );
    RightBottom = XMVector3Rotate( RightBottom, Orientation );
    LeftTop = XMVector3Rotate( LeftTop, Orientation );
    LeftBottom = XMVector3Rotate( LeftBottom, Orientation );
    
    XMVECTOR Corners0, Corners1, Corners2, Corners3, Corners4, Corners5, Corners6, Corners7;
    Corners0 = Origin + RightTop * Near;
    Corners1 = Origin + RightBottom * Near;
    Corners2 = Origin + LeftTop * Near;
    Corners3 = Origin + LeftBottom * Near;
    Corners4 = Origin + RightTop * Far;
    Corners5 = Origin + RightBottom * Far;
    Corners6 = Origin + LeftTop * Far;
    Corners7 = Origin + LeftBottom * Far;

    XMVECTOR Outside, Inside;

    // Test against each plane.
    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane0, Outside, Inside );
                               
    XMVECTOR AnyOutside = Outside;
    XMVECTOR AllInside = Inside;

    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane1, Outside, Inside );
                               
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane2, Outside, Inside );
                               
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane3, Outside, Inside );
                               
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane4, Outside, Inside );
                               
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );

    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane5, Outside, Inside );
                               
    AnyOutside = XMVectorOrInt( AnyOutside, Outside );
    AllInside = XMVectorAndInt( AllInside, Inside );
    
    // If the frustum is outside any plane it is outside.
    if ( XMVector4EqualInt( AnyOutside, XMVectorTrueInt() ) )
        return 0;
    
    // If the frustum is inside all planes it is inside.
    if ( XMVector4EqualInt( AllInside, XMVectorTrueInt() ) )
        return 2;

    // The frustum is not inside all planes or outside a plane, it may intersect.
    return 1;
}



//-----------------------------------------------------------------------------
INT IntersectTrianglePlane( XMVECTOR V0, XMVECTOR V1, XMVECTOR V2, XMVECTOR Plane )
{
    XMVECTOR One;

    XMASSERT( XMPlaneIsUnit( Plane ) );

    One = XMVectorSplatOne();

    // Set w of the points to one so we can dot4 with a plane.
    V0 = XMVectorInsert(V0, One, 0, 0, 0, 0, 1);
    V1 = XMVectorInsert(V1, One, 0, 0, 0, 0, 1);
    V2 = XMVectorInsert(V2, One, 0, 0, 0, 0, 1);

    XMVECTOR Outside, Inside;
    FastIntersectTrianglePlane( V0, V1, V2, Plane, Outside, Inside );
    
    // If the triangle is outside any plane it is outside.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If the triangle is inside all planes it is inside.
    if ( XMVector4EqualInt( Inside, XMVectorTrueInt() ) )
        return 2;

    // The triangle is not inside all planes or outside a plane it intersects.
    return 1;
}



//-----------------------------------------------------------------------------
INT IntersectSpherePlane( const Sphere* pVolume, const XMVECTOR Plane )
{
    XMASSERT( pVolume );
    XMASSERT( XMPlaneIsUnit( Plane ) );

    // Load the sphere.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Radius = XMVectorReplicate( pVolume->Radius );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1 );

    XMVECTOR Outside, Inside;
    FastIntersectSpherePlane( Center, Radius, Plane, Outside, Inside );

    // If the sphere is outside any plane it is outside.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If the sphere is inside all planes it is inside.
    if ( XMVector4EqualInt( Inside, XMVectorTrueInt() ) )
        return 2;

    // The sphere is not inside all planes or outside a plane it intersects.
    return 1;
}



//-----------------------------------------------------------------------------
INT IntersectAxisAlignedBoxPlane( const AxisAlignedBox* pVolume, XMVECTOR Plane )
{
    XMASSERT( pVolume );
    XMASSERT( XMPlaneIsUnit( Plane ) );

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    XMVECTOR Outside, Inside;
    FastIntersectAxisAlignedBoxPlane( Center, Extents, Plane, Outside, Inside );

    // If the box is outside any plane it is outside.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If the box is inside all planes it is inside.
    if ( XMVector4EqualInt( Inside, XMVectorTrueInt() ) )
        return 2;

    // The box is not inside all planes or outside a plane it intersects.
    return 1;
}



//-----------------------------------------------------------------------------
INT IntersectOrientedBoxPlane( const OrientedBox* pVolume, XMVECTOR Plane )
{
    XMASSERT( pVolume );
    XMASSERT( XMPlaneIsUnit( Plane ) );

    // Load the box.
    XMVECTOR Center = XMLoadFloat3( &pVolume->Center );
    XMVECTOR Extents = XMLoadFloat3( &pVolume->Extents );
    XMVECTOR BoxOrientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( BoxOrientation ) );

    // Set w of the center to one so we can dot4 with a plane.
    Center = XMVectorInsert( Center, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    // Build the 3x3 rotation matrix that defines the box axes.
    XMMATRIX R = XMMatrixRotationQuaternion( BoxOrientation );

    XMVECTOR Outside, Inside;
    FastIntersectOrientedBoxPlane( Center, Extents, R.r[0], R.r[1], R.r[2], Plane, Outside, Inside );

    // If the box is outside any plane it is outside.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If the box is inside all planes it is inside.
    if ( XMVector4EqualInt( Inside, XMVectorTrueInt() ) )
        return 2;

    // The box is not inside all planes or outside a plane it intersects.
    return 1;
}



//-----------------------------------------------------------------------------
INT IntersectFrustumPlane( const Frustum* pVolume, XMVECTOR Plane )
{
    XMASSERT( pVolume );
    XMASSERT( XMPlaneIsUnit( Plane ) );

    // Load origin and orientation of the frustum.
    XMVECTOR Origin = XMLoadFloat3( &pVolume->Origin );
    XMVECTOR Orientation = XMLoadFloat4( &pVolume->Orientation );

    XMASSERT( XMQuaternionIsUnit( Orientation ) );

    // Set w of the origin to one so we can dot4 with a plane.
    Origin = XMVectorInsert( Origin, XMVectorSplatOne(), 0, 0, 0, 0, 1);

    // Build the corners of the frustum (in world space).
    XMVECTOR RightTop = XMVectorSet( pVolume->RightSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR RightBottom = XMVectorSet( pVolume->RightSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR LeftTop = XMVectorSet( pVolume->LeftSlope, pVolume->TopSlope, 1.0f, 0.0f );
    XMVECTOR LeftBottom = XMVectorSet( pVolume->LeftSlope, pVolume->BottomSlope, 1.0f, 0.0f );
    XMVECTOR Near = XMVectorSet( pVolume->Near, pVolume->Near, pVolume->Near, 0.0f );
    XMVECTOR Far = XMVectorSet( pVolume->Far, pVolume->Far, pVolume->Far, 0.0f );
  
    RightTop = XMVector3Rotate( RightTop, Orientation );
    RightBottom = XMVector3Rotate( RightBottom, Orientation );
    LeftTop = XMVector3Rotate( LeftTop, Orientation );
    LeftBottom = XMVector3Rotate( LeftBottom, Orientation );
    
    XMVECTOR Corners0, Corners1, Corners2, Corners3, Corners4, Corners5, Corners6, Corners7;
    Corners0 = Origin + RightTop * Near;
    Corners1 = Origin + RightBottom * Near;
    Corners2 = Origin + LeftTop * Near;
    Corners3 = Origin + LeftBottom * Near;
    Corners4 = Origin + RightTop * Far;
    Corners5 = Origin + RightBottom * Far;
    Corners6 = Origin + LeftTop * Far;
    Corners7 = Origin + LeftBottom * Far;
    
    XMVECTOR Outside, Inside;
    FastIntersectFrustumPlane( Corners0, Corners1, Corners2, Corners3, 
                               Corners4, Corners5, Corners6, Corners7, 
                               Plane, Outside, Inside );

    // If the frustum is outside any plane it is outside.
    if ( XMVector4EqualInt( Outside, XMVectorTrueInt() ) )
        return 0;
    
    // If the frustum is inside all planes it is inside.
    if ( XMVector4EqualInt( Inside, XMVectorTrueInt() ) )
        return 2;

    // The frustum is not inside all planes or outside a plane it intersects.
    return 1;
}


} // namespace ATG
