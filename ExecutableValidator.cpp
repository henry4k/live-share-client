#include "ExecutableValidator.h"

ExecutableValidator::ExecutableValidator(QWidget *parent)
{
}

ExecutableValidator::~ExecutableValidator()
{
}

QValidator::State ExecutableValidator::validate(QString &input, int &pos) const
{
    // valid file name or found in PATH
    return QValidator::Acceptable; // TODO
}
