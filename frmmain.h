#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>

class QToolButton;

namespace Ui
{
    class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

private:
    Ui::frmMain *ui;

    QList<int> pixCharLeft;
    QList<QPixmap> pixNormalLeft;
    QList<QPixmap> pixDarkLeft;
    QList<QToolButton *> tbtnLeft;

    QList<int> pixCharTop;
    QList<QPixmap> pixNormalTop;
    QList<QPixmap> pixDarkTop;
    QList<QToolButton *> tbtnTop;

private slots:
    void initForm();
    void buttonClickedLeft();
    void buttonClickedTop();
    void initNavLeft();
    void initNavTop();
    void setStyle(QWidget *widget, int borderWidth = 3, QString borderColor = "#029FEA",
                  QString normalBgColor = "#292F38", QString selectBgColor = "#1D2025",
                  QString normalTextColor = "#54626F", QString selectTextColor = "#FDFDFD");

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
};

#endif // FRMMAIN_H
