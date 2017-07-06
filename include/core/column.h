#pragma once
#include <QtCore>

//用于进行列操作的Column类型, E必须是int,double,float三种类型中的一种
template<typename E>
struct Column {
    template<typename T>
    friend Column<T> operator + (const Column<T> &lhs, const Column<T> &rhs);
    template<typename T>
    friend Column<T> operator + (const Column<T> &lhs, const double &rhs);
    template<typename T>
    friend Column<T> operator + (const double &lhs, const Column<E> &rhs);
    template<typename T>
    friend Column<T> operator - (const Column<T> &lhs, const Column<T> &rhs);
    template<typename T>
    friend Column<T> operator - (const Column<T> &lhs, const double &rhs);
    template<typename T>
    friend Column<T> operator * (const Column<T> &lhs, const Column<T> &rhs);
    template<typename T>
    friend Column<T> operator * (const Column<T> &lhs, const double &rhs);
    template<typename T>
    friend Column<T> operator * (const double &lhs, const Column<E> &rhs);
    template<typename T>
    friend Column<T> operator - (const Column<T> &lhs, const Column<T> &rhs);
    template<typename T>
    friend Column<T> operator - (const Column<T> &lhs, const double &rhs);
protected:
    QVector<E> _data;
public:
    Column();
    explicit Column(const QVector<E> &columnData);
    Column(const unsigned int size, const E data);
    Column(const Column<E> &column);
    void changeData(const QVector<E> &columnData);
    QVector<E>& data();
    int size() const;
    void push_back(const E data);
    void pop_back();
    void clear();
    bool isEmpty() const;
    void replace(const E &oldData, const E &newData);
    void replace(std::function<bool(E)> condition, const E &newData);
    E sum() const;
    Column<E>& operator = (const Column<E> &rhs);
    Column<E>& operator += (const Column<E> &rhs);
    Column<E>& operator += (const double &rhs);
    Column<E>& operator -= (const Column<E> &rhs);
    Column<E>& operator -= (const double &rhs);
    Column<E>& operator *= (const Column<E> &rhs);
    Column<E>& operator *= (const double &rhs);
    Column<E>& operator /= (const Column<E> &rhs);
    Column<E>& operator /= (const double &rhs);
    E& operator [] (const int index);
};

template<typename E>
Column<E>::Column()
{
    _data.clear();
}

template<typename E>
Column<E>::Column(const QVector<E> &columnData)
{
    if (!columnData.isEmpty())
    {
        _data.clear();
        _data = columnData;
    }
    else
    {
        _data.clear();
    }
}

template<typename E>
Column<E>::Column(const unsigned int size, const E data)
{
    _data.clear();
    if (size != 0)
    {
        _data = QVector<E>(size, data);
    }
}

template<typename E>
Column<E>::Column(const Column<E> &column)
{
    *this = column;
}

template<typename E>
void Column<E>::changeData(const QVector<E> &columnData)
{
    Column newColumn(columnData);
    *this = newColumn;
}


template<typename E>
QVector<E>& Column<E>::data()
{
    return _data;
}

template<typename E>
int Column<E>::size() const
{
    return _data.size();
}

template<typename E>
void Column<E>::push_back(const E data)
{
    _data.push_back(data);
}

template<typename E>
void Column<E>::pop_back()
{
    _data.pop_back();
}

template<typename E>
void Column<E>::clear()
{
    _data.clear();
}

template<typename E>
bool Column<E>::isEmpty() const
{
    return _data.isEmpty();
}

template<typename E>
void Column<E>::replace(const E &oldData, const E &newData)
{
    for (auto &data: _data)
    {
        data = (data != oldData) ? data : newData;
    }
}

template<typename E>
void Column<E>::replace(std::function<bool(E)> condition, const E &newData)
{
    for (auto &data: _data)
    {
        data = (condition(data)) ? newData : data;
    }
}

