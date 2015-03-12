#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/highgui/highgui.hpp>
#include <QDebug>

///////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_debugIndex(0)
{
    ui->setupUi(this);

    // adding images to the GUI
    ui->imagesLayout->addWidget(&m_images[0]);
    ui->imagesLayout->addWidget(&m_images[1]);

    connect(&m_images[0], SIGNAL(onSelectionStarted()),
            &m_images[1], SLOT(releaseSelection()) );
    connect(&m_images[1], SIGNAL(onSelectionStarted()),
            &m_images[0], SLOT(releaseSelection()) );

    // debugging
    connect(&m_images[0], SIGNAL(debugMat(cv::Mat*)),
            this, SLOT(outputDebugMat(cv::Mat*)) );
    connect(&m_images[1], SIGNAL(debugMat(cv::Mat*)),
            this, SLOT(outputDebugMat(cv::Mat*)) );
}

///////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////////
void MainWindow::outputDebugMat(cv::Mat* mat)
{
    QString fn = "debug" + QString::number(m_debugIndex++) + ".jpg";
    cv::imwrite(fn.toStdString(), *mat);
}

///////////////////////////////////////////////////////////
void MainWindow::on_pbLoadImage1_clicked()
{
    onLoadImage(0);
}

///////////////////////////////////////////////////////////
void MainWindow::on_pbLoadImage2_clicked()
{
    onLoadImage(1);
}

///////////////////////////////////////////////////////////
void MainWindow::onLoadImage(unsigned short index)
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Image",
        "",
        "Image Files (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty())
    {
        m_images[index].loadImage(fileName);
    }
}

///////////////////////////////////////////////////////////
void MainWindow::on_pbCompute_clicked()
{
    // if not two images loaded, warning to user
    if (!m_images[0].isLoaded() || !m_images[1].isLoaded())
    {
        QMessageBox::warning(this, "Error", "Please load two images!");
        return;
    }

    // detect the inputs to process (selection in image and image to search in)
    QRect selections[2];
    selections[0] = m_images[0].selection();
    selections[1] = m_images[1].selection();
    unsigned int searchImage = selections[0].isEmpty() ? 0 : 1;

    // if no selection, warning to user
    if (selections[!searchImage].isEmpty())
    {
        QMessageBox::warning(this, "Error", "Please select a region to detect!");
        return;
    }

    // status info to user
    ui->statusBar->showMessage("Detecting in image " + QString::number(searchImage+1) + "...");
    ui->statusBar->repaint();

    // get the inputs for the detection
    cv::Mat* image = m_images[searchImage].toMat(); // image to search in
    cv::Mat* pattern = m_images[!searchImage].selectionToMat(); // template to search

    // run the detection
    Detection detection(image, pattern);

    cv::Mat* dbg = detection.run();
    //outputDebugMat(dbg);

    // status info to user
    ui->statusBar->clearMessage();

    if (detection.isMatched())
        m_images[searchImage].highlight(selections[!searchImage],
                                        detection.matchX(),
                                        detection.matchY());
    else
        ui->statusBar->showMessage("No match found");

    delete image;
    delete pattern;
    delete dbg;
}
