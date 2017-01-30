#include "qnoesiswidget.h"
#include "qnsrenderdevice.h"

using namespace Noesis::GUI;
using namespace Noesis::Render;


QNoesisWidget::QNoesisWidget(QWidget* parent) :
    QNoesisWidget(Noesis::Core::Ptr<Noesis::IView>(), parent)
{}

QNoesisWidget::QNoesisWidget(Noesis::FrameworkElement* content, QWidget* parent) :
    QNoesisWidget(CreateView(content), parent)
{}

QNoesisWidget::QNoesisWidget(const Noesis::Core::Ptr<Noesis::IView>& view, QWidget* parent) :
    QOpenGLWidget(parent)
{
    setMouseTracking(true);

    // Connect widget's slots
    connect(this, &QNoesisWidget::aboutToCompose, this, &QNoesisWidget::onAboutToCompose);
    connect(this, &QNoesisWidget::frameSwapped, this, &QNoesisWidget::onFrameSwapped);
    connect(this, &QNoesisWidget::aboutToResize, this, &QNoesisWidget::onAboutToResize);
    connect(this, &QNoesisWidget::resized, this, &QNoesisWidget::onResized);

    // Init renderer
    m_renderThread = new QThread;
    m_renderer = new Renderer(this);
    m_renderer->moveToThread(m_renderThread);
    connect(m_renderThread, &QThread::finished, [this](){
#ifdef Q_OS_MAC
        m_view.Reset();
#endif
        m_renderer->deleteLater();
    });

    connect(m_renderer, &Renderer::updateViewAndReleaseCtx, this, &QNoesisWidget::onUpdateViewAndReleaseCtx);

    // Set view and start render thread
    setView(view);
    m_renderThread->start();
}

QNoesisWidget::~QNoesisWidget()
{
    m_renderer->prepareToExit();
    m_renderThread->quit();
    m_renderThread->wait();

    delete m_renderThread;
}

void QNoesisWidget::setView(const Noesis::Core::Ptr<Noesis::IView>& view)
{
    if(!view) return;
    m_renderer->lockRenderer();

    m_view.Reset();
    m_view = view;
    m_view->SetSize(this->width(), this->height());

    m_renderer->setView(m_view.GetPtr());
    m_renderer->unlockRenderer();
}

Noesis::Core::Ptr<Noesis::IView> QNoesisWidget::view() const
{
    return m_view;
}

void QNoesisWidget::onAboutToCompose()
{
    m_renderer->lockRenderer();
}

void QNoesisWidget::onFrameSwapped()
{
    m_renderer->unlockRenderer();
    emit renderRequested();
}

void QNoesisWidget::onAboutToResize()
{
    // Get context ownership before continuing with resizing event
    m_renderer->lockRenderer();
    auto* ctx = this->context();
    if(ctx && ctx->thread() != QThread::currentThread())
    {
        // Context is still not owned by gui thread, re-sync.
        // This can happen in-between renderer grabbing context and resize event arriving at the same tame.
        m_renderer->unlockRenderer();
        QThread::usleep(1);
        m_renderer->lockRenderer();
        Q_ASSERT(ctx->thread() == QThread::currentThread());
    }
}

void QNoesisWidget::onResized()
{
    m_renderer->unlockRenderer();
}

void QNoesisWidget::onUpdateViewAndReleaseCtx(qreal timeInSeconds)
{
    m_renderer->lockRenderer();
    QMutexLocker lock(m_renderer->grabCtxMutex());
    if (m_view) {
        m_view->Update(timeInSeconds);
    }

    context()->moveToThread(m_renderThread);
    m_renderer->grabCtxCond()->wakeAll();
    m_renderer->unlockRenderer();
}

