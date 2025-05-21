#include "glad/glad.h"
#include "Window.h"

#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
    std::cout << "GLFW Error: " << description << std::endl;
}

static void APIENTRY gl_debug_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (id == 131185) // Filter useless notification
        return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;

    std::cout << std::endl;

    // TODO: DEBUG ONLY
    if (severity == GL_DEBUG_SEVERITY_HIGH && type == GL_DEBUG_TYPE_ERROR)
    {
        throw;
    }
}

static void cursor_pos_callback(GLFWwindow* window, f64 x, f64 y)
{
    WindowUserData* input_data = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
    input_data->mouse_delta.x += x;
    input_data->mouse_delta.y += y;
}

static void mouse_wheel_callback(GLFWwindow* window, f64 x, f64 y)
{
    WindowUserData* input_data = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
    input_data->wheel_delta.x += x;
    input_data->wheel_delta.y += y;
}

static void window_size_callback(GLFWwindow* window, s32 width, s32 height)
{
    glViewport(0, 0, width, height);
    WindowUserData* user_data = static_cast<WindowUserData*>(glfwGetWindowUserPointer(window));
    if (user_data->on_resize)
        user_data->on_resize(width, height);
}

Window::Window(const WindowCreation& creation_data)
    : user_data(std::make_unique<WindowUserData>()),
    last_frame_time(DBL_MIN)
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // needed for deprecated OpenGL immediate
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, creation_data.resizable ? GLFW_TRUE : GLFW_FALSE);

    width = creation_data.width;
    height = creation_data.height;
    GLFWmonitor* monitor = creation_data.fullscreen_mode != FullscreenMode::Windowed ? glfwGetPrimaryMonitor() : nullptr;

    if (creation_data.fullscreen_mode == FullscreenMode::Borderless)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        width = mode->width;
        height = mode->height;
    }

    handle = glfwCreateWindow(width, height, creation_data.title, monitor, NULL);
    if (!handle)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(handle, user_data.get());
    glfwMakeContextCurrent(handle);
    gladLoadGL();

    // FOR DEBUG ONLY
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, 0);

    glfwSwapInterval(creation_data.vsync ? 1 : 0);

    glfwSetCursorPosCallback(handle, cursor_pos_callback);
    glfwSetScrollCallback(handle, mouse_wheel_callback);
    glfwSetWindowSizeCallback(handle, window_size_callback);

    glViewport(0, 0, width, height);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    last_time = glfwGetTime();
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

bool Window::poll_events()
{
    user_data->mouse_delta = glm::vec2(0.0f);
    user_data->wheel_delta = glm::vec2(0.0f);

    glfwPollEvents();

    return glfwWindowShouldClose(handle);
}

void Window::swap_buffers()
{
    glfwSwapBuffers(handle);

    double now = glfwGetTime();
    last_frame_time = now - last_time;
    last_time = now;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::set_title(const char* title)
{
    glfwSetWindowTitle(handle, title);
}

u32 Window::get_width() const
{
    return width;
}

u32 Window::get_height() const
{
    return height;
}

f64 Window::get_last_frame_time() const
{
    return last_frame_time;
}

glm::vec2 Window::get_cursor_pos() const
{
    f64 x, y;
    glfwGetCursorPos(handle, &x, &y);
    return glm::vec2((f32) x, (f32) y);
}

const glm::vec2& Window::get_cursor_delta() const
{
    return user_data->mouse_delta;
}

const glm::vec2& Window::get_wheel_delta() const
{
    return user_data->wheel_delta;
}

bool Window::is_key_pressed(GLFWKey key) const
{
    return glfwGetKey(handle, key);
}

bool Window::is_mouse_button_pressed(GLFWButton button) const
{
    return glfwGetMouseButton(handle, button);
}

void Window::set_on_resize(std::function<void(u32, u32)> on_resize)
{
    this->user_data->on_resize = on_resize;
}

