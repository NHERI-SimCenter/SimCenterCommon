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

// Written: Sangri Yi, a global function to read json file easier

#include <QFile>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "JsonFunctions.h"

bool parseJsonFile(QString filePath, QJsonValue &jsonVal, QString &errorMsg)
{

    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        errorMsg = QString("Error: could not open file ") + filePath;
        return false;
    }

    // Read the contents of the file
    QByteArray fileData = file.readAll();
    file.close();

    // Parse the JSON data
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        errorMsg = QString("JSON parse error: ") + parseError.errorString();
        return false;
    }

    // Check if the document is an object
    if (jsonDoc.isArray()) {
        jsonVal = jsonDoc.array(); // Assign to QJsonValue
    } else if (jsonDoc.isObject()) {
        jsonVal = jsonDoc.object(); // Assign to QJsonValue
    } else {
        errorMsg = "The JSON document format not identified.";
        return false;
    }

    return true;
}


bool validateJson(const QJsonValue &jsonValue, const QJsonObject &schema, QString &errorMsg, QString parentkey, bool integerAllowedForArray) {
    // Ignore the $schema key during validation
    if (schema.contains("$schema")) {
        // Handle the $schema key if necessary
    }

    errorMsg = "Json Validation error: ";

    if (schema["type"].isString()) {
        QString expectedType = schema["type"].toString();

        if (expectedType == "object") {
            // Validate properties of the object
            if (!jsonValue.isObject()) {
                errorMsg = errorMsg+"Expected an object for: " + parentkey;
                return false;
            }

            const QJsonObject jsonObject = jsonValue.toObject();
            if (!schema["properties"].isObject()) {
                errorMsg = errorMsg+"Expected properties to be an object for: " + parentkey;
                return false;
            }

            const QJsonObject properties = schema["properties"].toObject();
            for (const QString &key : properties.keys()) {
                if (!jsonObject.contains(key)) {
                    errorMsg = errorMsg+"Missing required key: " + key;
                    return false;
                }
                if (!validateJson(jsonObject[key], properties[key].toObject(), errorMsg, key)) {
                    return false;
                }
            }
        } else if (expectedType == "array") {
            if ((!jsonValue.isArray()) && (!jsonValue.isDouble())) {
                    errorMsg = errorMsg+"Expected an array or integer for: " + parentkey;
                    return false;
                }

            if (jsonValue.isArray()) {
                    const QJsonArray jsonArray = jsonValue.toArray();
                    if (schema.contains("items")) {
                        for (const QJsonValue &item : jsonArray) {


//                            if (!item.isObject()) {
//                                errorMsg = "Validation error: Items in array must be objects for key: " + parentkey;
//                                return false;
//                            }
                            if (!validateJson(item, schema["items"].toObject(), errorMsg, parentkey)) {
                                return false;
                            }
                        }
                    }

            }
        } else if (expectedType == "string" && !jsonValue.isString()) {
            errorMsg = errorMsg+"Expected string type for: " + parentkey;
            return false;
        } else if (expectedType == "integer" && !jsonValue.isDouble()) {
            errorMsg = errorMsg+"Expected integer type for: " + parentkey;
            return false;
        } else if (expectedType == "boolean" && !jsonValue.isBool()) {
            errorMsg = errorMsg+"Expected boolean type for: " + parentkey;
            return false;
        }
    } else{
        errorMsg = "Type is not provided in the schema, so json file cannot be validated.";
        return false;
    }

    return true; // All checks passed
}
