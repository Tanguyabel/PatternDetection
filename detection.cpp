#include "detection.hh"

///////////////////////////////////////////////////////////
Detection::Detection(cv::Mat* image, cv::Mat* temp) :
    m_matched(false),
    m_matchX(0),
    m_matchY(0)
{
    setImage(image);
    setTemplate(temp);
}

///////////////////////////////////////////////////////////
void Detection::setImage(cv::Mat* image)
{
    m_image.create(image->rows, image->cols, image->type());
    image->copyTo(m_image);

    // converting to gray scale image
    cv::cvtColor(m_image, m_image, CV_RGB2GRAY);
}

///////////////////////////////////////////////////////////
void Detection::setTemplate(cv::Mat* temp)
{
    m_template.create(temp->rows, temp->cols, temp->type());
    temp->copyTo(m_template);

    // converting to grayscale
    cv::cvtColor(m_template, m_template, CV_RGB2GRAY);
}

///////////////////////////////////////////////////////////
unsigned int Detection::matchX() const
{
    return m_matchX;
}

///////////////////////////////////////////////////////////
unsigned int Detection::matchY() const
{
    return m_matchY;
}

///////////////////////////////////////////////////////////
bool Detection::isMatched() const
{
    return m_matched;
}

///////////////////////////////////////////////////////////
cv::Mat* Detection::run()
{
    ///////////////////////////////////////////////////////////
    // first we are detecting the position for which the
    // templates matches the best in the image
    ///////////////////////////////////////////////////////////

    int matchMethod = CV_TM_CCOEFF_NORMED;
    // possible distances for matchTemplate:
    // - CV_TM_SQDIFF           !not supported!
    // - CV_TM_SQDIFF_NORMED    !not supported!
    // - CV_TM_CCORR
    // - CV_TM_CCORR_NORMED
    // - CV_TM_CCOEFF
    // - CV_TM_CCOEFF_NORMED

    cv::Mat* result = new cv::Mat(m_image.cols - m_template.cols + 1,
                                  m_image.rows - m_template.rows + 1,
                                  CV_8UC1);
    matchTemplate(m_image, m_template, *result, matchMethod);

    // force conversion to 1-channel
    result->convertTo(*result, CV_8U, 255, 0);

    // normalize image
    normalize(*result, *result, 0, 255, cv::NORM_MINMAX, -1, cv::Mat());

    // get maximum and maximum from the distance map
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(*result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    // setting the matched point
    if (matchMethod == CV_TM_SQDIFF || matchMethod == CV_TM_SQDIFF_NORMED)
    {
        m_matchX = minLoc.x;
        m_matchY = minLoc.y;
    }
    else
    {
        m_matchX = maxLoc.x;
        m_matchY = maxLoc.y;
    }

    m_matched = true;

    ///////////////////////////////////////////////////////////
    // then we are trying to discard the match if it is not
    // considered as a real match, for one the following
    // reasons:
    // - too many other matches are present in the picture
    // - the shape is far from being a small circle
    ///////////////////////////////////////////////////////////

    // binarize the distance map to keep only maximum value points
    // notice: from here the approach used is not working if used with
    // CV_TM_SQDIFF*
    cv::threshold(*result, *result, 0.9*255, 0, CV_THRESH_TOZERO);

    // detect the different blobs on the distance map
    // (for debugging purpose we keep the map unchanged)
    cv::Mat contourMat(result->rows, result->cols, result->type());
    result->copyTo(contourMat);
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(contourMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    // analyse the found blobs
    unsigned int matches = contours.size();
    unsigned int badMatches = 0;
    for (unsigned int i = 0; i < matches; ++i)
    {
        // fitEllipse requires more than 5 points; under that is considered as noise
        // note: if noise is present the number of matches wil be high and we'll
        // consider there is no match (line 140)
        if (contours[i].size() > 5)
        {
            // a good match is pretty much a small circle so we discard what is not
            // by detecting the bounding box and its dimension rate
            cv::RotatedRect rotRec = cv::fitEllipse(contours[i]);
            cv::Rect bounding = rotRec.boundingRect();
            double r = double(bounding.width) / double(bounding.height);
            if (r < 0.2 || r > 5)
                ++badMatches;
        }
    }

    if (badMatches > 0 || matches > 5)
        m_matched = false;

    // return the distance map for debugging purpose
    return result;
}
