#include <Client/Renderer.hh>

#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>
#include <cmath>

static app::Renderer *g_Renderer = nullptr;

#ifdef EMSCRIPTEN
#include <emscripten.h>

extern "C"
{
    void KeyEvent(uint8_t op, int32_t key)
    {
        if (op == 1)
            g_Renderer->m_KeysPressed[key] = 1;
        else if (op == 0)
            g_Renderer->m_KeysPressed[key] = 0;
    }
    void Render(int32_t width, int32_t height)
    {
        if (!g_Renderer)
            return;
        g_Renderer->SetSize(width, height);
        g_Renderer->Render();
    }
}
#else
#define SK_GL
#define SK_GANESH
#include <GLFW/glfw3.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkColor.h>
#include <skia/include/core/SkPaint.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkColorSpace.h>
#include <skia/include/gpu/gl/GrGLInterface.h>
#include <skia/include/gpu/GrDirectContext.h>
#include <skia/include/gpu/GrBackendSurface.h>
void SkDebugf(const char format[], ...)
{
}

void GlfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
        g_Renderer->m_KeysPressed[key] = 1;
    else if (action == GLFW_RELEASE)
        g_Renderer->m_KeysPressed[key] = 0;
}

#endif

#include <Client/Simulation.hh>

namespace app
{
    Renderer::Renderer(Simulation &simulation)
        : m_Simulation(simulation)
    {
        assert(!g_Renderer);
        g_Renderer = this;
    }

    void Renderer::Initialize()
    {
#ifndef EMSCRIPTEN
        // 16:9 aspect ratio for 500 height
        m_Width = 889;
        m_Height = 500;
        glfwSetErrorCallback([](int error, char const *description)
                             { std::cerr << "code " << error << ' ' << description << '\n'; });
        glfwInit();
        GLFWwindow *window = glfwCreateWindow(m_Width, m_Height, "rrolf native client", NULL, NULL);
        glfwSetKeyCallback(window, GlfwKeyCallback);

        if (!window)
        {
            std::cerr << "window thing failed\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        sk_sp<GrGLInterface const> interface = GrGLMakeNativeInterface();
        GrDirectContext *context = GrDirectContext::MakeGL(interface).release();
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = GL_RGBA8;
        SkColorType colorType = kRGBA_8888_SkColorType;
        GrBackendRenderTarget backendRenderTarget(m_Width, m_Height,
                                                  0, // sample count
                                                  0, // stencil bits
                                                  framebufferInfo);
        SkSurface *surface = SkSurface::MakeFromBackendRenderTarget(context, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
        m_Canvas = surface->getCanvas();

        while (!glfwWindowShouldClose(window))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            context->flush();
            Render();
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
#else
        EM_ASM({
            Module.canvas = document.createElement("canvas");
            canvasElement.id = "canvas";
            document.body.appendChild(Module.canvas);
            Module.ctx = Module.canvas.getContext('2d');
            Module.paths = [...new Array(128)].fill(null);
            Module.FindPathIndex = function()
            {

            }
            const loop = _ = >
            {
                Module._Render();
                requestAnimationFrame(loop);
            }
            requestAnimationFrame(loop);
        });
    }
#endif
    }

    Renderer::Guard Renderer::AutoSaveRestore()
    {
        return Guard(this);
    }

    Guard::Guard(Renderer *renderer)
        : m_Renderer(renderer)
    {
        m_Renderer->Save();
    }

    Guard::~Guard()
    {
        m_Renderer->Restore();
    }
    void Renderer::SetTransform(float a, float b, float c, float d, float e, float f)
    {
        m_Matrix[0] = a;
        m_Matrix[1] = b;
        m_Matrix[2] = c;
        m_Matrix[3] = d;
        m_Matrix[4] = e;
        m_Matrix[5] = f;
#ifndef EMSCRIPTEN
        m_CurrentMatrix.set9(a, b, c, d, e, f, 0, 0, 1);
#else
    EM_ASM({Module.ctx.setTransform($0, $1, $2, $3, $4, $5); }, a, b, c, d, e, f);
#endif
    }
    void Renderer::UpdateTransform() {
#ifdef EMSCRIPTEN
        EM_ASM({
            Module.ctx.setTransform($0, $1, $2, $3, $4, $5);
        }, m_Matrix[0], m_Matrix[1], m_Matrix[3], m_Matrix[4], m_Matrix[2], m_Matrix[5]);
#else
        SkMatrix m;
        // silly
        m.set9(m_Matrix[0], m_Matrix[1], m_Matrix[2], m_Matrix[3], m_Matrix[4], m_Matrix[5], m_Matrix[6], m_Matrix[7], m_Matrix[8]);
        m_Canvas->setMatrix(m);
#endif
    }
    void Renderer::ResetTransform()
    {
        m_Matrix = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        UpdateTransform();
    }
    void Renderer::Rotate(float a)
    {
        float cos_a = std::cos(a);
        float sin_a = std::sin(a);
        m_Matrix[0] = m_Matrix[0] * cos_a + m_Matrix[3] * sin_a;
        m_Matrix[1] = m_Matrix[1] * cos_a + m_Matrix[4] * sin_a;
        m_Matrix[3] = - m_Matrix[0] * sin_a + m_Matrix[3] * cos_a; //wait lol
        m_Matrix[4] = - m_Matrix[1] * sin_a + m_Matrix[4] * cos_a; //multiplying by cos sin -sin cos
        UpdateTransform();
    }
    void Renderer::Translate(float x, float y)
    {
        m_Matrix[2] += x * (m_Matrix[0] + m_Matrix[3]);
        m_Matrix[5] += y * (m_Matrix[1] + m_Matrix[4]);
        UpdateTransform();
    }

    void Renderer::Scale(float x, float y)
    {
        m_Matrix[0] *= x;
        m_Matrix[1] *= y;
        m_Matrix[3] *= x;
        m_Matrix[4] *= y;
        UpdateTransform();
    }

    void Renderer::Save()
    {
#ifndef EMSCRIPTEN
        m_Canvas->save();
#else
    EM_ASM({
        Module.canvas.save();
    });
#endif
    }

    void Renderer::Restore()
    {
#ifndef EMSCRIPTEN
        m_Canvas->restore();
#else
    EM_ASM({
        Module.canvas.restore();
    });
#endif
    }

    void Renderer::SetSize(int32_t width, int32_t height)
    {
#ifdef EMSCRIPTEN
        m_Width = width;
        m_Height = height;
#else
    assert(false);
#endif
    }
    void Renderer::SetFill(uint32_t fill)
    {
#ifdef EMSCRIPTEN
        EM_ASM({
            // fillStyle = "argb(a,r,g,b)" might be easier
            Module.ctx.fillStyle = '#' + ($0 << 8 | 0 >> 24).toString(16).padStart(8, "0"); //gg ez trust
        }, fill);
#else
        m_FillPaint.setARGB(fill >> 24 & 255, fill >> 16 & 255, fill >> 8 & 255, fill >> 0 & 255);
#endif
    }
    
    void Renderer::SetStroke(uint32_t stroke)
    {
#ifdef EMSCRIPTEN
        EM_ASM({
            // fillStyle = "argb(a,r,g,b)" might be easier
            Module.ctx.strokeStyle = '#' + (0 << 8 | 0 >> 24).toString(16).padStart(8, "0"); //gg ez trust
        }, stroke);
#else
        m_StrokePaint.setARGB(stroke >> 24 & 255, stroke >> 16 & 255, stroke >> 8 & 255, stroke >> 24 & 255);
#endif
    }
    
    void Renderer::SetLineWidth(float w)
    {
#ifdef EMSCRIPTEN
        EM_ASM({
            Module.ctx.lineWidth = $0;
        }, w);
#else
        m_StrokePaint.setLineWidth(w);
#endif
    }

    void Renderer::SetLineCap(LineCap l)
    {
#ifdef EMSCRIPTEN
        EM_ASM({
            if ($0 === 0) Module.ctx.lineCap = 'butt';
            else if ($0 === 1) Module.ctx.lineCap = 'round';
            else Module.ctx.lineCap = 'square';
        }, l);
#else
        // TODO later
#endif
    }
    
    void Renderer::BeginPath()
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.beginPath(); });
#else
        m_Path = SkPath{};
