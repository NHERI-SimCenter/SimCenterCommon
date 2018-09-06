#if defined(_WIN32) || defined(_WIN62)
    #include <direct.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif
#include <cstdio>
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

  // Remove directories is they still exist from previous runs, then create them again
  std::remove("./test_dir/test_copy.txt");
  rmdir("./test_dir");
  rmdir("./another_test_dir");
  mkdir("./test_dir", S_IRWXU);
  mkdir("./another_test_dir", S_IRWXU);  
  
  SECTION("Copy input path") {
    REQUIRE(app_widget.copyPath("./does_not_exist", "./test_dir", false) == false);
    REQUIRE(app_widget.copyPath("./another_test_dir", "./test_dir", false) == true);
  }

  SECTION("Copy file to destination") {
    REQUIRE(app_widget.copyFile("does_not_exist", "./test_dir") == false);
    REQUIRE(app_widget.copyFile("test_copy.txt", "./test_dir") == true);
  }
}
