#include "quadrature3d.h"

namespace fe {

Quadrature3D::Quadrature3D() : max_order(-1) {}

//

static QuadPt3D std_pts_0_1_3d_tet[] = { QuadPt3D(-1.0 / 2, -1.0 / 2, -1.0 / 2, 4.0 / 3) };

static QuadPt3D std_pts_2_3d_tet[] = {
    QuadPt3D(-0.7236067977499790, -0.7236067977499790, -0.7236067977499790, 0.3333333333333330),
    QuadPt3D(0.1708203932499370, -0.7236067977499790, -0.7236067977499790, 0.3333333333333330),
    QuadPt3D(-0.7236067977499790, 0.1708203932499370, -0.7236067977499790, 0.3333333333333330),
    QuadPt3D(-0.7236067977499790, -0.7236067977499790, 0.1708203932499370, 0.3333333333333330)
};

static QuadPt3D std_pts_3_3d_tet[] = {
    QuadPt3D(-0.5000000000000000, -0.5000000000000000, -0.5000000000000000, -1.0666666666666671),
    QuadPt3D(-0.6666666666666670, -0.6666666666666670, -0.6666666666666670, 0.6000000000000000),
    QuadPt3D(-0.6666666666666670, -0.6666666666666670, 0.0000000000000000, 0.6000000000000000),
    QuadPt3D(-0.6666666666666670, 0.0000000000000000, -0.6666666666666670, 0.6000000000000000),
    QuadPt3D(0.0000000000000000, -0.6666666666666670, -0.6666666666666670, 0.6000000000000000)
};

static int std_np_3d_tet[] = { sizeof(std_pts_0_1_3d_tet) / sizeof(QuadPt3D),
                               sizeof(std_pts_0_1_3d_tet) / sizeof(QuadPt3D),
                               sizeof(std_pts_2_3d_tet) / sizeof(QuadPt3D),
                               sizeof(std_pts_3_3d_tet) / sizeof(QuadPt3D) };

static QuadPt3D * std_tables_3d_tet[] = { std_pts_0_1_3d_tet,
                                          std_pts_0_1_3d_tet,
                                          std_pts_2_3d_tet,
                                          std_pts_3_3d_tet };

QuadStdTetra::QuadStdTetra() : Quadrature3D()
{
    this->max_order = 3;
    this->np = std_np_3d_tet;
    this->tables = std_tables_3d_tet;
}

//

static QuadPt3D std_pts_0_1_3d_hex[] = { QuadPt3D(0., 0., 0., 8.0) };

static QuadPt3D std_pts_2_3_3d_hex[] = {
    QuadPt3D(-0.57735026918963, -0.57735026918963, -0.57735026918963, 1.),
    QuadPt3D(0.57735026918963, -0.57735026918963, -0.57735026918963, 1),
    QuadPt3D(-0.57735026918963, 0.57735026918963, -0.57735026918963, 1),
    QuadPt3D(0.57735026918963, 0.57735026918963, -0.57735026918963, 1),
    QuadPt3D(-0.57735026918963, -0.57735026918963, 0.57735026918963, 1.),
    QuadPt3D(0.57735026918963, -0.57735026918963, 0.57735026918963, 1),
    QuadPt3D(-0.57735026918963, 0.57735026918963, 0.57735026918963, 1),
    QuadPt3D(0.57735026918963, 0.57735026918963, 0.57735026918963, 1),
};

static int std_np_3d_hex[] = { sizeof(std_pts_0_1_3d_hex) / sizeof(QuadPt3D),
                               sizeof(std_pts_0_1_3d_hex) / sizeof(QuadPt3D),
                               sizeof(std_pts_2_3_3d_hex) / sizeof(QuadPt3D),
                               sizeof(std_pts_2_3_3d_hex) / sizeof(QuadPt3D) };

static QuadPt3D * std_tables_3d_hex[] = { std_pts_0_1_3d_hex,
                                          std_pts_0_1_3d_hex,
                                          std_pts_2_3_3d_hex,
                                          std_pts_2_3_3d_hex };

QuadStdHex::QuadStdHex() : Quadrature3D()
{
    this->max_order = 3;
    this->np = std_np_3d_hex;
    this->tables = std_tables_3d_hex;
}

} // namespace fe
