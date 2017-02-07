#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRubberBand>
#include <QStack>
#include <QScrollBar>
#include <QInputDialog>
#include <iostream>
#include <QPainter>
#include <QApplication>
#include <QRect>
#include <QPoint>
#include <QDebug>


QPoint myPoint;
QRubberBand *band;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionRotate_With_Angle_triggered()
{
        bool ok;
        QString angle  = QInputDialog::getText(this, tr("Rotate"),
                                             tr("Angle in Degrees:"), QLineEdit::Normal,
                                             "", &ok);
        //check that the user enters a value and clicked enter
        if (!(ok && !angle.isEmpty())){
            return;
        }
        //bool to check for valid input value
        bool is_angle;
        float RotAngle = angle.toFloat(&is_angle) + original_angle;

        if(!is_angle){
            //error
            QMessageBox::information(this, tr("Error"),tr("Entered string is not a number !"));
            return;
        }
        //create pixmap containing the working image
        QPixmap pixmap(QPixmap::fromImage(working_img));
        //create a temporary pixmap
        QPixmap tempPixmap(pixmap.size());

        QPainter painter(&tempPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        //change the position of the coordinates
        painter.translate(tempPixmap.size().width() / 2, tempPixmap.size().height() / 2);
        //rotate the pixmap
        painter.rotate(RotAngle);
        //return the position of the coordinate back again
        painter.translate(-tempPixmap.size().width() / 2, -tempPixmap.size().height() / 2);

        //put the pixmap on the tempPixmap
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        //load tempPixmap on the working image
        pixmap = tempPixmap;

        //update the ui label according to the new pixmap
        ui->label->resize(pixmap.size());
        ui->label->setPixmap(pixmap);
        //add to undo_stack
        undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
        //remove the redo_stack
        redo_stack.clear();
        //set the current image with the working image
        current_image = pixmap.toImage();
        original_angle = RotAngle;
        paintImage();
}
/*
 *  Function used to open a selected image to make a pocess on it
 *  Return type : void
 */
void MainWindow::on_actionOpen_triggered()
{
         QFileDialog fileDialog(this);
         //only support these types of extention
        fileDialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
        fileDialog.setViewMode(QFileDialog::Detail);
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open..."), "", tr("Image Format (*.png *.jpg *.bmp)"));
        //check for filename existance
        if (!fileName.isEmpty())
        {
            QImage image(fileName);
            if (image.isNull()) {
                //invalid image so generate error
                QMessageBox::information(this, tr("Image Viewer"),
                                         tr("Cannot load %1 \n The image is invalid.").arg(fileName));
                return;
            }
            //append on undo stack
            undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
            //clear redo stack
            redo_stack.clear();
            //intialize global parameters
            original_image = image;
            original_angle = 0;
            working_img = image;
            current_image = image;
            ScaleFactor = 1;

            ui->label->setPixmap(QPixmap::fromImage(image));
            ui->label->repaint();
        }
}
/*
 * Method used to save image in a specefic path with specific format
 * In case of there is no format PNG is the defult
 * Return: method is void
 **/
void MainWindow::on_actionSave_triggered()
{
    //if there is no image , Do nothing
    if(!ui->label->pixmap())
            return ;

        QFileDialog saveDialog ;
        QString filename = saveDialog.getSaveFileName(this, tr("Save As.."),"",tr("Joint Photographic Experts Group(*.jpg);;Bitmap image(*.bmp);;Portable Network Graphics(.png)") );

        //check if there is a file name or not
        if( !filename.isNull() ){
            //if there is no extention .png is the default value
            if (!filename.contains("."))
                filename += ".png";

            //create a file with the given name and load the current image on it
            QFile file(filename);
            file.open(QIODevice::WriteOnly);
            QPixmap pixmap(*ui->label->pixmap());
            pixmap.save(&file);
        }
}
/*
 * Method used to exit by showing a dialoge message to confirm exit
 * Return value is void
 * */

void MainWindow::on_actionClose_triggered()
{
    //create a reply box with answer yes to close answer no to open
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Image Viewer", "Are you sure you want to Exit ?",
                                                                  QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            //if the answer yes and there is an image then save
            if(!original_image.isNull()){
                on_actionSave_triggered();
            }
            exit(0);
        } else {
            //else return
            return;
        }
}
/*
 * method to rotate the picture -90 degree (left rotate)
 * by converting the image into pixmap then rotate it and repaint the picture again
 * Return value is void
 * */
