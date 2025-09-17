#include "mainwindow.h"
#include <QUrl>
#include <QPdfView>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QPdfPageSelector>
#include <QPdfSearchModel>
#include <QDockWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPdfBookmarkModel>
#include <QMouseEvent>
#include <QScrollBar>
#include <QScrollArea>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    setMouseTracking(true);
    init();
    setUi();
}

MainWindow::~MainWindow()
{

}

void MainWindow::open(QUrl url)
{

}

void MainWindow::init()
{
    viewer = new QPdfView(this);
    document = new QPdfDocument(this);
    pageSelector = new QPdfPageSelector(this);
    pageSelector->setDocument(document);
    searchModel = new QPdfSearchModel(this);
    searchModel->setDocument(document);
    lineEdit = new QLineEdit(this);

    treeWidget = new QTreeWidget(this);

    dockWidget = new QDockWidget(this);
    toolBar = new QWidget(this);
    model = new QPdfBookmarkModel(this);
    model->setDocument(document);

    openFile = new QPushButton(toolBar);
    zoomIn = new QPushButton(toolBar);
    zoomOut = new QPushButton(toolBar);
    reset = new QPushButton(toolBar);
    cursor = new QPushButton(toolBar);
    drag = new QPushButton(toolBar);
    overlay = new QLabel(this);
    pixmap = new QPixmap("/home/vivek/Pictures/Screenshots/250820_18h06m55s_screenshot.png");

    connect(openFile,&QPushButton::clicked,this,&MainWindow::openPdfFile);
    connect(viewer->pageNavigator(),&QPdfPageNavigator::currentPageChanged,pageSelector,&QPdfPageSelector::setCurrentPage);
    connect(pageSelector,&QPdfPageSelector::currentPageChanged,this,[this](int index){
        if(index > document->pageCount()){
            pageSelector->setCurrentPage( index = document->pageCount() );
        }
        viewer->pageNavigator()->jump(index,QPointF());
    });

    connect(document, &QPdfDocument::statusChanged, this, [&](QPdfDocument::Status s){
        if (s != QPdfDocument::Status::Ready) return;

        treeWidget->clear();

        for (int i = 0; i < model->rowCount(); ++i){
            addBookmark(model->index(i, 0), nullptr);
        }
    });

    connect(zoomIn,&QPushButton::clicked,[this](){
        this->viewer->setZoomFactor(viewer->zoomFactor()*1.1);
    });

    connect(zoomOut,&QPushButton::clicked,[this](){
        this->viewer->setZoomFactor(viewer->zoomFactor()*0.9);
    });

    connect(reset,&QPushButton::clicked,[this](){
        this->viewer->setZoomFactor(1);
    });

    connect(cursor,&QPushButton::clicked,[this](){
        this->isCursor = true;
        viewer->viewport()->setCursor(Qt::ArrowCursor);
    });

    connect(drag,&QPushButton::clicked,[this](){
        this->isCursor = false;
        viewer->viewport()->setCursor(Qt::OpenHandCursor);
        qDebug() << "sdsd";
    });

    connect(treeWidget,&QTreeWidget::itemClicked,this,[this](QTreeWidgetItem *item, int column){
        Q_UNUSED(column);
        int pageNumber = item->data(0, Qt::UserRole).toInt();
        QPointF location = item->data(0, Qt::UserRole + 1).toPointF();
        if (pageNumber >= 0) {
            // 方法1：简单跳转到页面
            viewer->pageNavigator()->jump(pageNumber, QPointF(), viewer->zoomFactor());

            // 方法2：跳转到具体位置（如果有位置信息）
            if (!location.isNull()) {
                viewer->pageNavigator()->jump(pageNumber, location, viewer->zoomFactor());
            }
        }
    });

}

void MainWindow::openPdfFile()
{
    QString fileName= QFileDialog::getOpenFileName(this,"open your pdf file",QDir::currentPath(),"pdf (*.pdf)");
    if(QFileInfo::exists(fileName)){
        file.setFileName(fileName);

    }
    if(file.open(QIODevice::ReadOnly)){
        QPdfDocument::Error err = document->load(fileName);   // ← 关键一行
        viewer->setDocument(document);
        overlay->hide();
        if (err != QPdfDocument::Error::None) {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Failed to load PDF: %1").arg(fileName));
            delete document;
            return;
        }
    }
}

