#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtWidgets>
#endif

namespace
{

enum class alignment
{
    top_left,
    top,
    top_right,
    left,
    center,
    right,
    bottom_left,
    bottom,
    bottom_right
};

}

static alignment align = alignment::bottom_right;
static int monitor_id = 0;
static int repeat_after = 0;
static std::string filename = ":peek";
static int animationSpeed = 100;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   
    QCoreApplication::setApplicationName("nggyu");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser cmdl;
    cmdl.addHelpOption();
    cmdl.addVersionOption();
    cmdl.setApplicationDescription("Never Gonna Give You Up!");

    QCommandLineOption align_arg(QStringList() << "a" << "align", "Align on the screen. Alignment can be:\n[top_left]\n[top]\n[top_right]\n[left]\n[center]\n[right]\n[bottom_left]\n[bottom]\n[bottom_right]", "alignment");
    cmdl.addOption(align_arg);

    QCommandLineOption monitor_arg(QStringList() << "m" << "monitor", "Id of the monitor", "monitor_id");
    cmdl.addOption(monitor_arg);

    QCommandLineOption repeat_arg(QStringList() << "r" << "repeat", "Repeat after time", "millisec");
    cmdl.addOption(repeat_arg);

    QCommandLineOption input_arg(QStringList() << "i" << "input", "Input filename.", "filename");
    cmdl.addOption(input_arg);

    QCommandLineOption fullscreen_arg(QStringList() << "f" << "fullscreen", "Sets the window on fullscreen.");
    cmdl.addOption(fullscreen_arg);

    QCommandLineOption nomouse_arg(QStringList() << "n" << "nomouse", "Hides the mouse cursor.");
    cmdl.addOption(nomouse_arg);

    QCommandLineOption speed_arg(QStringList() << "s" << "speed", "Speed of the animation", "speed");
    cmdl.addOption(speed_arg);

    cmdl.process(app);

    auto user_align = cmdl.value(align_arg);
    if (user_align == "top_left") align = alignment::top_left;
    if (user_align == "top") align = alignment::top;
    if (user_align == "top_right") align = alignment::top_right;
    if (user_align == "left") align = alignment::left;
    if (user_align == "center") align = alignment::center;
    if (user_align == "right") align = alignment::right;
    if (user_align == "bottom_left") align = alignment::bottom_left;
    if (user_align == "bottom") align = alignment::bottom;
    if (user_align == "bottom_right") align = alignment::bottom_right;

    monitor_id = cmdl.value(monitor_arg).toInt();
    repeat_after = cmdl.value(repeat_arg).toInt();

    auto user_filename = cmdl.value(input_arg);
    if ( user_filename != "")
        filename = user_filename.toStdString();

    if (cmdl.isSet(nomouse_arg))
    {
        QCursor cursor(Qt::BlankCursor);
        QApplication::setOverrideCursor(cursor);
        QApplication::changeOverrideCursor(cursor);
    }

    if (cmdl.isSet(speed_arg))
    {
        animationSpeed = cmdl.value(speed_arg).toUInt();
    }

    QWidget window;
    window.setAttribute(Qt::WA_TranslucentBackground);
    window.setAttribute(Qt::WA_ShowWithoutActivating);
    window.setWindowFlags(Qt::Tool | Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);

    QRect screenRect = app.desktop()->availableGeometry(monitor_id);

    QMovie *movie = new QMovie(filename.c_str());
    if (cmdl.isSet(fullscreen_arg))
    {
        movie->setScaledSize(screenRect.size());
    }
    movie->setSpeed(animationSpeed);

    QLabel *processLabel = new QLabel("", &window);
    processLabel->setMovie(movie);
    movie->start();
    QObject::connect(movie, &QMovie::frameChanged, [&](int frame)
    {
        if (frame >= movie->frameCount() - 1)
        {
            movie->stop();
            window.hide();

            QTimer::singleShot(repeat_after, [&]()
            {
                window.show();
                movie->jumpToFrame(0);
                movie->start();
            });
        }
    });

    window.setFixedSize(movie->currentImage().size().width(), movie->currentImage().size().height());

    switch (align)
    {
        case alignment::top_left:
            window.move(screenRect.x(), 0);
        break;

        case alignment::top:
            window.move(screenRect.x() + (screenRect.width() / 2) - (window.size().width() / 2), 0);
        break;

        case alignment::top_right:
            window.move(screenRect.x() + screenRect.width() - window.size().width(), 0);
        break;

        case alignment::left:
            window.move(screenRect.x(), (screenRect.height() / 2) - (window.size().height() / 2));
        break;

        case alignment::center:
            window.move(screenRect.x() + (screenRect.width() / 2) - (window.size().width() / 2), (screenRect.height() / 2) - (window.size().height() / 2));
        break;

        case alignment::right:
            window.move(screenRect.x() + screenRect.width() - window.size().width(), (screenRect.height() / 2) - (window.size().height() / 2));
        break;

        case alignment::bottom_left:
            window.move(screenRect.x(), screenRect.height());
        break;

        case alignment::bottom:
            window.move(screenRect.x() + (screenRect.width() / 2) - (window.size().width() / 2), screenRect.height());
        break;

        case alignment::bottom_right:
            window.move(screenRect.x() + screenRect.width() - window.size().width(), screenRect.height());
        break;

        default:
        break;
    }

    if (cmdl.isSet(fullscreen_arg))
    {
        window.showFullScreen();
    }
    else
    {
        window.show();
    }

    return app.exec();
}
