#include "refmapss.h"
#include "lobatto.h"

namespace fe {

// RefMapShapesetEdge

// FN

static double
refmap_edge_f0(double x)
{
    return l0(x);
}

static double
refmap_edge_f1(double x)
{
    return l1(x);
}

static shape_fn_1d_t refmap_edge_fn[] = { refmap_edge_f0, refmap_edge_f1 };

// DX

static double
refmap_edge_dx_f0(double x)
{
    return dl0(x);
}

static double
refmap_edge_dx_f1(double x)
{
    return dl1(x);
}

static shape_fn_1d_t refmap_edge_dx[] = { refmap_edge_dx_f0, refmap_edge_dx_f1 };

//

static int refmap_edge_vertex_indices[] = { 0, 1 };

static shape_fn_1d_t * refmap_edge_fn_table[] = { refmap_edge_fn };
static shape_fn_1d_t * refmap_edge_dx_table[] = { refmap_edge_dx };

RefMapShapesetEdge::RefMapShapesetEdge() : Shapeset1D()
{
    this->num_components = 1;
    this->shape_table[FN] = refmap_edge_fn_table;
    this->shape_table[DX] = refmap_edge_dx_table;
    this->vertex_indices = refmap_edge_vertex_indices;
}

// RefMapShapesetTri

// FN

inline double
refmap_tri_f0(double x, double y)
{
    return lambda1_2d(x, y);
}

inline double
refmap_tri_f1(double x, double y)
{
    return lambda2_2d(x, y);
}

inline double
refmap_tri_f2(double x, double y)
{
    return lambda0_2d(x, y);
}

static shape_fn_2d_t refmap_tri_fn[] = { refmap_tri_f0, refmap_tri_f1, refmap_tri_f2 };

// DX

inline double
refmap_tri_dx_f0(double x, double y)
{
    return lambda1x_2d(x, y);
}

inline double
refmap_tri_dx_f1(double x, double y)
{
    return lambda2x_2d(x, y);
}

inline double
refmap_tri_dx_f2(double x, double y)
{
    return lambda0x_2d(x, y);
}

static shape_fn_2d_t refmap_tri_dx[] = { refmap_tri_dx_f0, refmap_tri_dx_f1, refmap_tri_dx_f2 };

// DY

inline double
refmap_tri_dy_f0(double x, double y)
{
    return lambda1y_2d(x, y);
}

inline double
refmap_tri_dy_f1(double x, double y)
{
    return lambda2y_2d(x, y);
}

inline double
refmap_tri_dy_f2(double x, double y)
{
    return lambda0y_2d(x, y);
}

static shape_fn_2d_t refmap_tri_dy[] = { refmap_tri_dy_f0, refmap_tri_dy_f1, refmap_tri_dy_f2 };

static int refmap_tri_vertex_indices[3] = { 0, 1, 2 };

static shape_fn_2d_t * refmap_tri_fn_table[] = { refmap_tri_fn };
static shape_fn_2d_t * refmap_tri_dx_table[] = { refmap_tri_dx };
static shape_fn_2d_t * refmap_tri_dy_table[] = { refmap_tri_dy };

RefMapShapesetTri::RefMapShapesetTri() : Shapeset2D()
{
    this->num_components = 1;
    this->shape_table[FN] = refmap_tri_fn_table;
    this->shape_table[DX] = refmap_tri_dx_table;
    this->shape_table[DY] = refmap_tri_dy_table;
    this->vertex_indices = refmap_tri_vertex_indices;
}

// RefMapShapesetQuad

// FN

static double
refmap_quad_f0(double x, double y)
{
    return l0(x) * l0(y);
}

static double
refmap_quad_f1(double x, double y)
{
    return l1(x) * l0(y);
}

static double
refmap_quad_f2(double x, double y)
{
    return l1(x) * l1(y);
}

static double
refmap_quad_f3(double x, double y)
{
    return l0(x) * l1(y);
}

static shape_fn_2d_t refmap_quad_fn[] = { refmap_quad_f0,
                                          refmap_quad_f1,
                                          refmap_quad_f2,
                                          refmap_quad_f3 };

// DX

static double
refmap_quad_dx_f0(double x, double y)
{
    return dl0(x) * l0(y);
}

static double
refmap_quad_dx_f1(double x, double y)
{
    return dl1(x) * l0(y);
}

static double
refmap_quad_dx_f2(double x, double y)
{
    return dl1(x) * l1(y);
}

static double
refmap_quad_dx_f3(double x, double y)
{
    return dl0(x) * l1(y);
}

static shape_fn_2d_t refmap_quad_dx[] = { refmap_quad_dx_f0,
                                          refmap_quad_dx_f1,
                                          refmap_quad_dx_f2,
                                          refmap_quad_dx_f3 };

// DY

static double
refmap_quad_dy_f0(double x, double y)
{
    return l0(x) * dl0(y);
}

static double
refmap_quad_dy_f1(double x, double y)
{
    return l1(x) * dl0(y);
}

static double
refmap_quad_dy_f2(double x, double y)
{
    return l1(x) * dl1(y);
}

static double
refmap_quad_dy_f3(double x, double y)
{
    return l0(x) * dl1(y);
}

static shape_fn_2d_t refmap_quad_dy[] = { refmap_quad_dy_f0,
                                          refmap_quad_dy_f1,
                                          refmap_quad_dy_f2,
                                          refmap_quad_dy_f3 };

//

static int refmap_quad_vertex_indices[] = { 0, 1, 2, 3 };

static shape_fn_2d_t * refmap_quad_fn_table[] = { refmap_quad_fn };
static shape_fn_2d_t * refmap_quad_dx_table[] = { refmap_quad_dx };
static shape_fn_2d_t * refmap_quad_dy_table[] = { refmap_quad_dy };

RefMapShapesetQuad::RefMapShapesetQuad() : Shapeset2D()
{
    this->num_components = 1;
    this->shape_table[FN] = refmap_quad_fn_table;
    this->shape_table[DX] = refmap_quad_dx_table;
    this->shape_table[DY] = refmap_quad_dy_table;
    this->vertex_indices = refmap_quad_vertex_indices;
}

// RefMapShapesetTetra

// FN

static double
refmap_tetra_f0(double x, double y, double z)
{
    return lambda1(x, y, z);
}

static double
refmap_tetra_f1(double x, double y, double z)
{
    return lambda2(x, y, z);
}

static double
refmap_tetra_f2(double x, double y, double z)
{
    return lambda0(x, y, z);
}

static double
refmap_tetra_f3(double x, double y, double z)
{
    return lambda3(x, y, z);
}

static shape_fn_3d_t refmap_tetra_fn[] = { refmap_tetra_f0,
                                           refmap_tetra_f1,
                                           refmap_tetra_f2,
                                           refmap_tetra_f3 };

// DX

double
refmap_tetra_dx_f0(double x, double y, double z)
{
    return lambda1dx(x, y, z);
}

double
refmap_tetra_dx_f1(double x, double y, double z)
{
    return lambda2dx(x, y, z);
}

double
refmap_tetra_dx_f2(double x, double y, double z)
{
    return lambda0dx(x, y, z);
}

double
refmap_tetra_dx_f3(double x, double y, double z)
{
    return lambda3dx(x, y, z);
}

static shape_fn_3d_t refmap_tetra_dx[] = { refmap_tetra_dx_f0,
                                           refmap_tetra_dx_f1,
                                           refmap_tetra_dx_f2,
                                           refmap_tetra_dx_f3 };

// DY

double
refmap_tetra_dy_f0(double x, double y, double z)
{
    return lambda1dy(x, y, z);
}

double
refmap_tetra_dy_f1(double x, double y, double z)
{
    return lambda2dy(x, y, z);
}

double
refmap_tetra_dy_f2(double x, double y, double z)
{
    return lambda0dy(x, y, z);
}

double
refmap_tetra_dy_f3(double x, double y, double z)
{
    return lambda3dy(x, y, z);
}

static shape_fn_3d_t refmap_tetra_dy[] = { refmap_tetra_dy_f0,
                                           refmap_tetra_dy_f1,
                                           refmap_tetra_dy_f2,
                                           refmap_tetra_dy_f3 };

// DZ

double
refmap_tetra_dz_f0(double x, double y, double z)
{
    return lambda1dz(x, y, z);
}

double
refmap_tetra_dz_f1(double x, double y, double z)
{
    return lambda2dz(x, y, z);
}

double
refmap_tetra_dz_f2(double x, double y, double z)
{
    return lambda0dz(x, y, z);
}

double
refmap_tetra_dz_f3(double x, double y, double z)
{
    return lambda3dz(x, y, z);
}

static shape_fn_3d_t refmap_tetra_dz[] = { refmap_tetra_dz_f0,
                                           refmap_tetra_dz_f1,
                                           refmap_tetra_dz_f2,
                                           refmap_tetra_dz_f3 };

//

static int refmap_tetra_vertex_indices[] = { 0, 1, 2, 3 };

static shape_fn_3d_t * refmap_tetra_fn_table[] = { refmap_tetra_fn };
static shape_fn_3d_t * refmap_tetra_dx_table[] = { refmap_tetra_dx };
static shape_fn_3d_t * refmap_tetra_dy_table[] = { refmap_tetra_dy };
static shape_fn_3d_t * refmap_tetra_dz_table[] = { refmap_tetra_dz };

//

RefMapShapesetTetra::RefMapShapesetTetra() : Shapeset3D()
{
    this->num_components = 1;
    this->shape_table[FN] = refmap_tetra_fn_table;
    this->shape_table[DX] = refmap_tetra_dx_table;
    this->shape_table[DY] = refmap_tetra_dy_table;
    this->shape_table[DZ] = refmap_tetra_dz_table;
    this->vertex_indices = refmap_tetra_vertex_indices;
}

// RefMapShapesetHex

// FN

static double
refmap_hex_f0(double x, double y, double z)
{
    return l0(x) * l0(y) * l0(z);
}

static double
refmap_hex_f1(double x, double y, double z)
{
    return l1(x) * l0(y) * l0(z);
}

static double
refmap_hex_f2(double x, double y, double z)
{
    return l1(x) * l1(y) * l0(z);
}

static double
refmap_hex_f3(double x, double y, double z)
{
    return l0(x) * l1(y) * l0(z);
}

static double
refmap_hex_f4(double x, double y, double z)
{
    return l0(x) * l0(y) * l1(z);
}

static double
refmap_hex_f5(double x, double y, double z)
{
    return l1(x) * l0(y) * l1(z);
}

static double
refmap_hex_f6(double x, double y, double z)
{
    return l1(x) * l1(y) * l1(z);
}

static double
refmap_hex_f7(double x, double y, double z)
{
    return l0(x) * l1(y) * l1(z);
}

static shape_fn_3d_t refmap_hex_fn[] = {
    refmap_hex_f0, refmap_hex_f1, refmap_hex_f2, refmap_hex_f3,
    refmap_hex_f4, refmap_hex_f5, refmap_hex_f6, refmap_hex_f7
};

// DX

static double
refmap_hex_dx_f0(double x, double y, double z)
{
    return dl0(x) * l0(y) * l0(z);
}

static double
refmap_hex_dx_f1(double x, double y, double z)
{
    return dl1(x) * l0(y) * l0(z);
}

static double
refmap_hex_dx_f2(double x, double y, double z)
{
    return dl1(x) * l1(y) * l0(z);
}

static double
refmap_hex_dx_f3(double x, double y, double z)
{
    return dl0(x) * l1(y) * l0(z);
}

static double
refmap_hex_dx_f4(double x, double y, double z)
{
    return dl0(x) * l0(y) * l1(z);
}

static double
refmap_hex_dx_f5(double x, double y, double z)
{
    return dl1(x) * l0(y) * l1(z);
}

static double
refmap_hex_dx_f6(double x, double y, double z)
{
    return dl1(x) * l1(y) * l1(z);
}

static double
refmap_hex_dx_f7(double x, double y, double z)
{
    return dl0(x) * l1(y) * l1(z);
}

static shape_fn_3d_t refmap_hex_dx[] = { refmap_hex_dx_f0, refmap_hex_dx_f1, refmap_hex_dx_f2,
                                         refmap_hex_dx_f3, refmap_hex_dx_f4, refmap_hex_dx_f5,
                                         refmap_hex_dx_f6, refmap_hex_dx_f7 };

// DY

static double
refmap_hex_dy_f0(double x, double y, double z)
{
    return l0(x) * dl0(y) * l0(z);
}

static double
refmap_hex_dy_f1(double x, double y, double z)
{
    return l1(x) * dl0(y) * l0(z);
}

static double
refmap_hex_dy_f2(double x, double y, double z)
{
    return l1(x) * dl1(y) * l0(z);
}

static double
refmap_hex_dy_f3(double x, double y, double z)
{
    return l0(x) * dl1(y) * l0(z);
}

static double
refmap_hex_dy_f4(double x, double y, double z)
{
    return l0(x) * dl0(y) * l1(z);
}

static double
refmap_hex_dy_f5(double x, double y, double z)
{
    return l1(x) * dl0(y) * l1(z);
}

static double
refmap_hex_dy_f6(double x, double y, double z)
{
    return l1(x) * dl1(y) * l1(z);
}

static double
refmap_hex_dy_f7(double x, double y, double z)
{
    return l0(x) * dl1(y) * l1(z);
}

static shape_fn_3d_t refmap_hex_dy[] = { refmap_hex_dy_f0, refmap_hex_dy_f1, refmap_hex_dy_f2,
                                         refmap_hex_dy_f3, refmap_hex_dy_f4, refmap_hex_dy_f5,
                                         refmap_hex_dy_f6, refmap_hex_dy_f7 };

// DZ

static double
refmap_hex_dz_f0(double x, double y, double z)
{
    return l0(x) * l0(y) * dl0(z);
}

static double
refmap_hex_dz_f1(double x, double y, double z)
{
    return l1(x) * l0(y) * dl0(z);
}

static double
refmap_hex_dz_f2(double x, double y, double z)
{
    return l1(x) * l1(y) * dl0(z);
}

static double
refmap_hex_dz_f3(double x, double y, double z)
{
    return l0(x) * l1(y) * dl0(z);
}

static double
refmap_hex_dz_f4(double x, double y, double z)
{
    return l0(x) * l0(y) * dl1(z);
}

static double
refmap_hex_dz_f5(double x, double y, double z)
{
    return l1(x) * l0(y) * dl1(z);
}

static double
refmap_hex_dz_f6(double x, double y, double z)
{
    return l1(x) * l1(y) * dl1(z);
}

static double
refmap_hex_dz_f7(double x, double y, double z)
{
    return l0(x) * l1(y) * dl1(z);
}

static shape_fn_3d_t refmap_hex_dz[] = { refmap_hex_dz_f0, refmap_hex_dz_f1, refmap_hex_dz_f2,
                                         refmap_hex_dz_f3, refmap_hex_dz_f4, refmap_hex_dz_f5,
                                         refmap_hex_dz_f6, refmap_hex_dz_f7 };

//

static int refmap_hex_vertex_indices[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

static shape_fn_3d_t * refmap_hex_fn_table[] = { refmap_hex_fn };
static shape_fn_3d_t * refmap_hex_dx_table[] = { refmap_hex_dx };
static shape_fn_3d_t * refmap_hex_dy_table[] = { refmap_hex_dy };
static shape_fn_3d_t * refmap_hex_dz_table[] = { refmap_hex_dz };

RefMapShapesetHex::RefMapShapesetHex() : Shapeset3D()
{
    this->num_components = 1;
    this->shape_table[FN] = refmap_hex_fn_table;
    this->shape_table[DX] = refmap_hex_dx_table;
    this->shape_table[DY] = refmap_hex_dy_table;
    this->shape_table[DZ] = refmap_hex_dz_table;
    this->vertex_indices = refmap_hex_vertex_indices;
}

} // namespace fe
