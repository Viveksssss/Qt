
#include "screencapturepreview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ScreenCapturePreview screenCapturePreview;
    screenCapturePreview.show();
    return app.exec();
}
