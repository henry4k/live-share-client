#ifndef EXECUTABLEVALIDATOR_H
#define EXECUTABLEVALIDATOR_H

#include <QValidator>

class ExecutableValidator : public QValidator
{
    Q_OBJECT

public:
    explicit ExecutableValidator(QWidget *parent = nullptr);
    ~ExecutableValidator();

    QValidator::State validate(QString &, int &) const Q_DECL_OVERRIDE;
};

#endif
