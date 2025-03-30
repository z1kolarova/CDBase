#ifndef SONG_H
#define SONG_H

#include <QObject>
using namespace std;


class Song
{
private:
    int id;
    int albumID;

public:
    Song();
    Song(int id, int albumID, QString title, int lastsSeconds);
    Song(int id, int albumID, QString title, QTime lastsSeconds);
    string title;
    int durationInSeconds;
    void update(QString songTitle, QTime duration);
    int getID();
    int getAlbumID();
    void updateID(int id);
    void updateAlbumID(int albumID);
};

#endif // SONG_H
