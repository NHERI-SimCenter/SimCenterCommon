#include <QApplication>
#include <QDir>
#include <QString>
#include "catch.hpp"
#include "SimCenterAppWidget.h"

TEST_CASE("Test SimCenterAppWidget copy file and path functions","[AppWidget]") {

  int argc = 1;
  char * argv[] = {(char *)"test"};
  QApplication app(argc, argv);
  SimCenterAppWidget app_widget;
  
  SECTION("Copy input path") {
    REQUIRE(app_widget.copyPath("./does_not_exist", "./test_dir", false) == false);
    REQUIRE(app_widget.copyPath("./another_test_dir", "./test_dir", false) == true);
  }

  SECTION("Copy file to destination") {
    REQUIRE(app_widget.copyFile("does_not_exist", "./test_dir") == false);
    REQUIRE(app_widget.copyFile("test_copy.txt", "./test_dir") == true);
  }
}