void MainWindow::on_actionRotate_Left_triggered()
{
    //if the image is null return
    if(original_image.isNull()){
            return;
        }

        QPixmap pixmap(QPixmap::fromImage(current_image));
        //define new matrix
        QMatrix matrix;
        //rotate -90 degree
        matrix.rotate(-90);
        //trasnform matrix to pixmap
        pixmap = pixmap.transformed(matrix);
        //update ui
        ui->label->resize(pixmap.size());
        ui->label->setPixmap(pixmap);

        //append into undo stack
        undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
        //clear redo stack
        redo_stack.clear();
        current_image = pixmap.toImage();
        QPixmap temp(QPixmap::fromImage(working_img));
        temp = temp.transformed(matrix);
        working_img = temp.toImage();

        paintImage();
}

/*
 * method to rotate the picture +90 degree (right rotate)
 * by converting the image into pixmap then rotate it and repaint the picture again
 * Return value is void
 * */
void MainWindow::on_actionRotate_Right_triggered()
{
    //if image is null Do nothing
    if(original_image.isNull()){
            return;
        }
        QPixmap pixmap(QPixmap::fromImage(current_image));

        QMatrix matrix;
        //rotate 90 degree
        matrix.rotate(90);

        //transform matrix to pixmap
        pixmap = pixmap.transformed(matrix);
        ui->label->resize(pixmap.size());
        ui->label->setPixmap(pixmap);

        //append into undo stack
        undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
        //clear redo stack
        redo_stack.clear();
        current_image = pixmap.toImage();
        QPixmap temp(QPixmap::fromImage(working_img));
        temp = temp.transformed(matrix);
        working_img = temp.toImage();
        paintImage();
}
/*
 * method is used for undo by pop the pervious state from the stack
 * and load it to be current image then repaint the picture
 * Return value is void
 * */
void MainWindow::on_actionUndo_triggered()
{
    if(!undo_stack.size())
            return;

        QImage old_current_image = undo_stack.back().first;
        QImage old_prev_working_img = undo_stack.back().second.first;
        float old_original_angle = undo_stack.back().second.second.first;
        float old_scale_factor = undo_stack.back().second.second.second;
        undo_stack.pop_back();

        redo_stack.push_back(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle, 1/old_scale_factor))));

        current_image = old_current_image;
        working_img = old_prev_working_img;
        original_angle = old_original_angle;
        ScaleFactor /= old_scale_factor;
        ui->label->setPixmap(QPixmap::fromImage(current_image));
        paintImage();
}

/*
 * method is used for redo the pictures
 * Return value is void
 * */

void MainWindow::on_actionRedo_triggered()
{
    if(!redo_stack.size())
          return;

      QImage new_current_image = redo_stack.back().first;
      QImage new_working_img = redo_stack.back().second.first;
      float new_original_angle = redo_stack.back().second.second.first;
      float new_scale_factor = redo_stack.back().second.second.second;

      redo_stack.pop_back();

      undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1/new_scale_factor))));


      current_image = new_current_image;
      working_img = new_working_img;
      original_angle = new_original_angle;
      ScaleFactor /= new_scale_factor;

      ui->label->setPixmap(QPixmap::fromImage(current_image));
      paintImage();
}

/*
 * Method used to reset the picture to its original state
 * by make current image equal to loaded original image
 * Return value is void
 * */
void MainWindow::on_actionReset_to_Origin_triggered()
{
    if(!ui->label->pixmap())
           return ;
    //apend on undo stack
    undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
    //clear redo stack
    redo_stack.clear();
    //reset
       ui->label->setPixmap(QPixmap::fromImage(original_image));
       current_image = original_image;
       working_img = original_image;
       original_angle = 0.0;
       paintImage();
}
/*
 * Method used to zoom in by changing the scale factor of the image
 * Return value is void
 * */

