#include "simfigure.h"
#include "ui_simfigure.h"

#include <QVBoxLayout>
#include <QBrush>
#include <QFont>
#include <QVector>
#include <QPoint>
#include <QPolygon>
#include <QPointF>
#include <QPolygonF>
#include <QMap>
#include <QMapIterator>
#include <QFileDialog>
#include <QSize>
#include <QSizeF>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_renderer.h>

#include "qwt_picker.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_item.h"
#include "qwt_plot_shapeitem.h"
#include "qwt_picker_machine.h"

#include <QDebug>

#include <algorithm>

#define MIN(vec) *std::min_element(vec.constBegin(), vec.constEnd())
#define MAX(vec) *std::max_element(vec.constBegin(), vec.constEnd())

SimFigure::SimFigure(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::SimFigure)
  /*! SimFigure
   *  is a widget that emulates a MATLAB-like
   *  interface to Qwt, allowing for a quick porting of
   *  MATLAB graphics to Qt5. Creating an instance of
   *  SimFigure is equivalent to MATLAB's
   *
   *  h = figure
   *
   * @param parent is a pointer to the parent widget
   * \header simfigure.h "code/simfigure.h"
   */
{
    ui->setupUi(this);

    m_grid   = nullptr;
    m_legend = nullptr;
    m_curves.clear();

    m_plot = new QwtPlot(this);
    QVBoxLayout *lyt = new QVBoxLayout(ui->pltWidgetSpace);
    lyt->addWidget(m_plot);
    m_plot->setCanvasBackground(QBrush(Qt::white));

    ui->btn_standard->setChecked(true);

    m_axisType = AxisType::Default;
    m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine(10));
    m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine(10));

    m_plot->setAxisScale(QwtPlot::yLeft,   1, 100);
    m_plot->setAxisScale(QwtPlot::xBottom, 1, 100);

    grid(true, true);

    m_picker = new QwtPlotPicker(m_plot->canvas());
    m_picker->setStateMachine(new QwtPickerClickPointMachine);
    //m_picker->setTrackerMode(QwtPicker::AlwaysOff);
    m_picker->setTrackerMode(QwtPicker::AlwaysOn);
    m_picker->setRubberBand(QwtPicker::RectRubberBand);

    m_zoomer = new QwtPlotZoomer(m_plot->canvas());

    connect(m_picker, SIGNAL(activated(bool)), this, SLOT(on_picker_activated(bool)));
    connect(m_picker, SIGNAL(selected(const QPolygon &)), this, SLOT(on_picker_selected(const QPolygon &)));
    connect(m_picker, SIGNAL(appended(const QPoint &)), this, SLOT(on_picker_appended(const QPoint &)));
    connect(m_picker, SIGNAL(moved(const QPoint &)), this, SLOT(on_picker_moved(const QPoint &)));
    connect(m_picker, SIGNAL(removed(const QPoint &)), this, SLOT(on_picker_removed(const QPoint &)));
    connect(m_picker, SIGNAL(changed(const QPolygon &)), this, SLOT(on_picker_changed(const QPolygon &)));
}

/*! the SimFIgure destructor */
SimFigure::~SimFigure()
{
    delete ui;
}

/*! This signal is emited whenever one of the 'Default', 'LogX', 'LogY', or 'LogLog'
 * radiobuttons is clicked.
 * It is further emited if the axistype is changed via a call to
 */
