#ifndef MESSAGEAREA_H
#define MESSAGEAREA_H

#include <QWidget>

class MessageDelegate;
class MessageModel;
class QListView;
class MessageArea : public QWidget
{
    Q_OBJECT
public:
    explicit MessageArea(QWidget *parent = nullptr);
    void setupUI();
    void setupConnections();
    MessageModel*getModel();

public slots:
    void do_area_to_bottom(); // from InputArea::on_message_sent
public:

    QListView *list;
    MessageDelegate*delegate;
    MessageModel*model;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MESSAGEAREA_H
