#include "mainwindow.h"
#include "drawwidget.h"
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QDebug>
#include <QLabel>
#include <QDir>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_MouseInArea(false)
    , m_LeftIsVisible(false)
    , m_IsDrawing(false)


{
    m_FilePath = QDir::currentPath();
    this->setMouseTracking(true);
    this->setAutoFillBackground(true);
    this->setStyleSheet("MainWindow{background-color:rgb(255, 182, 193);}");
    init();
}

MainWindow::~MainWindow()
{

}

void MainWindow::init()
{
    initResource();
    initButton();
}

void MainWindow::initResource()
{
    m_ArrowLabel = new QLabel(this);
    m_LeftWidget = new QWidget(this);
    m_LeftWidget->setMouseTracking(true);
    m_LeftLayout = new QVBoxLayout(m_LeftWidget);
    m_RectangleBtn = new QToolButton(this);
    m_EllipserBtn= new QToolButton(this);
    m_TriangleBtn= new QToolButton(this);
    m_LineBtn= new QToolButton(this);
    m_TextBtn= new QToolButton(this);


    m_ResetBtn= new QToolButton(this);
    m_RotateLeftBtn= new QToolButton(this);
    m_RotateRightBtn= new QToolButton(this);
    m_ZoomInBtn= new QToolButton(this);
    m_ZoomOutBtn= new QToolButton(this);
    m_SaveBtn = new QToolButton(this);
    m_LoadBtn = new QToolButton(this);
    m_ClearBtn = new QToolButton(this);

    // m_FrontBtn= new QToolButton(this);
    // m_BackBtn= new QToolButton(this);
    // m_DeleteBtn= new QToolButton(this);
    m_CloseBtn= new QToolButton(this);

    m_DrawWidget = new DrawWidget(this);
    animation = new QPropertyAnimation(this);

}