void SimFigure::axisTypeChanged(void)
{
    if (ui->btn_standard->isChecked())
    {
        if (m_axisType != AxisType::Default)
        {
            m_axisType = AxisType::Default;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_logX->isChecked())
    {
        if (m_axisType != AxisType::LogX)
        {
            m_axisType = AxisType::LogX;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_logY->isChecked())
    {
        if (m_axisType != AxisType::LogY)
        {
            m_axisType = AxisType::LogY;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_loglog->isChecked())
    {
        if (m_axisType != AxisType::LogLog)
        {
            m_axisType = AxisType::LogLog;

            m_plot->setAxisMaxMajor( QwtPlot::yLeft, 6 );
            m_plot->setAxisMaxMinor( QwtPlot::yLeft, 9 );

            m_plot->setAxisMaxMajor( QwtPlot::xBottom, 6 );
            m_plot->setAxisMaxMinor( QwtPlot::xBottom, 9 );

            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }

    grid(true, true);

    m_plot->repaint();

    //qDebug() << "signal axisTypeChanged received " << int(m_axisType);
}

void SimFigure::grid(bool major, bool minor)
/*! generate a grid with major (true|false) and minor (true|false) grid markers and lines.
 *
 * grid()   turns major and monor grid on
 *
 * grid( false, false ) turns the grid off
 */
{
    m_showMajorGrid = major;
    m_showMinorGrid = minor;

    refreshGrid();
}

SimFigure::AxisType SimFigure::axisType(void)
/*! returns the current AxisType */
{
    return m_axisType;
}

/*! set the AxisType for the current grid */
void SimFigure::setAxisType(enum AxisType type)
{
    m_axisType = type;
    switch (m_axisType) {
    case AxisType::LogX :
        ui->btn_logX->setChecked(true);
        break;
    case AxisType::LogY :
        ui->btn_logY->setChecked(true);
        break;
    case AxisType::LogLog :
        ui->btn_loglog->setChecked(true);
        break;
    case AxisType::Default :
        ui->btn_standard->setChecked(true);
        break;
    }
    this->axisTypeChanged();
}

/*! The plot() method provides a plot functionality similar to MATLAP's plot function
 * x and y are regerences to QVector<double>.  The must be of equal length.
 *
 * plot returns an integer serving as a unique handle for the curve. The following functions use that handle
 * to read or change settings for that curve:
 *
 * lineWidth(),  lineWidthF(), setLineWidth(),
 * setLineWidthF(), lineStyle(), setLineStyle(),
 * lineColor(), setLineColor(), setMarker()
 *
 * @param lt a member of the SimFigure::LineType enum
 * @param color a QColor object defining the line color.  You may also use pre-defined colors like Qt::red, ...
 * @param mk a member of the SimFigure::Marker enum.
 */
int SimFigure::plot(QVector<double> &x, QVector<double> &y, LineType lt, QColor color, Marker mk)
{
    if (x.length() <= 0 || y.length() <= 0) return -1;

    // update min and max values

    if (MAX(x) > m_data_xmax) m_data_xmax=MAX(x);
    if (MIN(x) < m_data_xmin) m_data_xmin=MIN(x);
    if (MAX(y) > m_data_ymax) m_data_ymax=MAX(y);
    if (MIN(y) < m_data_ymin) m_data_ymin=MIN(y);

    // now add that curve

    QwtPlotCurve *curve = new QwtPlotCurve("default");
    curve->setSamples(x,y);

    setLineStyle(curve, lt);
    setMarker(curve, mk, 10);
    setLineColor(curve, color);

    curve->attach(m_plot);

    m_curves.append(curve);

    //grid(true,true);
    fit_data();
    m_plot->replot();

    int idx = m_curves.length();
    m_plotInvMap.insert(curve, idx);

    return idx;
}

/*! The scatter() method provides a plot functionality similar to MATLAP's scatter function
 * x and y are regerences to QVector<double>.  The must be of equal length.
 *
 * scatter returns an integer serving as a unique handle for all points. The following functions use that handle
 * to read or change settings for that curve:
 *
 * lineWidth(),  lineWidthF(), setLineWidth(),
 * setLineWidthF(), lineStyle(), setLineStyle(),
 * lineColor(), setLineColor(), setMarker()
 *
 * @param color a QColor object defining the line color.  You may also use pre-defined colors like Qt::red, ...
 * @param mk a member of the SimFigure::Marker enum.
 */
int SimFigure::scatter(QVector<double> &x, QVector<double> &y, QColor color, Marker mk)
{
    return plot(x, y, SimFigure::LineType::None, color, mk);
}

/**
 * @brief Sets x-axis limits to given values.  xmax must be larger than xmin.
 * @brief See maxX() and minX() on how to obtain the current limits.
 */
void SimFigure::setXlimits(double xmin, double xmax)
{
    if (xmin<xmax) {
        m_xmax = xmax;
        m_xmin = xmin;
        this->rescale();
    }
}


/**
 * @brief Sets y-axis limits to given values.  ymax must be larger than ymin.
 * @brief See maxY() and minY() on how to obtain the current limits.
 */
void SimFigure::setYlimits(double ymin, double ymax)
{
    if (ymin<ymax) {
        m_ymax = ymax;
        m_ymin = ymin;
        this->rescale();
    }
}

/**
 * @brief SimFigure::xLabel()
 * @return title of the current x-axis label
 */
QString SimFigure::xLabel(void)
{
    return m_plot->axisTitle(QwtPlot::xBottom).text();
}

/**
 * @brief SimFigure::yLabel()
 * @return text of the current y-axis label
 */
QString SimFigure::yLabel(void)
{
    return m_plot->axisTitle(QwtPlot::yLeft).text();
}

/**
 * @brief returns the current font size used for xLabel() and yLabel()
 */
int SimFigure::labelFontSize()
{
    int size = m_plot->axisTitle(QwtPlot::xBottom).font().pointSize();
    return size;
}

/**
 * @brief returns the current font size used for title()
 */
int SimFigure::titleFontSize()
{
    int size = m_plot->title().font().pointSize();
    return size;
}

/**
 * @brief sets the current font size used for xLabel() and yLabel() to sz
 */
void SimFigure::setLabelFontSize(int sz)
{
    if (sz>0)
    {
        QwtText text = m_plot->axisTitle(QwtPlot::xBottom);
        QFont font = text.font();
        font.setPointSize(sz);
        text.setFont(font);
        m_plot->setAxisTitle(QwtPlot::xBottom, text);

        text = m_plot->axisTitle(QwtPlot::yLeft);
        text.setFont(font);
        m_plot->setAxisTitle(QwtPlot::yLeft, text);
    }
}

/**
 * @brief sets the current font size used for title() to sz
 */
void SimFigure::setTitleFontSize(int sz)
{
    if (sz>0)
    {
        QwtText text = m_plot->title();
        QFont font = text.font();
        font.setPointSize(sz);
        text.setFont(font);
        m_plot->setTitle(text);
    }
}

/**
 * @brief SimFigure::title()
 * @return title of the current figure
 */
QString SimFigure::title(void)
{
    return m_plot->title().text();
}

/**
 * set the title for the x-axis
 */
void SimFigure::setXLabel(QString label)
{
    m_plot->setAxisTitle(QwtPlot::xBottom, label);
}

/**
 * set the title for the y-axis
 */
void SimFigure::setYLabel(QString label)
{
    m_plot->setAxisTitle(QwtPlot::yLeft, label);
}

/**
 * set the figure title
 */
void SimFigure::setTitle(QString title)
{
    m_plot->setTitle(title);
}

/*! reinitialize the scale engine for both axes (private) */
void SimFigure::rescale(void)
{
    if (m_curves.length() > 0)
    {
        m_plot->setAxisScale(QwtPlot::yLeft,   m_ymin, m_ymax);
        m_plot->setAxisScale(QwtPlot::xBottom, m_xmin, m_xmax);
    }
    else
    {
        m_plot->setAxisScale(QwtPlot::yLeft,   1, 100);
        m_plot->setAxisScale(QwtPlot::xBottom, 1, 100);
    }
    m_plot->replot();
    m_plot->repaint();

}

/*! set limits for x axis (private) */
void SimFigure::setXLim(double xmin, double xmax)
{
    m_plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);
    m_plot->replot();
}

/*! set limits for y axis (private) */
void SimFigure::setYLim(double ymin, double ymax)
{
    m_plot->setAxisScale(QwtPlot::yLeft, ymin, ymax);
    m_plot->replot();
}

/*! Regenerate th egrid with new settings (Type, limits) - (private) */
void SimFigure::refreshGrid(void)
{

    if (m_grid != nullptr)
    {
        m_grid->detach();
        delete m_grid;
        m_grid = nullptr;
    }
    // Create Background Grid for Plot
    if (m_showMajorGrid)
    {
        m_grid = new QwtPlotGrid();
        m_grid->attach( m_plot );
        m_grid->setAxes(QwtPlot::xBottom, QwtPlot::yLeft);

        //m_plot->enableAxis(QwtPlot::xBottom);

        m_grid->setMajorPen(QPen(Qt::darkGray, 0.8));
        m_grid->setMinorPen(QPen(Qt::lightGray, 0.5));
        m_grid->setZ(1);
        m_grid->enableX(true);
        m_grid->enableY(true);

        if (m_showMinorGrid)
        {
            m_grid->enableXMin(true);
            m_grid->enableYMin(true);
        }
        else {
            m_grid->enableXMin(false);
            m_grid->enableYMin(false);
        }

        switch (m_axisType) {
        case AxisType::Default:
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
            break;
        case AxisType::LogX:
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
            break;
        case AxisType::LogY:
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
            break;
        case AxisType::LogLog:
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
            break;
        }
    }

    m_plot->replot();
}

/*! clear curent axes -- clears the figure but preserves the grid type
 *
 * identical to cla()
 */
void SimFigure::clear(void)
{
    cla();
}

/*! clear curent axes -- clears the figure but preserves the grid type */
void SimFigure::cla(void)
{
    foreach (QwtPlotCurve *curve, m_curves)
    {
        curve->detach();
        delete curve;

        m_plotInvMap.clear();
    }
    m_curves.clear();

    // reset label and title fonts
    setTitleFontSize(14);
    setLabelFontSize(12);

    lastSelection.object = nullptr;
    lastSelection.plotID = -1;

    m_data_xmin = 1.e20;
    m_data_xmax = 1.e-20;
    m_data_ymin = 1.e20;
    m_data_ymax = 1.e-20;

    m_xmin = 1.e20;
    m_xmax = 1.e-20;
    m_ymin = 1.e20;
    m_ymax = 1.e-20;

    m_plot->replot();

    emit curve_selected(-1);
}

/*! Add a legend to the current plot.
 *
 * Location is an enum class.
 */
void SimFigure::legend(QList<QString> labels, Location loc)
{
    moveLegend(loc);

    if (labels.length()>0)
    {
        showLegend();
    }
}

/*! Move the legend to a new location identified by Location
 * (enum class Location) */
void SimFigure::moveLegend(Location loc)
{
    if (m_legend == nullptr)
    {
        m_legend = new QwtPlotLegendItem();
        m_legend->attach(m_plot);
    }
    m_legend->setMaxColumns(1);

    uint alignment = 0;

    if (m_legend) {

        switch (loc) {
        case Location::Top:
        case Location::North:
            alignment = Qt::AlignTop|Qt::AlignHCenter;
            break;
        case Location::Bottom:
        case Location::South:
            alignment = Qt::AlignBottom|Qt::AlignHCenter;
            break;
        case Location::Left:
        case Location::West:
            alignment = Qt::AlignLeft|Qt::AlignVCenter;
            break;
        case Location::Right:
        case Location::East:
            alignment = Qt::AlignRight|Qt::AlignVCenter;
            break;
        case Location::TopLeft:
        case Location::NorthWest:
            alignment = Qt::AlignTop|Qt::AlignLeft;
            break;
        case Location::BottomLeft:
        case Location::SouthWest:
            alignment = Qt::AlignBottom|Qt::AlignLeft;
            break;
        case Location::TopRight:
        case Location::NorthEast:
            alignment = Qt::AlignRight|Qt::AlignTop;
            break;
        case Location::BottomRight:
        case Location::SouthEast:
            alignment = Qt::AlignRight|Qt::AlignBottom; break;
        }

        m_legend->setAlignment(Qt::Alignment(alignment));
        m_plot->replot();
        m_plot->repaint();
    }
}

/*! show (on=true) or hide (on=false) a legend for the current plots. */
void SimFigure::showLegend(bool on)
{
    if (m_legend == nullptr)
    {
        m_legend = new QwtPlotLegendItem();
        m_legend->attach(m_plot);
    }
    m_legend->setMaxColumns(1);

    if (on)
    {
        m_legend->show();
    }
    else {
        m_legend->hide();
    }

    m_plot->replot();
    m_plot->repaint();
}

/*! check if legend is currently visible.*/
bool SimFigure::legendVisible(void)
{
    return (m_legend!=nullptr && m_legend->isVisible());
}

/*! DEPRICATED */
void SimFigure::on_picker_activated(bool on)
{
    //qWarning() << "picker activated " << on;
}

/*! DEPRICATED */
void SimFigure::on_picker_selected(const QPolygon &polygon)
{
    //qWarning() << "picker selected " << polygon;
}

/*! upon mouse click inside the plot canvas, identify the QwtPlotItem most likely selected by that mouse event.  There is a 5 pixel tolerance to either side of a curve for which a hit will be detected. */
void SimFigure::on_picker_appended (const QPoint &pos)
{
    //qWarning() << "picker appended " << pos;

    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = m_plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = m_plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = m_plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = m_plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    QwtPlotItem *item = itemAt(pos);

    if ( item )
    {
        if ( item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *theShape = static_cast<QwtPlotShapeItem *>(item);
            theShape->setPen(Qt::cyan, 4);
            QBrush brush = theShape->brush();
            QColor color = brush.color();
            color.setAlpha(64);
            brush.setColor(color);
            theShape->setBrush(brush);
        }

        if ( item->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *theCurve = static_cast<QwtPlotCurve *>(item);

            if (lastSelection.object != item)
            {
                if (lastSelection.object != nullptr) clearSelection();
                select(item);
            }

            // we need a way to revert to original color schema when a different curve is selected.

        }

        m_plot->replot();
    }
    else
    {
        qWarning() << "no item itentified at" << coords[QwtPlot::xBottom] << coords[QwtPlot::yLeft];
    }
}

/*! DEPRICATED */
void SimFigure::on_picker_moved (const QPoint &pos)
{
    //qWarning() << "picker moved " << pos;
}

/*! DEPRICATED */
void SimFigure::on_picker_removed (const QPoint &pos)
{
    //qWarning() << "picker removed " << pos;
}

/*! DEPRICATED */
void SimFigure::on_picker_changed (const QPolygon &selection)
{
    //qWarning() << "picker changed " << selection;
}


void SimFigure::showAxisControls(bool show)
{
    this->ui->axisControls->setVisible(show);
}


void SimFigure::fit_data()
{
    m_xmax = m_data_xmax;
    m_xmin = m_data_xmin;
    m_ymax = m_data_ymax;
    m_ymin = m_data_ymin;
    rescale();

    //m_zoomer->zoom(QRectF(m_xmin,m_ymax,m_xmax-m_xmin,m_ymax-m_ymin));
}

/*! returns a pointer to the QwtPlotItem selected by the last mouse click (private)*/
QwtPlotItem* SimFigure::itemAt( const QPoint& pos ) const
{
    if ( m_plot == nullptr )
        return nullptr;

    // translate pos into the plot coordinates
    double coords[ QwtPlot::axisCnt ];
    coords[ QwtPlot::xBottom ] = m_plot->canvasMap( QwtPlot::xBottom ).invTransform( pos.x() );
    coords[ QwtPlot::xTop ]    = m_plot->canvasMap( QwtPlot::xTop ).invTransform( pos.x() );
    coords[ QwtPlot::yLeft ]   = m_plot->canvasMap( QwtPlot::yLeft ).invTransform( pos.y() );
    coords[ QwtPlot::yRight ]  = m_plot->canvasMap( QwtPlot::yRight ).invTransform( pos.y() );

    QwtPlotItemList items = m_plot->itemList();
    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        QwtPlotItem *item = items[ i ];
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            double dist;

            QwtPlotCurve *curveItem = static_cast<QwtPlotCurve *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( curveItem->boundingRect().contains( p ) || true )
            {
                // trace curves ...
                dist = 1000.;
                for (size_t line=0; line < curveItem->dataSize() - 1; line++)
                {
                    QPointF pnt;
                    double x, y;

                    pnt = curveItem->sample(line);
                    x = m_plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = m_plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x0(x,y);

                    pnt = curveItem->sample(line+1);
                    x = m_plot->canvasMap( QwtPlot::xBottom ).transform( pnt.x() );
                    y = m_plot->canvasMap( QwtPlot::yLeft ).transform( pnt.y() );
                    QPointF x1(x,y);

                    QPointF r  = pos - x0;
                    QPointF s  = x1 - x0;
                    double s2  = QPointF::dotProduct(s,s);

                    if (s2 > 1e-6)
                    {
                        double xi  = QPointF::dotProduct(r,s) / s2;

                        if ( 0.0 <= xi && xi <= 1.0 )
                        {
                            QPointF t(-s.y()/sqrt(s2), s.x()/sqrt(s2));
                            double d1 = QPointF::dotProduct(r,t);
                            if ( d1 < 0.0 )  { d1 = -d1; }
                            if ( d1 < dist ) { dist = d1;}
                        }
                    }
                    else
                    {
                        dist = sqrt(QPointF::dotProduct(r,r));
                        QPointF r2 = pos - x1;
                        double d2  = sqrt(QPointF::dotProduct(r2,r2));  // review
                        if ( d2 < dist ) { dist = d2; }
                    }
                }

                //qWarning() << "curve dist =" << dist;

                if ( dist <= 5 ) return static_cast<QwtPlotItem *>(curveItem);
            }
        }
        if ( item->isVisible() && item->rtti() == QwtPlotItem::Rtti_PlotShape )
        {
            QwtPlotShapeItem *shapeItem = static_cast<QwtPlotShapeItem *>( item );
            const QPointF p( coords[ item->xAxis() ], coords[ item->yAxis() ] );

            if ( shapeItem->boundingRect().contains( p ) && shapeItem->shape().contains( p ) )
            {
                return static_cast<QwtPlotItem *>(shapeItem);
            }
        }
    }

    return nullptr;
}


/*! select the curve identified by its integer handle ID.
 * Upon completion, this function will emit a SIGNAL(curve_selected(int)) with
 * value = integer handle of that curve.*/
void SimFigure::select(int ID)
{
    QwtPlotItem *theItem = nullptr;

    if (ID > 0 && m_curves.length() >= ID && m_curves.value(ID-1) != nullptr)
        theItem = m_curves.value(ID-1);

    if (theItem) select(theItem);
}

/*! select the curve identified by the provided pointer (private).
 * Upon completion, this function will emit a SIGNAL(curve_selected(int)) with value = integer handle of that curve.*/
void SimFigure::select(QwtPlotItem *item)
{
    clearSelection();

    if (item != nullptr)
    {
        QwtPlotCurve *theCurve = static_cast<QwtPlotCurve *>(item);

        // save settings
        lastSelection.object = item;  // we need to use the generic pointer
        lastSelection.plotID = m_plotInvMap.value(theCurve, -1);
        lastSelection.pen = theCurve->pen();
        lastSelection.brush = theCurve->brush();

        // visually ID selected
        theCurve->setPen(Qt::cyan, 4);

        // let code now that selection changed
        int ID = m_plotInvMap.value(theCurve, -1);
        m_plot->replot();

        emit curve_selected(ID);
    }
}

/*! clear selection of any curve. Upon completion, this function will emit a SIGNAL(curve_selected(int)) with value -1.*/
void SimFigure::clearSelection(void)
{
    if (lastSelection.object != nullptr)
    {
        // restore old settings
        QwtPlotCurve *lastCurve = static_cast<QwtPlotCurve *>(lastSelection.object);
        lastCurve->setPen(lastSelection.pen);
        lastCurve->setBrush(lastSelection.brush);
        lastSelection.object = nullptr;  // we need to use the generic pointer
        lastSelection.plotID = -1;

        m_plot->replot();
    }

    emit curve_selected(-1);
}

/*! return the line width of the curve with handle ID */
int SimFigure::lineWidth(int ID)
{
    int w = 0;

    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        QPen pen = m_curves.value(ID-1)->pen();
        w = pen.width();
    }
    return w;
}

