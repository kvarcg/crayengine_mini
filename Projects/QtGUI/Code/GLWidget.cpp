/*
File:  GLWidget.cpp

Description: GLWidget Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "GLWidget.h"
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <qopenglpixeltransferoptions.h>

// defines /////////////////////////////////////////

#define SAFE_DELETE(_x)       { if ((_x) != nullptr) { delete   (_x);  _x = nullptr; } }

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
    m_vao(nullptr),
    m_vbo(nullptr),
    m_ibo(nullptr),
    m_texture(nullptr),
    m_program(nullptr),
    m_update_texture(false) {
}

GLWidget::~GLWidget() {
    cleanup();
}

void GLWidget::cleanup() {
    makeCurrent();
    SAFE_DELETE(m_program);
    SAFE_DELETE(m_ibo);
    SAFE_DELETE(m_vbo);
    SAFE_DELETE(m_vao);
    SAFE_DELETE(m_texture);
    doneCurrent();
}

QSize GLWidget::minimumSizeHint() const {
    return QSize(32, 32);
}

QSize GLWidget::sizeHint() const {
    return QSize(400, 400);
}

static const char *vertexShaderSourceCore =
"#version 330 core\n"
"layout(location = 0) in vec2 vertex;\n"
"layout(location = 1) in vec2 uv;\n"
"out vec2 texuv;\n"
"void main() {\n"
"   texuv = uv;\n"
"   gl_Position = vec4(vertex,0,1);\n"
"}\n";

static const char *fragmentShaderSourceCore =
"#version 330 core\n"
"layout(location = 0) out vec4 out_color;"
"in highp vec2 texuv;\n"
"uniform sampler2D tex\n;"
"uniform int uniform_enforce_gamma;"
"uniform float uniform_gamma;"
"void main() {\n"
"   vec4 final_value = texture(tex, texuv);\n"
"   if (uniform_enforce_gamma == 1) {\n"
"       vec3 g = vec3(1.0 / uniform_gamma);\n"
"       final_value.rgb = pow(final_value.rgb, g);\n"
"   }\n"
"   out_color = vec4(final_value.rgb, 1.0);\n"
//"   out_color = vec4(0.0, 0.0, 0.0, 1.0);\n"
"}\n";

GLenum er = GL_NO_ERROR;

void GLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    /*
    QSurfaceFormat fmt = format();
    bool m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    int depth = fmt.depthBufferSize();
    int stencil = fmt.stencilBufferSize();
    int r = fmt.redBufferSize();
    int g = fmt.greenBufferSize();
    int b = fmt.blueBufferSize();
    int a = fmt.alphaBufferSize();
    int s = fmt.samples();
    QSurfaceFormat::SwapBehavior swap = fmt.swapBehavior();
    bool ha = fmt.hasAlpha();
    QSurfaceFormat::OpenGLContextProfile prof = fmt.profile();
    QSurfaceFormat::RenderableType rend = fmt.renderableType();
    int mav = fmt.majorVersion();
    int miv = fmt.minorVersion();
    QPair<int, int> p = fmt.version();
    bool st = fmt.stereo();
    QSurfaceFormat::FormatOptions fo = fmt.options();
    int si = fmt.swapInterval();
    QSurfaceFormat::ColorSpace cs = fmt.colorSpace();
    */

    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    gl->initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCore);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCore);
    m_program->link();
    m_program->bind();
    GLint vertex = m_program->attributeLocation("vertex");
    GLint uv = m_program->attributeLocation("uv");
    GLint tex = m_program->uniformLocation("tex");
    m_program->setUniformValue(tex, 0);
    m_program->release();

    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();
    QOpenGLVertexArrayObject::Binder vaoBinder(m_vao);

    m_vbo = new QOpenGLBuffer();
    m_vbo->create();
    m_vbo->bind();
    GLfloat triangle_data[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, //bl
    -1.0f,  1.0f, 0.0f, 0.0f, //tl
        1.0f,  1.0f, 1.0f, 0.0f, //tr
        1.0f, -1.0f, 1.0f, 1.0f  //br
    };
    m_vbo->allocate(static_cast<void*>(triangle_data), sizeof(triangle_data));
    gl->glEnableVertexAttribArray(vertex);
    gl->glEnableVertexAttribArray(uv);
    gl->glVertexAttribPointer(vertex, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
    gl->glVertexAttribPointer(uv, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(8));
    m_vbo->release();
    m_vao->release();

    m_ibo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    m_ibo->create();
    m_ibo->bind();
    GLushort index_data[] = { 1, 2, 0, 3 };
    m_ibo->allocate(static_cast<void*>(index_data), sizeof(index_data));
    m_ibo->release();

    gl->glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT);
    gl->glDisable(GL_DEPTH_TEST);
    gl->glDisable(GL_CULL_FACE);

    er = gl->glGetError();

    m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
}