void MainWindow::initButton()
{
    m_ArrowLabel->setPixmap(QPixmap(":/icon/resources/arrow.png").scaled(30,30,Qt::KeepAspectRatio));
    m_ArrowLabel->setStyleSheet("background-color:transparent;");
    m_ArrowLabel->raise();


    // 形状工具组
    m_LeftLayout->setAlignment(Qt::AlignCenter);
    m_LeftLayout->setContentsMargins(3, 0, 0, 0);
    m_LeftWidget->setLayout(m_LeftLayout);
    m_LeftWidget->setTabletTracking(true);
    m_LeftLayout->addWidget(m_RectangleBtn);
    m_LeftLayout->addWidget(m_EllipserBtn);
    m_LeftLayout->addWidget(m_TriangleBtn);
    m_LeftLayout->addWidget(m_LineBtn);
    m_LeftLayout->addWidget(m_TextBtn);
    m_LeftWidget->setStyleSheet(R"(border-radius:5px;)");
    // 形状工具组
    m_RectangleBtn->setText(QStringLiteral("矩形"));
    m_RectangleBtn->setIcon(QIcon(":/icon/resources/rectangleBtn.png"));
    m_RectangleBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_RectangleBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_RectangleBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    m_RectangleBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_EllipserBtn->setText(QStringLiteral("椭圆"));
    m_EllipserBtn->setIcon(QIcon(":/icon/resources/ellipseBtn.png"));
    m_EllipserBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_EllipserBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_EllipserBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    m_EllipserBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   border-radius:10px;"
                        "   color:#0066FF;"

                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_TriangleBtn->setText(QStringLiteral("三角形"));
    m_TriangleBtn->setIcon(QIcon(":/icon/resources/triangleBtn.png"));
    m_TriangleBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_TriangleBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_TriangleBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    m_TriangleBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_LineBtn->setText(QStringLiteral("直线"));
    m_LineBtn->setIcon(QIcon(":/icon/resources/lineBtn.png"));
    m_LineBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_LineBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_LineBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    m_LineBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_TextBtn->setText(QStringLiteral("文本"));
    m_TextBtn->setIcon(QIcon(":/icon/resources/textBtn.png"));
    m_TextBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_TextBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_TextBtn->setFixedSize(BTN_SIZE,BTN_SIZE);
    m_TextBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    // 操作工具组
    m_ResetBtn->setText(QStringLiteral("重置"));
    m_ResetBtn->setIcon(QIcon(":/icon/resources/resetBtn.png"));
    m_ResetBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_ResetBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ResetBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_RotateLeftBtn->setText(QStringLiteral("左旋转"));
    m_RotateLeftBtn->setIcon(QIcon(":/icon/resources/rotateLeftBtn.png"));
    m_RotateLeftBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_RotateLeftBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_RotateLeftBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_RotateRightBtn->setText(QStringLiteral("右旋转"));
    m_RotateRightBtn->setIcon(QIcon(":/icon/resources/rotateRightBtn.png"));
    m_RotateRightBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_RotateRightBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_RotateRightBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_ZoomInBtn->setText(QStringLiteral("放大"));
    m_ZoomInBtn->setIcon(QIcon(":/icon/resources/zoomInBtn.png"));
    m_ZoomInBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_ZoomInBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ZoomInBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_ZoomOutBtn->setText(QStringLiteral("缩小"));
    m_ZoomOutBtn->setIcon(QIcon(":/icon/resources/zoomOutBtn.png"));
    m_ZoomOutBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_ZoomOutBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ZoomOutBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");
    m_SaveBtn->setText(QStringLiteral("保存"));
    m_SaveBtn->setIcon(QIcon(":/icon/resources/saveBtn.png"));
    m_SaveBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_SaveBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_SaveBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_LoadBtn->setText(QStringLiteral("加载"));
    m_LoadBtn->setIcon(QIcon(":/icon/resources/loadBtn.png"));
    m_LoadBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_LoadBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_LoadBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: #FFCCFF;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");

    m_ClearBtn->setText(QStringLiteral("清空"));
    m_ClearBtn->setIcon(QIcon(":/icon/resources/clear.png"));
    m_ClearBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    m_ClearBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_ClearBtn->setStyleSheet(
                "QToolButton {"
                "   background-color: #FFCCFF;"  // 默认透明背景
                "   border: none;"                  // 移除默认边框
                "   padding: 2px;"                  // 内边距
                "   font-size: 8pt;"
                "   color:#0066FF;"
                "   border-radius:10px;"
                "}"
                "QToolButton:hover{"
                "background-color:red;"
                "border-bottom: 2px solid #FF5722;"
                "}");

    // 层级工具组
    // m_FrontBtn->setText(QStringLiteral("前置"));
    // m_FrontBtn->setIcon(QIcon(":/icon/resources/frontBtn.png"));
    // m_FrontBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    // m_FrontBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // m_BackBtn->setText(QStringLiteral("后置"));
    // m_BackBtn->setIcon(QIcon(":/icon/resources/backBtn.png"));
    // m_BackBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    // m_BackBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // 删除按钮
    // m_DeleteBtn->setText(QStringLiteral("删除"));
    // m_DeleteBtn->setIcon(QIcon(":/icon/resources/deleteBtn.png"));
    // m_DeleteBtn->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    // m_DeleteBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    m_CloseBtn->setIcon(QIcon(":/icon/resources/closeBtn.png"));
    m_CloseBtn->setIconSize(QSize(20, 20));
    m_CloseBtn->setStyleSheet(
                "QToolButton {"
                        "   background-color: transparent;"  // 默认透明背景
                        "   border: none;"                  // 移除默认边框
                        "   padding: 2px;"                  // 内边距
                        "   font-size: 8pt;"
                        "   color:#0066FF;"
                        "   border-radius:10px;"
                        "}"
                "QToolButton:hover{"
                  "background-color:red;"
                  "border-bottom: 2px solid #FF5722;"
                  "}");


    QFont font = m_RotateLeftBtn->font();
    font.setPointSize(8);  // 设置字体大小为 8 磅
    m_RotateLeftBtn->setFont(font);
    m_RotateRightBtn->setFont(font);
    m_TriangleBtn->setFont(font);

    int i = 1;
    for(QToolButton*p :m_LeftWidget->findChildren<QToolButton*>()){
        connect(p,&QToolButton::clicked,this,[this,i](){
            m_DrawWidget->setShapeType(static_cast<ShapeType>(i));
        });
        i++;
    }

    connect(m_ResetBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::reset);
    connect(m_RotateLeftBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::RotateLeft);
    connect(m_RotateRightBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::RotateRight);
    connect(m_ZoomInBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::m_ScalingIn);
    connect(m_ZoomOutBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::m_ScalingOut);
    connect(m_SaveBtn,&QToolButton::clicked,this,&MainWindow::saveFile);
    connect(m_LoadBtn,&QToolButton::clicked,this,&MainWindow::loadFile);
    connect(m_ClearBtn,&QToolButton::clicked,m_DrawWidget,&DrawWidget::clear);
    connect(m_CloseBtn,&QToolButton::clicked,[this](){
        close();
    });
}

