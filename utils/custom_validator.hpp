#ifndef CUSTOM_VALIDATOR_HPP
#define CUSTOM_VALIDATOR_HPP
#include <QValidator>

class IntValidator: public QIntValidator {
    Q_OBJECT
public:
    explicit IntValidator(QObject *parent = 0):QIntValidator(parent) {}
    IntValidator(int bottom, int top, QObject *parent = 0):QIntValidator(bottom, top, parent) {}
    virtual void setRange(int bottom, int top) { QIntValidator::setRange(bottom, top); }
    virtual QValidator::State validate(QString &str, int &pos) const { return QIntValidator::validate(str, pos); }
    virtual void fixup(QString &input) const
    {
        input = QString::number(bottom());
    }
};

#endif // CUSTOM_VALIDATOR_HPP
