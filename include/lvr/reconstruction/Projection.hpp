#ifndef PROJECTION_H
#define PROJECTION_H
#include <cmath>

#include <lvr/reconstruction/ModelToImage.hpp>

namespace lvr
{

class Projection
{
public:

    Projection(int width, int height, int minH, int maxH, int minV, int maxV, bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

    virtual void project(int&i , int&j, float& r, float x, float y, float z) = 0;

    int w() { return m_width;}
    int h() { return m_height;}

protected:

    inline float toPolar(const float point[], float polar[]);

    float       m_xSize;
    float       m_ySize;
    float       m_xFactor;
    float       m_yFactor;
    int         m_width;
    int         m_height;
    float       m_minH;
    float       m_maxH;
    float       m_minV;
    float       m_maxV;

    bool        m_optimize;
    ModelToImage::CoordinateSystem        m_system;

    void setImageRatio();

    static constexpr float m_ph = 1.570796327;
};

class EquirectangularProjection: public Projection
{
public:
    EquirectangularProjection(int width, int height,
                              int minH, int maxH,
                              int minV, int maxV,
                              bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

    virtual void project(int&i , int&j, float& r, float x, float y, float z) override;

protected:
    float       m_xFactor;
    float       m_yFactor;
    int         m_maxWidth;
    int         m_maxHeight;
    float       m_lowShift;
};

class MercatorProjection: public Projection
{
public:
    MercatorProjection(int width, int height,
                       int minH, int maxH,
                       int minV, int maxV,
                       bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);
protected:
    float       m_heightLow;
    int         m_maxWidth;
    int         m_maxHeight;
};

class CylindricalProjection: public Projection
{
public:
    CylindricalProjection(int width, int height,
                          int minH, int maxH,
                          int minV, int maxV,
                          bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

protected:
    float       m_heightLow;
    int         m_maxWidth;
    int         m_maxHeight;
};


class ConicProjection : public Projection
{
public:
    ConicProjection(int width, int height,
                    int minH, int maxH,
                    int minV, int maxV,
                    bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

protected:
    float       m_lat0;
    float       m_long0;
    float       m_lat1;
    float       m_phi1;
    float       m_phi2;
    float       m_n;
    float       m_c;
    float       m_rho0;
    float       m_maxX;
    float       m_minX;
    float       m_minY;
    float       m_maxY;
    int         m_maxWidth;
    int         m_maxHeight;

};

class RectilinearProjection : public Projection
{
public:
    RectilinearProjection(int width, int height,
                          int minH, int maxH,
                          int minV, int maxV,
                          bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);
protected:
    float       m_interval;
    float       m_iMinY;
    float       m_iMaxY;
    float       m_iMinX;
    float       m_iMaxX;
    float       m_coscRectilinear;
    float       m_l0;
    float       m_coscRectlinear;
    float       m_max;
    float       m_min;
    float       m_p1;

};

class PanniniProjection: public Projection
{
public:
    PanniniProjection(int width, int height,
                      int minH, int maxH,
                      int minV, int maxV,
                      bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

protected:
    float       m_interval;
    float       m_iMinY;
    float       m_iMaxY;
    float       m_iMinX;
    float       m_iMaxX;
    float       m_max;
    float       m_min;
    float       m_l0;
    float       m_sPannini;
    float       m_p1;
};

class StereographicProjection: public Projection
{
public:
    StereographicProjection(int width, int height,
                            int minH, int maxH,
                            int minV, int maxV,
                            bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

protected:
    float       m_interval;
    float       m_iMinY;
    float       m_iMaxY;
    float       m_iMinX;
    float       m_iMaxX;
    float       m_max;
    float       m_min;
    float       m_l0;
    float       m_p1;
    float       m_k;
};

class AzimuthalProjection: public Projection
{
public:
    AzimuthalProjection(int width, int height,
                        int minH, int maxH,
                        int minV, int maxV,
                        bool optimize, ModelToImage::CoordinateSystem system = ModelToImage::NATIVE);

protected:
    float       m_kPrime;
    float       m_long0;
    float       m_phi1;
    float       m_maxX;
    float       m_minX;
    float       m_minY;
    float       m_maxY;
    int         m_maxWidth;
    int         m_maxHeight;
};


} // namespace lvr

#include "Projection.tcc"

#endif // PROJECTION_H
