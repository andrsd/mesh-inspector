#include "quadrature2d.h"

namespace fe {

Quadrature2D::Quadrature2D() : max_order(-1) {}

//

static QuadPt2D std_pts_0_2d_tri[] = { QuadPt2D(-1.0 / 3, -1.0 / 3, 2.0) };

static QuadPt2D std_pts_1_2d_tri[] = { QuadPt2D(-1.0 / 3, -1.0 / 3, 2.0) };

static QuadPt2D std_pts_2_2d_tri[] = { QuadPt2D(-2.0 / 3, -2.0 / 3, 2.0 / 3),
                                       QuadPt2D(-2.0 / 3, 1.0 / 3, 2.0 / 3),
                                       QuadPt2D(1.0 / 3, -2.0 / 3, 2.0 / 3) };

static QuadPt2D std_pts_3_2d_tri[] = { QuadPt2D(-1.0 / 3, -1.0 / 3, -1.125),
                                       QuadPt2D(-0.6, -0.6, 1.041666666666667),
                                       QuadPt2D(-0.6, 0.2, 1.041666666666667),
                                       QuadPt2D(0.2, -0.6, 1.041666666666667) };

static int std_np_2d_tri[] = { sizeof(std_pts_0_2d_tri) / sizeof(QuadPt2D),
                               sizeof(std_pts_1_2d_tri) / sizeof(QuadPt2D),
                               sizeof(std_pts_2_2d_tri) / sizeof(QuadPt2D),
                               sizeof(std_pts_3_2d_tri) / sizeof(QuadPt2D) };

static QuadPt2D * std_tables_2d_tri[] = { std_pts_0_2d_tri,
                                          std_pts_1_2d_tri,
                                          std_pts_2_2d_tri,
                                          std_pts_3_2d_tri };

QuadStdTri::QuadStdTri() : Quadrature2D()
{
    this->max_order = 3;
    this->np = std_np_2d_tri;
    this->tables = std_tables_2d_tri;
}

//

static QuadPt2D std_pts_0_1_2d_quad[] = { QuadPt2D(0., 0., 4.0) };

static QuadPt2D std_pts_2_3_2d_quad[] = { QuadPt2D(-0.57735026918963, -0.57735026918963, 1.),
                                          QuadPt2D(0.57735026918963, -0.57735026918963, 1),
                                          QuadPt2D(-0.57735026918963, 0.57735026918963, 1),
                                          QuadPt2D(0.57735026918963, 0.57735026918963, 1) };

static int std_np_2d_quad[] = { sizeof(std_pts_0_1_2d_quad) / sizeof(QuadPt2D),
                                sizeof(std_pts_0_1_2d_quad) / sizeof(QuadPt2D),
                                sizeof(std_pts_2_3_2d_quad) / sizeof(QuadPt2D),
                                sizeof(std_pts_2_3_2d_quad) / sizeof(QuadPt2D) };

static QuadPt2D * std_tables_2d_quad[] = { std_pts_0_1_2d_quad,
                                           std_pts_0_1_2d_quad,
                                           std_pts_2_3_2d_quad,
                                           std_pts_2_3_2d_quad };

QuadStdQuad::QuadStdQuad() : Quadrature2D()
{
    this->max_order = 3;
    this->np = std_np_2d_quad;
    this->tables = std_tables_2d_quad;
}

} // namespace fe
