#include "refmap1d.h"
#include "quadrature1d.h"
#include "refmapss.h"
#include "determinant.h"
#include "element.h"

namespace fe {

static RefMapShapesetEdge ref_map_ss_edge;

static ShapeFunction1D ref_map_pss_edge(&ref_map_ss_edge);

RefMap1D::RefMap1D() : pss(nullptr), n_coefs(0), coefs(nullptr) {}

void
RefMap1D::set_element(Element * e)
{
    switch (e->get_type()) {
    case 3:
        this->pss = &ref_map_pss_edge;
        break;
    default:
        throw std::logic_error("Not implemented");
        break;
    }
    this->pss->set_active_element(e);

    int n_vertices = e->get_num_vertices();

    Shapeset1D * shapeset = this->pss->get_shapeset();
    for (int i = 0, k = 0; i < n_vertices; i++)
        this->indices[k++] = shapeset->get_vertex_index(i);
    for (int iv = 0; iv < n_vertices; iv++) {
        double * coords = e->get_vertex_coords(iv);
        this->vertex[iv].x = coords[0];
    }
    this->coefs = this->vertex;
    this->n_coefs = n_vertices;
}

double1x1 *
RefMap1D::get_ref_map(const int np, const QuadPt1D * pt)
{
    double1x1 * m = new double1x1[np];
    MEM_CHECK(m);
    memset(m, 0, np * sizeof(double1x1));
    for (int i = 0; i < this->n_coefs; i++) {
        double *dx;
        pss->set_active_shape(indices[i]);
        pss->precalculate(np, pt, FN_VAL_1D | FN_DX_1D);
        pss->get_dx_values(dx);
        for (int j = 0; j < np; j++) {
            m[j][0][0] += coefs[i].x * dx[j];
        }
    }
    return m;
}

double *
RefMap1D::get_jacobian(const int np, const QuadPt1D * pt, bool trans)
{
    double * jac = new double[np];
    MEM_CHECK(jac);
    double1x1 * m = get_ref_map(np, pt);
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
