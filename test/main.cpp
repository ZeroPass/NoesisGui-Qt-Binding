#include "qnoesisapplication.h"
#include "qnoesiswidget.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QNoesisApplication a(argc, argv);

    MainWindow mw;
    mw.setMinimumSize(640, 480);
    mw.show();

/*
    Noesis::GUI::SetResourceProvider("Data");
    auto root = Noesis::GUI::LoadXaml<Noesis::UserControl>("test.xaml");

    QNoesisWidget w(root.GetPtr());
    w.show();
*/

    return a.exec();
}




