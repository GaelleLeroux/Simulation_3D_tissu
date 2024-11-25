#include "myWindow.hpp"

#include "myWidgetGL.hpp"
#include "../../lib/common/error_handling.hpp"
#include "ui_mainwindow.h"

#include <iostream>

myWindow::myWindow(QWidget *parent)
    :QMainWindow(parent),ui(new Ui::MainWindow)
{
    try
    {
        //Setup window layout
        ui->setupUi(this);

        //Create openGL context
        QGLFormat qglFormat;
        qglFormat.setVersion(1,2);

        //Create OpenGL Widget renderer
        glWidget=new myWidgetGL(qglFormat);

        //Add the OpenGL Widget into the layout
        ui->layout_scene->addWidget(glWidget);
    }
    catch(cpe::exception_cpe const& e)
    {
        std::cout<<std::endl<<e.report_exception()<<std::endl;
    }

    //Connect slot and signals
    connect(ui->quit,SIGNAL(clicked()),this,SLOT(action_quit()));
    connect(ui->draw,SIGNAL(clicked()),this,SLOT(action_draw()));
    connect(ui->wireframe,SIGNAL(clicked()),this,SLOT(action_wireframe()));
    connect(ui->restart,SIGNAL(clicked()),this,SLOT(restart_animation()));
    connect(ui->raideur,SIGNAL(valueChanged(int)),this,SLOT(change_raideur(int)));
    connect(ui->vent,SIGNAL(valueChanged(int)),this,SLOT(change_vent(int)));
    connect(ui->temps,SIGNAL(valueChanged(int)),this,SLOT(change_temps(int)));
    connect(ui->attenuation,SIGNAL(valueChanged(int)),this,SLOT(change_attenuation(int)));
}

myWindow::~myWindow()
{}

void myWindow::action_quit()
{
    close();
}

void myWindow::action_draw()
{
    glWidget->change_draw_state();
}

void myWindow::action_wireframe()
{
    bool const state_wireframe=ui->wireframe->isChecked();
    glWidget->wireframe(state_wireframe);
}

void myWindow::restart_animation()
{
    glWidget->restart_animation();
}

void myWindow::change_raideur(int K)
{
    glWidget->change_raideur(K);
    QString text = QString::number(K);
    ui->K_value->setText(text);
}

void myWindow::change_vent(int Kw)
{
    glWidget->change_vent(Kw);
    QString text = QString::number(Kw/1000.0f);
    ui->Kw_value->setText(text);
}

void myWindow::change_temps(int T)
{
    glWidget->change_temps(T);
    QString text = QString::number(T/1000.0f);
    ui->T_value->setText(text);
}

void myWindow::change_attenuation(int A)
{
    glWidget->change_attenuation(A);
    QString text = QString::number(A/1000.0f);
    ui->A_value->setText(text);
}