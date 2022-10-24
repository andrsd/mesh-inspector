#include "quadrature1d.h"

namespace fe {

Quadrature1D::Quadrature1D() : max_order(-1), np(nullptr), tables(nullptr) {}

Quadrature1D::~Quadrature1D() {}

//

static QuadPt1D std_pts_0_1_1d[] = { QuadPt1D(0.0, 2.0) };

static QuadPt1D std_pts_2_3_1d[] = {
    QuadPt1D(-0.57735026918963, 1.0), // (-1.0/sqrt(3.0), 1.0),
    QuadPt1D(0.57735026918963, 1.0) // ( 1.0/sqrt(3.0), 1.0),
};

static QuadPt1D * std_tables_1d[] = { std_pts_0_1_1d,
                                      std_pts_0_1_1d,
                                      std_pts_2_3_1d,
                                      std_pts_2_3_1d };

static int std_np_1d[] = { sizeof(std_pts_0_1_1d) / sizeof(QuadPt1D),
                           sizeof(std_pts_0_1_1d) / sizeof(QuadPt1D),
                           sizeof(std_pts_2_3_1d) / sizeof(QuadPt1D),
                           sizeof(std_pts_2_3_1d) / sizeof(QuadPt1D) };

QuadStd1D::QuadStd1D() : Quadrature1D()
{
    this->max_order = 3;
    this->np = std_np_1d;
    this->tables = std_tables_1d;
}

QuadStd1D::~QuadStd1D() {}

}
