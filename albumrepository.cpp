#include "albumrepository.h"
#include <iostream>
#include <sstream>

void AlbumRepository::bindTextParameter(sqlite3_stmt *statementpointer, string parameterName, string parameterValue)
{
    sqlite3_bind_text(statementpointer,
                      sqlite3_bind_parameter_index(statementpointer, parameterName.c_str()),
                      parameterValue.c_str(),
                      parameterValue.length(),
                      SQLITE_TRANSIENT);
}
void AlbumRepository::bindIntParameter(sqlite3_stmt *statementpointer, string parameterName, int parameterValue)
{
    sqlite3_bind_int(statementpointer,
                      sqlite3_bind_parameter_index(statementpointer, parameterName.c_str()),
                      parameterValue);
}

AlbumRepository::AlbumRepository()
{
    dbpointer = NULL;
    sqlite3_open("CDBase.db", &dbpointer);

    const char *tableCreation = "CREATE TABLE IF NOT EXISTS Albums ( "
                                    "ID  INTEGER PRIMARY KEY, "
                                    "Title           TEXT    NOT NULL, "
                                    "Author          TEXT    NOT NULL, "
                                    "ReleaseYear     INTEGER NULL, "
                                    "PictureLocation TEXT    NULL, "
                                    "Genre           TEXT    NULL "
                                    "); ";

    char *errMsg = 0;

    sqlite3_exec(dbpointer, tableCreation, NULL, 0, &errMsg);
    cout << errMsg;
}

AlbumRepository::~AlbumRepository()
{
    sqlite3_close_v2(dbpointer);
}

std::vector<Album> AlbumRepository::getAll()
{
    int returnCode;
    const char *selectAll = "SELECT ID, Title, Author, ReleaseYear, PictureLocation, Genre FROM Albums ORDER BY Title;";
    vector<Album> albums = vector<Album>();

    sqlite3_stmt *statementpointer;
    returnCode = sqlite3_prepare_v2(dbpointer, selectAll, -1, &statementpointer, NULL);

    if (returnCode != SQLITE_OK) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        return albums;
    }

    while ((returnCode = sqlite3_step(statementpointer)) == SQLITE_ROW) {
        int id = sqlite3_column_int(statementpointer, 0);
        const char* title = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 1));
        const char* author = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 2));
        const int releaseYear = (sqlite3_column_int(statementpointer, 3));
        const char* pictureLoc = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 4));
        const char* genre = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 5));

        // let's assume number can be NULL:
        albums.push_back(Album(id, title, author, releaseYear, pictureLoc ? pictureLoc : "", genre));
    }

    if (returnCode != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        // if you return/throw here, don't forget the finalize
    }
    sqlite3_finalize(statementpointer);
    return albums;
}

std::vector<Album> AlbumRepository::getSearchResults(string searchTitle, string searchAuthor, int searchYear, string searchGenre)
{
    int returnCode;
    const char *selectAll = "SELECT ID, Title, Author, ReleaseYear, PictureLocation, Genre FROM Albums "
                            "WHERE Title LIKE '%' || @searchTitle || '%' AND Author LIKE '%' || @searchAuthor || '%' "
                            "AND (ReleaseYear = @searchYear OR @searchYear = 0) AND Genre LIKE '%' || @searchGenre || '%' ORDER BY Title;";

    vector<Album> albums = vector<Album>();

    sqlite3_stmt *statementpointer;
    returnCode = sqlite3_prepare_v2(dbpointer, selectAll, -1, &statementpointer, NULL);
    bindTextParameter(statementpointer, "@searchTitle", searchTitle);
    bindTextParameter(statementpointer, "@searchAuthor", searchAuthor);
    bindIntParameter(statementpointer, "@searchYear", searchYear);
    bindTextParameter(statementpointer, "@searchGenre", searchGenre);

    if (returnCode != SQLITE_OK) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        return albums;
    }

    while ((returnCode = sqlite3_step(statementpointer)) == SQLITE_ROW) {
        int id = sqlite3_column_int(statementpointer, 0);
        const char* title = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 1));
        const char* author = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 2));
        const int releaseYear = (sqlite3_column_int(statementpointer, 3));
        const char* pictureLoc = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 4));
        const char* genre = reinterpret_cast<const char*>(sqlite3_column_text(statementpointer, 5));

        albums.push_back(Album(id, title, author, releaseYear, pictureLoc ? pictureLoc : "", genre));
    }

    if (returnCode != SQLITE_DONE) {
        cerr << "SELECT failed: " << sqlite3_errmsg(dbpointer) << endl;
        // if you return/throw here, don't forget the finalize
    }
    sqlite3_finalize(statementpointer);
    return albums;
}

void AlbumRepository::addAlbum(Album &album)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "INSERT INTO Albums (Title, Author, ReleaseYear, PictureLocation, Genre)"
                 " VALUES( @title, @author, @releaseYear, @pictureLocation, @genre );";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), -1, &statementpointer, NULL);
    bindTextParameter(statementpointer, "@title", album.title);
    bindTextParameter(statementpointer, "@author", album.author);
    bindIntParameter(statementpointer, "@releaseYear", album.releaseYear);
    bindTextParameter(statementpointer, "@pictureLocation", album.pictureLoc);
    bindTextParameter(statementpointer, "@genre", album.genre);

    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "INSERT failed: " << sqlite3_errmsg(dbpointer) << endl;
    }
    else{
        int id = sqlite3_last_insert_rowid(dbpointer);
        album.updateID(id);
    }

    sqlite3_finalize(statementpointer);
}

void AlbumRepository::updateAlbum(Album &album)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "UPDATE Albums"
                 " SET"
                 "  Title = @title,"
                 "  Author = @author,"
                 "  ReleaseYear = @releaseYear,"
                 "  PictureLocation = @pictureLocation,"
                 "  Genre = @genre"
                 " WHERE ID = @id;";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), -1, &statementpointer, NULL);
    bindTextParameter(statementpointer, "@title", album.title);
    bindTextParameter(statementpointer, "@author", album.author);
    bindIntParameter(statementpointer, "@releaseYear", album.releaseYear);
    bindTextParameter(statementpointer, "@pictureLocation", album.pictureLoc);
    bindTextParameter(statementpointer, "@genre", album.genre);
    bindIntParameter(statementpointer, "@id", album.getID());

    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "UPDATE failed: " << sqlite3_errmsg(dbpointer) << endl;
    }

    sqlite3_finalize(statementpointer);
}

void AlbumRepository::deleteAlbum(int id)
{
    int returnCode;
    sqlite3_stmt *statementpointer;
    string sql = "DELETE FROM Albums"
                 " WHERE ID = @id;";

    sqlite3_prepare_v2(dbpointer, sql.c_str(), -1, &statementpointer, NULL);
    bindIntParameter(statementpointer, "@id", id);


    returnCode = sqlite3_step(statementpointer);
    if (returnCode != SQLITE_DONE) {
        cerr << "DELETE failed: " << sqlite3_errmsg(dbpointer) << endl;
    }
}



