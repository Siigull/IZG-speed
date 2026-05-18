/*!
 * @file
 * @brief This file contains implementation of window class
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <cstdlib>
#include <string.h>

#include <framework/window.hpp>

/**
 * @brief Constructor of Window instance
 *
 * @param width width of the window
 * @param height height of the window
 * @param name name of the window
 */
Window::Window(int32_t width,int32_t height,char const*name){
  initSDL();
  initWindow(width,height,name);
  initSurface();
  initRenderer();
  initEvents();
}

/**
 * @brief Destructor of Window instance
 */
Window::~Window(){
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

/**
 * @brief This function sets callback for event.
 *
 * @param event event type from SDL
 * @param clb callback
 */
void Window::setCallback(uint32_t event,EventCallback const&clb){
  eventCallbacks[event] = clb;
}

/**
 * @brief This function sets idle callback - function that is called when there are not events.
 *
 * @param clb callback
 */
void Window::setIdleCallback(IdleCallback const&clb){
  idleCallback = clb;
}
    
/**
 * @brief This function reinits SDL renderer
 */
void Window::reInitRenderer(){
  surface = SDL_GetWindowSurface(window);
  SDL_DestroyRenderer(renderer);
  initRenderer();
}

/**
 * @brief This function returns SDL window handle 
 *
 * @return SDL window handle
 */
SDL_Window*Window::getWindow(){
  return window;
}

/**
 * @brief This function initializes SDL
 */
void Window::initSDL()
{
  // enable logging
  SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

  // initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO)) return;

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail: %s\n",
               SDL_GetError());
  exit(1);
}

/**
 * @brief This funciton initialized SDL window
 *
 * @param width width of the window
 * @param height height of the window
 * @param name name of the window
 */
void Window::initWindow(
                       int32_t       width,
                       int32_t       height,
                       char const*   name)
{
  // create window
  window =
      SDL_CreateWindow(name, width, height, SDL_WINDOW_RESIZABLE);
  if (window) return;
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow fail: %s\n",
               SDL_GetError());
  exit(1);
}

/**
 * @brief This function initializes SDL surface
 */
void Window::initSurface()
{
  // create surface
  surface = SDL_GetWindowSurface(window);
  if (surface) return;
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetWindowSurface fail: %s\n",
               SDL_GetError());
  exit(1);
}

/**
 * @brief This function inzializes SDL renderer
 */
void Window::initRenderer()
{
  // create renderer
  renderer = SDL_CreateSoftwareRenderer(surface);
  if (renderer) return;
  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateSoftwareRenderer: %s\n",
               SDL_GetError());
  exit(1);
}

/**
 * @brief This function initializes SDL vents
 */
void Window::initEvents()
{
  setCallback(SDL_EVENT_QUIT,[&](SDL_Event const&){running = false;});
}

/**
 * @brief This funciton processes one SDL event
 *
 * @param event SDL event
 */
void Window::processEvent(SDL_Event const&event){
  auto it = eventCallbacks.find(event.type);
  if(it == eventCallbacks.end())return;
  it->second(event);
}

/**
 * @brief This function processes all SDL eents 
 */
void Window::processEvents(){
  SDL_Event event;
  while (SDL_PollEvent(&event))
    processEvent(event);
}

/**
 * @brief This function calls user defined idle callback.
 */
void Window::callIdleCallback(){
  if(idleCallback)
    idleCallback();
}


/**
 * @brief This is the main loop of the window
 */
void Window::mainLoop()
{
  running = true;
  // main loop
  while (running) {
    processEvents();

    SDL_LockSurface(surface);

    callIdleCallback();

    SDL_UnlockSurface(surface);
    SDL_UpdateWindowSurface(window);
  }
}

