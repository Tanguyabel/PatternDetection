/*
 * Detection
 *
 * The algorithm for detecting a pattern in an image
 */

#ifndef DETECTAREA_HH
#define DETECTAREA_HH

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class Detection
{
public:
    /// constructor taking an image and a template to detect
    Detection(cv::Mat *image, cv::Mat *temp);

    /// set the image to search in
    void setImage(cv::Mat* image);

    /// set the image to detect
    void setTemplate(cv::Mat* temp);

    /// start to compute the detection
    /// for debugging purpose, returns a mat of the distances map computed
    cv::Mat* run();

    /// return x coordinate of the matched selection, in pixels
    unsigned int matchX() const;

    /// return y coordinate of the matched selection, in pixels
    unsigned int matchY() const;

    /// returns true if a match have been found, false otherwise
    bool isMatched() const;

private:
    bool            m_matched;
    unsigned int    m_matchX, m_matchY;

    cv::Mat         m_image;
    cv::Mat         m_template;
};

#endif // DETECTAREA_HH
