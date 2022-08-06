#include "infowindow.h"
#include <QWidget>
#include <QVBoxLayout>

InfoWindow::InfoWindow(QWidget * parent) : widget(nullptr), layout(nullptr)
{
    setupWidgets();

    this->widget = new QWidget();
    this->widget->setLayout(this->layout);
    this->setWidget(this->widget);
    this->setWindowTitle("Info");
    this->setFixedWidth(300);
    this->setWidgetResizable(true);
    this->setWindowFlag(Qt::Tool);

    show();
}

void
InfoWindow::setupWidgets()
{
    this->layout = new QVBoxLayout();
    this->layout->setContentsMargins(20, 10, 20, 10);
    this->layout->setSpacing(8);

    // this->color_picker = new ColorPicker(self);
    // this->color_picker.colorChanged.connect(self.onBlockColorPicked);

    // this->setupBlocksWidgets();
    // this->layout.addWidget(HLine());
    // this->setupSidesetsWidgets();
    // this->layout.addWidget(HLine());
    // this->setupNodesetsWidgets();
    // this->layout.addWidget(HLine());
    // this->setupSummaryWidgets();
    // this->layout.addWidget(HLine());
    // this->setupRangeWidgets();
    // this->layout.addWidget(HLine());

    this->layout->addStretch();
}