#endif
    }

    void Renderer::MoveTo(float x, float y)
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.moveTo($0, $1); }, x, y);
#else
        m_CurrentPath.moveTo(x, y);
#endif
    }    
    
    void Renderer::LineTo(float x, float y)
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.lineTo($0, $1); }, x, y);
#else
        m_CurrentPath.lineTo(x, y);
#endif
    }
    
    void Renderer::QuadraticCurveTo(float x1, float y1, float x, float y)
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.quadraticCurveTo($0, $1, $2, $3); }, x1, y1, x, y);
#else
        m_CurrentPath.quadTo(x1, y1, x, y);
#endif
    }
    
    void Renderer::Arc(float x, float y, float r)
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.arc($0, $1, $2, 0, 6.283185307179586); }, x, y, r);
#else
        m_CurrentPath.addCircle(x, y, r);
#endif
    }
    
    void Renderer::Fill()
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.fill(); });
#else
        m_FillPaint.setStyle(SkPaint::kFill_Style);
        m_Canvas->drawPath(m_CurrentPath, m_FillPaint)
#endif
    }
    
    void Renderer::Stroke()
    {
#ifdef EMSCRIPTEN
        EM_ASM({ Module.ctx.stroke(); });
#else
        m_StrokePaint.setStyle(SkPaint::kStroke_Style);
        m_Canvas->drawPath(m_CurrentPath, m_StrokePaint)
#endif
    }
  
    void Renderer::Render()
    {
        m_Simulation.TickRenderer(this);
    }
}