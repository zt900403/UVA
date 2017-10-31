#include "rOg_image.h"
#include "utils/filesystem.h"
#include <QDir>
#include <QDebug>
#include <QSize>
#include <QMatrix>
// Constructor of the class, create the scene and set default parameters
rOg_image::rOg_image(QWidget * parent, bool isContextualMenu):
    QGraphicsView(parent)
{
    // Set default zoom factors
    zoomFactor=DEFAULT_ZOOM_FACTOR;
    zoomCtrlFactor=DEFAULT_ZOOM_CTRL_FACTOR;

    // Create the scene
    scene = new QGraphicsScene();

    // Allow mouse tracking even if no button is pressed
    this->setMouseTracking(true);

    // Add the scene to the QGraphicsView
    this->setScene(scene);

    // Update all the view port when needed, otherwise, the drawInViewPort may experience trouble
    this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    this->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    // Initialize contextual menu if requested
    if (isContextualMenu)
    {
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
    }

    // Disable scroll bar to avoid a unwanted resize recursion
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the default pixmap at startup
    pixmapItem = scene->addPixmap(pixmap);

}


// Destructor of the class
rOg_image::~rOg_image()
{
    delete pixmapItem;
    delete scene;
}



// Display contextual menu
void rOg_image::showContextMenu(const QPoint & pos)
{
    // Get the mouse position in the scene
    QPoint globalPos = mapToGlobal(pos);
    // Create the menu and add action
    QMenu contextMenu;
    contextMenu.addAction("重置画面", this, SLOT(fitImage()));
    // Display the menu
    contextMenu.exec(globalPos);
}

int rOg_image::yaw() const
{
    return m_yaw;
}

void rOg_image::setYaw(int yaw)
{
    m_yaw = yaw;
}

QPoint rOg_image::gisPosition() const
{
    return m_gisPosition;
}

void rOg_image::setGisPosition(const QPoint &gisPosition)
{
    m_gisPosition = gisPosition;
}




// Set or update the image in the scene

void rOg_image::setImage(const QImage & image)
{
    // Update the pixmap in the scene
    pixmap=QPixmap::fromImage(image);
    QPainter *paint = new QPainter(&pixmap);
    paint->setPen(Qt::red);
    paint->drawLine(100,100,1000,1000);
    pixmapItem->setPixmap(pixmap);
    paint->end();
    // Resize the scene (needed is the new image is smaller)
    scene->setSceneRect(QRect (QPoint(0,0),image.size()));

    // Store the image size
    imageSize = image.size();
}


// Set an image from raw data
void rOg_image::setImageFromRawData(const uchar * data, int width, int height, bool mirrorHorizontally, bool mirrorVertically)
{
    // Convert data into QImage
    QImage image(data, width, height, width*3, QImage::Format_RGB888);

    // Update the pixmap in the scene
    pixmap=QPixmap::fromImage(image.mirrored(mirrorHorizontally,mirrorVertically));
    pixmapItem->setPixmap(pixmap);

    // Resize the scene (needed is the new image is smaller)
    scene->setSceneRect(QRect (QPoint(0,0),image.size()));

    // Store the image size
    imageSize = image.size();
}



// Fit the image in the widget
void rOg_image::fitImage()
{
    // Get current scroll bar policy
    Qt::ScrollBarPolicy	currentHorizontalPolicy = horizontalScrollBarPolicy();
    Qt::ScrollBarPolicy	currentverticalPolicy = verticalScrollBarPolicy();

    // Disable scroll bar to avoid a margin around the image
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Fit the scene in the QGraphicsView
    this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    // Restaure scroll bar policy
    setHorizontalScrollBarPolicy(currentHorizontalPolicy);
    setVerticalScrollBarPolicy(currentverticalPolicy);
}


// Called when a mouse button is pressed
void rOg_image::mousePressEvent(QMouseEvent *event)
{
    // Drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton)  this->setDragMode(QGraphicsView::ScrollHandDrag);
    //    if (event->button() == Qt::RightButton) this->fitImage();
    QGraphicsView::mousePressEvent(event);
}


// Called when a mouse button is pressed
void rOg_image::mouseReleaseEvent(QMouseEvent *event)
{
    // Exit drag mode : change the cursor's shape
    if (event->button() == Qt::LeftButton) this->setDragMode(QGraphicsView::NoDrag);
    QGraphicsView::mouseReleaseEvent(event);
}


