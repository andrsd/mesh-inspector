#include "meshqualitytool.h"
#include "meshqualitywidget.h"
#include "mainwindow.h"
#include "model.h"
#include "view.h"
#include "colorprofile.h"
#include "blockobject.h"
#include "vtkLookupTable.h"
#include "vtkScalarBarActor.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkTextProperty.h"
#include "vtkCellData.h"
#include "vtkCellQuality.h"
#include "vtkUnstructuredGrid.h"
#include "vtkMapper.h"
#include "vtkRenderer.h"

const char * MeshQualityTool::MESH_QUALITY_FIELD_NAME = "CellQuality";

MeshQualityTool::MeshQualityTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    model(main_wnd->getModel()),
    view(main_wnd->getView()),
    mesh_quality(nullptr),
    lut(nullptr),
    color_bar(nullptr)
{
}

MeshQualityTool::~MeshQualityTool()
{
    delete this->mesh_quality;
    this->lut->Delete();
}

void
MeshQualityTool::setupVtk()
{
    setupLookupTable();
    setupColorBar();
}

void
MeshQualityTool::setupWidgets()
{
    this->mesh_quality = new MeshQualityWidget(this->main_window);
    connect(this->mesh_quality,
            &MeshQualityWidget::metricChanged,
            this,
            &MeshQualityTool::onMetricChanged);
    connect(this->mesh_quality, &MeshQualityWidget::closed, this, &MeshQualityTool::onClose);
    this->mesh_quality->setVisible(false);
}

void
MeshQualityTool::update()
{
    auto renderer = this->view->getRenderer();
    renderer->AddActor2D(this->color_bar);
}

bool
MeshQualityTool::isVisible() const
{
    return this->mesh_quality->isVisible();
}

void
MeshQualityTool::done()
{
    this->mesh_quality->done();
}

void
MeshQualityTool::setColorProfile(ColorProfile * profile)
{
    auto qclr = profile->getColor("color_bar_label");
    auto prop = this->color_bar->GetLabelTextProperty();
    prop->SetColor(qclr.redF(), qclr.greenF(), qclr.blueF());
}

void
MeshQualityTool::onMeshQuality()
{
    this->mesh_quality->adjustSize();
    this->mesh_quality->show();
    updateLocation();

    auto metric_id = this->mesh_quality->getMetricId();
    onMetricChanged(metric_id);

    this->main_window->updateMenuBar();
    this->color_bar->VisibilityOn();
}

void
MeshQualityTool::updateLocation()
{
    auto width = this->main_window->getRenderWindowWidth();
    int left = (width - this->mesh_quality->width()) / 2;
    int top = this->main_window->geometry().height() - this->mesh_quality->height() - 10;
    this->mesh_quality->move(left, top);
}

void
MeshQualityTool::onMetricChanged(int metric_id)
{
    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;

        auto grid = block->getUnstructuredGrid();
        auto cell_quality = vtkCellQuality::New();
        switch (metric_id) {
        default:
        case MESH_METRIC_JACOBIAN:
            cell_quality->SetQualityMeasureToJacobian();
            break;
        case MESH_METRIC_AREA:
            cell_quality->SetQualityMeasureToArea();
            break;
        case MESH_METRIC_VOLUME:
            cell_quality->SetQualityMeasureToVolume();
            break;
        case MESH_METRIC_ASPECT_RATIO:
            cell_quality->SetQualityMeasureToAspectRatio();
            break;
        case MESH_METRIC_CONDITION:
            cell_quality->SetQualityMeasureToCondition();
            break;
        }
        cell_quality->SetInputData(grid);
        cell_quality->Update();
        auto out = cell_quality->GetOutput();
        grid->GetCellData()->AddArray(out->GetCellData()->GetArray("CellQuality"));
        cell_quality->Delete();
    }

    double range[2];
    getCellQualityRange(range);

    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;
        setBlockMeshQualityProperties(block, range);
        block->modified();
        block->update();
    }
}

