#ifndef SIMFIGURE_ENUMS_H
#define SIMFIGURE_ENUMS_H

/*! AxisType is used for selecting linear or logarithmic scales on th eX and Y axes. */
enum class AxisType { Default, /*!< linear scales for x and y */
                      LogX,    /*!< log scale for x, linear scale for y */
                      LogY,    /*!< linear scale for x, log scale for y */
                      LogLog   /*!< log scales for x and y */
                    };

/*! LineType is used to define the line type used by plot() */
enum class LineType { None,      /*!< no line is drawn (only markers) */
                      Solid,     /*!< a solid line is drawn */
                      Dotted,    /*!< a dotted line is drawn */
                      Dashed,    /*!< a dashed line is drawn */
                      DashDotted /*!< a dash-dotted line is drawn */
                    };

/*! Marker is used to define the marker used for a particular plot().*/
enum class Marker { None,
                    Asterisk,
                    Circle,
                    Plus,
                    Triangle,
                    DownTriangle,
                    RightTriangle,
                    LeftTriangle,
                    Box,
                    Ex };

enum class Location { Bottom,
                      Top,
                      Left,
                      Right,
                      TopLeft,
                      TopRight,
                      BottomLeft,
                      BottomRight,
                      North,
                      South,
                      East,
                      West,
                      NorthWest,
                      NorthEast,
                      SouthWest,
                      SouthEast };

#endif // SIMFIGURE_ENUMS_H
