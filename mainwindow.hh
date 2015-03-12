/*
 * MainWindow
 *
 * GUI system and user interactions
 */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QLabel>

#include "imagecontainer.hh"
#include "detection.hh"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    // on click on button to browse file for image 1
    void on_pbLoadImage1_clicked();

    // on click on button to browse file for image 2
    void on_pbLoadImage2_clicked();

    // on click on button to compute
    void on_pbCompute_clicked();

    // generic function to load image from file (index = 0|1)
    void onLoadImage(unsigned short index);

    // print a mat in a file for debug
    void outputDebugMat(cv::Mat*);

private:
    Ui::MainWindow* ui;
    unsigned int    m_debugIndex;

    ImageContainer  m_images[2];
};

#endif // MAINWINDOW_HH
