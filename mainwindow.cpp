#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "albumrepository.h"
#include "album.h"
#include "albumitemmodel.h"
#include <iostream>
#include <QItemSelectionModel>
#include <QFileDialog>
#include <QImageReader>
#include <QGraphicsPixmapItem>
#include <QStandardPaths>
#include <filesystem>
#include <iostream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir appDir(dataDir);
    if(!appDir.exists())
    {
        appDir.mkpath(".");
    }

    _albumModel = new AlbumItemModel(4);
    albumRepo = new AlbumRepository();
    loadAllAlbums();
    ui->tvDisplayedCDs->setModel(_albumModel);

    _songModel = new SongItemModel(2);
    songRepo = new SongRepository();
    vector<Song> songs = songRepo->getAllInAlbum(0);
    vector<Song> originalSongs = songRepo->getAllInAlbum(0);
    loadSongsIntoTableView(songs);
    ui->tvDisplayedSongs->setModel(_songModel);

    //SIGNALS & SLOTS
    connect(ui->tvDisplayedCDs->selectionModel(),
            SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this,
            SLOT(albumSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->tvDisplayedSongs->selectionModel(),
            SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this,
            SLOT(songSelectionChanged(QItemSelection,QItemSelection)));

    connect(ui->btnSearch, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui->btnResetSearch, SIGNAL(clicked()), this, SLOT(loadAllAlbums()));
    connect(ui->btnResetSearch, SIGNAL(clicked()), this, SLOT(clearSearchEditableObjects()));
    connect(ui->btnSelectPicture, SIGNAL(clicked()), this, SLOT(getNewPicture()));

    connect(ui->btnAddSong, SIGNAL(clicked()), this, SLOT(addNewSong()));
    connect(ui->btnEditSong, SIGNAL(clicked()), this, SLOT(editSong()));
    connect(ui->btnRemoveSong, SIGNAL(clicked()), this, SLOT(removeSong()));

    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(throwAwayCDChanges()));
    connect(ui->btnAddCD, SIGNAL(clicked()), this, SLOT(addNewCD()));
    connect(ui->btnSaveEdit, SIGNAL(clicked()), this, SLOT(editCD()));
    connect(ui->btnRemoveCD, SIGNAL(clicked()), this, SLOT(removeCD()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNewCD()
{
    int intYear;
    if(yearIsIntVerification(this->ui->leCDYear, intYear))
    {
        Album newAlbum = Album(0,
                               this->ui->leCDName->text(),
                               this->ui->leCDAuthor->text(),
                               intYear,
                               newPictureLoc,
                               this->ui->leGenre->text());

        albumRepo->addAlbum(newAlbum);

        _albumModel->addNewElementToData(newAlbum);

        int numberOfSongs = _songModel->rowCount();
        int i = 0;
        while (i < numberOfSongs) {
            Song song = _songModel->getSongOnIndex(i);
            song.updateAlbumID(newAlbum.getID());
            songRepo->addSong(song);
            _songModel->replaceElementInData(i, song);
            i++;
        }
    }
}

void MainWindow::editCD()
{
    int intYear;
    if(yearIsIntVerification(this->ui->leCDYear, intYear))
    {
        newPictureLoc = saveTempBookletAsPermanent(currentAlbum.getID());
        currentAlbum.update(this->ui->leCDName->text(),
                            this->ui->leCDAuthor->text(),
                            intYear,
                            newPictureLoc,
                            this->ui->leGenre->text());

        albumRepo->updateAlbum(currentAlbum);

        songRepo->deleteAllSongsInAlbum(currentAlbum.getID());
        while(_songModel->rowCount() > 0)
        {
            Song s = _songModel->getSongOnIndex(0);
            s.updateAlbumID(currentAlbum.getID());
            songRepo->addSong(s);
            _songModel->removeElementInData(0);
        }
        preEditSongs = songRepo->getAllInAlbum(currentAlbum.getID());
        loadSongsIntoTableView(preEditSongs);

        QModelIndexList indexes = ui->tvDisplayedCDs->selectionModel()->selection().indexes();
        QModelIndex leftmost = indexes.at(0);
        QModelIndex rightmost = indexes.at(indexes.size()-1);
        int vectorIndex = leftmost.row();
        _albumModel->replaceElementInData(vectorIndex, currentAlbum);

        this->ui->tvDisplayedCDs->setModel(_albumModel);

        ui->tvDisplayedCDs->model()->dataChanged(leftmost, rightmost);
    }
}

void MainWindow::removeCD()
{
    QModelIndexList indexes = ui->tvDisplayedCDs->selectionModel()->selection().indexes();
    if(!indexes.empty())
    {
        songRepo->deleteAllSongsInAlbum(currentAlbum.getID());
        while (_songModel->rowCount() > 0) {
            _songModel->removeElementInData(0);
        }
        clearSongEditingObjects();

        QString location = (currentAlbum.pictureLoc).c_str();
        if(!location.isNull() && !location.isEmpty())
        {
            QFile::remove((currentAlbum.pictureLoc).c_str());
        }
        albumRepo->deleteAlbum(currentAlbum.getID());
        QModelIndex index = indexes.at(0);
        _albumModel->removeElementInData(index.row());
        clearCDEditingObjects();
    }
}

void MainWindow::albumSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    clearCDEditingObjects();
    clearSongEditingObjects();

    // Get all selections
    QModelIndexList indexes = ui->tvDisplayedCDs->selectionModel()->selection().indexes();
    bool hasItem = !indexes.empty();
    this->ui->btnSaveEdit->setEnabled(hasItem);
    this->ui->btnRemoveCD->setEnabled(hasItem);
    if(hasItem)
    {
        QModelIndex index = indexes.at(0); // To get the row/column numbers use index.row() / index.column()
        int rowNbr = index.row();

        currentAlbum = _albumModel->getAlbumOnIndex(rowNbr);
        currentSong = Song();

        this->ui->leCDName->setText(currentAlbum.title.c_str());
        this->ui->leCDAuthor->setText(currentAlbum.author.c_str());
        QString yearAsString = to_string(currentAlbum.releaseYear).c_str();
        this->ui->leCDYear->setText(yearAsString);
        this->ui->leGenre->setText(currentAlbum.genre.c_str());

        this->ui->gvBooklet->setScene(NULL);
        loadPictureFromFilename(currentAlbum.pictureLoc.c_str());

        preEditSongs = songRepo->getAllInAlbum(currentAlbum.getID());
        vector<Song> songs = songRepo->getAllInAlbum(currentAlbum.getID());
        loadSongsIntoTableView(songs);
    }
}

void MainWindow::addNewSong()
{
    Song newSong = Song(0,
                        0,
                        this->ui->leSongTitle->text(),
                        this->ui->teSongDuration->time());
    _songModel->addNewElementToData(newSong);
}

void MainWindow::editSong()
{
    currentSong.update(this->ui->leSongTitle->text(),
                        this->ui->teSongDuration->time());

    QModelIndexList indexes = ui->tvDisplayedSongs->selectionModel()->selection().indexes();
    QModelIndex leftmost = indexes.at(0);
    QModelIndex rightmost = indexes.at(indexes.size()-1);
    int vectorIndex = leftmost.row();
    _songModel->replaceElementInData(vectorIndex, currentSong);

    ui->tvDisplayedSongs->model()->dataChanged(leftmost, rightmost);
}

void MainWindow::removeSong()
{
    QModelIndexList indexes = ui->tvDisplayedSongs->selectionModel()->selection().indexes();
    if(!indexes.empty())
    {
        QModelIndex index = indexes.at(0);
        _songModel->removeElementInData(index.row());
        clearSongEditingObjects();
    }
}

void MainWindow::songSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    clearSongEditingObjects();

    // Get all selections
    QModelIndexList indexes = ui->tvDisplayedSongs->selectionModel()->selection().indexes();
    bool hasItem = !indexes.empty();
    this->ui->btnEditSong->setEnabled(hasItem);
    this->ui->btnRemoveSong->setEnabled(hasItem);
    if(hasItem)
    {
        QModelIndex index = indexes.at(0); // To get the row/column numbers use index.row() / index.column()
        int rowNbr = index.row();

        currentSong = _songModel->getSongOnIndex(rowNbr);

        this->ui->leSongTitle->setText(currentSong.title.c_str());
        int seconds = currentSong.durationInSeconds;
        QTime q = QTime(0,0,0,0).addSecs(seconds);
        this->ui->teSongDuration->setTime(q);
    }
}

