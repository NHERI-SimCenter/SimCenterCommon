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

#include "SectionTitle.h"

SectionTitle::SectionTitle(QWidget *parent) : QFrame(parent)
{
    //Create Frame and Section Title
    sectionLabel = new QLabel(this);
    sectionLabel->setText(tr("Test Section"));
    sectionLabel->setObjectName(QString::fromUtf8("sectionTitle")); //styleSheet

    // Create a section line
    line = new QFrame();
    line->setObjectName(QString::fromUtf8("line"));
    line->setMaximumHeight(3);
    line->setMinimumHeight(3);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    //add line to Layout
    sectionLayout = new QGridLayout(); //VBoxLayout();
    sectionLayout->addWidget(sectionLabel,0,0);
    sectionLayout->addWidget(line,1,0,1,2);
    //sectionLayout->setSpacing(0);
    //sectionLayout->setMargin(0);

    this->setLayout(sectionLayout);
    this->setContentsMargins(0,0,0,0);
}

void SectionTitle::setTitle(QString s)
{
    sectionLabel->setText(s);
}

void SectionTitle::toPlainText(void)
{
    QString titleString = sectionLabel->text();
    sectionLayout->removeWidget(sectionLabel);
    sectionLayout->removeWidget(line);
    sectionLabel->deleteLater();
    line->deleteLater();

    sectionLayout->addWidget(new QLabel(titleString),0,0);

}

void SectionTitle::addWidget(QWidget *theWidget) {
    sectionLayout->addWidget(theWidget, 0, 1);
}