/*! return the line width of the curve with handle ID */
double SimFigure::lineWidthF(int ID)
{
    double w = 0;

    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        QPen pen = m_curves.value(ID-1)->pen();
        w = pen.widthF();
    }
    return w;
}

/*! change the line width of the curve with handle ID */
void SimFigure::setLineWidth(int ID, int wd)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        QPen pen = m_curves.value(ID-1)->pen();
        pen.setWidth(wd);
        m_curves.value(ID-1)->setPen(pen);
    }
}

/*! change the line width of the curve with handle ID */
void SimFigure::setLineWidthF(int ID, double wd)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        QPen pen = m_curves.value(ID-1)->pen();
        pen.setWidthF(wd);
        m_curves.value(ID-1)->setPen(pen);
    }
}

/*! returns the line style of a curve. */
SimFigure::LineType SimFigure::lineStyle(int ID)
{
    return LineType::Solid;  // for now
}

/*! used to change the current line style of a curve. */
void SimFigure::setLineStyle(int ID, LineType lt, Marker mk)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        setLineStyle(m_curves.value(ID-1), lt);
        setMarker(m_curves.value(ID-1), mk, 10);
    }
}

/*! used to change the current line style of a curve. (private) */
void SimFigure::setLineStyle(QwtPlotCurve *curve, LineType lt)
{
    QPen pen = curve->pen();
    pen.setWidth(2);

    switch (lt)
    {
    case LineType::None :
        pen.setStyle(Qt::NoPen);
        break;
    case LineType::Solid :
        pen.setStyle(Qt::SolidLine);
        break;
    case LineType::Dashed :
        pen.setStyle(Qt::DashLine);
        break;
    case LineType::Dotted :
        pen.setStyle(Qt::DotLine);
        break;
    case LineType::DashDotted :
        pen.setStyle(Qt::DashDotLine);
        break;
    }

    curve->setPen(pen);
}

