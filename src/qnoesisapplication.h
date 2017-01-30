#ifndef QNOESISAPPLICATION_H
#define QNOESISAPPLICATION_H
#include <QApplication>
#include <QDebug>
#include <QString>
#include <mutex>

#include <Noesis.h>
#include <NoesisGUI.h>

using QNsAppErrorHandler = void(const QString& filename, qint32 line, const QString& desc, bool fatal);

class QNoesisApplication : public QApplication
{
    Q_OBJECT
public:
#ifdef Q_QDOC
    QApplication(int &argc, char **argv, QNsErrorHandler h = QNsDefaultErrorHandler, Noesis::Core::MemoryAllocator* allocator = 0) :
        QApplication(argc, argv)
#else

    QNoesisApplication(int &argc, char **argv, QNsAppErrorHandler h = QNsDefaultErrorHandler, Noesis::Core::MemoryAllocator* allocator = 0) :
        QNoesisApplication(argc, argv, ApplicationFlags, h, allocator){}

    QNoesisApplication(int &argc, char **argv, int appFlags, QNsAppErrorHandler h = QNsDefaultErrorHandler, Noesis::Core::MemoryAllocator* allocator = 0) :
        QApplication(argc, argv, appFlags)
#endif
    {
        std::call_once(onceFlag(), [&h, &allocator]() {
            static auto gh = h;
            Noesis::GUI::Init([](auto filename, auto line, auto desc, auto fatal){
               if(gh)gh(QString(filename), line, QString(desc), fatal);
            }, allocator);
        });
    }

    virtual ~QNoesisApplication()
    {
        Noesis::GUI::Shutdown();
    }

private:
    static std::once_flag& onceFlag()
    {
        static std::once_flag of;
        return of;
    }

    static void QNsDefaultErrorHandler(const QString& filename, qint32 line, const QString& desc, bool fatal)
    {
        qCritical() << "NoesisGui Error:" << desc;
        qCritical() << "    File:" << filename;
        qCritical() << "    Line:" << line;

        if(fatal) {
            qFatal("Noesis GUI fatal error occurred!");
        }
    }
};

#endif // QNOESISAPPLICATION_H
