/*
 * Vertex.tcc
 *
 *  Created on: 10.02.2011
 *      Author: Thomas Wiemann
 */

#include <stdexcept>

namespace lssr
{

template<typename CoordType>
CoordType Vertex<CoordType>::operator[](const int &index) const
{
    CoordType ret = 0.0;

    switch(index){
    case 0:
        ret = x;
        break;
    case 1:
        ret = y;
        break;
    case 2:
        ret = z;
        break;
    default:
        throw std::overflow_error("Access index out of range.");
    }
    return ret;
}

template<typename CoordType>
CoordType& Vertex<CoordType>::operator[](const int &index)
{
    switch(index){
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    default:
        throw std::overflow_error("Access index out of range.");
    }
}

template<typename CoordType>
bool Vertex<CoordType>::operator==(const Vertex &other) const
{
    return fabs(x - other.x) <= Vertex::epsilon &&
           fabs(y - other.y) <= Vertex::epsilon &&
           fabs(z - other.z) <= Vertex::epsilon;
}

template<typename CoordType>
void Vertex<CoordType>::operator/=(const CoordType &scale)
{
    if(scale != 0)
    {
        x /= scale;
        y /= scale;
        z /= scale;
    }
    else
    {
        x = y = z = 0;
    }
}

template<typename CoordType>
void Vertex<CoordType>::operator*=(const CoordType &scale)
      {
    x *= scale;
    y *= scale;
    z *= scale;
}

template<typename CoordType>
void Vertex<CoordType>::operator+=(const Vertex &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

template<typename CoordType>
void Vertex<CoordType>::operator-=(const Vertex &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

template<typename CoordType>
Vertex<CoordType> Vertex<CoordType>::operator-(const Vertex &other) const
{
    return Vertex<CoordType>(x - other.x, y - other.y, z - other.z);
}

template<typename CoordType>
Vertex<CoordType> Vertex<CoordType>::operator+(const Vertex &other) const
{
    return Vertex<CoordType>(x + other.x, y + other.y, z + other.z);
}

template<typename CoordType>
Vertex<CoordType> Vertex<CoordType>::operator*(const CoordType &scale) const
{
    return Vertex<CoordType>(x * scale, y * scale, z * scale);
}

template<typename CoordType>
CoordType Vertex<CoordType>::operator*(const Vertex<CoordType> &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

template<typename CoordType>
void Vertex<CoordType>::crossTo(const Vertex<CoordType>  &other)
{
    x = y * other.z - z * other.y;
    y = z * other.x - x * other.z;
    z = x * other.y - y * other.x;
}

template<typename CoordType>
void Vertex<CoordType>::rotateCM(const Matrix4<CoordType> &m)
{
    CoordType _x, _y, _z;
    _x = x * m[0 ] + y * m[4 ] + z * m[8 ];
    _y = x * m[1 ] + y * m[5 ] + z * m[9 ];
    _z = x * m[2 ] + y * m[6 ] + z * m[10];

    x = _x;
    y = _y;
    z = _z;
}

template<typename CoordType>
void Vertex<CoordType>::rotateRM(const Matrix4<CoordType> &m)
{
    CoordType _x, _y, _z;
    _x = x * m[0 ] + y * m[1 ] + z * m[2 ];
    _y = x * m[4 ] + y * m[5 ] + z * m[6 ];
    _z = x * m[8 ] + y * m[9 ] + z * m[10];

    x = _x;
    y = _y;
    z = _z;
}


template<typename CoordType>
void Vertex<CoordType>::transformCM(const Matrix4<CoordType> &m)
{
	rotateCM(m);
	x += m[12];
	y += m[13];
	z += m[14];
}

template<typename CoordType>
void Vertex<CoordType>::transformRM(const Matrix4<CoordType> &m)
{
    rotateRM(m);
    x += m[12];
    y += m[13];
    z += m[14];
}

template<typename CoordType>
void Vertex<CoordType>::transform(const Matrix4<CoordType> &m)
{
    transformCM(m);
}

template<typename CoordType>
void Vertex<CoordType>::rotate(const Matrix4<CoordType> &m)
{
    rotateRM(m);
}

template<typename CoordType>
Vertex<CoordType> Vertex<CoordType>::cross(const Vertex<CoordType> &other) const
{
    CoordType tx = y * other.z - z * other.y;
    CoordType ty = z * other.x - x * other.z;
    CoordType tz = x * other.y - y * other.x;

    return Vertex<CoordType>(tx, ty, tz);
}

template<typename CoordType>
CoordType Vertex<CoordType>::length()
{
    return sqrt(x * x + y * y + z * z);
}


} // namespace lssr