/**
 * @brief SimFigure::marker
 * @param ID ... unique handle for the curve of interest
 * @return SimFigure::Marker enum to identify the marker for curve with handle=ID
 */
SimFigure::Marker SimFigure::marker(int ID)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        const QwtSymbol *sym = m_curves.value(ID-1)->symbol();
        int mk = sym->style();
        switch (mk) {
            case QwtSymbol::NoSymbol:
                return Marker::None;
            case QwtSymbol::XCross:
                return Marker::Ex;
            case QwtSymbol::Rect:
                return Marker::Box;
            case QwtSymbol::Cross:
                return Marker::Plus;
            case QwtSymbol::Ellipse:
                return Marker::Circle;
            case QwtSymbol::Star1:
                return Marker::Asterisk;
            case QwtSymbol::Triangle:
                return Marker::Triangle;
            case QwtSymbol::DTriangle:
                return Marker::DownTriangle;
            case QwtSymbol::LTriangle:
                return Marker::LeftTriangle;
            case QwtSymbol::RTriangle:
                return Marker::RightTriangle;
        }
    }
    else {
        return Marker::None;
    }
}

/**
 * @brief SimFigure::markerSize
 * @param ID ... unique handle for the curve of interest
 * @return size of the marker in pixels
 */
int SimFigure::markerSize(int ID)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        const QwtSymbol *sym = m_curves.value(ID-1)->symbol();
        int size = sym->size().width();
        return size;
    }
    else {
        return -1;
    }
}

