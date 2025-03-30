#ifndef ALBUMITEMMODEL_H
#define ALBUMITEMMODEL_H

#include "album.h"
#include <QAbstractTableModel>

class AlbumItemModel : public QAbstractTableModel
{
public:
    AlbumItemModel();
    AlbumItemModel(int numberOfColumns);
    ~AlbumItemModel();

    //override from virutal pure
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    //override from virtual
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    //classic
    void addNewElementToData(Album data);
    void replaceElementInData(int index, Album newAlbum);
    void removeElementInData(int index);
    Album getAlbumOnIndex(int rowNumber);

private:
    vector<Album> _data;
    int _numberOfColumns;
};

#endif // ALBUMITEMMODEL_H
