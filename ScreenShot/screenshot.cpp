#include <QtWidgets>

#include "screenshot.h"
#include <QSpinBox>
#include <QLabel>

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QUrl>
#include <QTemporaryFile>
#include <QImageReader>


Screenshot::Screenshot()
    :  screenshotLabel(new QLabel(this))
{
    screenshotLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    screenshotLabel->setAlignment(Qt::AlignCenter);

    const QRect screenGeometry = this->screen()->geometry();
    screenshotLabel->setMinimumSize(screenGeometry.width() / 8,screenGeometry.height() /8);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout -> addWidget(screenshotLabel);

    QGroupBox*optionsGroupBox = new QGroupBox("Options",this);
    optionsGroupBox->setAlignment(Qt::AlignHCenter);

    delaySpinBox = new QSpinBox(optionsGroupBox);
    delaySpinBox->setSuffix("s");
    delaySpinBox->setMaximum(60);

    connect(delaySpinBox,static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&Screenshot::updateCheckBox);
    hideThisWindowCheckBox = new QCheckBox("Hide this window",optionsGroupBox);

    QGridLayout *optionsGroupBoxLayout = new QGridLayout(optionsGroupBox);
    // 为GridLayout中的每个widget添加边框
    optionsGroupBox->setStyleSheet(
        "QGroupBox {"
        "    border: 1px solid #ccc;"
        "    border-radius: 4px;"
        "    margin-top: 3ex;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top center;"
        "    padding: 0 5px;"
        "}"
        "QLabel, QSpinBox, QCheckBox {"
        "    border: 1px solid #ddd;"
        "    padding: 4px;"
        "    margin: 1px;"
        "    background-color: #f9f9f9;"
        "}"
        );
    optionsGroupBoxLayout->addWidget(new QLabel("Screenshot Delay:",this),0,0);
    optionsGroupBoxLayout->addWidget(delaySpinBox,0,1);
    optionsGroupBoxLayout->addWidget(hideThisWindowCheckBox,1,0,1,2);

    mainLayout->addWidget(optionsGroupBox);

    QHBoxLayout*buttonsLayout = new QHBoxLayout;
    newScreenshotButton = new QPushButton("New Screenshot",this);
    connect(newScreenshotButton,&QPushButton::clicked,this,&Screenshot::newScreenshot);
    buttonsLayout->addWidget(newScreenshotButton);
    QPushButton*saveScreenshotButton = new QPushButton("Save Screenshot",this);
    connect(saveScreenshotButton,&QPushButton::clicked,this,&Screenshot::saveScreenshot);
    buttonsLayout->addWidget(saveScreenshotButton);
    QPushButton*quitScreenButton = new QPushButton("Quit",this);
    quitScreenButton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(quitScreenButton,&QPushButton::clicked,this,&QWidget::close);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(quitScreenButton);
    mainLayout->addLayout(buttonsLayout);

    shootScreen();
    setWindowTitle("Screenshot");
    resize(300,200);
}

void Screenshot::resizeEvent(QResizeEvent *event)
{
    QSize scaledSize = originalPixmap.size();
    scaledSize = scaledSize.scaled(screenshotLabel->size(),Qt::KeepAspectRatio);
    if(scaledSize != screenshotLabel->pixmap().size()){
        updateScreenshotLabel();
    }

}

void Screenshot::newScreenshot()
{
    if(hideThisWindowCheckBox->isChecked()){
        hide();
    }
    newScreenshotButton->setDisabled(true);
    QTimer::singleShot(delaySpinBox->value()*1000,this,&Screenshot::shootScreen);
}

void Screenshot::saveScreenshot()
{
    const QString format = "png";
    QString initialPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir dir(initialPath);
    if (!dir.exists()) {
        initialPath = QDir::home().absoluteFilePath("Images");
    }

    // 基础文件名（不包含序号）
    QString baseName = "pic";
    QString fullPath;
    int counter = 1;

    // 构建初始路径
    fullPath = QDir(initialPath).filePath(baseName + "." + format);

    // 检查文件是否存在，如果存在则添加序号
    QFileInfo fileInfo(fullPath);
    while (fileInfo.exists()) {
        fullPath = QDir(initialPath).filePath(baseName + QString::number(counter) + "." + format);
        fileInfo.setFile(fullPath);
        counter++;
    }

    QFileDialog fileDialog(this,"Save as",fullPath);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setDirectory(fullPath);

    QStringList mimeTypes;
    const QList<QByteArray>baMimeTypes = QImageWriter::supportedMimeTypes();
    for(const QByteArray&bf:baMimeTypes){
        mimeTypes.append(QLatin1String(bf));
    }

    fileDialog.setMimeTypeFilters(mimeTypes);
    fileDialog.selectMimeTypeFilter("image/" + format);
    fileDialog.setDefaultSuffix(format);
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    const QString fileName = fileDialog.selectedFiles().first();
    if (!originalPixmap.save(fileName)) {
        QFile file(fileName);
        QFileDevice::FileError error = file.error();
        QString errorString = file.errorString();
        qDebug() << errorString;
        qDebug() << "Saving to:" << fileName;
        qDebug() << "File path exists:" << QFileInfo(fileName).path();
        qDebug() << "Is writable:" << QFileInfo(fileName).isWritable();
        qDebug() << "Supported formats:" << QImageWriter::supportedImageFormats();
        QMessageBox::warning(this, tr("Save Error"),
                             tr("The image could not be saved to \"%1\".\n\n"
                                "Error: %2\n"
                                "Error code: %3")
                                 .arg(QDir::toNativeSeparators(fileName))
                                 .arg(errorString)
                                 .arg(error));
    }

}

void Screenshot::shootScreen()
{
    QScreen*screen = QGuiApplication::primaryScreen();
    if(const QWindow*window = windowHandle()){
        screen = window->screen();
    }
    if(!screen)
        return;

    if(delaySpinBox->value()!=0){
        QApplication::beep();
    }

    originalPixmap = screen->grabWindow(0);
    updateScreenshotLabel();
    newScreenshotButton->setDisabled(false);
    if(hideThisWindowCheckBox->isChecked()){
        show();
    }
}

void Screenshot::updateCheckBox(int value)
{
    if(delaySpinBox->value() == 0){
        hideThisWindowCheckBox->setDisabled(true);
        hideThisWindowCheckBox->setChecked(false);
    }else{
        hideThisWindowCheckBox->setDisabled(false);
    }
}

void Screenshot::updateScreenshotLabel()
{
    screenshotLabel->setPixmap(originalPixmap.scaled(screenshotLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}
