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

enum class AxisType { Default,
                      LogX,
                      LogY,
                      LogLog };

enum class LineType { None,
                      Solid,
                      Dotted,
                      Dashed,
                      DashDotted };

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
    void grid(bool mayor, bool minor);
    int plot(QVector<double> &, QVector<double> &, LineType = LineType::Solid, QColor col = Qt::red, Marker = Marker::None);
    void clear(void);
    void cla(void);
    void legend(QList<QString> labels, Location loc=Location::South);
    void moveLegend(Location loc);
    void showLegend(bool = true);
    bool legendVisible(void);
    void select(int);
    void clearSelection(void);

    int    lineWidth(int ID);
    double lineWidthF(int ID);
    void setLineWidth(int ID, int wd);
    void setLineWidth(int ID, double wd);
    LineType lineStyle(int ID);
    void setLineStyle(int ID, LineType lt);

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
    QwtPlotItem* itemAt( const QPoint& pos ) const;
    void rescale(void);

    Ui::SimFigure *ui;
    QwtPlot       *m_plot;
    QwtPlotGrid   *m_grid;
    QwtPlotPicker *m_picker;
    QwtPlotLegendItem  *m_legend;
    QMap<QwtPlotCurve *, int> m_plotInvMap;
    QMap<QwtPlotItem *, int>  m_itemInvMap;

    QVector<QwtPlotCurve *> m_curves;

    AxisType axisType;
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
};

#endif // SIMFIGURE_H
