#ifndef ALBUMREPOSITORY_H
#define ALBUMREPOSITORY_H

#include <QObject>
#include "album.h"
#include "sqlite34/sqlite3.h"

class AlbumRepository
{
private:
    sqlite3 *dbpointer;
    void bindTextParameter(sqlite3_stmt *statementpointer, string parameterName, string parameterValue);
    void bindIntParameter(sqlite3_stmt *statementpointer, string parameterName, int parameterValue);

public:
    AlbumRepository();
    ~AlbumRepository();
    std::vector<Album> getAll();
    std::vector<Album> getSearchResults(string searchTitle, string searchAuthor, int searchYear, string searchGenre);
    void addAlbum(Album &album);
    void updateAlbum(Album &album);
    void deleteAlbum(int id);
};

#endif // ALBUMREPOSITORY_H
