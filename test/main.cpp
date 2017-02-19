#include "qnoesisapplication.h"
#include "qnoesiswidget.h"
#include "qnoesiswindow.h"
#include "mainwindow.h"

#include <QDebug>

#include <Noesis.h>
#include <NoesisGUI.h>

using namespace Noesis;
using namespace Noesis::GUI;

int main(int argc, char *argv[])
{
    QNoesisApplication a(argc, argv);

    SetResourceProvider("Data");
    auto root = LoadXaml<UserControl>("test.xaml");

    QNoesisWindow nw(root.GetPtr());
    {
        // Animmations
        Storyboard* animStackNext = root->FindResource<Storyboard>(ResourceKeyString::Create("stackAnimationNext").GetPtr());
        Storyboard* animStackBack = root->FindResource<Storyboard>(ResourceKeyString::Create("stackAnimationBack").GetPtr());

        // Text box
        TextBox* textBoxMyIp = root->FindName<TextBox>("textBoxIpAddr");
        textBoxMyIp->SetTextAlignment(Noesis::Gui::TextAlignment_Center);

        // Buttons
        Button* buttonNext = root->FindName<Button>("buttonNext");
        Button* buttonExit = root->FindName<Button>("buttonExit");
        Button* buttonBack = root->FindName<Button>("buttonBack");
        Button* buttonMyIp = root->FindName<Button>("buttonIpAddr");
        buttonMyIp->SetIsEnabled(false);

        // Button delegates
        buttonNext->Click() += [animStackNext](auto*, const auto&){qDebug() << "Button Next clicked"; animStackNext->Begin();};
        buttonBack->Click() += [animStackBack](auto*, const auto&){qDebug() << "Button Back clicked"; animStackBack->Begin();};
        buttonExit->Click() += [](auto*, const auto&){qDebug() << "Button Exit clicked"; QNoesisApplication::quit();};
    }

    nw.setTitle("Test QNoesisWindow");
    nw.resize(640, 480);
    nw.setX(0);
    nw.show();


    MainWindow mw;
    mw.setWindowTitle("Test QNoesisWidget window");
    mw.setMinimumSize(640, 480);
    mw.move(nw.width(), 0);
    mw.show();

/*
    Noesis::GUI::SetResourceProvider("Data");
    auto root = Noesis::GUI::LoadXaml<Noesis::UserControl>("test.xaml");

    QNoesisWidget w(root.GetPtr());
    w.show();
*/

    return a.exec();
}