void
MeshQualityTool::getCellQualityRange(double range[])
{
    range[0] = std::numeric_limits<double>::max();
    range[1] = -std::numeric_limits<double>::max();
    for (auto & it : this->model->getBlocks()) {
        auto * block = it.second;
        auto cell_data = block->getCellData();

        double block_range[2];
        cell_data->GetRange(MESH_QUALITY_FIELD_NAME, block_range);
        range[0] = std::min(range[0], block_range[0]);
        range[1] = std::max(range[1], block_range[1]);
    }
}

void
MeshQualityTool::onClose()
{
    for (auto & it : this->model->getBlocks()) {
        BlockObject * block = it.second;
        auto * mapper = block->getMapper();
        mapper->ScalarVisibilityOff();
        block->update();
    }

    this->view->activateRenderMode();
    this->main_window->updateMenuBar();
    this->color_bar->VisibilityOff();
}

void
MeshQualityTool::setupLookupTable()
{
    // rainbow uniform color map by Colin Ware
    double colors[] = { 0.02,
                        0.38129999999999997,
                        0.99809999999999999,
                        1.,
                        0.02000006,
                        0.42426776799999999,
                        0.96906968999999998,
                        1.,
                        0.02,
                        0.46723376300000002,
                        0.94003304300000001,
                        1.,
                        0.02,
                        0.51019999999999999,
                        0.91100000000000003,
                        1.,
                        0.02000006,
                        0.54640149400000004,
                        0.87266943799999996,
                        1.,
                        0.02,
                        0.58260036199999998,
                        0.83433294999999996,
                        1.,
                        0.02,
                        0.61880000000000002,
                        0.79600000000000004,
                        1.,
                        0.02000006,
                        0.65253515600000001,
                        0.74980243400000002,
                        1.,
                        0.02,
                        0.68626700399999996,
                        0.70359953799999997,
                        1.,
                        0.02,
                        0.71999999999999997,
                        0.65739999999999998,
                        1.,
                        0.02000006,
                        0.757035456,
                        0.60373535899999997,
                        1.,
                        0.02,
                        0.79406703700000003,
                        0.55006613000000004,
                        1.,
                        0.02,
                        0.83109999999999995,
                        0.49640000000000001,
                        1.,
                        0.021354336738172372,
                        0.86453685552616311,
                        0.42855794607611591,
                        1.,
                        0.023312914349117714,
                        0.89799935992448399,
                        0.36073871343115577,
                        1.,
                        0.015976108242848862,
                        0.9310479513349017,
                        0.29256318150880922,
                        1.,
                        0.27421074700988196,
                        0.95256296099508297,
                        0.15356836602739213,
                        1.,
                        0.49335462816816988,
                        0.96190386253094817,
                        0.11119493614749336,
                        1.,
                        0.64390000000000003,
                        0.97729999999999995,
                        0.046899999999999997,
                        1.,
                        0.76240181299999998,
                        0.98466959099999996,
                        0.034600153000000002,
                        1.,
                        0.88090118500000003,
                        0.99203340699999998,
                        0.022299876999999999,
                        1.,
                        0.99952854326271467,
                        0.99951937067814922,
                        0.0134884641450013,
                        1.,
                        0.99940299799999999,
                        0.95503637600000002,
                        0.079066628,
                        1.,
                        0.99939999999999996,
                        0.910666223,
                        0.148134024,
                        1.,
                        0.99939999999999996,
                        0.86629999999999996,
                        0.2172,
                        1.,
                        0.99926966500000003,
                        0.81803598099999997,
                        0.21720065199999999,
                        1.,
                        0.99913333199999999,
                        0.76976618399999996,
                        0.2172,
                        1.,
                        0.999,
                        0.72150000000000003,
                        0.2172,
                        1.,
                        0.99913633000000002,
                        0.673435546,
                        0.21720065199999999,
                        1.,
                        0.99926666799999997,
                        0.62536618600000005,
                        0.2172,
                        1.,
                        0.99939999999999996,
                        0.57730000000000004,
                        0.2172,
                        1.,
                        0.99940299799999999,
                        0.52106845499999999,
                        0.21720065199999999,
                        1.,
                        0.99939999999999996,
                        0.46483277099999998,
                        0.2172,
                        1.,
                        0.99939999999999996,
                        0.40860000000000002,
                        0.2172,
                        1.,
                        0.99475999176873464,
                        0.33177297300202935,
                        0.21123096385202059,
                        1.,
                        0.98671295054795893,
                        0.25951834109149341,
                        0.19012239549291934,
                        1.,
                        0.99124588756464194,
                        0.14799417507952672,
                        0.21078892136920357,
                        1.,
                        0.94990303700000001,
                        0.11686717100000001,
                        0.252900603,
                        1.,
                        0.903199533,
                        0.078432949000000002,
                        0.29180038899999999,
                        1.,
                        0.85650000000000004,
                        0.040000000000000001,
                        0.33069999999999999,
                        1.,
                        0.79890262700000003,
                        0.043333450000000003,
                        0.35843429799999998,
                        1.,
                        0.74129942400000004,
                        0.046666699999999998,
                        0.38616694400000001,
                        1.,
                        0.68369999999999997,
                        0.050000000000000003,
                        0.41389999999999999,
                        1. };

    this->lut = vtkLookupTable::New();
    this->lut->SetNumberOfTableValues(43);
    double * clr_ptr = colors;
    for (int i = 0; i < 43; i++, clr_ptr += 4)
        this->lut->SetTableValue(i, clr_ptr);

    this->lut->Build();
}

