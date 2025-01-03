#include <QtTest/QtTest>
#include "colorprofile.h"

class ColorProfileTest : public QObject {
    Q_OBJECT

private slots:
    void
    testColorProfile()
    {
        ColorProfile profile("cp", { { "a", QColor(1, 2, 3) }, { "b", QColor(4, 5, 6) } });
        QCOMPARE(profile.getName(), QString("cp"));
        QCOMPARE(profile.getColor("a"), QColor(1, 2, 3));
        QCOMPARE(profile.getColor("b"), QColor(4, 5, 6));
    }
};

QTEST_MAIN(ColorProfileTest)

#include "ColorProfile_test.moc"