/**
 * used to change the current marker of a curve.
 */
void SimFigure::setMarker(int ID, Marker mk, int size)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        setMarker(m_curves.value(ID-1), mk, size);
    }
}

/*! used to change the current marker of a curve. (private) */
void SimFigure::setMarker(QwtPlotCurve *curve, Marker mk, int size)
{
    QColor color;
    QColor brush;

    if (curve) {

        /* these colors return Qt::black even though th ecurve has color. May be bug in Qwt (?) */
        color = curve->pen().color();
        brush = curve->brush().color();

        //qDebug() << "Color / brush: " << color << " / " << brush;

        QwtSymbol *newSymbol = nullptr;

        if (size>0) {

            switch (mk)
            {
            case Marker::None :
                newSymbol = new QwtSymbol(QwtSymbol::NoSymbol);
                break;
            case Marker::Ex :
                newSymbol = new QwtSymbol(QwtSymbol::XCross);
                break;
            case Marker::Box :
                newSymbol = new QwtSymbol(QwtSymbol::Rect);
                break;
            case Marker::Plus :
                newSymbol = new QwtSymbol(QwtSymbol::Cross);
                break;
            case Marker::Circle :
                newSymbol = new QwtSymbol(QwtSymbol::Ellipse);
                break;
            case Marker::Asterisk :
                newSymbol = new QwtSymbol(QwtSymbol::Star1);
                break;
            case Marker::Triangle :
                newSymbol = new QwtSymbol(QwtSymbol::Triangle);
                break;
            case Marker::DownTriangle :
                newSymbol = new QwtSymbol(QwtSymbol::DTriangle);
                break;
            case Marker::LeftTriangle :
                newSymbol = new QwtSymbol(QwtSymbol::LTriangle);
                break;
            case Marker::RightTriangle :
                newSymbol = new QwtSymbol(QwtSymbol::RTriangle);
                break;
            }
        }
        else {
            newSymbol = new QwtSymbol(QwtSymbol::NoSymbol);
        }

        newSymbol->setSize(size);

        newSymbol->setPen(color, 0.5);
        newSymbol->setColor(Qt::lightGray);

        curve->setSymbol(newSymbol);
    }
}