void
MeshQualityTool::setupColorBar()
{
    this->color_bar = vtkScalarBarActor::New();
    this->color_bar->VisibilityOff();
    this->color_bar->SetNumberOfLabels(5);
    this->color_bar->SetLookupTable(this->lut);
    this->color_bar->SetBarRatio(0.2);
    this->color_bar->SetHeight(0.5);
    this->color_bar->SetWidth(0.08);
    this->color_bar->SetMaximumNumberOfColors(16);
    this->color_bar->SetPosition(0.9, 0.3);
    this->color_bar->SetLabelFormat("    %-#6.3g");
    this->color_bar->UnconstrainedFontSizeOn();

    {
        auto prop = this->color_bar->GetTitleTextProperty();
        prop->BoldOff();
        prop->ItalicOff();
        prop->ShadowOff();
        prop->SetColor(0, 0, 0);
        prop->SetFontFamilyToArial();
        prop->SetFontSize(18);
    }
    {
        auto prop = this->color_bar->GetLabelTextProperty();
        prop->BoldOff();
        prop->ItalicOff();
        prop->ShadowOff();
        prop->SetColor(0, 0, 0);
        prop->SetFontFamilyToArial();
        prop->SetFontSize(17);
    }
    {
        auto prop = this->color_bar->GetFrameProperty();
        prop->SetColor(0, 0, 0);
        prop->SetLineWidth(4);
    }
    {
        auto prop = this->color_bar->GetBackgroundProperty();
        prop->SetOpacity(0.99);
        prop->SetColor(1, 1, 1);
    }
}

void
MeshQualityTool::setBlockMeshQualityProperties(BlockObject * block, double range[])
{
    auto * property = block->getProperty();
    property->SetRepresentationToSurface();
    property->SetAmbient(0.4);
    property->SetDiffuse(0.6);
    property->SetOpacity(1.0);
    property->SetEdgeVisibility(true);
    property->SetEdgeColor(View::SIDESET_EDGE_CLR.redF(),
                           View::SIDESET_EDGE_CLR.greenF(),
                           View::SIDESET_EDGE_CLR.blueF());
    property->SetLineWidth(this->main_window->HIDPI(View::EDGE_WIDTH));

    auto mapper = block->getMapper();
    mapper->ScalarVisibilityOn();
    mapper->SelectColorArray(MESH_QUALITY_FIELD_NAME);
    mapper->SetScalarModeToUseCellFieldData();
    mapper->InterpolateScalarsBeforeMappingOn();
    mapper->SetColorModeToMapScalars();
    mapper->SetScalarRange(range);
    mapper->SetLookupTable(this->lut);
}