void MainWindow::clearCDEditingObjects()
{
    this->ui->leCDName->clear();
    this->ui->leCDAuthor->clear();
    this->ui->leCDYear->clear();
    this->ui->leGenre->clear();
    this->ui->gvBooklet->setScene(NULL);
}

void MainWindow::clearSongEditingObjects()
{
    this->ui->leSongTitle->clear();
    this->ui->teSongDuration->setTime(QTime(0,0));
}

void MainWindow::clearSearchEditableObjects()
{
    this->ui->leSearchTitle->clear();
    this->ui->leSearchAuthor->clear();
    this->ui->leSearchYear->clear();
    this->ui->leSearchGenre->clear();
}

void MainWindow::throwAwayCDChanges()
{
    clearCDEditingObjects();
    clearSongEditingObjects();

    this->ui->leCDName->setText(currentAlbum.title.c_str());
    this->ui->leCDAuthor->setText(currentAlbum.author.c_str());
    QString yearAsString = to_string(currentAlbum.releaseYear).c_str();
    this->ui->leCDYear->setText(yearAsString);
    this->ui->leGenre->setText(currentAlbum.genre.c_str());

    this->ui->gvBooklet->setScene(NULL);
    loadPictureFromFilename(currentAlbum.pictureLoc.c_str());

    loadSongsIntoTableView(preEditSongs);
}

