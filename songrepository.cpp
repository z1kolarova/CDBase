#include "songrepository.h"
#include <iostream>

void SongRepository::bindTextParameter(sqlite3_stmt *statementpointer, string parameterName, string parameterValue)
{
    sqlite3_bind_text(statementpointer,
                      sqlite3_bind_parameter_index(statementpointer, parameterName.c_str()),
                      parameterValue.c_str(),
                      parameterValue.length(),
                      SQLITE_TRANSIENT);
}
void SongRepository::bindIntParameter(sqlite3_stmt *statementpointer, string parameterName, int parameterValue)
{
    sqlite3_bind_int(statementpointer,
                      sqlite3_bind_parameter_index(statementpointer, parameterName.c_str()),
                      parameterValue);
}

SongRepository::SongRepository()
{
    dbpointer = NULL;
    sqlite3_open("CDBase.db", &dbpointer);

    const char *tableCreation = "CREATE TABLE IF NOT EXISTS Songs ("
                                    "ID  INTEGER PRIMARY KEY, "
                                    "AlbumID         INTEGER NOT NULL, "
                                    "Title           TEXT    NOT NULL, "
                                    "DurationSeconds INTEGER NULL"
                                    ");";

    char *errMsg = 0;

    sqlite3_exec(dbpointer, tableCreation, NULL, 0, &errMsg);
    cout << errMsg;
}

std::vector<Song> SongRepository::getAllInAlbum(int albumID)
{
    int returnCode;
    const char *sql = "SELECT ID, Title, DurationSeconds FROM Songs WHERE AlbumID = @albumID;";
    vector<Song> songs = vector<Song>();

    sqlite3_stmt *statementpointer;
    returnCode = sqlite3_prepare_v2(dbpointer, sql, -1, &statementpointer, NULL);
    bindIntParameter(statementpointer, "@albumID", albumID);

    if (returnCode != SQLITE_OK) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        return songs;
    }

    while ((returnCode = sqlite3_step(statementpointer)) == SQLITE_ROW) {
        int id = sqlite3_column_int(statementpointer, 0);
        const char* title = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 1));
        const int durationSeconds = (sqlite3_column_int(statementpointer, 2));

        songs.push_back(Song(id, albumID, title, durationSeconds));
    }

    if (returnCode != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        // if you return/throw here, don't forget the finalize
    }
    sqlite3_finalize(statementpointer);
    return songs;
}

void SongRepository::addSong(Song &song)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "INSERT INTO Songs (AlbumID, Title, DurationSeconds)"
                 " VALUES(@albumID, @title, @durationSeconds);";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), sql.length(), &statementpointer, NULL);
    bindIntParameter(statementpointer, "@albumID", song.getAlbumID());
    bindTextParameter(statementpointer, "@title", song.title);
    bindIntParameter(statementpointer, "@durationSeconds", song.durationInSeconds);

    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "INSERT failed: " << sqlite3_errmsg(dbpointer) << endl;
    }
    else {
        int id = sqlite3_last_insert_rowid(dbpointer);
        song.updateID(id);
    }

    sqlite3_finalize(statementpointer);
}

void SongRepository::updateSong(Song &song)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "UPDATE Songs"
                 " SET"
                 "  Title = @title,"
                 "  DurationSeconds = @durationSeconds"
                 " WHERE ID = @id;";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), sql.length(), &statementpointer, NULL);
    bindTextParameter(statementpointer, "@title", song.title);
    bindIntParameter(statementpointer, "@durationSeconds", song.durationInSeconds);
    bindIntParameter(statementpointer, "@id", song.getID());

    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "UPDATE failed: " << sqlite3_errmsg(dbpointer) << endl;
    }

    sqlite3_finalize(statementpointer);
}

void SongRepository::deleteSong(int id)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "DELETE FROM Songs"
                 " WHERE ID = @id;";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), sql.length(), &statementpointer, NULL);
    bindIntParameter(statementpointer, "@id", id);


    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "DELETE failed: " << sqlite3_errmsg(dbpointer) << endl;
    }
}

void SongRepository::deleteAllSongsInAlbum(int albumID)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "DELETE FROM Songs"
                 " WHERE AlbumID = @albumID;";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), sql.length(), &statementpointer, NULL);
    bindIntParameter(statementpointer, "@albumID", albumID);


    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "DELETE failed: " << sqlite3_errmsg(dbpointer) << endl;
    }
}
