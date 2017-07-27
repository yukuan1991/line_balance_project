#pragma once
#include <QDialog>
#include <QLineEdit>

class QLabel;
class takt_time_dlg : public QDialog
{
    Q_OBJECT
signals:
//    void currenttextchanged(const QString &s);
    void confirm();
    void cancel();
public:
    takt_time_dlg(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void set_takt_time(const QString& s) { takt_time_->setText(s); }
    QString text() const { return takt_time_->text(); }
protected:
    QLineEdit* takt_time_;
    QLabel* label_;
    QPushButton* confirm_;
    QPushButton* cancel_;
};

