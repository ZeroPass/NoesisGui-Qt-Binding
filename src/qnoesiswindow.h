#ifndef QNOESISWINDOW_H
#define QNOESISWINDOW_H
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QResizeEvent>
#include <QScopedPointer>
#include <QElapsedTimer>

#include <Noesis.h>
#include <NoesisGUI.h>



class QNoesisWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit QNoesisWindow(QWindow* parent = nullptr);
    QNoesisWindow(Noesis::FrameworkElement* content, QWindow* parent = nullptr);
    QNoesisWindow(const Noesis::Core::Ptr<Noesis::IView>& view, QWindow* parent = nullptr);
    virtual ~QNoesisWindow();

    void setView(const Noesis::Core::Ptr<Noesis::IView>& view);
    Noesis::Core::Ptr<Noesis::IView> view() const;

    void setVGOptions(const Noesis::VGOptions& opt);

    void setAnimating(bool animating);

public slots:
    void renderLater();
    void renderNow();

protected:
    virtual void initialize();
    virtual void render();
    bool event(QEvent* e) override;
    void exposeEvent(QExposeEvent* e) override;
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;

private:
    bool m_animating;

    QScopedPointer<QOpenGLContext> m_ctx;
    QElapsedTimer m_elapsed;
    qint64 m_frame;

    Noesis::Core::Ptr<Noesis::IView> m_view;
    Noesis::Core::Ptr<Noesis::VGContext> m_vgctx;
    Noesis::VGOptions m_vgopt;
};
#endif // QNOESISWINDOW_H
