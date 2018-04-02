Check out this commit: 1f9fcee

Particularly changes to:

- InputWidgetSheetBM.cpp
- InputWidgetSheetBM.h
- ConnectionInputWidget.cpp
- ConnectionInputWidget.h

for the pattern on how to make other widgets DRY-er and how to implement Slabsections (only one I didn't get to).

Of particular note:

1) the constructor that takes a connection type as `QString connectionType` in `ConnectionInputWidget.cpp`
2) the `outputToJSON` and `inputFromJSON` member functions which take `QJsonArray` (rather than QJsonObject) in `ConnectionInputWidget.cpp` - pay attention to "bolt group ", or check out `FramesectionInputWidget.cpp` for an example of "longitudinal rebar " and "transverse rebar " field handling.
3) the `QStringList theConnectionTypes`, defined in `InputWidgetSheetBM.cpp`, and the various places in that file `theConnectionTypes` is used to iterate over `QMap<QString, ConnectionInputWidget> theConnectionInputs`.