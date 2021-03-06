/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

// include required headers
#include "StandardHeaders.h"
#include "Vector.h"


namespace MCore
{
    // forward declarations
    class PlaneEq;
    class AABB;
    class BoundingSphere;

    /**
     * The Ray template/class.
     * A ray is normally an infinite line, starting at a given point (the origin) and heading into a given direction.
     * However, this template does not represent an infinite line, but a finite line, since that will be a lot more useful.
     * This means we have an origin, which is the starting point and a destination, which is the end point.
     * This automatically gives us a direction vector too. So basically we now have a finite ray, which is just a 3D line.
     * We can use rays mainly to perform intersection tests. This class provides you with methods to calculate interection information
     * between the ray and bounding spheres, axis aligned bounding boxes, triangles and planes. More intersection tests might be added in a later stage.
     * Or they have already been added, but this documentation should be updated :)
     * Example fields where rays are (often) used are: collision and hit detection, raytracing images, global illumination, lightmap generation, pathtracing,
     * real-time volumetric effects, lensflares, etc.
     */
    class MCORE_API Ray
    {
    public:
        /**
         * Default constructor. Does NOT initialize any members. So this would not be a valid ray.
         */
        MCORE_INLINE Ray()                                                                                      {}

        /**
         * Constructor which sets the start and end point of the ray.
         * @param org The origin of the ray.
         * @param endPoint The end (destination) point of the ray.
         */
        MCORE_INLINE Ray(const Vector3& org, const Vector3& endPoint)
            : mOrigin(org)
            , mDest(endPoint)
            , mDirection((endPoint - org).Normalize()) {}

        /**
         * Constructor which sets the origin, destination point and direction.
         * @param org The origin of the ray, so where it starts.
         * @param endPoint The destination point of the ray, so where it should end.
         * @param dir The normalized direction vector of the ray, which should be (endPoint - startPoint).Normalize()
         */
        MCORE_INLINE Ray(const Vector3& org, const Vector3& endPoint, const Vector3& dir)
            : mOrigin(org)
            , mDest(endPoint)
            , mDirection(dir) {}

        /**
         * Set the origin and destination point (end point) of the ray.
         * The direction vector will be calculated automatically.
         * @param org The origin of the ray, so the start point.
         * @param endPoint The destination of the ray, so the end point.
         */
        MCORE_INLINE void Set(const Vector3& org, const Vector3& endPoint)                                      { mOrigin = org; mDest = endPoint; mDirection = (mDest - mOrigin).Normalize(); }

        /**
         * Set the origin of the ray, so the start point. The direction will automatically be updated as well.
         * @param org The origin.
         */
        MCORE_INLINE void SetOrigin(const Vector3& org)                                                         { mOrigin = org; mDirection = (mDest - mOrigin).Normalize(); }

        /**
         * Set the destination point of the ray.
         * @param dest The destination of the ray.
         */
        MCORE_INLINE void SetDest(const Vector3& dest)                                                          { mDest = dest; mDirection = (mDest - mOrigin).Normalize(); }

        /**
         * Get the origin of the ray.
         * @result The origin of the ray, so where it starts.
         */
        MCORE_INLINE const Vector3& GetOrigin() const                                                           { return mOrigin; }

        /**
         * Get the destination of the ray.
         * @result The destination point of the ray, so where it ends.
         */
        MCORE_INLINE const Vector3& GetDest() const                                                             { return mDest; }

        /**
         * Get the direction of the ray.
         * @result The normalized direction vector of the ray, so the direction its heading to.
         */
        MCORE_INLINE const Vector3& GetDirection() const                                                        { return mDirection; }

        /**
         * Perform a ray/sphere intersection test.
         * @param s The bounding sphere to test with.
         * @param intersectA If not nullptr, the closest intersection point will be stored in this vector, in case of an intersection.
         * @param intersectB If not nullptr, the farthest intersection point will be stored in this vector, in case of an intersection.
         * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersectA' and 'intersectB' won't be changed.
         */
        bool Intersects(const BoundingSphere& s, Vector3* intersectA = nullptr, Vector3* intersectB = nullptr) const;

        /**
         * Perform a ray/plane intersection test.
         * @param p The plane to test with.
         * @param intersect If not nullptr, the intersection point will be stored in this vector, in case of an intersection.
         * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersect' will not be changed.
         */
        bool Intersects(const PlaneEq& p, Vector3* intersect = nullptr) const;

        /**
         * Perform a ray/triangle intersection test.
         * @param p1 The first point of the triangle.
         * @param p2 The second point of the triangle.
         * @param p3 The third point of the triangle.
         * @param intersect If not nullptr, the intersection point will be stored in here, in case of an intersection.
         * @param baryU If not nullptr, the 'u' barycentric coordinate will be stored in here.
         * @param baryV If not nullptr, the 'v' barycentric coordinate will be stored in here.
         * @result Returns true in case of an intersection, otherwise false. If there is no intersection, 'intersect', 'baryU' and 'baryV' will not be modified. You
         * can calculate the uv or normal or whatsoever at the intersection point by using the baryU and baryV values.
         *
         * The calculation goes like:
         *
         * valueAtIntersectionPoint = (1-u-v)*A + u*B + v*C;
         *
         * Where u and v are the values written in baryU and baryV and A, B and C are the three 'attributes' on the 3 points of the triangle. For example the three
         * vertex normals or uv coordinates or colors. Where A is the attribute linked with 'p1', B the attribute linked with 'p2' and C the attribute linked with 'p3'.
         *
         * To make it easy for you, we created a function caled BarycentricInterpolate() which takes the required parameters and returns the attribute value at the given
         * barycentric coordinates for you. The usage would now be:
         *
         * valueAtIntersectionPoint = BarycentricInterpolate<Vector3>(u, v, A, B, C);
         *
         * Where A, B and C could be the vertex normals of the triangle for example.
         * You can easily also calculate the intersection point yourself by using the u and v, by doing this:
         *
         * intersectionPoint = BarycentricInterpolate<Vector3>(u, v, p1, p2, p3);
         *
         * @see BarycentricInterpolate
         */
        bool Intersects(const Vector3& p1, const Vector3& p2, const Vector3& p3, Vector3* intersect = nullptr, float* baryU = nullptr, float* baryV = nullptr) const;

        /**
         * Perform a ray/AABB (Axis Aligned Bounding Box) intersection test.
         * @param b The box to test with.
         * @param intersectA If not nullptr, the closest intersection point will be stored in this vector, in case of an intersection.
         * @param intersectB If not nullptr, the farthest intersection point will be stored in this vector, in case of an intersection.
         * @result Returns true when an intersection occured, otherwise false. If there is no intersection, 'intersectA' and 'intersectB' won't be modified.
         */
        bool Intersects(const AABB& b, Vector3* intersectA = nullptr, Vector3* intersectB = nullptr) const;

        /**
         * Calculates the length of the ray.
         * @result The length of the ray.
         */
        MCORE_INLINE float Length() const                                                                       { return (mDest - mOrigin).SafeLength(); }

    private:
        Vector3     mOrigin;    /**< The origin of the ray. */
        Vector3     mDest;      /**< The destination of the ray. */
        Vector3     mDirection; /**< The normalized direction vector of the ray. */
    };
}   // namespace MCore
