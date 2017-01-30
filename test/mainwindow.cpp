#include "mainwindow.h"

#include <QGuiApplication>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QDebug>

#include <QPushButton>
#include <QTimer>
constexpr bool bShowFps = false;
MainWindow::MainWindow()
{
    // Load Xaml
    Noesis::GUI::SetResourceProvider("Data");
    m_root = Noesis::GUI::LoadXaml<Noesis::UserControl>("test.xaml");

    // Init Noesis Widget
    m_NsWidget = new QNoesisWidget(m_root.GetPtr(), this);
    this->initXamlResources();
    this->setCentralWidget(m_NsWidget);

    // Setup buttons for loading test xaml files
    QPushButton* pb = new QPushButton(m_NsWidget);
    pb->setFixedSize(150, 25);
    pb->move(0, bShowFps ? 30 : 0);
    pb->setText("Load Test Xaml 1");
    pb->setVisible(false);


    QPushButton* pb2 = new QPushButton(m_NsWidget);
    pb2->setFixedSize(150, 25);
    pb2->move(0, bShowFps ? 30 : 0);
    pb2->setText("Load Test Xaml 2");

    connect(pb, &QPushButton::clicked, this, [=](){
        pb->setVisible(false);
        pb2->setVisible(true);
        this->loadXaml("test.xaml");
    });

    connect(pb2, &QPushButton::clicked, this, [=](){
        pb->setVisible(true);
        pb2->setVisible(false);
        this->loadXaml("test2.xaml");
    });


    // Setup FPS counter
    if(bShowFps)
    {
        QLabel* fpsLabel = new QLabel(m_NsWidget);
        fpsLabel->setAlignment(Qt::AlignHCenter);
        fpsLabel->setStyleSheet("QLabel { background-color : red; color : yellow; }");
        fpsLabel->hide();

        QFont font = fpsLabel->font();
        font.setPointSize(16);
        font.setBold(true);
        fpsLabel->setFont(font);

        QTimer* t = new QTimer;
        t->setInterval(1000);

        connect(t, &QTimer::timeout, this, [=]()
        {
            qreal fps = m_NsWidget->getRenderer()->fps();
            if(fps == 0.0) return;
            else if(fpsLabel->isHidden()) fpsLabel->show();

            QString textFps(QString("%1 fps").arg(QString::number(fps, '2', 2)));

            QFontMetrics fm(fpsLabel->font());
            fpsLabel->setFixedWidth(fm.width(textFps) + 4);
            fpsLabel->setText(textFps);
        }, Qt::QueuedConnection);

        t->start();
    }
}

void MainWindow::loadXaml(const QString& name)
{
    m_root = Noesis::GUI::LoadXaml<Noesis::UserControl>(name.toUtf8().data());
    m_NsWidget->setView(Noesis::GUI::CreateView(m_root.GetPtr()));

    initXamlResources();
}

void MainWindow::initXamlResources()
{
    // Animmations
    m_animStackNext = m_root->FindResource<Noesis::Storyboard>(Noesis::ResourceKeyString::Create("stackAnimationNext").GetPtr());
    m_animStackBack = m_root->FindResource<Noesis::Storyboard>(Noesis::ResourceKeyString::Create("stackAnimationBack").GetPtr());

    // Text box
    m_textBoxMyIp = m_root->FindName<Noesis::TextBox>("textBoxIpAddr");
    m_textBoxMyIp->SetTextAlignment(Noesis::Gui::TextAlignment_Center);

    // Buttons
    m_buttonNext = m_root->FindName<Noesis::Button>("buttonNext");
    m_buttonExit = m_root->FindName<Noesis::Button>("buttonExit");
    m_buttonBack = m_root->FindName<Noesis::Button>("buttonBack");
    m_buttonMyIp = m_root->FindName<Noesis::Button>("buttonIpAddr");

    // Button delegates
    m_buttonNext->Click() += [this](auto* sender, const auto& e){qDebug() << "Button Next clicked"; m_animStackNext->Begin();};
    m_buttonBack->Click() += [this](auto* sender, const auto& e){qDebug() << "Button Back clicked"; m_animStackBack->Begin();};
    m_buttonExit->Click() += [this](auto* sender, const auto& e){qDebug() << "Button Exit clicked"; QGuiApplication::quit();};
    m_buttonMyIp->Click() += Noesis::Core::MakeDelegate(this, &MainWindow::getIpAddress);
}

void MainWindow::getIpAddress(Noesis::Core::BaseComponent*, const Noesis::Gui::RoutedEventArgs&)
{
    qDebug() << "Button MyIp clicked";
    m_buttonMyIp->SetIsEnabled(false);
    m_textBoxMyIp->SetFontSize(16);
    m_textBoxMyIp->SetText("Getting IP address...");

    QThread* thread = new QThread(this);
    QNetworkAccessManager *manager = new QNetworkAccessManager;
    manager->moveToThread(thread);

    // Connect response signal
    connect(manager, &QNetworkAccessManager::finished, [this, thread](auto* reply)
    {
        reply->deleteLater();
        if(reply->error() == QNetworkReply::NoError)
        {
            // Get the reply status code
            int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (v >= 200 && v < 300) { // Success
               m_textBoxMyIp->SetText(reply->readAll().data());
            }
            else if (v >= 300 && v < 400) // Redirection
            {
                // Get the redirection url
                QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
                newUrl = reply->url().resolved(newUrl);

                QNetworkAccessManager *manager = reply->manager();
                QNetworkRequest redirection(newUrl);
                manager->get(redirection);

                return;
            }
        }
        else { // Response error
            m_textBoxMyIp->SetText("Error");
        }

        m_textBoxMyIp->SetFontSize(24);
        m_buttonMyIp->SetIsEnabled(true);

        reply->manager()->deleteLater();
        thread->quit();
    });


    connect(thread, &QThread::started, [manager](){manager->get(QNetworkRequest(QUrl("https://api.ipify.org/")));});
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