/*! returns line color of the curve with handle ID. */
QColor SimFigure::lineColor(int ID)
{
    QColor color;
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        color = m_curves.value(ID-1)->pen().color();
    }
    else {
        color = Qt::red;
    }
    return color;
}

/*! used to change the current color of a curve. */
void SimFigure::setLineColor(int ID, QColor color)
{
    if (ID > 0 && m_curves.length() <= ID && m_curves.value(ID-1) != nullptr)
    {
        setLineColor(m_curves.value(ID-1), color);
    }
}

/*! used to change the current color of a curve. (private) */
void SimFigure::setLineColor(QwtPlotCurve *curve, QColor color)
{
    QPen pen = curve->pen();
    pen.setColor(color);
    curve->setPen(pen);
}

/**
 * @brief save image to file
 *
 * used to save the plot to an image file.  Location and filename are defined
 * from within the code.  No user interaction required.
 *
 * @param filename includes fully specified path
 * @param type default is SimFigure::FileType::PNG
 * @param size QSizeF(width in MM, height in MM).
 *             Defaults to 300mm/200mm (~12in/8in)
 * @param res resolution: use 75-95 for screen, ~100-150 for presentations, 300 for quality print.
 *            Defaults to 85
 */
void SimFigure::saveToFile(QString filename, SimFigure::FileType type, QSizeF size, int res)
{
    if (res<50) res=50;

    QFileInfo *file = new QFileInfo(filename);
    QString base = file->completeBaseName();
    QString ext  = (file->suffix()).toLower();

    // check for valid type
    if (ext == "ps" || ext == "pdf" || ext == "png" || ext == "svg" || ext == "bmp")
    {
        // use extension from provided file type
    }
    else {
        // fallback to user specified type

        switch (type)
        {
        case SimFigure::FileType::PS  : ext = "ps"; break;
        case SimFigure::FileType::PDF : ext = "pdf"; break;
        case SimFigure::FileType::PNG : ext = "png"; break;
        case SimFigure::FileType::SVG : ext = "svg"; break;
        case SimFigure::FileType::BMP : ext = "bmp"; break;
        }
    }

    QString newFilename = base + "." + ext;

    QwtPlotRenderer *renderer = new QwtPlotRenderer(this);
    renderer->renderDocument(m_plot, newFilename, ext, size, res);
}

