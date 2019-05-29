#ifndef SIMFIGURE_H
#define SIMFIGURE_H

#include <QFrame>
#include <QList>
#include <QMap>
#include <QPen>
#include <QBrush>

class QwtPlot;
class QwtPlotGrid;
class QwtPlotItem;
class QwtPlotShapeItem;
class QwtPlotCurve;
class QwtPlotLegendItem;
class QString;
class QwtPlotPicker;

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


namespace Ui {
class SimFigure;
}


class SimFigure : public QFrame
{
    Q_OBJECT

public:
    explicit SimFigure(QWidget *parent = nullptr);
    ~SimFigure();
    void grid(bool mayor=true, bool minor=true);
    int plot(QVector<double> &, QVector<double> &, LineType = LineType::Solid, QColor col = Qt::red, Marker = Marker::None);
    void clear(void);
    void cla(void);
    void legend(QList<QString> labels, Location loc=Location::South);
    void moveLegend(Location loc);
    void showLegend(bool = true);
    bool legendVisible(void);
    void select(int);
    void clearSelection(void);
    AxisType AxisType(void);
    void setAxisType(enum AxisType type);

    int      lineWidth(int ID);
    double   lineWidthF(int ID);
    void     setLineWidth(int ID, int wd);
    void     setLineWidth(int ID, double wd);
    LineType lineStyle(int ID);
    void     setLineStyle(int ID, LineType lt=LineType::Solid, Marker mk=Marker::None);
    QColor   lineColor(int ID);
    void     setLineColor(int ID, QColor color);
    void     setMarker(int ID, Marker mk);

private slots:
    void axisTypeChanged(void);

public slots:
    void on_picker_activated (bool on);
    void on_picker_selected (const QPolygon &polygon);
    void on_picker_appended (const QPoint &pos);
    void on_picker_moved (const QPoint &pos);
    void on_picker_removed (const QPoint &pos);
    void on_picker_changed (const QPolygon &selection);

signals:
    void curve_selected(int ID);

private:
    void select(QwtPlotItem *);
    void setLineStyle(QwtPlotCurve *, LineType lt);
    void setLineColor(QwtPlotCurve *, QColor color);
    void setMarker(QwtPlotCurve *curve, Marker mk);
    QwtPlotItem* itemAt( const QPoint& pos ) const;
    void rescale(void);
    void refreshGrid(void);

    Ui::SimFigure *ui;
    QwtPlot       *m_plot;
    QwtPlotGrid   *m_grid;
    QwtPlotPicker *m_picker;
    QwtPlotLegendItem  *m_legend;
    QMap<QwtPlotCurve *, int> m_plotInvMap;
    QMap<QwtPlotItem *, int>  m_itemInvMap;

    QVector<QwtPlotCurve *> m_curves;

    enum AxisType axisType;
    double  m_xmin = 1.e20;
    double  m_xmax = 1.e-20;
    double  m_ymin = 1.e20;
    double  m_ymax = 1.e-20;

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
