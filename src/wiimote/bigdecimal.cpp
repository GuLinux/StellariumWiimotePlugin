#include "bigdecimal.h"
#include <string>
#include <sstream>
#include <QMetaType>

Q_DECLARE_METATYPE(BigDecimal)
class _RegisterBigDecimalMetatype {
public:
    _RegisterBigDecimalMetatype() { qRegisterMetaType<BigDecimal>(); }
};

static _RegisterBigDecimalMetatype _regBigDecimal;

using namespace cln;

#define op(symbol, other) return BigDecimal(this->m_internal symbol other.m_internal)
#define op_eq(symbol, other) this->m_internal symbol other.m_internal; return (*this);

BigDecimal::BigDecimal()
    : m_internal(cl_F((double) 0.0))
{
}

BigDecimal::BigDecimal(double doubleValue)
    : m_internal(cl_F(doubleValue))
{
}

BigDecimal::BigDecimal(const cl_F &internal)
    : m_internal(internal)
{
}

QString BigDecimal::toString(int precision) const
{
    if(precision>-1) {
        return QString::number(toDouble(), 'g', precision);
    }
    std::ostringstream a;
    a << m_internal;
    return QString::fromStdString(a.str());
}

BigDecimal::operator QString() const
{
    return toString();
}

BigDecimal BigDecimal::operator -(const BigDecimal &other) const
{
    op(-, other);
}

BigDecimal BigDecimal::operator *(const BigDecimal &other) const
{
    op(*, other);
}

BigDecimal BigDecimal::operator *=(const BigDecimal &other)
{
    op_eq(*=, other);
}

BigDecimal BigDecimal::operator /=(const BigDecimal &other)
{
    op_eq(/=, other);
}

bool BigDecimal::operator ==(const BigDecimal &other) const
{
    return this->m_internal == other.m_internal;
}

BigDecimal BigDecimal::operator /(const BigDecimal &other) const
{
    op(/, other);
}

BigDecimal BigDecimal::operator +(const BigDecimal &other) const
{
    op(+, other);
}

BigDecimal BigDecimal::operator +=(const BigDecimal &other)
{
    op_eq(+=, other);
}

BigDecimal BigDecimal::operator -=(const BigDecimal &other)
{
    op_eq(-=, other);
}


double BigDecimal::toDouble() const
{
    return double_approx(m_internal);
}
