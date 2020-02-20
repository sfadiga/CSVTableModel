#ifndef TEST_H
#define TEST_H

#include <QWidget>
#include "csvtablemodel.h"

namespace Ui {
class Test;
}

class Test : public QWidget
{
    Q_OBJECT

public:
    explicit Test(QWidget *parent = nullptr);
    ~Test();

private slots:
    void on_lineEdit_editingFinished();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButtonReload_clicked();

    void on_pushButtonOpen_clicked();

private:
    Ui::Test *ui;
    CSVTableModel* m_model;
    QChar m_separator;
    QString m_currentFile;
    bool m_header;
    QString m_path;
};

#endif // TEST_H
