#include "qnoesiswindow.h"
#include "qnsrenderdevice.h"

#include <QDebug>
#include <QTimer>

#include <NsCore/PtrForward.h>
#include <NsCore/HashMap.h>
#include <NsCore/Ptr.h>



using namespace Noesis;
using namespace Noesis::Core;
using namespace Noesis::Gui;
using namespace Noesis::GUI;
using namespace Noesis::Render;


QNoesisWindow::QNoesisWindow(QWindow *parent) :
    QNoesisWindow(Ptr<IView>(), parent)
{}

QNoesisWindow::QNoesisWindow(FrameworkElement* content, QWindow* parent) :
    QNoesisWindow(CreateView(content), parent)
{}

QNoesisWindow::QNoesisWindow(const Ptr<IView>& view, QWindow* parent) :
    QWindow(parent),
    m_animating(false),
    m_frame(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
    setView(view);
    setAnimating(true);
}


QNoesisWindow::~QNoesisWindow()
{
    m_ctx.reset();
    m_vgctx.Reset();

    /* Delete m_view */
    // TODO: improve code below!
    QTimer* t = new QTimer;
    t->setSingleShot(true);

    connect(t, &QTimer::timeout, [t, v = m_view]() mutable {
        v.Reset(); // QOpenGlCntext should be deleted by now!
        t->deleteLater();
    });

    // Start timer
    // 1s interval, ensures that QOpenGlContext is deleted before m_view
    t->start(1000);
}

void QNoesisWindow::setView(const Ptr<IView>& view)
{
    if(!view) return;
    m_view = view;
    m_view->SetSize(this->width(), this->height());
}

Ptr<IView> QNoesisWindow::view() const
{
    return m_view;
}

void QNoesisWindow::setVGOptions(const Noesis::VGOptions& opt)
{
    m_vgopt = opt;
}

void QNoesisWindow::initialize()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Init VGContext
   Ptr<QNsRenderDevice> device = *new QNsRenderDevice;
   m_vgctx = Noesis::GUI::CreateVGContext(device.GetPtr(), m_vgopt);
   m_view->GetRenderer()->Init(m_vgctx.GetPtr());
}

void QNoesisWindow::render()
{
    m_view->Update(m_elapsed.elapsed() / 1000.0);

    Noesis::IRenderer* nsRenderer = m_view->GetRenderer();
    nsRenderer->UpdateRenderTree();

    // Render offscreen textures
    if (nsRenderer->NeedsOffscreen()) {
        nsRenderer->RenderOffscreen();
    }

    // Restore state. This must be done per frame because NoesisGUI modifies the GPU state
    // TODO: improve this by adding a SaveState() and RestoreState() to GLRenderDevice
    glColorMask(true, true, true, true);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Update viewport
    const qreal pr = this->devicePixelRatio();
    glViewport(0, 0, this->width() * pr, this->height() * pr);

    // Render View
    nsRenderer->Render();

    glFlush();
    glFinish();

    m_frame++;
}

void QNoesisWindow::renderLater()
{
    requestUpdate();
}

bool QNoesisWindow::event(QEvent* e)
{
    switch (e->type())
    {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(e);
    }
}

void QNoesisWindow::exposeEvent(QExposeEvent* e)
{
    Q_UNUSED(e);

    if (isExposed()) {
        renderNow();
    }
}

void QNoesisWindow::resizeEvent(QResizeEvent* e)
{
    if(m_view)
    {
        const qreal pr = this->devicePixelRatio();
        m_view->SetSize(e->size().width() * pr, e->size().height() * pr);
    }
    QWindow::resizeEvent(e);
}

void QNoesisWindow::mousePressEvent(QMouseEvent* e)
{
    if(!m_view) return;

    if(e->button() & Qt::LeftButton) {
        m_view->MouseButtonDown(e->x(), e->y(), Noesis::MouseButton_Left);
    }
    else if(e->button() & Qt::RightButton) {
        m_view->MouseButtonDown(e->x(), e->y(), Noesis::MouseButton_Right);
    }
    else if(e->button() & Qt::MiddleButton) {
        m_view->MouseButtonDown(e->x(), e->y(), Noesis::MouseButton_Middle);
    }
    else if(e->button() & Qt::XButton1) {
        m_view->MouseButtonDown(e->x(), e->y(), Noesis::MouseButton_XButton1);
    }
    else if(e->button() & Qt::XButton2) {
        m_view->MouseButtonDown(e->x(), e->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if(!m_view) return;

    if(e->button() & Qt::LeftButton) {
        m_view->MouseButtonUp(e->x(), e->y(), Noesis::MouseButton_Left);
    }
    else if(e->button() & Qt::RightButton) {
        m_view->MouseButtonUp(e->x(), e->y(), Noesis::MouseButton_Right);
    }
    else if(e->button() & Qt::MiddleButton) {
        m_view->MouseButtonUp(e->x(), e->y(), Noesis::MouseButton_Middle);
    }
    else if(e->button() & Qt::XButton1) {
        m_view->MouseButtonUp(e->x(), e->y(), Noesis::MouseButton_XButton1);
    }
    else if(e->button() & Qt::XButton2) {
        m_view->MouseButtonUp(e->x(), e->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWindow::mouseDoubleClickEvent(QMouseEvent* e)
{
    if(!m_view)
        return;

    if(e->button() & Qt::LeftButton) {
        m_view->MouseDoubleClick(e->x(), e->y(), Noesis::MouseButton_Left);
    }
    else if(e->button() & Qt::RightButton) {
        m_view->MouseDoubleClick(e->x(), e->y(), Noesis::MouseButton_Right);
    }
    else if(e->button() & Qt::MiddleButton) {
        m_view->MouseDoubleClick(e->x(), e->y(), Noesis::MouseButton_Middle);
    }
    else if(e->button() & Qt::XButton1) {
        m_view->MouseDoubleClick(e->x(), e->y(), Noesis::MouseButton_XButton1);
    }
    else if(e->button() & Qt::XButton2) {
        m_view->MouseDoubleClick(e->x(), e->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWindow::mouseMoveEvent(QMouseEvent* e)
{
    if(m_view) {
        m_view->MouseMove(e->x(), e->y());
    }
}

void QNoesisWindow::renderNow()
{
    if (!isExposed() || !m_view) return;

    bool needsInitialize = false;

    if (!m_ctx)
    {
        m_ctx.reset(new QOpenGLContext(this));
        m_ctx->setFormat(requestedFormat());
        m_ctx->create();

        needsInitialize = true;
    }

    m_ctx->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
        m_elapsed.start();
    }

    render();

    m_ctx->swapBuffers(this);

    if (m_animating){
        renderLater();
    }
}

void QNoesisWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating){
        renderLater();
    }
}
