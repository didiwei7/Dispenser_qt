#include "photo.h"

Photo::Photo(QWidget *parent) : QWidget(parent)
{
    setupUi();
    setConnect();
}


void Photo::setupUi()
{
    setEdgeDetection();

    // 【1】 导航栏
    vnavigationbar = new QVNavigationBar(this);
    vnavigationbar->setSelectColor("#FFC0CB");
    vnavigationbar->setRowHeight(30);
    vnavigationbar->setWidth(80);
    vnavigationbar->addItem(QStringLiteral("边缘检测"));
    vnavigationbar->addItem(QStringLiteral("凸包检测"));
    vnavigationbar->addItem(QStringLiteral("其他"));

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setFixedWidth(170);
    stackedWidget->insertWidget(0, w_EdgeDetection);
    stackedWidget->insertWidget(1, new QLabel(QStringLiteral("凸包检测")));
    stackedWidget->insertWidget(2, new QLabel(QStringLiteral("其他")));

    // 【2】 布局
    label_pic = new QLabel("...", this);
    label_pic->setStyleSheet("background-color: rgb(170, 255, 127);");

    QHBoxLayout *layout_1   = new QHBoxLayout();
    layout_1->setContentsMargins(0, 0, 0, 0);
    layout_1->setSpacing(0);

    layout_1->addWidget(label_pic);
    layout_1->addWidget(stackedWidget);
    layout_1->addWidget(vnavigationbar);

    this->setLayout(layout_1);

}

void Photo::setConnect()
{
    // 【1】 导航栏
    connect(vnavigationbar, &QVNavigationBar::currentItemChanged,
            stackedWidget,  &QStackedWidget::setCurrentIndex);

    // 【2】 边缘检测
    connect(btn_openfile, &QPushButton::clicked, this, &Photo::on_openFile);
    connect(slider_burl, &QMySlider::sliderValueChanged, this, &Photo::updateEdgeDetection);
    connect(slider_canny_threshold1, &QMySlider::sliderValueChanged,
            this, &Photo::updateEdgeDetection);
    connect(slider_canny_threshold2, &QMySlider::sliderValueChanged,
            this, &Photo::updateEdgeDetection);
}


void Photo::setEdgeDetection()
{
    // 滤波
    slider_burl = new QMySlider();
    slider_burl->setText(QStringLiteral("均值滤波Burl"));
    slider_burl->setRange(1,100);

    // 检测
    slider_canny_threshold1 = new QMySlider();
    slider_canny_threshold2 = new QMySlider();
    slider_canny_threshold1->setText(QStringLiteral("Canny threshold1"));
    slider_canny_threshold2->setText(QStringLiteral("Canny threshold2"));
    slider_canny_threshold1->setRange(1, 200);
    slider_canny_threshold2->setRange(1, 200);

    // 复选, 打开
    btn_canny = new QCheckBox("Canny");
    btn_openfile = new QPushButton("Open");

    // 布局
    QVBoxLayout *layout_1 = new QVBoxLayout();
    layout_1->setSpacing(15);
    layout_1->addWidget(slider_burl);
    layout_1->addWidget(slider_canny_threshold1);
    layout_1->addWidget(slider_canny_threshold2);
    layout_1->addWidget(btn_canny);
    layout_1->addWidget(btn_openfile);
    layout_1->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    w_EdgeDetection = new QWidget(this);
    w_EdgeDetection->setLayout(layout_1);

}


void Photo::updateEdgeDetection()
{
    Mat img_copy = img_src.clone();

    if(!btn_canny->isChecked())
    {
        int value_burl  = slider_burl ->currentValue();
        blur(img_copy, img_copy, Size(value_burl, value_burl));
        show_cvImg_to_label(img_copy);
    }
    else
    {
        int value_burl  = slider_burl ->currentValue();
        int value_threshold1 = slider_canny_threshold1->currentValue();
        int value_threshold2 = slider_canny_threshold2->currentValue();

        // 【1】 转换为灰度
        cvtColor(img_copy, img_copy, COLOR_BGR2GRAY);

        // 【2】 滤波, 使用Canny算子
        blur(img_copy, img_copy, Size(value_burl, value_burl));
        Canny(img_copy, img_copy, value_threshold1, value_threshold2, 3);

        // 【3】 创建一个矩阵来存放它
        Mat dst;
        dst.create(img_src.size(), img_src.type());
        dst = Scalar::all(0);

        // 【4】 拷贝到输出图像
        img_src.copyTo(dst, img_copy);

        // 【5】 显示图像
        show_cvImg_to_label(dst);
    }
}

void Photo::on_openFile()
{
    // 【1】 打开文件对话框,获取文件路径
    const QStringList imgSysPaths = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QString imgPath = QFileDialog::getOpenFileName(NULL, QStringLiteral("Open File"),
                            imgSysPaths.isEmpty() ? QDir::homePath() : imgSysPaths.first(),
                            QStringLiteral("Image files (*.png *.bmp *.jpg *.jpeg) ;; All files (*.*)"));

    if(imgPath.isEmpty())
    {
        qDebug() << QStringLiteral("打开失败");
        return;
    }

    // 【2】 获取Mat型数据
    img_src = imread(imgPath.toStdString());
    Mat img_src_copy = img_src.clone();

    // 【3】 显示在Label中
    cvtColor(img_src_copy, img_src_copy, 4, 0);

    img_show_src = QImage(img_src_copy.data, img_src_copy.cols, img_src_copy.rows,
                          img_src_copy.cols * img_src_copy.channels(),
                          QImage::Format_RGB888);

    label_pic->clear();
    label_pic->setPixmap(QPixmap::fromImage(img_show_src));
    label_pic->resize(label_pic->pixmap()->size());
}

/** 显示Mat型矩阵到Label中
 * @brief
 * @arguments
            Mat img 输入图像
 * @return  NULL
 * @author  Didiwei
 * @date    2017-12-08
 */
void Photo::show_cvImg_to_label(Mat img)
{
    // 【1】 转换为 rgb
    cvtColor(img, img, CV_BGR2RGB);

    // 【2】 将图像数据Copy到QImage中
    QImage qimg = QImage(img.data, img.cols, img.rows, img.cols * img.channels(), QImage::Format_RGB888);

    // 【3】 在Label中显示
    label_pic->clear();
    label_pic->setPixmap(QPixmap::fromImage(qimg));
    label_pic->resize(label_pic->pixmap()->size());
}
