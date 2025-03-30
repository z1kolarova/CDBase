#ifndef ALBUM_H
#define ALBUM_H

#include <QObject>
using namespace std;

class Album
{
private:
    int id;

public:
    string title;
    string author;
    int releaseYear;
    string pictureLoc;
    string genre;
    Album();
    Album(int id, QString title, QString author, int releaseYear, QString pictureLocation, QString genre);
    void update(QString title, QString author, int releaseYear, QString pictureLocation, QString genre);
    int getID();
    void updateID(int id);
};

#endif // ALBUM_H
