/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
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

// Written: fmckenna

#include "FooterWidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPixmap>
#include <QBitmap>
#include <QDebug>


FooterWidget::FooterWidget(QWidget *parent)
    :QGroupBox(parent)
{
  // putting NSF logo in label
  QLabel *nsfLogo = new QLabel();
  QPixmap pixmap(":/imagesCommon/nsf.gif");
  QPixmap newPixmap = pixmap.scaled(QSize(40,40),  Qt::KeepAspectRatio);
  nsfLogo->setPixmap(newPixmap);
  nsfLogo->setMask(newPixmap.mask());
  nsfLogo->show();
  
  // putting some text in another
  QLabel *nsfText = new QLabel();
  nsfText->setObjectName(QString::fromUtf8("nsfText"));
  nsfText->setText(tr("This work is based on material supported by the National Science Foundation under grant 1612843"));

  //simcenter logo in label
  QLabel *simLogo = new QLabel();
  QPixmap pixmap2(":/imagesCommon/sim_logo_footer.png");
  QPixmap newPixmap2 = pixmap2;
  simLogo->setPixmap(newPixmap2);
  simLogo->show();
  
  // adding both to a layout
  QHBoxLayout *layout = new QHBoxLayout;
  layout->setAlignment(Qt::AlignCenter); //can this be done in CSS???
  layout->addWidget(nsfLogo);
  layout->addWidget(nsfText);
  layout->addWidget(simLogo);

  this->setLayout(layout);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);  
}

FooterWidget::~FooterWidget()
{

}

