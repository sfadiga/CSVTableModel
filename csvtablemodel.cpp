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

#include "csvtablemodel.h"

#include <QFile>

CSVTableModel::CSVTableModel(QString file, QChar separator, bool haveHeader, QObject *parent)
    : QAbstractTableModel (parent), _file(file), _separator(separator), _haveHeader(haveHeader), _internalStatus(false)
{
    load(file);
}

bool CSVTableModel::haveHeader() const
{
    return _haveHeader;
}

void CSVTableModel::setHaveHeader(bool haveHeader)
{
    _haveHeader = haveHeader;
}

QChar CSVTableModel::separator() const
{
    return _separator;
}

void CSVTableModel::setSeparator(const QChar &separator)
{
    _separator = separator;
}

bool CSVTableModel::load(QString file)
{
    _file = file;
    QFile data(_file);
    if (data.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&data);
        QString line;
        while (in.readLineInto(&line))
        {
            _fileLines.append(line);
        }
        data.close();
    }

    _internalStatus = parse();
    return _internalStatus;
}


void CSVTableModel::save()
{
    CSVTableModel::save(_file);
}

void CSVTableModel::save(QString file)
{
    QFile data(file);
    if (data.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream _out(&data);
        QString line;
        foreach (QStringList row, _rows)
        {
            _out << row.join(_separator) << endl;
        }
        _out.flush();
        data.close();
    }
}

bool CSVTableModel::parse()
{
    bool result = true;
    emit beginResetModel();
    // treat strings declared between " " with separator inside
    QString str("(?:\\%1\\s*|\n|^)(\"(?:(?:\"\")*[^\"]*)*\"|[^\"\\%1\n]*|(?:\n|$))");
    QRegExp exp(str.arg(_separator));
    bool isheader = _haveHeader;
    foreach(QString line, _fileLines)
    {
        // sanity check for selected separator
        if(line.contains(_separator))
        {
            int pos = 0;
            QStringList tokens;
            while ((pos = exp.indexIn(line, pos)) != -1)
            {
                tokens << exp.cap(1);
                pos += exp.matchedLength();
            }
            if(isheader)
            {
                _header = tokens;
                isheader = false;
            }
            else
            {
                _rows.append(tokens);
            }
        }
        else
        {
            _rows.append(QStringList(line));
            result = false;
        }
    }
    emit endResetModel();
    return (_rows.count() > 0 && result);
}

int CSVTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _rows.count();
}

int CSVTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if(_rows.count() > 0)
    {
        return _rows.at(0).count();
    }
    else {
        return 0;
    }
}

QVariant CSVTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    else if (index.row() >= _rows.count() || index.row() < 0)
    {
        return QVariant();
    }
    else if (role == Qt::DisplayRole)
    {
        if(index.row() <= _rows.count() - 1)
        {
            if(index.column() <= _rows.at(index.row()).count() - 1)
            {
                return _rows.at(index.row()).at(index.column());
            }
            else
            {
                return QVariant();
            }
        }
        else
        {
            return QVariant();
        }
    }
    return QVariant();
}

bool CSVTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int column = index.column();
    if (!index.isValid())
    {
        return false;
    }
    else if(row < 0 || row >= _rows.count() || column >= _rows.at(row).count() || column < 0)
    {
        return false;
    }
    else if (role == Qt::DisplayRole)
    {
        _rows[row][column] = value.toString();
        return true;
    }
    return false;
}

QVariant CSVTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }
    else if (orientation == Qt::Horizontal)
    {
        if(_header.count() > 0)
        {
            return _header[section];
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return section + 1;
    }
}

bool CSVTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::DisplayRole)
    {
        return false;
    }
    else if (orientation == Qt::Horizontal)
    {
        if(_header.count() > 0 && section < _header.count())
        {
            _header[section] = value.toString();
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void CSVTableModel::sort(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();
    for ( int i = 1; i < rowCount(); )
    {
        if ( order == Qt::AscendingOrder ? lessThan(column, i - 1, i) : lessThan(column, i, i - 1))
        {
            ++i;
        }
        else
        {
            swapRows(i - 1, i);
            --i;
            if ( i == 0 )
            {
                i = 1;
            }
        }
    }
    emit layoutChanged();
}


void CSVTableModel::swapRows(int r1, int r2)
{
    qSwap(_rows[r1], _rows[r2]);
}

bool CSVTableModel::lessThan(int column, int r1, int r2)
{
    return _rows[r1].at(column).compare(_rows[r2].at(column)) < 0;
}

bool CSVTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    emit beginInsertRows(parent, row, row + count);
    if(row > 0 && row < _rows.count())
    {
        _rows.insert(row, count, QStringList());
        return true;
    }
    emit endInsertRows();
    return false;
}

bool CSVTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    emit beginInsertColumns(parent, column, column + count);
    if(column > 0 && column < columnCount(parent))
    {
        for (int i = 0; i < _rows.count() ; i++)
        {
            for(int j = 0; j < count; j++)
            {
                _rows[i].insert(column+j, QString());
            }
        }
        return true;
    }
    emit endInsertColumns();
    return false;
}

bool CSVTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    emit beginRemoveRows(parent, row, row + count);
    if(row > 0 && row < _rows.count() && (row + count < _rows.count())) //TODO check this limit
    {
        _rows.remove(row, count);
        return true;
    }
    emit endRemoveRows();
    return false;
}

bool CSVTableModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    emit beginRemoveColumns(parent, column, column + count);
    if(column > 0 && column < columnCount(parent) && (column + count < columnCount(parent)))
    {
        for (int i = 0; i < _rows.count() ; i++)
        {
            for(int j = 0; j < count; j++)
            {
                _rows[i].removeAt(column+j);
            }
        }
        return true;
    }
    emit endRemoveColumns();
    return false;
}

bool CSVTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    emit beginMoveRows(sourceParent, sourceRow, sourceRow + count, destinationParent, destinationChild);
    if(sourceRow > 0 && sourceRow < _rows.count() && (sourceRow + count < _rows.count())) //TODO check this limit
    {
        for (int i = 0; i < count ; i++)
        {
            _rows.move(sourceRow + i, destinationChild + i);
        }
        return true;
    }
    emit endMoveRows();
    return false;
}

bool CSVTableModel::moveColumns(const QModelIndex &sourceParent, int sourceColumn, int count, const QModelIndex &destinationParent, int destinationChild)
{
    emit beginMoveColumns(sourceParent, sourceColumn, sourceColumn + count, destinationParent, destinationChild);
    if(sourceColumn > 0 && sourceColumn < columnCount(sourceParent) && (sourceColumn + count < columnCount(sourceParent)))
    {
        for (int i = 0; i < _rows.count() ; i++)
        {
            for(int j = 0; j < count; j++)
            {
                _rows[i].move(sourceColumn + j, destinationChild + j);
            }
        }
        return true;
    }
    emit endMoveColumns();
    return false;
}

