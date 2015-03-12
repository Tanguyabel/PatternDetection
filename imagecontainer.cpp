#include "imagecontainer.hh"
#include <QMouseEvent>
#include <QPainter>

#define FIXED_HEIGHT 500 // used to scale to large pictures

///////////////////////////////////////////////////////////
ImageContainer::ImageContainer(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent, f),
    m_selectionOn(false),
    m_highlightOn(false)
{
}

///////////////////////////////////////////////////////////
void ImageContainer::releaseSelection()
{
    m_selectionOn = false;
    m_selectionRect = QRect(); // clearing selection area
    this->repaint();
}

///////////////////////////////////////////////////////////
void ImageContainer::loadImage(QString filename)
{
    m_pixmap.load(filename);
    m_image = m_pixmap.toImage();

    if (m_pixmap.height() > FIXED_HEIGHT)
        m_pixmap = m_pixmap.scaledToHeight(FIXED_HEIGHT);

    m_scale = QSize(m_image.width() / m_pixmap.width(),
                    m_image.height() / m_pixmap.height());

    this->setPixmap(m_pixmap);
}

///////////////////////////////////////////////////////////
void ImageContainer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit onSelectionStarted();
        m_selectionOn = true;
        m_highlightOn = false;
        m_selectionRect.setTopLeft(event->pos());
    }
}

///////////////////////////////////////////////////////////
void ImageContainer::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_selectionOn)
    {
        // if selection was on, make the selected rect valid if not
        if (m_selectionRect.width() < 0)
        {
            m_selectionRect = QRect(m_selectionRect.topRight(),
                                    QSize(-m_selectionRect.width(),
                                          m_selectionRect.height())
                                    );
        }
        if (m_selectionRect.height() < 0)
        {
            m_selectionRect = QRect(m_selectionRect.bottomLeft(),
                                    QSize(m_selectionRect.width(),
                                          -m_selectionRect.height())
                                    );
        }
    }

    if (event->button() == Qt::LeftButton)
        m_selectionOn = false;
}

///////////////////////////////////////////////////////////
void ImageContainer::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_selectionOn)
        return;

    m_selectionRect.setBottomRight(event->pos());
    this->repaint();
}

///////////////////////////////////////////////////////////
cv::Mat* ImageContainer::toMat()
{
    cv::Mat mat = cv::Mat(m_image.height(),
                          m_image.width(),
                          CV_8UC4,
                          (uchar*)m_image.bits(),
                          m_image.bytesPerLine());
    cv::cvtColor(mat, mat, CV_RGB2BGR);
    cv::Mat* mat2 = new cv::Mat(mat.rows, mat.cols, CV_8UC3);

    int mixChannelsArray[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, mat2, 1, mixChannelsArray, 3);

    return mat2;
}


///////////////////////////////////////////////////////////
void ImageContainer::setFromMat(const cv::Mat& mat)
{
    m_pixmap = QPixmap::fromImage(QImage((unsigned char*) mat.data,
    mat.cols,
    mat.rows,
    QImage::Format_RGB888));

    m_image = m_pixmap.toImage();

    if (m_pixmap.height() > FIXED_HEIGHT)
        m_pixmap = m_pixmap.scaledToHeight(FIXED_HEIGHT);
    this->repaint();
}

///////////////////////////////////////////////////////////
void ImageContainer::paintEvent(QPaintEvent* event)
{
    if (m_pixmap.isNull())
        return;

    QPainter painter;
    painter.begin(this);

    painter.drawPixmap(0, 0, m_pixmap);

    if (m_selectionOn)
        painter.fillRect(m_selectionRect, QColor(0, 0, 255, 100));
    else if (m_highlightOn)
        painter.fillRect(m_selectionRect, QColor(0, 255, 0, 100));

    painter.end();
}

///////////////////////////////////////////////////////////
QRect& ImageContainer::selection()
{
    return m_selectionRect;
}

///////////////////////////////////////////////////////////
bool ImageContainer::isLoaded() const
{
    return !m_pixmap.isNull();
}

///////////////////////////////////////////////////////////
QSize ImageContainer::size() const
{
    return m_image.size();
}

///////////////////////////////////////////////////////////
cv::Mat* ImageContainer::selectionToMat()
{
    if (m_selectionRect.isEmpty())
        return NULL;

    cv::Rect rect = cv::Rect(m_selectionRect.x() * m_scale.width(),
                             m_selectionRect.y() * m_scale.height(),
                             m_selectionRect.width() * m_scale.width(),
                             m_selectionRect.height() * m_scale.height());

    cv::Mat& image = *toMat();
    cv::Mat* mat = new cv::Mat(image.rows, image.cols, image.type());

    cv::Mat cropped = image(rect);
    cropped.copyTo(*mat);

    return mat;
}

///////////////////////////////////////////////////////////
void ImageContainer::highlight(const QRect& selection, unsigned int x, unsigned int y)
{
    m_selectionOn = false;
    m_highlightOn = true;

    QPoint p(x / m_scale.width(), y / m_scale.height());
    m_selectionRect = QRect(p, selection.size());

    repaint();
}
