/*
 * Detection
 *
 * An extended QLabel intended to support user selection and cv::Mat conversions
 */


#ifndef IMAGECONTAINER_HH
#define IMAGECONTAINER_HH

#include <QLabel>
#include <QRect>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ImageContainer : public QLabel
{
    Q_OBJECT

public:
    // default parent constructor
    ImageContainer(QWidget * parent = 0, Qt::WindowFlags f = 0 );

public slots:
    // loads an image file
    void loadImage(QString filename);

    // reimplements mousePressEvent to handle the click
    void mousePressEvent(QMouseEvent* event);

    // reimplements mouseReleaseEvent to handle the click
    void mouseReleaseEvent(QMouseEvent* event);

    // reimplements mouseMoveEvent to handle the selection
    void mouseMoveEvent(QMouseEvent* event);

    // reimplements paintEvent to draw the selection
    void paintEvent(QPaintEvent* event);

    // release selection
    void releaseSelection();

    // convert the image converted into cv::mat
    cv::Mat* toMat();

    // returns the selection converted into a cv::mat image, or null if no selection
    cv::Mat* selectionToMat();

    // set image from a cv::mat (for debug purpose)
    void setFromMat(const cv::Mat&);

    // returns the selected area (empty if non existing)
    QRect& selection();

    // return true if an image has been loaded, false otherwise
    bool isLoaded() const;

    // return the real size of the image (eventually different from displayed size)
    QSize size() const;

    // highlight a selection
    void highlight(const QRect& selection, unsigned int x, unsigned int y);

signals:
    // emitted when user started a selection
    void onSelectionStarted();

    // emitted when a mat can be displayed for debug purpose
    void debugMat(cv::Mat*);

private:
    bool        m_selectionOn;
    bool        m_highlightOn;
    QImage      m_image;
    QPixmap     m_pixmap;
    QRect       m_selectionRect;
    QSizeF      m_scale;
};

#endif // IMAGECONTAINER_HH