/**
 * @brief save image to file
 *
 * used to save the plot to an image file.  Location and filename are suggestions only.
 * The user will face a QFileDialog to define actual filename and location.
 *
 * @param filename specified via QFileDialog
 * @param type default is SimFigure::FileType::PNG
 * @param size QSizeF(width in MM, height in MM).
 *             Defaults to 300mm/200mm (~12in/8in)
 * @param res resolution: use 75-95 for screen, ~100-150 for presentations, 300 for quality print.
 *            Defaults to 85
 */
void SimFigure::exportToFile(QString filename, SimFigure::FileType type, QSizeF size, int res)
{
    if (res<50) res=50;

    QFileInfo *file = new QFileInfo(filename);
    QString base = file->completeBaseName();
    QString ext  = (file->suffix()).toLower();

    // check for valid type
    if (ext == "ps" || ext == "pdf" || ext == "png" || ext == "svg" || ext == "bmp")
    {
        // use extension from provided file type
    }
    else {
        // fallback to user specified type

        switch (type)
        {
        case SimFigure::FileType::PS  : ext = "ps"; break;
        case SimFigure::FileType::PDF : ext = "pdf"; break;
        case SimFigure::FileType::PNG : ext = "png"; break;
        case SimFigure::FileType::SVG : ext = "svg"; break;
        case SimFigure::FileType::BMP : ext = "bmp"; break;
        }
    }

    QString newFilename = base + "." + ext;

    QwtPlotRenderer *renderer = new QwtPlotRenderer(this);
    renderer->exportTo(m_plot, newFilename, size, res);
}
