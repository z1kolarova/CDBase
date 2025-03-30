#include "albumitemmodel.h"

#include <QColor>
#include <QFont>
#include <iostream>

AlbumItemModel::AlbumItemModel()
{

}

AlbumItemModel::AlbumItemModel(int numberOfColumns) :
    _numberOfColumns(numberOfColumns)
{

}

AlbumItemModel::~AlbumItemModel()
{

}

QVariant AlbumItemModel::data(const QModelIndex &index, int role) const
{
    QVariant result = QVariant();
    int row = index.row();
    int column = index.column();

    if(!index.isValid() || row >= rowCount() || column >= columnCount())
    {
        return result;
    }

    switch(role)
    {
        case Qt::DisplayRole:
            //checking row and column to get data
            switch(column)
            {
                //title
                case 0:
                    result = _data.at(row).title.c_str();
                    break;

                //author
                case 1:
                    result = _data.at(row).author.c_str();
                    break;

                //releaseYear
                case 2:
                    result = _data.at(row).releaseYear;
                    break;

                //genre
                case 3:
                    result = _data.at(row).genre.c_str();
                    break;
            }
            break;
    }
    return result;
}

int AlbumItemModel::rowCount(const QModelIndex &parent) const
{
    return _data.size();
}

int AlbumItemModel::columnCount(const QModelIndex &parent) const
{
    return _numberOfColumns;
}

QVariant AlbumItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result = QVariant();
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                result = "Název";
                break;
            case 1:
                result = "Autor";
                break;
            case 2:
                result = "Rok vydání";
                break;
            case 3:
                result = "Žánr";
                break;
        }
    }
    return result;
}

void AlbumItemModel::addNewElementToData(Album data)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _data.push_back(data);
    endInsertRows();
}

void AlbumItemModel::replaceElementInData(int index, Album newAlbum)
{
    _data[index] = newAlbum;
}

void AlbumItemModel::removeElementInData(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _data.erase(_data.begin() + index);
    endRemoveRows();
}

Album AlbumItemModel::getAlbumOnIndex(int rowNumber)
{
    if(_data.size() > rowNumber)
    {
        return _data[rowNumber];
    }
    cerr << "AlbumItemModel::getAlbumOnIndex was called with index out of range" << endl;
}
