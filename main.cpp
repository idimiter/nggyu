#include <QtGui>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#endif
#include <QWidget>
#include <QObject>

int main(int argc, char *argv[])
{
//    if (fork()) exit(0);

    QApplication app(argc, argv);   
    QWidget window;

    window.setAttribute(Qt::WA_TranslucentBackground);
    window.setAttribute(Qt::WA_ShowWithoutActivating);
    window.setWindowFlags(Qt::Tool | Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);

    QMovie *movie = new QMovie(":peek");
    QLabel *processLabel = new QLabel("", &window);
    processLabel->setMovie(movie);
    movie->start();
    QObject::connect(movie, &QMovie::frameChanged, [&](int frame)
    {
        if (frame >= movie->frameCount() - 1)
        {
            movie->stop();
            window.hide();

            QTimer::singleShot(rand() % (3600000 * 12), [&]()
            {
                window.show();
                movie->jumpToFrame(0);
                movie->start();
            });
        }
    });

    window.setFixedSize(movie->currentImage().size().width(), movie->currentImage().size().height());
    QRect screenRect = app.desktop()->availableGeometry(0);
    window.move(screenRect.width() - window.size().width(), screenRect.height());

    window.show();

    return app.exec();
}
