#include "checkforupdatetool.h"
#include "meshinspectorconfig.h"
#include "mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMenu>
#include <QAction>

CheckForUpdateTool::CheckForUpdateTool(MainWindow * main_wnd) :
    main_window(main_wnd),
    namgr(new QNetworkAccessManager()),
    check_update_action(nullptr)
{
    connect(this->namgr, &QNetworkAccessManager::finished, this, &CheckForUpdateTool::onHttpReply);
}

CheckForUpdateTool::~CheckForUpdateTool()
{
    delete this->namgr;
}

void
CheckForUpdateTool::setupMenu(QMenu * menu)
{
    this->check_update_action =
        menu->addAction("Check for update...", this, &CheckForUpdateTool::onCheckForUpdate);
    this->check_update_action->setMenuRole(QAction::ApplicationSpecificRole);
}

void
CheckForUpdateTool::onCheckForUpdate()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.github.com/repos/andrsd/mesh-inspector/releases/latest"));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/vnd.github+json"));
    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Bearer "
                                    "github_pat_11AAAU5AA0uy230CCje7ng_"
                                    "8pj7WNyfPIj8TZZRv7ZSF4RmpULmyRtN0Y9glzoh0bePPT6765WCoolMPhf"));
    this->namgr->get(request);
}

void
CheckForUpdateTool::onHttpReply(QNetworkReply * reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QVersionNumber latest = getVersionFromReply(reply);
        QVersionNumber current(MESH_INSPECTOR_MAJOR_VERSION, MESH_INSPECTOR_MINOR_VERSION);

        if (QVersionNumber::compare(latest, current) == 0)
            this->main_window->showNotification("You are running the latest version.");
        else
            this->main_window->showNotification(QString("A newer version (%1.%2) is available.")
                                                    .arg(latest.majorVersion())
                                                    .arg(latest.minorVersion()));
    }
    else
        this->main_window->showNotification("Check for a new version failed.");
}

QVersionNumber
CheckForUpdateTool::getVersionFromReply(QNetworkReply * reply)
{
    QByteArray val = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val);
    auto tag_name = doc["tag_name"].toString();
    QRegularExpression re("v(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(tag_name);
    if (match.hasMatch())
        return QVersionNumber(match.captured(1).toInt(), match.captured(2).toInt());
    else
        return QVersionNumber(-1);
}