#include <QMessageBox>

void MainWindow::saveFile()
{

    m_FilePath = QFileDialog::getExistingDirectory(this,"保存文件",QDir::currentPath());
    if(m_FilePath.isEmpty()){
        QMessageBox::warning(this,"错误","路径不存在或出错");
        return;
    }
    QString s = "paintDraw";
    QDir dir(m_FilePath);
    for(const QFileInfo&info:dir.entryInfoList()){
        if(info.baseName() == s){
            s+="X";
        }
    }

    QFile file(m_FilePath+"/"+s+".draw");
    if(file.open(QIODevice::WriteOnly)){
        auto btn = QMessageBox::question(this,"确认","是否保存文件至此？",QMessageBox::Yes | QMessageBox::No);
        if(btn == QMessageBox::No)
            return;
        QJsonArray shapesArray;
        for(auto*p :( StaticData::GetStaticData()->getVec())){
            QJsonObject json = p->toJson();
            shapesArray.append(json);
        }
        QJsonObject json;
        json["shapes"] = shapesArray;
        file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
    }

}

void MainWindow::loadFile()
{
    m_FilePath = QFileDialog::getOpenFileName(this,"加载文件",QDir::currentPath(),"draw(*.draw)");
    if(m_FilePath.isEmpty()){
        QMessageBox::warning(this,"错误","文件不存在或出错");
        return;
    }
    QFile file(m_FilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"错误","文件不存在或出错");
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(),&error);
    if(error.error != QJsonParseError::NoError){
        QMessageBox::warning(this,"错误","json解析错误");
        return;
    }
    QJsonObject root = doc.object();
    QJsonArray array = root["shapes"].toArray();
    for(const QJsonValue&val:array){
        if(val.isObject()){
            QJsonObject obj = val.toObject();
            Shape*p = StaticData::createFromJson(obj);
            StaticData::GetStaticData()->getVec().push_back(p);
        }
    }
    m_DrawWidget->update();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{

    /*
        m_RectangleBtn->setGeometry(0, BTN_SIZE * 1, BTN_SIZE, BTN_SIZE);
        m_EllipserBtn->setGeometry(0, BTN_SIZE * 2, BTN_SIZE, BTN_SIZE);
        m_TriangleBtn->setGeometry(0, BTN_SIZE * 3, BTN_SIZE, BTN_SIZE);
        m_LineBtn->setGeometry(0, BTN_SIZE * 4, BTN_SIZE, BTN_SIZE);
        m_TextBtn->setGeometry(0, BTN_SIZE * 5, BTN_SIZE, BTN_SIZE);
    */
    m_ArrowLabel->setGeometry(0,BTN_SIZE+20,m_ArrowLabel->sizeHint().width(),m_ArrowLabel->sizeHint().height());
    m_LeftWidget->setGeometry(0 - BTN_SIZE ,BTN_SIZE + 2,m_LeftWidget->sizeHint().width(),m_LeftWidget->sizeHint().height());
    m_LeftWidget->raise();
    // 操作工具按钮
    m_ResetBtn->setGeometry(0, 0, BTN_SIZE, BTN_SIZE);
    m_RotateLeftBtn->setGeometry(BTN_SIZE*1, 0, BTN_SIZE, BTN_SIZE);
    m_RotateRightBtn->setGeometry(BTN_SIZE*2,  0, BTN_SIZE, BTN_SIZE);
    m_ZoomInBtn->setGeometry(BTN_SIZE*3,  0, BTN_SIZE, BTN_SIZE);
    m_ZoomOutBtn->setGeometry(BTN_SIZE*4,  0, BTN_SIZE, BTN_SIZE);
    m_ClearBtn->setGeometry(BTN_SIZE*5,  0, BTN_SIZE, BTN_SIZE);
    m_SaveBtn->setGeometry(BTN_SIZE*6,  0, BTN_SIZE, BTN_SIZE);
    m_LoadBtn->setGeometry(BTN_SIZE*7,  0, BTN_SIZE, BTN_SIZE);
    // m_FrontBtn->setGeometry(BTN_SIZE*5,  0, BTN_SIZE, BTN_SIZE);
    // m_BackBtn->setGeometry(BTN_SIZE*6,  0, BTN_SIZE, BTN_SIZE);

    // 删除按钮
    // m_DeleteBtn->setGeometry(BTN_SIZE*7, 0, BTN_SIZE, BTN_SIZE);
    m_CloseBtn->setGeometry(this->width()-ICON_SIZE*2,ICON_SIZE-10,20,20);

    m_DrawWidget->setGeometry(5,BTN_SIZE+5,width()-10,height()-10-BTN_SIZE);    
    QWidget::resizeEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // if(!m_IsDrawing){
    //     m_DrawWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    // }else setAttribute(Qt::WA_TransparentForMouseEvents, false);

    m_MouseInArea =  event->pos().x()<=50 && event->pos().y()>=m_ArrowLabel->pos().y()&&event->pos().y() <= m_ArrowLabel->pos().y()+30;
    if(m_MouseInArea == true && m_LeftIsVisible == true){
        QWidget::mouseMoveEvent(event);
        return;
    }else if(m_MouseInArea == false && m_LeftIsVisible == false){
        QWidget::mouseMoveEvent(event);;
        return;
    }

    if(m_MouseInArea == true && m_LeftIsVisible == false){
        animation->stop();
        animation->setTargetObject(m_LeftWidget);
        animation->setPropertyName("pos");
        animation->setDuration(300);
        animation->setStartValue(QPoint(0-BTN_SIZE,BTN_SIZE + 2));
        animation->setEndValue(QPoint(0,BTN_SIZE + 2));
        animation->start();
        m_LeftIsVisible = true;
        qDebug() << m_LeftWidget->pos().x()<< "\t" << m_LeftWidget->pos().y();
        m_ArrowLabel->move(QPoint(0-30,BTN_SIZE+20));
    }else if(m_MouseInArea == false && m_LeftIsVisible == true&&!m_LeftWidget->geometry().contains(event->x(),event->y())){
        animation->stop();
        animation->setTargetObject(m_LeftWidget);
        animation->setPropertyName("pos");
        animation->setDuration(300);
        animation->setStartValue(QPoint(0,BTN_SIZE + 2));
        animation->setEndValue(QPoint(0-BTN_SIZE,BTN_SIZE + 2));
        animation->start();
        m_LeftIsVisible = false;
        qDebug() << m_LeftWidget->pos().x()<< "\t" << m_LeftWidget->pos().y();
        m_ArrowLabel->move(QPoint(0,BTN_SIZE+20));
    }

    QWidget::mouseMoveEvent(event);
}

