#include "refmap2d.h"
#include "quadrature2d.h"
#include "refmapss.h"
#include "determinant.h"
#include "element.h"

namespace fe {

static RefMapShapesetTri ref_map_ss_tri;
static RefMapShapesetQuad ref_map_ss_quad;

static ShapeFunction2D ref_map_pss_tri(&ref_map_ss_tri);
static ShapeFunction2D ref_map_pss_quad(&ref_map_ss_quad);

RefMap2D::RefMap2D() {}

void
RefMap2D::set_element(Element * e)
{
    switch (e->get_type()) {
    case 5: this->pss = &ref_map_pss_tri; break;
    case 9: this->pss = &ref_map_pss_quad; break;
    default: throw std::logic_error("Not implemented"); break;
    }
    this->pss->set_active_element(e);

    if (e == this->elem) return;
    this->elem = e;

    int n_vertices = this->elem->get_num_vertices();

    Shapeset2D *shapeset = this->pss->get_shapeset();
    for (int i = 0, k = 0; i < n_vertices; i++)
        this->indices[k++] = shapeset->get_vertex_index(i);
    for (int iv = 0; iv < n_vertices; iv++) {
        double * coords = e->get_vertex_coords(iv);
        this->vertex[iv].x = coords[0];
        this->vertex[iv].y = coords[1];
    }
    this->coefs = this->vertex;
    this->n_coefs = n_vertices;
}

double2x2 *
RefMap2D::get_ref_map(const int np, const QuadPt2D * pt)
{
    double2x2 * m = new double2x2[np];
    MEM_CHECK(m);
    memset(m, 0, np * sizeof(double2x2));
    for (int i = 0; i < this->n_coefs; i++) {
        double *dx, *dy;
        pss->set_active_shape(indices[i]);
        pss->precalculate(np, pt, FN_VAL_2D | FN_DX_2D | FN_DY_2D);
        pss->get_dx_dy_values(dx, dy);
        for (int j = 0; j < np; j++) {
            m[j][0][0] += coefs[i].x * dx[j];
            m[j][0][1] += coefs[i].x * dy[j];
            m[j][1][0] += coefs[i].y * dx[j];
            m[j][1][1] += coefs[i].y * dy[j];
        }
    }
    return m;
}

double *
RefMap2D::get_jacobian(const int np, const QuadPt2D * pt, bool trans)
{
    double * jac = new double[np];
    MEM_CHECK(jac);
    double2x2 * m = get_ref_map(np, pt);
    if (trans)
        for (int i = 0; i < np; i++)
            jac[i] = det(m[i]) * pt[i].w;
    else
        for (int i = 0; i < np; i++)
            jac[i] = det(m[i]);
    delete[] m;
    return jac;
}

} // namespace fe
