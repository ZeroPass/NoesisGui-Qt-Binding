#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QString>

#include <Noesis.h>
#include <NoesisGUI.h>

#include "qnoesiswidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
private:
    void loadXaml(const QString& name);
    void getIpAddress(Noesis::Core::BaseComponent* sender, const Noesis::Gui::RoutedEventArgs& e);
    void initXamlResources();

private:
    QNoesisWidget* m_NsWidget;
    Noesis::Core::Ptr<Noesis::UserControl> m_root;
    Noesis::Storyboard* m_animStackNext;
    Noesis::Storyboard* m_animStackBack;
    Noesis::Button* m_buttonNext;
    Noesis::Button* m_buttonExit;
    Noesis::Button* m_buttonBack;
    Noesis::Button* m_buttonMyIp;
    Noesis::TextBox* m_textBoxMyIp;
};
#endif
