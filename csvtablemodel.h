/*
MIT License

Copyright (c) 2020 csvtablemodel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef CSVTABLEMODEL_H
#define CSVTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QTextStream>
#include <QFile>
#include <QChar>

class CSVTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    //!
    //! \brief QCSVTableModel
    //! \param file the CSV file for this model
    //! \param header true will consider the first line of the file as a header
    //! \param separator the separator type , ; | <tab>=QChar::Tabulation etc...
    //! \param openMode
    //! \param parent
    //!
    CSVTableModel(QString file, QChar separator=',', bool haveHeader=true, QObject *parent=nullptr);
    ~CSVTableModel() override = default;

    //!
    //! \brief load
    //! \param file
    //! \return
    //!
    bool load(QString file);

    //!
    //! \brief save - overwrite the source file
    //!
    void save();

    //!
    //! \brief save - save the model to another file
    //! \param file
    //!
    void save(QString file);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
    bool insertRows(int row, int count, const QModelIndex &parent) override;
    bool insertColumns(int column, int count, const QModelIndex &parent) override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;
    bool removeColumns(int column, int count, const QModelIndex &parent) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild) override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;


    QChar separator() const;
    void setSeparator(const QChar &separator);

    bool haveHeader() const;
    void setHaveHeader(bool haveHeader);

private:
    void swapRows(int r1, int r2);
    bool lessThan(int column, int r1, int r2);

    //!
    //! \brief parse
    //! \return
    //!
    bool parse();

private:
    // the original file line by line
    QStringList _fileLines;
    // the header text by column
    QStringList _header;
    // a vector with text tokens, each row is a stringlist (each cell is a vec pos stringlist pos)
    QVector<QStringList> _rows;
    QString _file;
    QChar _separator;
    bool _haveHeader;
    bool _internalStatus;
};

#endif // QCSVTABLEMODEL_H
