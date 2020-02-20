#include "test.h"
#include "ui_test.h"

#include <QFileDialog>

Test::Test(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Test)
    , m_model(nullptr)
    , m_separator(QChar::Tabulation)
    , m_currentFile(QString())
    , m_header(false)
    , m_path(".")
{
    ui->setupUi(this);
}

Test::~Test()
{
    delete ui;
}

void Test::on_lineEdit_editingFinished()
{
    if(ui->lineEdit->text().isEmpty())
    {
        m_separator = QChar::Tabulation;
    }
    else
    {
        m_separator = ui->lineEdit->text().at(0);
    }
}

void Test::on_checkBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    m_header = ui->checkBox->isChecked();
}

void Test::on_pushButtonReload_clicked()
{
    if(!m_currentFile.isEmpty())
    {
        QFileInfo m(m_currentFile);
        m_path = m.path();

        if(m_model != nullptr)
        {
            delete m_model;
        }
        m_model = new CSVTableModel(m_currentFile, this->m_separator, this->m_header, ui->tableView);
        ui->tableView->setModel(m_model);
    }
}

void Test::on_pushButtonOpen_clicked()
{
    m_currentFile = QFileDialog::getOpenFileName(this, tr("Open File"), m_path, tr("Image Files (*.csv *.txt)"));
    on_pushButtonReload_clicked();
}
