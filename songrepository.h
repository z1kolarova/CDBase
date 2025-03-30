#ifndef SONGREPOSITORY_H
#define SONGREPOSITORY_H

#include <QObject>
#include "song.h"
#include "sqlite34/sqlite3.h"

class SongRepository
{

private:
    sqlite3 *dbpointer;
    void bindTextParameter(sqlite3_stmt *statementpointer, string parameterName, string parameterValue);
    void bindIntParameter(sqlite3_stmt *statementpointer, string parameterName, int parameterValue);

public:
    SongRepository();
    ~SongRepository();
    std::vector<Song> getAllInAlbum(int albumID);
    void addSong(Song &song);
    void updateSong(Song &song);
    void deleteSong(int id);
    void deleteAllSongsInAlbum(int ablumID);
};

#endif // SONGREPOSITORY_H
