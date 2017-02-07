#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QMouseEvent>
#include <QList>
//#include <Qpair>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionRotate_With_Angle_triggered();
    /*
     *  Function used to open a selected image to make a pocess on it
     *  Return type : void
     */
    void on_actionOpen_triggered();
    /*
     * Method used to save image in a specefic path with specific format
     * In case of there is no format PNG is the defult
     * Return: method is void
     **/
    void on_actionSave_triggered();
    /*
     * Method used to exit by showing a dialoge message to confirm exit
     * Return value is void
     * */
    void on_actionClose_triggered();
    /*
     * method to rotate the picture -90 degree (left rotate)
     * by converting the image into pixmap then rotate it and repaint the picture again
     * Return value is void
     * */
    void on_actionRotate_Left_triggered();
    /*
     * method to rotate the picture +90 degree (right rotate)
     * by converting the image into pixmap then rotate it and repaint the picture again
     * Return value is void
     * */
    void on_actionRotate_Right_triggered();
    /*
     * method is used for undo by pop the pervious state from the stack
     * and load it to be current image then repaint the picture
     * Return value is void
     * */
    void on_actionUndo_triggered();
    /*
     * method is used for redo the pictures
     * Return value is void
     * */
    void on_actionRedo_triggered();
    /*
     * Method used to reset the picture to its original state
     * by make current image equal to loaded original image
     * Return value is void
     * */
    void on_actionReset_to_Origin_triggered();
    /*
     * Method used to zoom in by changing the scale factor of the image
     * Return value is void
     * */
    void on_actionZoom_in_triggered();
    /*
     * Method used to zoom out by changing the scale factor of the image
     * Return value is void
     * */
    void on_actionZoom_out_triggered();
    /*
     * Method used to zoom on specific area and validating the intervals
     * Return valus is void
     * */
    void on_actionZoom_on_Area_triggered();
    /*
     * Method used to crop the image by selecting specific band
     * then crop it .
     * Return value is void
     * */

    void on_actionCrop_triggered();
    /*
     * Method used to take an action in case of mouse press happen by selecting ability to select an area
     * and by saving the point here the user start to select the area
     * Return Value is void
     * */
    void mousePressEvent(QMouseEvent *e);
    /*
     * Method used to take an action in case of area is selected
     * Return value is Void
     * */
    void mouseMoveEvent(QMouseEvent *e);
    /*
     * Method used to take an action is case of the user released the mouse
     * by validating the selected area and take an action
     * Return value is void
     * */
    void mouseReleaseEvent(QMouseEvent *e);
    /*
     * Method used to paint an image on tha label
     * Return value is void
     * */
    void paintImage();
    /*
     * Method is used to enable scroll bars in ase of vertical and horizontal
     * used in case of the picture is not fit in sizeof panal
     * Return value is Void
     */
    void showScrollBars();

    bool validate(QMouseEvent *event);



     void keyPressEvent(QKeyEvent* event);

private:
    Ui::MainWindow *ui;                                      // UI pointer
    float ScaleFactor,original_angle ;                       //Scale factor for zooming and orignal angle for rotate
    bool  firstBtnChk;                                       //Flag to indicate if mouse is select an area or not
    QImage original_image,current_image,working_img;        //to keep original image for reset
    QPoint selectedPoint , selectedFirstPnt ;                // supported points
    int height , width ;                                    // height and width of selected area
    QList<QPair<QImage,QPair<QImage,QPair<float,float> > > > undo_stack , redo_stack;  //list act as a stack for undo and redo
};

#endif // MAINWINDOW_H