void QNoesisWidget::mousePressEvent(QMouseEvent* event)
{
    if(!m_view)
        return;

    if(event->button() & Qt::LeftButton) {
        m_view->MouseButtonDown(event->x(), event->y(), Noesis::MouseButton_Left);
    }
    else if(event->button() & Qt::RightButton) {
        m_view->MouseButtonDown(event->x(), event->y(), Noesis::MouseButton_Right);
    }
    else if(event->button() & Qt::MiddleButton) {
        m_view->MouseButtonDown(event->x(), event->y(), Noesis::MouseButton_Middle);
    }
    else if(event->button() & Qt::XButton1) {
        m_view->MouseButtonDown(event->x(), event->y(), Noesis::MouseButton_XButton1);
    }
    else if(event->button() & Qt::XButton2) {
        m_view->MouseButtonDown(event->x(), event->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(!m_view)
        return;

    if(event->button() & Qt::LeftButton) {
        m_view->MouseButtonUp(event->x(), event->y(), Noesis::MouseButton_Left);
    }
    else if(event->button() & Qt::RightButton) {
        m_view->MouseButtonUp(event->x(), event->y(), Noesis::MouseButton_Right);
    }
    else if(event->button() & Qt::MiddleButton) {
        m_view->MouseButtonUp(event->x(), event->y(), Noesis::MouseButton_Middle);
    }
    else if(event->button() & Qt::XButton1) {
        m_view->MouseButtonUp(event->x(), event->y(), Noesis::MouseButton_XButton1);
    }
    else if(event->button() & Qt::XButton2) {
        m_view->MouseButtonUp(event->x(), event->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!m_view)
        return;

    if(event->button() & Qt::LeftButton) {
        m_view->MouseDoubleClick(event->x(), event->y(), Noesis::MouseButton_Left);
    }
    else if(event->button() & Qt::RightButton) {
        m_view->MouseDoubleClick(event->x(), event->y(), Noesis::MouseButton_Right);
    }
    else if(event->button() & Qt::MiddleButton) {
        m_view->MouseDoubleClick(event->x(), event->y(), Noesis::MouseButton_Middle);
    }
    else if(event->button() & Qt::XButton1) {
        m_view->MouseDoubleClick(event->x(), event->y(), Noesis::MouseButton_XButton1);
    }
    else if(event->button() & Qt::XButton2) {
        m_view->MouseDoubleClick(event->x(), event->y(), Noesis::MouseButton_XButton2);
    }
}

void QNoesisWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(m_view) {
        m_view->MouseMove(event->x(), event->y());
    }
}

void QNoesisWidget::resizeGL(int w, int h)
{
    if(m_view)
    {
        const qreal pr = this->devicePixelRatio();
        m_view->SetSize(w * pr, h * pr);
    }
}

Renderer* QNoesisWidget::getRenderer() const
{
    return m_renderer;
}




Renderer::Renderer(QNoesisWidget* widget):
    m_bInited(false),
    m_bExiting(false),
    m_glwidget(widget),
    m_renderMutex(QMutex::Recursive),
    m_frame(0),
    m_view(nullptr)
{
    connect(m_glwidget, &QNoesisWidget::renderRequested, this, &Renderer::render);
}

Renderer::~Renderer()
{}

void Renderer::setView(Noesis::IView* view)
{
    QMutexLocker lock(&m_renderMutex);
    m_view = view;

    if(m_vgctx) {
        m_view->GetRenderer()->Init(m_vgctx.GetPtr());
    }
}

void Renderer::setVGOptions(const Noesis::VGOptions& opt)
{
    m_vgopt = opt;
}

void Renderer::lockRenderer()
{
    m_renderMutex.lock();
}

void Renderer::unlockRenderer()
{
    m_renderMutex.unlock();
}

QMutex* Renderer::grabCtxMutex()
{ 
    return &m_grabCtxMutex;
}

QWaitCondition* Renderer::grabCtxCond()
{ 
    return &m_grabCtxCond;
}

void Renderer::prepareToExit()
{ 
    m_bExiting = true;
    m_grabCtxCond.wakeAll();
}

qreal Renderer::fps() const
{
    qreal fps = 0;
    if (m_elapsed.elapsed()) {
        fps = m_frame / ((qreal)m_elapsed.elapsed() /1000.0);;
    }

    return fps;
}


// Some OpenGL implementations have serious issues with compiling and linking
// shaders on multiple threads concurrently. Avoid this.
Q_GLOBAL_STATIC(QMutex, initMutex)

void Renderer::render()
{
    if (m_bExiting || !m_view)
        return;

    QOpenGLContext* ctx = m_glwidget->context();
    if (!ctx) // QOpenGLWidget not yet initialized
        return;

    // Grab the context.
    m_grabCtxMutex.lock();
    emit updateViewAndReleaseCtx(m_elapsed.elapsed() / 1000.0);

    m_grabCtxCond.wait(&m_grabCtxMutex);
    QMutexLocker lock(&m_renderMutex);
    m_grabCtxMutex.unlock();

    if (m_bExiting)
        return;

    Q_ASSERT(ctx->thread() == QThread::currentThread());

    // Make the context (and an offscreen surface) current for this thread
    m_glwidget->makeCurrent();

    if (!m_bInited)
    {
        initializeOpenGLFunctions();

         glEnable(GL_MULTISAMPLE);
         glEnable(GL_BLEND);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Init VGContext
        QMutexLocker initLock(initMutex());

        Noesis::Core::Ptr<QNsRenderDevice> device = *new QNsRenderDevice;
        m_vgctx = Noesis::GUI::CreateVGContext(device.GetPtr(), m_vgopt);
        m_view->GetRenderer()->Init(m_vgctx.GetPtr());

        m_elapsed.start();
        m_bInited = true;
    }

    // Apply changes to the render tree
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
    const qreal pr = m_glwidget->devicePixelRatio();
    glViewport(0, 0, m_glwidget->width() * pr, m_glwidget->height() * pr);

    // Render View
    nsRenderer->Render();
    m_frame++;

    glFlush();
    glFinish();

    // Move context back to the gui thread
    m_glwidget->doneCurrent();
    ctx->moveToThread(m_glwidget->thread());

    // Schedule composition on gui thread
    QMetaObject::invokeMethod(m_glwidget, "update", Qt::QueuedConnection);
}