void MainWindow::getNewPicture()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Image Files (*.png *.jpg *.bmp)"));

    if (dialog.exec() == QDialog::Accepted)
    {
        QString fileName = dialog.selectedFiles().constFirst();
        tempBookletPic = loadPictureFromFilename(fileName);
        if(!tempBookletPic.isNull())
        {
            QString tempPath = getFullPathToAppdataFile(tempBookletName);
            tempBookletPic.save(tempPath, "BMP");
            bookletPicChanges = true;
        }
    }
}

QImage MainWindow::loadPictureFromFilename(QString fileName)
{
    if(!fileName.isNull() && !fileName.isEmpty())
    {
        QFile picFile;
        picFile.setFileName(fileName);
        if(picFile.exists())
        {
            QGraphicsScene* scene = new QGraphicsScene();
            QImageReader reader(fileName);
            reader.setAutoTransform(true);
            const QImage newImage = reader.read();

            this->ui->gvBooklet->setScene(scene);
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(newImage));
            scene->addItem(item);

            ui->gvBooklet->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
            return newImage;
        }
    }
    return QImage();
}

QString MainWindow::saveTempBookletAsPermanent(int id)
{
    QString fileName = "";
    fileName = fileName + to_string(id).c_str() + ".bmp";

    QString fullPath = getFullPathToAppdataFile(fileName);

    tempBookletPic.save(fullPath);
    bookletPicChanges = false;
    return fullPath;
}

QString MainWindow::getFullPathToAppdataFile(QString name)
{
    filesystem::path stdPath = dataDir.toStdString();
    stdPath /= name.toStdString();
    QString fullPath = this->pathToQString(stdPath);
    return fullPath;
}

QString MainWindow::pathToQString(filesystem::path path)
{
#ifdef _WIN32
    return QString::fromStdWString(path.generic_wstring());
#else
    return QString::fromStdString(path.native());
#endif
}

filesystem::path MainWindow::qStringToPath(QString pathString)
{
#ifdef _WIN32
    auto * wptr = reinterpret_cast<const wchar_t*>(pathString.utf16());
    return filesystem::path(wptr, wptr + pathString.size());
#else
    return filesystem::path(pathString.toStdString());
#endif
}

void MainWindow::loadAllAlbums()
{
    vector<Album> albums = albumRepo->getAll();
    loadAlbumsIntoTableView(albums);
}

void MainWindow::search()
{
    int searchIntYear;
    if(yearIsIntVerification(this->ui->leSearchYear, searchIntYear))
    {
        string sTitle = this->ui->leSearchTitle->text().toStdString();
        string sAuthor = this->ui->leSearchAuthor->text().toStdString();
        string sGenre = this->ui->leSearchGenre->text().toStdString();

        vector<Album> searchResultsAlbums = albumRepo->getSearchResults(sTitle, sAuthor, searchIntYear, sGenre);
        loadAlbumsIntoTableView(searchResultsAlbums);
    }
}

void MainWindow::loadAlbumsIntoTableView(vector<Album> &items)
{
    unsigned int numberOfRows = items.size();
    while (_albumModel->rowCount() > 0){
        _albumModel->removeElementInData(0);
    }

    int i = 0;
    while (i < numberOfRows) {
        _albumModel->addNewElementToData(items[i]);
        i++;
    }
}

void MainWindow::loadSongsIntoTableView(vector<Song> &items)
{
    unsigned int numberOfRows = items.size();
    while (_songModel->rowCount() > 0){
        _songModel->removeElementInData(0);
    }

    int i = 0;
    while (i < numberOfRows) {
        _songModel->addNewElementToData(items[i]);
        i++;
    }
}

bool MainWindow::yearIsIntVerification(QLineEdit *textSource, int &cislo)
{
    bool isInt;
    cislo = textSource->text().toInt(&isInt);
    if(!isInt)
    {
        QMessageBox::warning(this, tr("CDBase"),
                                       tr("Year has to be a number."),
                                       QMessageBox::Ok);
    }
    return isInt;
}
