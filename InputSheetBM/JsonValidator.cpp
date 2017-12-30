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
//    Maurice Manning, University of California at Berkeley, CA, United States


#include "JsonValidator.h"

#include "../rapidjson/error/en.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/schema.h"
#include "../rapidjson/document.h"
#include "../rapidjson/writer.h"
#include "../rapidjson/stringbuffer.h"
#include <iostream>
#include <stdio.h>
#include <QDir.h>
#include <QMessageBox>
#include <QAction>
#include <QTranslator>
#include <QDebug>
#include <QCoreApplication>

using namespace rapidjson;



JsonValidator::JsonValidator()
{

}

void JsonValidator::validate(QWidget *parent, SCHEMA schema, const QString &fileName)
{

    QString schemaFilepath = "";
    if (schema == BIM) {


        //qDebug() << "homePath: " << QDir::homePath();
        //qDebug() << "applicationDirPath: " << QCoreApplication::applicationDirPath();
        //qDebug() << "applicationFilePath: " << QCoreApplication::applicationFilePath();

        //QDir::setSearchPaths("schema", QStringList(QDir::homePath() + "/schema"));

        const QString fileName = QStringLiteral(":schema/BIM.schema.json");
        QFile schemaFile(fileName);

        if (!schemaFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Cannot open" << QDir::toNativeSeparators(fileName)
                << ':' << schemaFile.errorString();
            return;
        }

        const QString schemaText(QString::fromUtf8(schemaFile.readAll()));

        //schemaFilepath = bimSchemaFilepath;
        //schemaFilepath = QDir().absoluteFilePath(bimSchemaFilepath);
        schemaFilepath = QDir().absoluteFilePath("/Users/mauricemanning/Dev/code/simcenter/widgets/schema/BIM.schema.json");
        //schemaFilepath = QDir().absoluteFilePath("/Users/mmmanning/Documents/Dev/code/simcenter/BIM.schema.json");
    }

    //
    // open the BIM schema file
    //
    // Read a JSON schema from file into Document
    Document d;
    char buffer[4096];

    FILE *fp = fopen(schemaFilepath.toStdString().c_str(), "r");

    //QFile schemaFile("qrc:/BIM.schema.json");
    //schemaFile.open(QIODevice::ReadOnly);

    //int FileDescriptor = schemaFile.handle();
    //FILE *fp = fopen(FileDescriptor, "r");

    // !schemaFile.exists()
    if (!fp) {
        printf("Schema file '%s' not found\n", schemaFilepath.toStdString().c_str() );
        return;
    }

    // read the schema document file

    FileReadStream fs(fp, buffer, sizeof(buffer));
    d.ParseStream(fs);


    //convert document to string
    StringBuffer strbuf;
    strbuf.Clear();
    Writer<StringBuffer> writer(strbuf);
    d.Accept(writer);
    std::string ownShipRadarString = strbuf.GetString();
    std::cout << "**********************************************" << ownShipRadarString << std::endl;


    if (d.HasParseError()) {
        fprintf(stderr, "Schema file '%s' is not a valid JSON\n", schemaFilepath.toStdString().c_str() );
        fprintf(stderr, "Error(offset %u): %s\n",  static_cast<unsigned>(d.GetErrorOffset()), GetParseError_En(d.GetParseError()));
        fclose(fp);
       // return;

        QMessageBox::warning(
            parent,
            QMessageBox::tr("Validation"),
            QMessageBox::tr("Invalid Schema Document")
        );


    }
    fclose(fp);

    // Then convert the Document into SchemaDocument
    SchemaDocument sd(d);

    // read the DATA document file
    FILE *fpdata = fopen(fileName.toStdString().c_str(), "r");
    if (!fpdata) {
        printf("Schema file '%s' not found\n", fileName.toStdString().c_str() );
        return;
    }


    // Use reader to parse the JSON in stdin, and forward SAX events to validator
    SchemaValidator validator(sd);
    Reader reader;
    FileReadStream is(fpdata, buffer, sizeof(buffer));
    if (!reader.Parse(is, validator) && reader.GetParseErrorCode() != kParseErrorTermination) {
        // Schema validator error would cause kParseErrorTermination, which will handle it in next step.
        fprintf(stderr, "Input is not a valid JSON\n");
        fprintf(stderr, "Error(offset %u): %s\n",
            static_cast<unsigned>(reader.GetErrorOffset()),
            GetParseError_En(reader.GetParseErrorCode()));


        QMessageBox::warning(
            parent,
            QMessageBox::tr("Validation"),
            QMessageBox::tr("Input is not valid JSON: %1:\n%2.")
                    .arg(reader.GetErrorOffset())
                    .arg(reader.GetErrorOffset())
        );

        //QErrorMessage* errorMessage = new QErrorMessage( parent );
        //std::string var = "Input is not valid JSON. \noffset: " +  std::to_string(reader.GetErrorOffset()) + "\nparse error: " + std::to_string(reader.GetParseErrorCode());
        //errorMessage->message(var);

    }

    // Check the validation result
    if (validator.IsValid()) {
        printf("Input JSON is valid.\n");
        return;
    }
    else {
        printf("Input JSON is invalid.\n");
        StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        fprintf(stderr, "Invalid schema: %s\n", sb.GetString());
        fprintf(stderr, "Invalid keyword: %s\n", validator.GetInvalidSchemaKeyword());
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        fprintf(stderr, "Invalid document: %s\n", sb.GetString());

        QMessageBox::warning(
            parent,
            QMessageBox::tr("Validation"),
            QMessageBox::tr("Validation errors.\n schema: %1:\n keyword: %2.")
                    .arg(sb.GetString())
                    .arg(validator.GetInvalidSchemaKeyword())
        );



        return;
    }



}