void MainWindow::on_actionZoom_in_triggered()
{
    if(ScaleFactor >= 3.0)
            return ;

    bool ok;
    QString scaleString  = QInputDialog::getText(this, tr("Zoom"),
                                         tr("Zoom Scale:"), QLineEdit::Normal,
                                         "", &ok);
    //check that the user enters a value and clicked enter
    if (!(ok && !scaleString.isEmpty())){
        return;
    }
    //bool to check for valid input value
    bool is_number;
    float scale = scaleString.toFloat(&is_number);

    if(!is_number){
        //error
        QMessageBox::information(this, tr("Error"),tr("Entered string is not a number !"));
        return;
    }
    if(scale < 1.0){
        QMessageBox::information(this, tr("Error"),tr("Entered scale is smaller than 1 !"));
        return;
    }

    ScaleFactor*= scale;
    //add to undo_stack
    undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,scale))));
    //remove the redo_stack
    redo_stack.clear();
    paintImage();
}


/*
 * Method used to zoom out by changing the scale factor of the image
 * Return value is void
 * */
void MainWindow::on_actionZoom_out_triggered()
{
    if(ScaleFactor <= 0.1)
            return ;

    bool ok;
    QString scaleString  = QInputDialog::getText(this, tr("Zoom"),
                                         tr("Zoom Scale:"), QLineEdit::Normal,
                                         "", &ok);
    //check that the user enters a value and clicked enter
    if (!(ok && !scaleString.isEmpty())){
        return;
    }
    //bool to check for valid input value
    bool is_number;
    float scale = scaleString.toFloat(&is_number);

    if(!is_number){
        //error
        QMessageBox::information(this, tr("Error"),tr("Entered string is not a number !"));
        return;
    }
    if(scale < 1.0){
        QMessageBox::information(this, tr("Error"),tr("Entered scale is smaller than 1 !"));
        return;
    }

        ScaleFactor*= 1/scale ;
        //add to undo_stack
        undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1/scale))));
        //remove the redo_stack
        redo_stack.clear();
        paintImage();
}
/*
 * Method used to zoom on specific area and validating the intervals
 * Return valus is void
 * */
void MainWindow::on_actionZoom_on_Area_triggered()
{
    if(original_image.isNull() || !band)
            return;

        if( band->height() * band->width() < 50 ){
            band->setVisible(false);
            band = NULL;
            return ;
        }

        float zoomScale = std::min(ui->scrollArea->width()/width,ui->scrollArea->height()/height);
        if(!(((ScaleFactor >= 3.0 && zoomScale > 1) || (ScaleFactor <= 0.1 && zoomScale < 1 )))){
             ScaleFactor*= zoomScale;
             paintImage();
        }
        QPoint loc = selectedPoint;
        loc *= zoomScale;

        ui->scrollArea->setUpdatesEnabled(true);
        ui->scrollArea->horizontalScrollBar()->setUpdatesEnabled(true);
        ui->scrollArea->verticalScrollBar()->setUpdatesEnabled(true);

        ui->scrollArea->horizontalScrollBar()->setRange(0,ui->label->width());
        ui->scrollArea->horizontalScrollBar()->setValue(loc.x());
        ui->scrollArea->verticalScrollBar()->setRange(0,ui->label->height());
        ui->scrollArea->verticalScrollBar()->setValue(loc.y());

        if(band){
            band->setVisible(false);
            band = NULL;
        }

        showScrollBars();
}
/*
 * Method used to crop the image by selecting specific band
 * then crop it .
 * Return value is void
 * */

void MainWindow::on_actionCrop_triggered()
{
    if(original_image.isNull())
            return;
        if(!band){
            return;
        }

        if( band->height() * band->width() < 50 ){
            band->setVisible(false);
            band = NULL;
            return ;
        }

        QPixmap pixmap(QPixmap::fromImage(current_image.scaled(current_image.width()*ScaleFactor , current_image.height()*ScaleFactor, Qt::IgnoreAspectRatio)));
        pixmap = pixmap.copy(selectedPoint.x(),selectedPoint.y(),width,height);
        QImage temp = pixmap.toImage();
        QPixmap new_px(QPixmap::fromImage(temp.scaled(temp.width()*(1.0/ScaleFactor) , temp.height()*(1.0/ScaleFactor), Qt::IgnoreAspectRatio)));

        undo_stack.append(qMakePair(current_image,qMakePair(working_img,qMakePair(original_angle,1))));
        redo_stack.clear();
        current_image = new_px.toImage();
        working_img = current_image;
        original_angle = 0;
        ui->label->setPixmap(new_px);

        if(band){
            band->setVisible(false);
            band = NULL;
        }
        showScrollBars();
        paintImage();
}