#ifndef QT_NO_WHEELEVENT

// Call when there is a scroll event (zoom in or zoom out)
void rOg_image::wheelEvent(QWheelEvent *event)
{
    // When zooming, the view stay centered over the mouse
    this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double factor = (event->modifiers() & Qt::ControlModifier) ? zoomCtrlFactor : zoomFactor;
    if(event->delta() > 0)
        // Zoom in
        scale(factor, factor);
    else
        // Zooming out
        scale(1.0 / factor, 1.0 / factor);

    // The event is processed
    event->accept();
}

#endif


// Overload the mouse MoveEvent to display the tool tip
void rOg_image::mouseMoveEvent(QMouseEvent *event)
{
    // Get the coordinates of the mouse in the scene
    QPointF imagePoint = mapToScene(QPoint(event->x(), event->y() ));
    // Call the function that create the tool tip
    setToolTip(setToolTipText(QPoint((int)imagePoint.x(),(int)imagePoint.y())));
    // Call the parent's function (for dragging)
    QGraphicsView::mouseMoveEvent(event);
}


// Overload the function to draw over the image
void rOg_image::drawForeground(QPainter *painter, const QRectF&)
{

    // Call the function to draw over the image
    this->drawOnImage(painter,imageSize);

    // Reset transformation and call the function draw in the view port
    painter->resetTransform();

    // Call the function to draw in the view port
    this->drawInViewPort(painter, this->viewport()->size());
}


// Overloaded functionthat catch the resize event
void rOg_image::resizeEvent(QResizeEvent *event)
{
    // First call, the scene is created
    if(event->oldSize().width() == -1 || event->oldSize().height() == -1) return;

    // Get the previous rectangle of the scene in the viewport
    QPointF P1=mapToScene(QPoint(0,0));
    QPointF P2=mapToScene(QPoint(event->oldSize().width(),event->oldSize().height()));

    // Stretch the rectangle around the scene
    if (P1.x()<0) P1.setX(0);
    if (P1.y()<0) P1.setY(0);
    if (P2.x()>scene->width()) P2.setX(scene->width());
    if (P2.y()>scene->height()) P2.setY(scene->height());

    // Fit the previous area in the scene
    this->fitInView(QRect(P1.toPoint(),P2.toPoint()),Qt::KeepAspectRatio);
}





// Define the virtual function to avoid the "unused parameter" warning
QString rOg_image::setToolTipText(QPoint imageCoordinates)
{
    (void)imageCoordinates;
    return QString("");
}


// Define the virtual function to avoid the "unused parameter" warning
void rOg_image::drawOnImage(QPainter* painter , QSize )
{

    QBrush b(Qt::white);
    painter->setBrush(b);
    QPixmap pixmap2 = pixmap;

    QPainter p(&pixmap2);
    QSize picSize(100,100);
    QString imageDir = FileSystem::directoryOf("images").absoluteFilePath("UAV.png");
    QPixmap image(imageDir);
    image = image.scaled(picSize,Qt::KeepAspectRatio);

    QMatrix leftmatrix;
    leftmatrix.rotate(m_yaw);
    QPixmap imageRot = image.transformed(leftmatrix,Qt::SmoothTransformation);

    p.drawPixmap(m_gisPosition.x()-imageRot.width()/2,m_gisPosition.y()-imageRot.height()/2,imageRot );


    p.end();

    pixmapItem->setPixmap(pixmap2);

}


// Define the virtual function to avoid the "unused parameter" warning
void rOg_image::drawInViewPort(QPainter* painter, QSize portSize)
{
    QBrush b(Qt::white);



    QPixmap transparentMap(300,300);
    transparentMap.fill(Qt::transparent);
    QPainter p(&transparentMap);

    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(b);
    QRectF f(QPoint(10, 10), QPoint(200, 100));
    QPainterPath path;
    path.addRoundedRect(f, 10, 10);

    p.setOpacity(0.5);
    //p.drawRect(f);
    p.fillPath(path, Qt::white);
    p.drawPath(path);
    QFont font( "Microsoft YaHei", 15, 75);
    p.setFont(font);
    p.drawText(f.adjusted(5, 5, 0, 0), tr("天气: 晴\n风向: 东南"));
//    p.drawText(f.adjusted(5, 25, 0, 0), tr("风向: 东南"));
    p.end();
    painter->drawPixmap(10,10,transparentMap);
    painter->end();

}
