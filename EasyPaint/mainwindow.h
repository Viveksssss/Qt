#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QPropertyAnimation>
#include "staticdata.h"

#include "Micro.h"
QT_BEGIN_NAMESPACE

class DrawWidget;
class QVBoxLayout;
class QLabel;

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget*m_LeftWidget;
    QVBoxLayout *m_LeftLayout;
    QLabel*m_ArrowLabel;
    QToolButton*m_RectangleBtn;
    QToolButton*m_EllipserBtn;
    QToolButton*m_TriangleBtn;
    QToolButton*m_LineBtn;
    QToolButton*m_TextBtn;

    QToolButton*m_ResetBtn;
    QToolButton*m_RotateLeftBtn;
    QToolButton*m_RotateRightBtn;
    QToolButton*m_ZoomInBtn;
    QToolButton*m_ZoomOutBtn;
    QToolButton*m_FrontBtn;
    QToolButton*m_BackBtn;
    QToolButton*m_SaveBtn;
    QToolButton*m_LoadBtn;
    QToolButton*m_ClearBtn;

    QToolButton*m_DeleteBtn;
    QToolButton*m_CloseBtn;

    DrawWidget*m_DrawWidget;
    QPropertyAnimation *animation;

    QString m_FilePath;

    bool m_MouseInArea;
    bool m_LeftIsVisible;
    bool m_IsDrawing;
private:
    void init();
    void initResource();
    void initButton();
    void saveFile();
    void loadFile();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent*event)override;
};
#endif // MAINWINDOW_H
