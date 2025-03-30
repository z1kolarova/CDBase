#ifndef SONGITEMMODEL_H
#define SONGITEMMODEL_H

#include "song.h"
#include <QAbstractTableModel>

class SongItemModel : public QAbstractTableModel
{
public:
    SongItemModel();
    SongItemModel(int numberOfColumns);
    ~SongItemModel();

    //override from virutal pure
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    //override from virtual
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    //classic
    void addNewElementToData(Song data);
    void replaceElementInData(int index, Song newSong);
    void removeElementInData(int index);
    Song getSongOnIndex(int rowNumber);

    //mine
    QString getQStringOfTimeFromSeconds(int seconds);

private:
    vector<Song> _data;
    int _numberOfColumns;
};

#endif // SONGITEMMODEL_H