void GLWidget::resizeGL(int32_t, int32_t) {

}

void GLWidget::updateGamma(float gamma) {
    makeCurrent();
    m_program->bind();
    GLint uniform_enforce_gamma = m_program->uniformLocation("uniform_enforce_gamma");
    GLint uniform_gamma = m_program->uniformLocation("uniform_gamma");
    m_program->setUniformValue(uniform_enforce_gamma, gamma == 1.0f);
    m_program->setUniformValue(uniform_gamma, gamma);
    m_program->release();
    doneCurrent();
}

void GLWidget::updateTexture(int32_t w, int32_t h, const float* ptr) {
    makeCurrent();
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    m_update_texture = true;
    er = gl->glGetError();
    if(m_texture->width() != w || m_texture->height() != h) {
        m_texture->destroy();
        m_texture->setAutoMipMapGenerationEnabled(false);
        m_texture->setFormat(QOpenGLTexture::RGB32F);
        m_texture->setMinificationFilter(QOpenGLTexture::Linear);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
        m_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::Repeat);
        m_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);
        m_texture->setSize(w, h);
        er = gl->glGetError();
        m_texture->allocateStorage();
        er = gl->glGetError();
    }
    QOpenGLPixelTransferOptions options;
    options.setAlignment(1);
    m_texture->setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, ptr, nullptr);
    er = gl->glGetError();
    m_texture->release();

    this->repaint();
    er = gl->glGetError();
    doneCurrent();
}

void GLWidget::paintGL() {
    if (!m_texture->isCreated()) {
        return;
    }
    QOpenGLFunctions *gl = QOpenGLContext::currentContext()->functions();
    gl->glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();
    m_vao->bind();
    m_ibo->bind();
    gl->glViewport(0, this->size().height() - m_texture->height() - 1, m_texture->width(), m_texture->height());
    m_texture->bind(0);
    gl->glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, nullptr);
    m_texture->release(0);
    er = gl->glGetError();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Press Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, true, true);
    g_Window->getUI()->spinBoxPixelStartX->setValue(event->pos().x());
    g_Window->getUI()->spinBoxPixelStartY->setValue(event->pos().y());
    g_Window->getUI()->spinBoxPixelEndX->setValue(event->pos().x());
    g_Window->getUI()->spinBoxPixelEndY->setValue(event->pos().y());
    event->accept();
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Press Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, true, false);
    event->accept();
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Release Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, false, false);
    event->accept();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {

    g_Window->GetInputEngine()->SceneMouseMotion(event);
    event->accept();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        QApplication::closeAllWindows();
    }

    g_Window->GetInputEngine()->CheckSpecialKeys(event);
    g_Window->GetInputEngine()->KeyboardPressed(event);
    event->accept();
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    g_Window->GetInputEngine()->CheckSpecialKeys(event);
    g_Window->GetInputEngine()->KeyboardUp(event);
    event->accept();
}

// eof ///////////////////////////////// class QGLWidget
