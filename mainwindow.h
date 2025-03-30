#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <filesystem>
#include <QStandardPaths>
#include <QDir>
#include <QLineEdit>
#include "album.h"
#include "albumitemmodel.h"
#include "albumrepository.h"
#include "qitemselectionmodel.h"
#include "song.h"
#include "songitemmodel.h"
#include "songrepository.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addNewCD();
    void editCD();
    void removeCD();
    void albumSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void addNewSong();
    void editSong();
    void removeSong();
    void songSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void clearCDEditingObjects();
    void clearSongEditingObjects();
    void clearSearchEditableObjects();

    void throwAwayCDChanges();
    void getNewPicture();
    void loadAllAlbums();
    void search();

private:
    Ui::MainWindow *ui;   
    QPointer<AlbumItemModel> _albumModel;
    AlbumRepository *albumRepo;
    Album currentAlbum;
    QPointer<SongItemModel> _songModel;
    SongRepository *songRepo;
    Song currentSong;
    vector<Song> preEditSongs;
    QString newPictureLoc;
    QString const dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString const tempBookletName = "tempBooklet.bmp";
    QImage tempBookletPic;
    bool bookletPicChanges;
    void loadAlbumsIntoTableView(vector<Album> &items);
    void loadSongsIntoTableView(vector<Song> &items);
    QImage loadPictureFromFilename(QString fileName);
    QString saveTempBookletAsPermanent(int id);
    QString getFullPathToAppdataFile(QString name);
    QTime getQTimeFromSeconds(int seconds);
    QString pathToQString(filesystem::path path);
    filesystem::path qStringToPath(QString pathString);
    bool yearIsIntVerification(QLineEdit *textSource, int &cislo);
};
#endif // MAINWINDOW_H