void MainWindow::setUi()
{
    /* ---------- 1. 左侧 dock ---------- */
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    dockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dockWidget->setWidget(treeWidget);
    treeWidget->setHeaderHidden(true);   // 等价写法


    /* ---------- 2. 顶部自定义工具栏 ---------- */
    toolBar->setFixedHeight(height()/10);
    toolBar->setStyleSheet("background:#e0e0e0;");

    auto *hLayout = new QHBoxLayout(toolBar);
    hLayout->setContentsMargins(6, 0, 6, 0);
    hLayout->addWidget(openFile);
    openFile->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/openFile.png"));
    hLayout->addWidget(zoomIn);
    zoomIn->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/big.png"));
    hLayout->addWidget(zoomOut);
    zoomOut->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/small.png"));
    hLayout->addWidget(reset);
    reset->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/reset.png"));
    hLayout->addWidget(cursor);
    cursor->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/cursor.png"));
    hLayout->addWidget(drag);
    drag->setIcon(QIcon("/home/vivek/Codes/Qt/PDFViewer/resources/drag.png"));
    hLayout->addWidget(pageSelector);
    pageSelector->setFixedWidth(100);
    hLayout->addWidget(lineEdit);
    lineEdit->setPlaceholderText("Input context...");
    hLayout->addStretch();
    pageSelector->setDocument(document);
    searchModel->setDocument(document);


    /* ---------- 3. 主窗口中央用垂直布局 ---------- */
    auto *central = new QWidget(this);
    viewer->setStyleSheet("QPdfView{"
                          "margin:10;"
                          "background:#ffffff;"
                          "border-radius:5px;"
                          "}");
    viewer->viewport()->installEventFilter(this);
    viewer->setPageMode(QPdfView::PageMode::MultiPage);
    viewer->setMouseTracking(true);
    viewer->viewport()->installEventFilter(this);
    auto *vLayout = new QVBoxLayout(central);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(viewer);
    scrollArea->setWidgetResizable(true);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->addWidget(toolBar);
    vLayout->addWidget(scrollArea, 1);

    setCentralWidget(central);
}

void MainWindow::addBookmark(const QModelIndex &idx, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = parent?new QTreeWidgetItem(parent):new QTreeWidgetItem(treeWidget);
    item->setText(0,idx.data(Qt::DisplayRole).toString());

    int page = idx.data(static_cast<int>(QPdfBookmarkModel::Role::Page)).toInt();
    item->setData(0,Qt::UserRole,page);
    item->setData(0, Qt::UserRole + 1,idx.data(static_cast<int>(QPdfBookmarkModel::Role::Location)).toPointF());

    treeWidget->expandItem(item);

    for (int i = 0; i < model->rowCount(idx); ++i)
        addBookmark(model->index(i, 0, idx), item);
}




bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 只处理QPdfView及其viewport的事件
    if (watched == viewer || watched == viewer->viewport()) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraging = true;
                lastDrag = mouseEvent->pos();
                qDebug() << "Mouse press on PDF view:" << lastDrag;
                return true; // 事件已处理
            }
            break;
        }
        case QEvent::MouseMove: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            qDebug() << "Mouse move on PDF view:" << mouseEvent->pos();

            if ((mouseEvent->buttons() & Qt::LeftButton) && isDraging && !isCursor) {
                QPoint delta = mouseEvent->pos() - lastDrag;
                lastDrag = mouseEvent->pos();
                qDebug() << "Dragging delta:" << delta;

                viewer->horizontalScrollBar()->setValue(viewer->horizontalScrollBar()->value()-delta.x());
                viewer->verticalScrollBar()->setValue(viewer->verticalScrollBar()->value()-delta.y());
                return true; // 事件已处理
            }
            break;
        }
        case QEvent::MouseButtonRelease: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                isDraging = false;
                qDebug() << "Mouse release on PDF view";
                return true;
            }
            break;
        }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (!overlay || !overlay->isVisible() || !viewer)
        return;

    QRect vis = viewer->viewport()->rect();
    QPoint topLeft = viewer->viewport()->mapToGlobal(vis.topLeft());
    QPoint bottomRight = viewer->viewport()->mapToGlobal(vis.bottomRight());

    QRect targetRect(topLeft, bottomRight);
    overlay->setGeometry(targetRect);
    overlay->raise();


    overlay->setPixmap((*pixmap).scaled(targetRect.size(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation));
}






