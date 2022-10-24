#include "refmap3d.h"
#include "quadrature3d.h"
#include "refmapss.h"
#include "determinant.h"
#include "element.h"

namespace fe {

static RefMapShapesetTetra ref_map_ss_tetra;
static RefMapShapesetHex ref_map_ss_hex;

static ShapeFunction3D ref_map_pss_tetra(&ref_map_ss_tetra);
static ShapeFunction3D ref_map_pss_hex(&ref_map_ss_hex);

RefMap3D::RefMap3D() :
    pss(nullptr),
    elem(nullptr),
    n_coefs(0),
    coefs(nullptr)
{
}

void
RefMap3D::set_element(Element * e)
{
    switch (e->get_type()) {
    case 10: this->pss = &ref_map_pss_tetra; break;
    case 12: this->pss = &ref_map_pss_hex; break;
    default: throw std::logic_error("Not implemented"); break;
    }
    this->pss->set_active_element(e);

    if (e == this->elem) return;
    this->elem = e;

    int n_vertices = this->elem->get_num_vertices();

    // prepare the shapes and coefficients of the reference map
    Shapeset3D *shapeset = this->pss->get_shapeset();
    int i, k = 0;
    for (i = 0; i < n_vertices; i++)
        this->indices[k++] = shapeset->get_vertex_index(i);
    for (int iv = 0; iv < n_vertices; iv++) {
        double * coords = e->get_vertex_coords(iv);
        this->vertex[iv].x = coords[0];
        this->vertex[iv].y = coords[1];
        this->vertex[iv].z = coords[2];

    }
    this->coefs = this->vertex;
    this->n_coefs = n_vertices;
}

double3x3 *
RefMap3D::get_ref_map(const int np, const QuadPt3D * pt)
{
    double3x3 * m = new double3x3[np];
    MEM_CHECK(m);
    memset(m, 0, np * sizeof(double3x3));
    for (int i = 0; i < this->n_coefs; i++) {
        double *dx, *dy, *dz;
        pss->set_active_shape(indices[i]);
        pss->precalculate(np, pt, FN_VAL_3D | FN_DX_3D | FN_DY_3D | FN_DZ_3D);
        pss->get_dx_dy_dz_values(dx, dy, dz);
        for (int j = 0; j < np; j++) {
            m[j][0][0] += coefs[i].x * dx[j];
            m[j][0][1] += coefs[i].x * dy[j];
            m[j][0][2] += coefs[i].x * dz[j];
            m[j][1][0] += coefs[i].y * dx[j];
            m[j][1][1] += coefs[i].y * dy[j];
            m[j][1][2] += coefs[i].y * dz[j];
            m[j][2][0] += coefs[i].z * dx[j];
            m[j][2][1] += coefs[i].z * dy[j];
            m[j][2][2] += coefs[i].z * dz[j];
        }
    }
    return m;
}

double *
RefMap3D::get_jacobian(const int np, const QuadPt3D * pt, bool trans)
{
    double * jac = new double[np];
    MEM_CHECK(jac);
    double3x3 * m = get_ref_map(np, pt);
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
