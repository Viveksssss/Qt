#ifndef FORGOTSCREEN_H
#define FORGOTSCREEN_H

#include <QWidget>

class ForgotScreen : public QWidget
{
    Q_OBJECT
public:
    explicit ForgotScreen(QWidget *parent = nullptr);
    void setupUI();
    void setupConnections();
signals:
    void goLogin();
};

#endif // FORGOTSCREEN_H
