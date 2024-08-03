// SPDX-FileCopyrightText: 2022 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "exporttool.h"
#include "mainwindow.h"
#include "view.h"
#include <QMenu>
#include <QFileDialog>
#include <QFileInfo>
#include "vtkGenericOpenGLRenderWindow.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkGL2PSExporter.h"

ExportTool::ExportTool(MainWindow * main_window) :
    main_window(main_window),
    export_as_png(nullptr),
    export_as_jpg(nullptr),
    export_as_pdf(nullptr)
{
}

void
ExportTool::setupMenu(QMenu * menu)
{
    this->export_as_png = menu->addAction("PNG...", this, &ExportTool::onExportAsPng);
    this->export_as_jpg = menu->addAction("JPG...", this, &ExportTool::onExportAsJpg);
    this->export_as_pdf = menu->addAction("PDF...", this, &ExportTool::onExportAsPdf);
}

void
ExportTool::setMenuEnabled(bool enabled)
{
    this->export_as_png->setEnabled(enabled);
    this->export_as_jpg->setEnabled(enabled);
    this->export_as_pdf->setEnabled(enabled);
}

QString
ExportTool::getFileName(const QString & window_title,
                        const QString & name_filter,
                        const QString & default_suffix)
{
    QFileDialog dialog;
    dialog.setWindowTitle(window_title);
    dialog.setNameFilter(name_filter);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(default_suffix);

    if (dialog.exec() == QDialog::Accepted)
        return { dialog.selectedFiles()[0] };

    return {};
}

void
ExportTool::onExportAsPng()
{
    auto fname = getFileName("Export to PNG", "PNG files (*.png)", "png");
    if (!fname.isNull()) {
        auto view = this->main_window->getView();
        auto * windowToImageFilter = vtkWindowToImageFilter::New();
        windowToImageFilter->SetInput(view->getRenderWindow());
        windowToImageFilter->SetInputBufferTypeToRGBA();
        windowToImageFilter->ReadFrontBufferOff();
        windowToImageFilter->Update();

        auto * writer = vtkPNGWriter::New();
        writer->SetFileName(fname.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();

        if (writer->GetErrorCode() == 0) {
            QFileInfo fi(fname);
            this->main_window->showNotification(
                QString("Export to '%1' was successful.").arg(fi.fileName()));
        }
    }
}

void
ExportTool::onExportAsJpg()
{
    auto fname = getFileName("Export to JPG", "JPG files (*.jpg)", "jpg");
    if (!fname.isNull()) {
        auto view = this->main_window->getView();
        auto * windowToImageFilter = vtkWindowToImageFilter::New();
        windowToImageFilter->SetInput(view->getRenderWindow());
        windowToImageFilter->ReadFrontBufferOff();
        windowToImageFilter->Update();

        auto * writer = vtkJPEGWriter::New();
        writer->SetFileName(fname.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();

        if (writer->GetErrorCode() == 0) {
            QFileInfo fi(fname);
            this->main_window->showNotification(
                QString("Export to '%1' was successful.").arg(fi.fileName()));
        }
    }
}

void
ExportTool::onExportAsPdf()
{
    auto fname = getFileName("Export to PDF", "PDF files (*.pdf)", "pdf");
    if (!fname.isNull()) {
        QFileInfo fi(fname);
        auto path = QDir(fi.absoluteDir());
        auto base_name = fi.baseName();
        auto file_prefix = QFileInfo(path, base_name).absoluteFilePath();

        auto view = this->main_window->getView();
        auto * writer = vtkGL2PSExporter::New();
        writer->SetFileFormatToPDF();
        writer->DrawBackgroundOff();
        writer->SetSortToBSP();
        writer->SetCompress(true);
        writer->SetInput(view->getRenderWindow());
        writer->SetFilePrefix(file_prefix.toStdString().c_str());
        writer->Write();
    }
}
