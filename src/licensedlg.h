#pragma once

#include <QDialog>

class QVBoxLayout;
class QTextEdit;

class LicenseDialog : public QDialog {
public:
    LicenseDialog(QWidget * parent = nullptr);
    virtual ~LicenseDialog();

protected:
    QVBoxLayout * layout;
    QTextEdit * license;
};
