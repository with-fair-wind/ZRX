#pragma once

#include <QWidget>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Form;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void on_clicked();

private:
    Ui::Form *ui;
};