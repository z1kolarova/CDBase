#include "album.h"
#include <iostream>
using namespace std;

Album::Album()
{

}

Album::Album(int id, QString title, QString author, int releaseYear, QString pictureLocation, QString genre)
{
    this->id = id;
    this->title = title.toStdString();
    this->author = author.toStdString();
    this->releaseYear = releaseYear;
    this->pictureLoc = pictureLocation.toStdString();
    this->genre = genre.toStdString();
}

void Album::update(QString title, QString author, int releaseYear, QString pictureLocation, QString genre)
{
    this->title = title.toStdString();
    this->author = author.toStdString();
    this->releaseYear = releaseYear;
    this->pictureLoc = pictureLocation.toStdString();
    this->genre = genre.toStdString();
}

int Album::getID()
{
    return id;
}

void Album::updateID(int id)
{
    this->id = id;
}


