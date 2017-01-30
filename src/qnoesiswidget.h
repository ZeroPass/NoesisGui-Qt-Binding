#ifndef QNOESISWIDGET_H
#define QNOESISWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QMouseEvent>

#include <Noesis.h>
#include <NoesisGUI.h>

class Renderer;

class QNoesisWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    QNoesisWidget(QWidget* parent = nullptr);
    QNoesisWidget(Noesis::FrameworkElement* content, QWidget* parent = nullptr);
    QNoesisWidget(const Noesis::Core::Ptr<Noesis::IView>& view, QWidget* parent = nullptr);
    ~QNoesisWidget();

    void setView(const Noesis::Core::Ptr<Noesis::IView>& view);
    Noesis::Core::Ptr<Noesis::IView> view() const;

    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    Renderer* getRenderer() const;

signals:
    void renderRequested();

protected:
    void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE {}

protected slots:
    void onUpdateViewAndReleaseCtx(qreal timeInSeconds);

private slots:
    void onAboutToCompose();
    void onFrameSwapped();
    void onAboutToResize();
    void onResized();

private:
    QThread* m_renderThread;
    Renderer* m_renderer;
    Noesis::Core::Ptr<Noesis::IView> m_view;

    friend class Renderer;
};

class Renderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    Renderer(QNoesisWidget* widget);
    virtual ~Renderer();
    void setView(Noesis::IView* view);
    void setVGOptions(const Noesis::VGOptions& opt);
    void lockRenderer();
    void unlockRenderer();
    QMutex* grabCtxMutex();
    QWaitCondition* grabCtxCond();
    void prepareToExit();
    qreal fps() const;

signals:
    void updateViewAndReleaseCtx(qreal timeInSeconds);

private slots:
    void render();

private:
    bool m_bInited;
    bool m_bExiting;
    QNoesisWidget* m_glwidget;

    QMutex m_renderMutex;
    QMutex m_grabCtxMutex;
    QWaitCondition m_grabCtxCond;

    QElapsedTimer m_elapsed;
    qint64 m_frame;

    Noesis::Core::Ptr<Noesis::VGContext> m_vgctx;
    Noesis::IView* m_view;
    Noesis::VGOptions m_vgopt;
};

#endif // QNOESISWIDGET_H
