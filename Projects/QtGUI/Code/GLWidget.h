/*
File:  GLWidget.h

Description: GLWidget Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QGL_WIDGET_H
#define QGL_WIDGET_H

// includes ////////////////////////////////////////
#include <qopengl.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    void                                updateGamma(float gamma);
    void                                updateTexture(int32_t w, int32_t h, const float* ptr);

public slots:
    void                                cleanup();

protected:
    void                                initializeGL() override;
    void                                paintGL() override;
    void                                resizeGL(int32_t w, int32_t h) override;

    void                                mouseDoubleClickEvent(QMouseEvent* event) override;
    void                                mousePressEvent(QMouseEvent* event) override;
    void                                mouseMoveEvent(QMouseEvent* event) override;
    void                                mouseReleaseEvent(QMouseEvent* event) override;
    void                                keyPressEvent(QKeyEvent *event) override;
    void                                keyReleaseEvent(QKeyEvent *event) override;

private:
    class                               QOpenGLVertexArrayObject* m_vao;
    class                               QOpenGLBuffer* m_vbo;
    class                               QOpenGLBuffer* m_ibo;
    class                               QOpenGLTexture* m_texture;
    class                               QOpenGLShaderProgram *m_program;
    bool                                m_update_texture;
};



#endif //QGL_WIDGET_H

// eof ///////////////////////////////// class QGLWidget
