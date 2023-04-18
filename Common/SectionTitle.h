/*********************************************************************************
**
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by:
//    Maxwell Rutmann, University of California at Berkeley, CA, United States
//    Peter Mackenzie-Helnwein, University of Washington, Seattle, WA, United States

/***********************************************************
 * HOW TO USE SectionTitle in Qt Creator/Designer          *
 *                                                         *
 * 1) create your section title as QLabel                  *
 * 2) promote this QLabel to SectionTitle                  *
 *                                                         *
 * You me need to create an entry for the promoted widget  *
 * the first time you want to use this technique in your   *
 * Qt Creator/Designer installation.                       *
 *                                                         *
 * Common issues:                                          *
 * make sure the path to the header file is given properly *
 * relative to the main folder.  When using the            *
 * NHERI-SimCenter widgets respository, the proper path is *
 *   ../widgets/Common/sectiontitle.h                      *
 *                                                         *
 ***********************************************************/

#ifndef SECTIONTITLE_H
#define SECTIONTITLE_H

#include <QWidget>
#include <QString>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
class QComboBox;

class SectionTitle : public QFrame
{
    Q_OBJECT

public:

    explicit SectionTitle(QWidget *parent = 0);
    void setTitle(QString);
    void setText(QString s) {this->setTitle(s);};
    void addWidget(QWidget *theWidget);
    void toPlainText(void);

signals:

public slots:

private:
    QGridLayout *sectionLayout;
    QLabel      *sectionLabel;
    QFrame      *line;
};

#endif // SECTIONTITLE_H
