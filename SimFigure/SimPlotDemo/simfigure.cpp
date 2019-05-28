#include "simfigure.h"
#include "ui_simfigure.h"

#include <QVBoxLayout>
#include <QBrush>
#include <QVector>
#include <QPoint>
#include <QPolygon>
#include <QPointF>
#include <QPolygonF>
#include <QMap>
#include <QMapIterator>

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>

#include "qwt_picker.h"
#include "qwt_plot_picker.h"
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

    axisType = AxisType::Default;
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

    connect(m_picker, SIGNAL(activated(bool)), this, SLOT(on_picker_activated(bool)));
    connect(m_picker, SIGNAL(selected(const QPolygon &)), this, SLOT(on_picker_selected(const QPolygon &)));
    connect(m_picker, SIGNAL(appended(const QPoint &)), this, SLOT(on_picker_appended(const QPoint &)));
    connect(m_picker, SIGNAL(moved(const QPoint &)), this, SLOT(on_picker_moved(const QPoint &)));
    connect(m_picker, SIGNAL(removed(const QPoint &)), this, SLOT(on_picker_removed(const QPoint &)));
    connect(m_picker, SIGNAL(changed(const QPolygon &)), this, SLOT(on_picker_changed(const QPolygon &)));

}

SimFigure::~SimFigure()
{
    delete ui;
}

void SimFigure::axisTypeChanged(void)
{
    if (ui->btn_standard->isChecked())
    {
        if (axisType != AxisType::Default)
        {
            axisType = AxisType::Default;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_logX->isChecked())
    {
        if (axisType != AxisType::LogX)
        {
            axisType = AxisType::LogX;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_logY->isChecked())
    {
        if (axisType != AxisType::LogY)
        {
            axisType = AxisType::LogY;
            m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine(10));
            m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine(10));

            rescale();

            m_plot->replot();
        }
    }
    else if (ui->btn_loglog->isChecked())
    {
        if (axisType != AxisType::LogLog)
        {
            axisType = AxisType::LogLog;

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

    //qDebug() << "signal axisTypeChanged received " << int(axisType);
}

void SimFigure::grid(bool mayor, bool minor)
{
    if (m_grid != nullptr)
    {
        m_grid->detach();
        delete m_grid;
        m_grid = nullptr;
    }
    // Create Background Grid for Plot
    if (mayor)
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

        if (minor)
        {
            m_grid->enableXMin(true);
            m_grid->enableYMin(true);
        }
        else {
            m_grid->enableXMin(false);
            m_grid->enableYMin(false);
        }

        switch (axisType) {
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

int SimFigure::plot(QVector<double> &x, QVector<double> &y, LineType lt, QColor color, Marker mk)
{
    if (x.length() <= 0 || y.length() <= 0) return -1;

    // update min and max values

    if (MAX(x) > m_xmax) m_xmax=MAX(x);
    if (MIN(x) < m_xmin) m_xmin=MIN(x);
    if (MAX(y) > m_ymax) m_ymax=MAX(y);
    if (MIN(y) < m_ymin) m_ymin=MIN(y);

    // now add that curve

    QwtPlotCurve *newCurve = new QwtPlotCurve("default");
    newCurve->setSamples(x,y);

    switch (lt)
    {
    case LineType::None :
        newCurve->setPen(color, 0.0);
        break;
    case LineType::Solid :
        newCurve->setPen(color, 2.0, Qt::SolidLine);
        break;
    case LineType::Dashed :
        newCurve->setPen(color, 2.0, Qt::DashLine);
        break;
    case LineType::Dotted :
        newCurve->setPen(color, 2.0, Qt::DotLine);
        break;
    case LineType::DashDotted :
        newCurve->setPen(color, 2.0, Qt::DashDotLine);
        break;
    }

    switch (mk)
    {
    case Marker::None :
        newCurve->setSymbol(nullptr);
        break;
    case Marker::Ex :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::XCross));
        break;
    case Marker::Box :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::Rect));
        break;
    case Marker::Plus :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::Cross));
        break;
    case Marker::Circle :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse));
        break;
    case Marker::Asterisk :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::Star1));
        break;
    case Marker::Triangle :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::Triangle));
        break;
    case Marker::DownTriangle :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::DTriangle));
        break;
    case Marker::LeftTriangle :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::LTriangle));
        break;
    case Marker::RightTriangle :
        newCurve->setSymbol(new QwtSymbol(QwtSymbol::RTriangle));
        break;
    }

    newCurve->attach(m_plot);

    m_curves.append(newCurve);

    //grid(true,true);
    rescale();
    m_plot->replot();

    int idx = m_curves.length();
    m_plotInvMap.insert(newCurve, idx);

    return idx;
}

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
}

void SimFigure::clear(void)
{
    cla();
}

void SimFigure::cla(void)
{
    foreach (QwtPlotCurve *curve, m_curves)
    {
        curve->detach();
        delete curve;

        m_plotInvMap.clear();
    }
    m_curves.clear();

    lastSelection.object = nullptr;
    lastSelection.plotID = -1;

    m_xmin = 1.e20;
    m_xmax = 1.e-20;
    m_ymin = 1.e20;
    m_ymax = 1.e-20;

    m_plot->replot();

    emit curve_selected(-1);
}

void SimFigure::legend(QList<QString> labels, Location loc)
{
    moveLegend(loc);

    if (labels.length()>0)
    {
        showLegend();
    }
}

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
    }
}

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
}

bool SimFigure::legendVisible(void)
{
    return (m_legend!=nullptr && m_legend->isVisible());
}

void SimFigure::on_picker_activated(bool on)
{
    //qWarning() << "picker activated " << on;
}

void SimFigure::on_picker_selected(const QPolygon &polygon)
{
    //qWarning() << "picker selected " << polygon;
}

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

void SimFigure::on_picker_moved (const QPoint &pos)
{
    //qWarning() << "picker moved " << pos;
}

void SimFigure::on_picker_removed (const QPoint &pos)
{
    //qWarning() << "picker removed " << pos;
}

void SimFigure::on_picker_changed (const QPolygon &selection)
{
    //qWarning() << "picker changed " << selection;
}

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
                        double d2  = sqrt(QPointF::dotProduct(r,r));
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

void SimFigure::select(int ID)
{
    QwtPlotItem *theItem = nullptr;

    if (ID > 0 && m_curves.length() >= ID && m_curves.value(ID-1) != nullptr)
        theItem = m_curves.value(ID-1);

    if (theItem) select(theItem);
}

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
