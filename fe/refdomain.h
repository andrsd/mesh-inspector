#pragma once

#include "enums.h"
#include "common.h"

namespace fe {

/// Reference domain for an edge (1D)
///
class RefEdge {
public:
    static const Point1D *
    get_vertices()
    {
        return vertices;
    }

protected:
    static const Point1D vertices[];
};

/// Reference domain for a triangle (2D)
///
class RefTri {
public:
    static const Point2D *
    get_vertices()
    {
        return vertices;
    }

    static const int *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

protected:
    static const Point2D vertices[];
    static const int2 edge_vtcs[];
};

/// Reference domain for a quadrilateral (2D)
///
/// @ingroup ref_domains
class RefQuad {
public:
    static const Point2D *
    get_vertices()
    {
        return vertices;
    }

    static const int *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

protected:
    static const Point2D vertices[];
    static const int2 edge_vtcs[];
};

/// Reference domain for tetrahedron (3D)
///
class RefTetra {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const int *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

    static int
    get_edge_orientations()
    {
        // two orientations of an edge
        return 2;
    }

    static int
    get_num_face_vertices(int face)
    {
        return face_nvtcs[face];
    }

    static int
    get_num_face_edges(int face)
    {
        return face_nedges[face];
    }

    static const int *
    get_face_vertices(int face)
    {
        return face_vtcs[face];
    }

    static const int *
    get_face_edges(int face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(int face)
    {
        return face_mode[face];
    }

    static int
    get_face_orientations(int face)
    {
        return face_orientations[face];
    }

    static const Point3D
    get_face_normal(int iface)
    {
        return face_normal[iface];
    }

protected:
    static const Point3D vertices[];
    static const int2 edge_vtcs[];
    static const int * face_vtcs[];
    static const int * face_edges[];
    static const int face_nvtcs[];
    static const int face_nedges[];
    static const EMode2D face_mode[];
    static const int face_orientations[];

    static const Point3D face_normal[];
};

/// Reference domain for hexahedron (3D)
///
class RefHex {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const int *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

    static int
    get_edge_orientations()
    {
        // two orientations of an edge
        return 2;
    }

    static int
    get_num_face_vertices(int face)
    {
        return face_nvtcs[face];
    }

    static int
    get_num_face_edges(int face)
    {
        return face_nedges[face];
    }

    static const int *
    get_face_vertices(int face)
    {
        return face_vtcs[face];
    }

    static const int *
    get_face_edges(int face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(int face)
    {
        return face_mode[face];
    }

    static int
    get_face_orientations(int face)
    {
        return face_orientations[face];
    }

    /// @param[in] ori - face orientation
    /// @return orientations of edges on a face. All edges are oriented the same way on all faces,
    /// so we do not have to care about face number
    static const int *
    get_face_edge_orientation(int ori)
    {
        return face_edge_ori[ori];
    }

    static int
    get_edge_tangent_direction(int edge)
    {
        return edge_tangent[edge];
    }

    static int
    get_face_tangent_direction(int face, int which)
    {
        return face_tangent[face][which];
    }

protected:
    static const Point3D vertices[];
    static const int2 edge_vtcs[];
    static const int * face_vtcs[];
    static const int * face_edges[];
    static const int face_nvtcs[];
    static const int face_nedges[];
    static const EMode2D face_mode[];
    static const int face_orientations[];
    static const int face_edge_ori[8][2];
    static const int edge_tangent[];
    static const int2 face_tangent[];
};

/// Reference domain for prism (3D)
///
class RefPrism {
public:
    static const Point3D *
    get_vertices()
    {
        return vertices;
    }

    static const int *
    get_edge_vertices(int edge)
    {
        return edge_vtcs[edge];
    }

    static int
    get_edge_orientations()
    {
        // two orientations of an edge
        return 2;
    }

    static int
    get_num_face_vertices(int face)
    {
        return face_nvtcs[face];
    }

    static int
    get_num_face_edges(int face)
    {
        return face_nedges[face];
    }

    static const int *
    get_face_vertices(int face)
    {
        return face_vtcs[face];
    }

    static const int *
    get_face_edges(int face)
    {
        return face_edges[face];
    }

    static EMode2D
    get_face_mode(int face)
    {
        return face_mode[face];
    }

    static int
    get_face_orientations(int face)
    {
        return face_orientations[face];
    }

protected:
    static const Point3D vertices[];
    static const int2 edge_vtcs[];
    static const int * face_vtcs[];
    static const int * face_edges[];
    static const int face_nvtcs[];
    static const int face_nedges[];
    static const EMode2D face_mode[];
    static const int face_orientations[];
};

}
