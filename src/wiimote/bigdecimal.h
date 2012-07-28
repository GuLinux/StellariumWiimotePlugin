#ifndef BIGDECIMAL_H
#define BIGDECIMAL_H

#include <QString>
#include <cln/cln.h>

class BigDecimal
{
public:
    BigDecimal();
    BigDecimal(double doubleValue);
    QString toString(int precision=-1) const;
    operator QString() const;
    double toDouble() const;

    BigDecimal operator +(const BigDecimal &other) const;
    BigDecimal operator -(const BigDecimal &other) const;
    BigDecimal operator *(const BigDecimal &other) const;
    BigDecimal operator /(const BigDecimal &other) const;
    BigDecimal operator +=(const BigDecimal &other);
    BigDecimal operator -=(const BigDecimal &other);
    BigDecimal operator *=(const BigDecimal &other);
    BigDecimal operator /=(const BigDecimal &other);
    bool operator ==(const BigDecimal &other) const;
private:
    BigDecimal(const cln::cl_F &internal);
    cln::cl_F m_internal;
};

#endif // BIGDECIMAL_H