template<typename E>
E Column<E>::sum() const
{
    E sum = 0;
    for (auto &data: _data)
    {
        sum += data;
    }
    return sum;
}

template<typename E>
Column<E>& Column<E>::operator = (const Column<E> &rhs)
{
    if (this != &rhs)
    {
        this->_data = rhs._data;
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator += (const Column<E> &rhs)
{
    if (rhs.size() > 0)
    {
        int modNum = rhs.size();
        for (int index = 0; index < _data.size(); index++)
        {
            _data[index] += rhs._data[index%modNum];
        }
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator += (const double &rhs)
{

    for (int index = 0; index < _data.size(); index++)
    {
        _data[index] += rhs;
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator -= (const Column<E> &rhs)
{
    if (rhs.size() > 0)
    {
        int modNum = rhs.size();
        for (int index = 0; index < _data.size(); index++)
        {
            _data[index] -= rhs._data[index%modNum];
        }
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator -= (const double &rhs)
{
    for (int index = 0; index < _data.size(); index++)
    {
        _data[index] -= rhs;
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator *= (const Column<E> &rhs)
{
    if (rhs.size() > 0)
    {
        int modNum = rhs.size();
        for (int index = 0; index < _data.size(); index++)
        {
            _data[index] *= rhs._data[index%modNum];
        }
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator *= (const double &rhs)
{
    for (int index = 0; index < _data.size(); index++)
    {
        _data[index] *= rhs;
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator /= (const Column<E> &rhs)
{
    if (rhs.size() > 0)
    {
        int modNum = rhs.size();
        for (int index = 0; index < _data.size(); index++)
        {
            if (rhs._data[index%modNum] != 0)
            {
                _data[index] /= rhs._data[index%modNum];
            }
            else
            {
                _data[index] = 0;
            }
        }
    }
    return *this;
}

template<typename E>
Column<E>& Column<E>::operator /= (const double &rhs)
{
    for (int index = 0; index < _data.size(); index++)
    {
        _data[index] = (rhs != 0) ? (_data[index] / rhs) : _data[index];
    }
    return *this;
}

template<typename E>
E& Column<E>::operator [] (const int index)
{
    return _data[index];
}

//------------------------------------friend functions------------------------------------
//operator +
template<typename T>
Column<T> operator + (const Column<T> &lhs, const Column<T> &rhs)
{
    Column<T> res(lhs);
    res += rhs;
    return res;
}

template<typename T>
Column<T> operator + (const Column<T> &lhs, const double &rhs)
{
    Column<T> res(lhs);
    res += rhs;
    return res;
}

template<typename T>
Column<T> operator + (const double &lhs, const Column<T> &rhs)
{
    Column<T> res(rhs);
    res += lhs;
    return res;
}

//operator -
template<typename T>
Column<T> operator - (const Column<T> &lhs, const Column<T> &rhs)
{
    Column<T> res(lhs);
    res -= rhs;
    return res;
}

template<typename T>
Column<T> operator - (const Column<T> &lhs, const double &rhs)
{
    Column<T> res(lhs);
    res -= rhs;
    return res;
}

//operator *
template<typename T>
Column<T> operator * (const Column<T> &lhs, const Column<T> &rhs)
{
    Column<T> res(lhs);
    res *= rhs;
    return res;
}

template<typename T>
Column<T> operator * (const Column<T> &lhs, const double &rhs)
{
    Column<T> res(lhs);
    res *= rhs;
    return res;
}

template<typename T>
Column<T> operator * (const double &lhs, const Column<T> &rhs)
{
    Column<T> res(rhs);
    res *= lhs;
    return res;
}

//operator /
template<typename T>
Column<T> operator / (const Column<T> &lhs, const Column<T> &rhs)
{
    Column<T> res(lhs);
    res /= rhs;
    return res;
}

template<typename T>
Column<T> operator / (const Column<T> &lhs, const double &rhs)
{
    Column<T> res(lhs);
    res /= rhs;
    return res;
}
