/* *****************************************************************************
Copyright (c) 2016-2021, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written by: Jinyan Zhao
#include  "SC_MovieWidget.h"
#include <QResizeEvent>
#include <QFile>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include <QLabel>

SC_MovieWidget::SC_MovieWidget(QWidget *parent, QString pathToMovie, bool showControls)
  :movie(0), movieLabel(0)
{

  QFile file(pathToMovie);
  if (file.exists()){
    if (movie){
      delete movie;
      movie=nullptr;
    }
    
    movie = new QMovie(pathToMovie);

    if (showControls == false) {
      this->setMovie(movie);
      this->setScaledContents(true);
    }
  }
  
  //
  // adding stop and start control QPushButtons
  //
  
  if (showControls == true) {
    
    QGridLayout *layout = new QGridLayout();
    
    QPushButton *startButton = new QPushButton();
    startButton->setIcon(QIcon::fromTheme("media-playback-start")); // Typical play icon
    startButton->setText("Start");
    
    QPushButton *stopButton = new QPushButton();
    stopButton->setIcon(QIcon::fromTheme("media-playback-stop")); // Typical stop icon
    stopButton->setText("Stop");
    
    connect(startButton, &QPushButton::clicked, [=]() {
      initialSize = size();
      setFixedSize(initialSize);
      if (movie != 0)
	movie->start();
    });
    
    connect(stopButton, &QPushButton::clicked, [=](){
      if (movie != 0)
	movie->stop();
      setMinimumSize(100, 100); // Reset minimum size to allow resizing
      setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    });    
    
    movieLabel = new QLabel;
    movieLabel->setMovie(movie);
    //movieLabel->setStyleSheet("background: lightgreen; padding: 10px;");
    movieLabel->setScaledContents(true);
    
    layout->addWidget(movieLabel,0,0,2,1);
    layout->setRowStretch(0,1);
    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem(spacer, 0,2);
      
    layout->setColumnStretch(3,1);    
    layout->addWidget(startButton,1,1);
    layout->addWidget(stopButton,1,2);
    this->setLayout(layout);
    initialSize = size();
  }

  if (movie != 0) 
    movie->start();
    
  this->setParent(parent);
}

void SC_MovieWidget::resizeEvent(QResizeEvent *event){
    
    if (movie) {

      if (movieLabel != 0) {
	
	QSize thisSize = event->size();
	double widthRatio = static_cast<double>(thisSize.width()) / origMovieSize.width();
	double heightRatio = static_cast<double>(thisSize.height()) / origMovieSize.height();
	
	// Select the minimum ratio
	double minRatio = qMin(widthRatio, heightRatio);
      
	// Compute new scaled size
	QSize newSize(origMovieSize.width() * minRatio, origMovieSize.height() * minRatio);
	qDebug() << "in resize : origSIZE: " <<origMovieSize;			
	qDebug() << "in resize : thisSIZE: " << thisSize;		
	qDebug() << "in resize : newSIZE: " << newSize;

	movie->setScaledSize(newSize);	
	movieLabel->setFixedSize(newSize);	
	movieLabel->setScaledContents(true);
	movieLabel->update();	
	
      } else {
	
	movie->setScaledSize(event->size());
	
      }
    }
}

bool SC_MovieWidget::updateGif(QString newPath){
  
    QFile file(newPath);
    
    if (file.exists()){
        if (movie){
            delete movie;
            movie = nullptr;
        }
	
        movie = new QMovie(newPath);

	if (movieLabel != 0) {
	  connect(movie, &QMovie::frameChanged, this, &SC_MovieWidget::updateGifSize);
	}
	
	if (movieLabel != 0) {

	  // Try to scale movie keeping same aspect ratio
	  QMovie *movie2 = new QMovie(newPath);	  
	  movie2->jumpToFrame(0);
	  origMovieSize = movie2->frameRect().size();
	  QSize thisSize = this->size();
	  double widthRatio = static_cast<double>(thisSize.width()) / origMovieSize.width();
	  double heightRatio = static_cast<double>(thisSize.height()) / origMovieSize.height();

	  // Select the minimum ratio
	  double minRatio = qMin(widthRatio, heightRatio);

	  // Compute new scaled size
	  QSize newSize(origMovieSize.width() * minRatio, origMovieSize.height() * minRatio);
	  qDebug() << "gifSIZE: " << origMovieSize;
	  qDebug() << "widgetSIZE: " << thisSize;	  
	  qDebug() << "newSIZE: " << newSize;
	  delete movie2;
	  movie->setScaledSize(newSize);
	  movieLabel->setFixedSize(newSize);
	  movieLabel->setScaledContents(true);
	  movieLabel->setMovie(movie);
	  // origMovieSize = newSize;
	  
	} else {

	  movie->setScaledSize(this->size());
	  this->setScaledContents(true);
	  this->setMovie(movie);
	  
	}


        movie->start();
	
        return true;
	
    } else {
        return false;
    }
}

void
SC_MovieWidget::updateGifSize() {

  if (movie->isValid()) {
    
    QSize movieSize = movie->currentImage().size(); // Get GIF frame size
    QSize maxSize = QSize(width(), height());       // Use widget size as boundary
    // Scale while keeping aspect ratio, but **limit it within the window size**
    //    QSize scaledSize = movieSize.scaled(maxSize, Qt::KeepAspectRatioByExpanding).boundedTo(maxSize);

    qDebug() << "movieSize: " << movieSize;
    qDebug() << "movieSLabelize: " << movieLabel->size();    
    qDebug() << "thisSize: " << maxSize;
    
    //    qDebug() << "scaledSize: " << scaledSize;
    
    //movie->setScalesSize(scaledSize); // Apply new size while keeping ratio    
    //movieLabel->setFixedSize(scaledSize); // Apply new size while keeping ratio
  }
}
