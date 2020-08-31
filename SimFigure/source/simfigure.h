#ifndef SIMFIGURE_H
#define SIMFIGURE_H

#include <QFrame>
#include <QList>
#include <QMap>
#include <QPen>
#include <QBrush>

//#include "simfigure_enums.h"

class QwtPlot;
class QwtPlotGrid;
class QwtPlotItem;
class QwtPlotShapeItem;
class QwtPlotCurve;
class QwtPlotLegendItem;
class QString;
class QwtPlotPicker;
class QwtPlotZoomer;


namespace Ui {
class SimFigure;
}


class SimFigure : public QFrame
{
    Q_OBJECT

public:

    /**
     * @brief The AxisType enum
     *
     * used for selecting linear or logarithmic scales on th eX and Y axes.
     */
    enum class AxisType { Default, ///< linear scales for x and y
                          LogX,    ///< log scale for x, linear scale for y
                          LogY,    ///< linear scale for x, log scale for y
                          LogLog   ///< log scales for x and y
                        };

    /**
      * @brief The LineType enum
      *
      * used to define the line type used by plot().
      */
    enum class LineType { None,      ///< no line is drawn (only markers)
                          Solid,     ///< a solid line is drawn
                          Dotted,    ///< a dotted line is drawn
                          Dashed,    ///< a dashed line is drawn
                          DashDotted ///< a dash-dotted line is drawn
                        };

    /**
     * @brief The Marker enum
     *
     * used to define the marker used for a particular plot().
     */
    enum class Marker { None,            ///< no marker (line only)
                        Asterisk,        ///< &lowast;
                        Circle,          ///< o
                        Plus,            ///< \+
                        Triangle,        ///< upright triangle
                        DownTriangle,    ///< upside-down triangle
                        RightTriangle,   ///< point to the right triangle
                        LeftTriangle,    ///< point to the left triangle
                        Box,             ///< a square box
                        Ex               ///< x
                      };

    /**
     * @brief The Location enum
     *
     * used to define the position of the legend (if visible) on the plot() canvas.
     */
    enum class Location { Bottom,       ///< centered horizontally at the bottom
                          Top,          ///< centered horizontally at the top
                          Left,         ///< centered vertically on the left
                          Right,        ///< centered vertically on the right
                          TopLeft,      ///< top left corner
                          TopRight,     ///< top right corner
                          BottomLeft,   ///< bottom left corner
                          BottomRight,  ///< bottom right corner
                          North,        ///< equivalent to Top
                          South,        ///< equivalent to Bottom
                          East,         ///< equivalent to Right
                          West,         ///< equivalent to Left
                          NorthWest,    ///< equivalent to TopLeft
                          NorthEast,    ///< equivalent to TopRight
                          SouthWest,    ///< equivalent to BottomLeft
                          SouthEast     ///< equivalent to BottomRight
                        };

    /**
     * @brief The FileType enum
     *
     * used to define file type in saveToFile() and exportToFile()
     */
    enum class FileType {
        PNG, ///< portable network graphic file
        BMP, ///< MicroSOft bitmap file
        PDF, ///< portable document format
        PS,  ///< PostScript file
        SVG  ///< scaleable vector graphic file
    };

    explicit SimFigure(QWidget *parent = nullptr);
    ~SimFigure();

    int plot(QVector<double> &, QVector<double> &, LineType lt = LineType::Solid, QColor col = Qt::red, Marker mk = Marker::None);
    int scatter(QVector<double> &, QVector<double> &, QColor col = Qt::blue, Marker mk = Marker::Circle);


    void clear(void);
    void cla(void);

    void grid(bool mayor=true, bool minor=true);

    void legend(QList<QString> labels, Location loc=Location::South);
    void moveLegend(Location loc);
    void showLegend(bool = true);
    bool legendVisible(void);

    void select(int);
    void clearSelection(void);

    SimFigure::AxisType axisType(void);
    void setAxisType( AxisType type);

    QString xLabel();
    QString yLabel();
    int labelFontSize();
    QString title();
    int titleFontSize();

    void setXLabel(QString lbl);
    void setYLabel(QString lbl);
    void setXLim(double xmin, double xmax);
    void setYLim(double ymin, double ymax);
    void setLabelFontSize(int);
    void setTitle(QString title);
    void setTitleFontSize(int);

    /**
     * @brief returns the currently displayed minimum value of x
     */
    double minX() { return m_xmin; };
    /**
     * @brief returns the currently displayed maximum value of x
     */
    double maxX() { return m_xmax; };
    /**
     * @brief returns the currently displayed minimum value of y
     */
    double minY() { return m_ymin; };
    /**
     * @brief returns the currently displayed maximum value of y
     */
    double maxY() { return m_ymax; };

    void setXlimits(double xmin, double xmax);
    void setYlimits(double ymin, double ymax);

    int      lineWidth(int ID);
    void     setLineWidth(int ID, int wd);

    double   lineWidthF(int ID);
    void     setLineWidthF(int ID, double wd);

    SimFigure::LineType lineStyle(int ID);
    void     setLineStyle(int ID, LineType lt=LineType::Solid, Marker mk=Marker::None);

    QColor   lineColor(int ID);
    void     setLineColor(int ID, QColor color);

    SimFigure::Marker marker(int ID);
    int      markerSize(int ID);
    void     setMarker(int ID, Marker mk, int size=10);

    void saveToFile(QString filename, SimFigure::FileType type=SimFigure::FileType::PNG, QSizeF size=QSizeF(300,200), int res=85);
    void exportToFile(QString filename, SimFigure::FileType type=SimFigure::FileType::PNG, QSizeF size=QSizeF(300,200), int res=85);

private slots:
    void axisTypeChanged(void);

public slots:
    void on_picker_activated (bool on);
    void on_picker_selected (const QPolygon &polygon);
    void on_picker_appended (const QPoint &pos);
    void on_picker_moved (const QPoint &pos);
    void on_picker_removed (const QPoint &pos);
    void on_picker_changed (const QPolygon &selection);
    void showAxisControls(bool show);
    void fit_data();

signals:
    void curve_selected(int ID);

protected:
    void select(QwtPlotItem *);
    void setLineStyle(QwtPlotCurve *, LineType lt);
    void setLineColor(QwtPlotCurve *, QColor color);
    void setMarker(QwtPlotCurve *curve, Marker mk, int size);
    QwtPlotItem* itemAt( const QPoint& pos ) const;
    void rescale(void);
    void refreshGrid(void);

private:
    Ui::SimFigure *ui;
    QwtPlot       *m_plot;
    QwtPlotGrid   *m_grid;
    QwtPlotPicker *m_picker;
    QwtPlotZoomer *m_zoomer = nullptr;
    QwtPlotLegendItem  *m_legend;
    QMap<QwtPlotCurve *, int> m_plotInvMap;
    QMap<QwtPlotItem *, int>  m_itemInvMap;

    QVector<QwtPlotCurve *> m_curves;

    AxisType m_axisType;
    double  m_xmin = 1.e20;
    double  m_xmax = 1.e-20;
    double  m_ymin = 1.e20;
    double  m_ymax = 1.e-20;

    double  m_data_xmin = 1.e20;
    double  m_data_xmax = 1.e-20;
    double  m_data_ymin = 1.e20;
    double  m_data_ymax = 1.e-20;

    struct SELECTION {
        QPen         pen;
        QBrush       brush;
        int          plotID = -1;
        QwtPlotItem *object = nullptr;
    } lastSelection;

    bool  m_showMajorGrid = true;
    bool  m_showMinorGrid = true;
};

#endif // SIMFIGURE_H
