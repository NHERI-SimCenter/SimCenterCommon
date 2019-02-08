#include <SimCenterTableWidget.h>
#include <SpreadsheetWidget.h>

SimCenterTableWidget::SimCenterTableWidget(QWidget *parent) :SimCenterWidget(parent)
{

}

SimCenterTableWidget::~SimCenterTableWidget()
{

}


const SpreadsheetWidget * SimCenterTableWidget::getSpreadsheetWidget()
{
    return theSpreadsheet;
}