bool MainWindow::validate(QMouseEvent *event)
{
    int scroll_w = ui->scrollArea->verticalScrollBar()->width();
    int scroll_h = ui->scrollArea->horizontalScrollBar()->height();
    QPoint temp = ui->scrollArea->mapFromGlobal(event->globalPos());

    if(ui->scrollArea->verticalScrollBar()->maximum() == 0){
        scroll_w = 0 ;
    }

    if(ui->scrollArea->horizontalScrollBar()->maximum() == 0){
        scroll_h = 0 ;
    }

    bool inScroll = ui->scrollArea->rect().contains(temp) ;
    bool inScrollExtra = ui->scrollArea->rect().contains(QPoint(temp.x()+scroll_w,temp.y()+scroll_h)) ;
    bool inLabel = ui->label->pixmap()->rect().contains(ui->label->mapFromGlobal(event->globalPos())) ;

    return inScroll && inLabel && inScrollExtra ;
}
/*
 * Method used to paint an image on tha label
 * Return value is void
 * */
void MainWindow::paintImage()
{
    if(original_image.isNull()){
        return;
    }

    QPixmap pixmap(QPixmap::fromImage(current_image.scaled(current_image.width()*ScaleFactor , current_image.height()*ScaleFactor, Qt::IgnoreAspectRatio)));

    ui->label->resize(pixmap.size());
    ui->label->setPixmap(pixmap);
}
/*
 * Method used to take an action in case of mouse press happen by selecting ability to select an area
 * and by saving the point hwere the user start to select the area
 * Return Value is void
 * */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!ui->label->pixmap())
        return ;

    if( band ){
        band->setVisible(false);
        band->close();
        band = NULL;
    }

    if(validate(event)){
        myPoint = event->pos();
        selectedFirstPnt = event->globalPos();
        firstBtnChk = 1 ;
    }
    else{
        firstBtnChk = 0 ;
    }

    if(band){
        band->close();
        band->setVisible(false);
    }


    band = NULL;

    showScrollBars();


}
/*
 * Method used to take an action in case of area is selected
 * Return value is Void
 * */

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!ui->label->pixmap())
        return ;


    if ( validate(event) && firstBtnChk ) {
        selectedPoint = ui->label->mapFromGlobal(QPoint(std::min(selectedFirstPnt.x(),event->globalPos().x()),std::min(selectedFirstPnt.y(),event->globalPos().y())));
        height = std::abs(selectedFirstPnt.y()-event->globalPos().y());
        width = std::abs(selectedFirstPnt.x()-event->globalPos().x());
        if(!band)
        {
            band = new QRubberBand(QRubberBand::Rectangle, this);
        }
        band->show();

        band->setGeometry(QRect(myPoint, event->pos()).normalized());
    }
}
/*
 * Method used to take an action is case of the user released the mouse
 * by validating the selected area and take an action
 * Return value is void
 * */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if( !band )return ;

    if( band->height() * band->width() < 10 ){
        band->setVisible(false);
        band = NULL;
        return ;
    }

    QScrollBar* vert = ui->scrollArea->verticalScrollBar();
    QScrollBar* horiz = ui->scrollArea->horizontalScrollBar();

    if(vert){
        vert->setEnabled(false);
    }
    if(horiz){
        horiz->setEnabled(false);
    }

}
/*
 * Method is used to enable scroll bars in ase of vertical and horizontal
 * used in case of the picture is not fit in sizeof panal
 * Return value is Void
 */
void MainWindow::showScrollBars(){

    QScrollBar* vert = ui->scrollArea->verticalScrollBar();
    QScrollBar* horiz = ui->scrollArea->horizontalScrollBar();

    if(vert){
        vert->setEnabled(true);
    }
    if(horiz){
        horiz->setEnabled(true);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if((event->key() == Qt::Key_S)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionSave_triggered();
    }else if((event->key() == Qt::Key_O)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionOpen_triggered();
    }else if((event->key() == Qt::Key_R)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionRotate_With_Angle_triggered();
    }else if((event->key() == Qt::Key_E)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionClose_triggered();
    }else if((event->key() == Qt::Key_Z)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionUndo_triggered();
    }else if((event->key() == Qt::Key_Y)&& QApplication::keyboardModifiers() && Qt::ControlModifier){
        on_actionRedo_triggered();
    }
}
