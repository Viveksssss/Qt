#ifndef SCREENCAPTUREPREVIEW_H
#define SCREENCAPTUREPREVIEW_H

#include <QWidget>
#include <QScreenCapture>
#include <QWindowCapture>
#include <QWidget>
#include <QItemSelection>
class QListView;
class QMediaCaptureSession;
class QVideoWidget;
class QGridLayout;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class ScreenListModel;
class WindowListModel;
class QLabel;
class ScreenCapturePreview : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenCapturePreview(QWidget *parent = nullptr);
    ~ScreenCapturePreview();
private slots:
    void onCurrentScreenSelectionChanged(QItemSelection index);
    void onCurrentWindowSelectionChanged(QItemSelection index);
    void onWindowCaptureErrorChanged();
    void onScreenCaptureErrorChanged();
    void onStartStopButtonClicked();
private:
    enum class SourceType { Screen, Window };
    void updateActive(SourceType sourceType, bool active);
    void updateStartStopButtonText();
    bool isActive() const;
private:
    ScreenListModel *screenListModel = nullptr;
    WindowListModel *windowListModel = nullptr;
    QListView *screenListView = nullptr;
    QListView *windowListView = nullptr;
    QScreenCapture *screenCapture = nullptr;
    QWindowCapture *windowCapture = nullptr;
    QMediaCaptureSession *mediaCaptureSession = nullptr;
    QVideoWidget *videoWidget = nullptr;
    QGridLayout *gridLayout = nullptr;
    QPushButton *startStopButton = nullptr;
    QLabel *screenLabel = nullptr;
    QLabel *windowLabel = nullptr;
    QLabel *videoWidgetLabel = nullptr;
    SourceType sourceType = SourceType::Screen;
signals:
};

#endif // SCREENCAPTUREPREVIEW_H
