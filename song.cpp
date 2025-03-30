#include "song.h"

#include <QTime>

Song::Song()
{
    this->id = 0;
    this->albumID = 0;
    this->title = "";
    this->durationInSeconds = 0;
}

Song::Song(int id, int albumID, QString title, int lastsSeconds)
{
    this->id = id;
    this->albumID = albumID;
    this->title = title.toStdString();
    this->durationInSeconds = lastsSeconds;
}

Song::Song(int id, int albumID, QString title, QTime lastsSeconds)
{
    this->id = id;
    this->albumID = albumID;
    this->title = title.toStdString();
    this->durationInSeconds = QTime(0,0).secsTo(lastsSeconds);
}

void Song::update(QString songTitle, QTime duration)
{
    this->title = songTitle.toStdString();
    this->durationInSeconds = QTime(0,0).secsTo(duration);
}

int Song::getID()
{
    return id;
}

int Song::getAlbumID()
{
    return albumID;
}

void Song::updateID(int id)
{
    this->id = id;
}

void Song::updateAlbumID(int albumID)
{
    this->albumID = albumID;
}
