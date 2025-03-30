#include "songitemmodel.h"
#include <iostream>;
#include <sstream>;
#include <string>;
#include <iomanip>;
#include <QTime>

SongItemModel::SongItemModel()
{

}

SongItemModel::SongItemModel(int numberOfColumns) :
    _numberOfColumns(numberOfColumns)
{

}

SongItemModel::~SongItemModel()
{

}

QVariant SongItemModel::data(const QModelIndex &index, int role) const
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

                //duration
                case 1:
                    int seconds = _data.at(row).durationInSeconds;
                    int minutes = seconds / 60;
                    int andSeconds = seconds % 60;
                    ostringstream oss;
                    oss << std::setfill('0') << std::setw(2) << minutes
                        << ":"
                        << std::setfill('0') << std::setw(2) << andSeconds;
                    result = oss.str().c_str();
                break;
            }
            break;
    }
    return result;
}

int SongItemModel::rowCount(const QModelIndex &parent) const
{
    return _data.size();
}

int SongItemModel::columnCount(const QModelIndex &parent) const
{
    return _numberOfColumns;
}

QVariant SongItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                result = "Délka";
                break;
        }
    }
    return result;
}

void SongItemModel::addNewElementToData(Song data)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _data.push_back(data);
    endInsertRows();
}

void SongItemModel::replaceElementInData(int index, Song newSong)
{
    _data[index] = newSong;
}

void SongItemModel::removeElementInData(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    _data.erase(_data.begin() + index);
    endRemoveRows();
}

Song SongItemModel::getSongOnIndex(int rowNumber)
{
    return _data[rowNumber];
}

QString SongItemModel::getQStringOfTimeFromSeconds(int seconds)
{
    int minutes = seconds / 60;
    int andSeconds = seconds % 60;
    string result = to_string(minutes) + ":" + to_string(andSeconds);
    return result.c_str();
}
