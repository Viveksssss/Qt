#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);

signals:

public slots:
    void showPix(QPixmap);

private:
    QPixmap windowPix;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};





#endif // WIDGET_H
