#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/memfd.h>
#include <wayland-client.h>


#define ASSERT(cond, msg)                       \
  if(!(cond)){                                  \
    fprintf(stderr, "%s:%d: %s\n",              \
            __FILE__, __LINE__, msg);           \
    exit(EXIT_FAILURE);                         \
  }                                             \


struct client {
  struct wl_display *display;
  struct wl_registry *registry;
  struct wl_compositor *compositor;
  struct wl_shell *shell;
  struct wl_shm *shm;
};

struct window{
  struct wl_surface *surface;
  struct wl_shell_surface *shell_surface;
  struct wl_buffer *buffers[2];
  bool busy[2];
  void *buffer;
};


static void
registry_handler(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version __attribute__((unused))) {
  struct client *client = (struct client *)data;

  if(strcmp(interface, "wl_compositor") == 0) {
    client->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
  } else if(strcmp(interface, "wl_shell") == 0) {
    client->shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
  } else if(strcmp(interface, "wl_shm") == 0) {
    client->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
  }
}

static const struct wl_registry_listener registry_listener = { registry_handler, NULL };


static void
init_client(struct client *client) {
  client->display = wl_display_connect(NULL);
  ASSERT(client->display, "cannot connect to display");
  client->registry = wl_display_get_registry(client->display);
  ASSERT(client->registry, "cannot get registry");
  wl_registry_add_listener(client->registry,&registry_listener,client);

  wl_display_roundtrip(client->display);
  ASSERT(client->compositor, "no compositor");
  ASSERT(client->shell, "no shell");
  ASSERT(client->shm, "no shm");
  wl_display_roundtrip(client->display);
}


static void
handle_ping(void *data __attribute__((unused)), struct wl_shell_surface *shell_surface, uint32_t serial) {
  wl_shell_surface_pong(shell_surface, serial);
}

static const struct wl_shell_surface_listener shell_surface_listener = { handle_ping, NULL, NULL };

static void
buffer_release(void *data, struct wl_buffer *buffer) {
  struct window *window = (struct window*) data;

  for(int i=0; i<2; i++)
    if(buffer == window->buffers[i])
      window->busy[i] = false;
}

static const struct wl_buffer_listener buffer_listener = { buffer_release };

extern const size_t width;
extern const size_t height;

void draw(unsigned char buffer[][4]);

static const struct wl_callback_listener frame_listener;

static void
redraw(void *data, struct wl_callback *callback, uint32_t time __attribute__((unused))) {
  struct window *window = data;

  if (callback)
    wl_callback_destroy(callback);

  int b = -1;

  for(int i=0; i<2; i++)
    if(!(window->busy[i]))
      b = i;

  ASSERT(b != -1, "no buffer available");
  window->busy[b] = true;

  size_t size = width*height * 4;

  draw(window->buffer + b*size);

  wl_surface_attach(window->surface, window->buffers[b], 0, 0);
  wl_surface_damage(window->surface, 0, 0, width, height);

  callback = wl_surface_frame(window->surface);
  wl_callback_add_listener(callback, &frame_listener, window);
  wl_surface_commit(window->surface);
}

static const struct wl_callback_listener frame_listener = { redraw };

static void
create_window(struct client *client, struct window *window) {
  window->surface = wl_compositor_create_surface(client->compositor);
  ASSERT(window->surface, "cannot create surface");
  window->shell_surface = wl_shell_get_shell_surface(client->shell, window->surface);
  ASSERT(window->shell_surface, "cannot get shell surface");

  wl_shell_surface_add_listener(window->shell_surface, &shell_surface_listener, NULL);
  wl_shell_surface_set_toplevel(window->shell_surface);

  int fd = syscall(SYS_memfd_create, "wayland-shm", MFD_CLOEXEC|MFD_ALLOW_SEALING);
  ASSERT(fd != -1, "cannot create shm");

  int size = width*height*4;

  ftruncate(fd, size*2);
  struct wl_shm_pool *pool = wl_shm_create_pool(client->shm, fd, size*2);
  ASSERT(pool, "cannot create pool");

  for(int i=0; i<2; i++) {
    window->buffers[i] = wl_shm_pool_create_buffer(pool, i*size, width, height, width*4, WL_SHM_FORMAT_XRGB8888);
    ASSERT(window->buffers[i], "cannot create buffer");
    wl_buffer_add_listener(window->buffers[i], &buffer_listener, window);
  }

  wl_shm_pool_destroy(pool);

  window->buffer = mmap(NULL, size*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  ASSERT(window->buffer != MAP_FAILED, "failed to mmap");
  close(fd);

  redraw(window, NULL, 0);
}


static void
main_loop(struct client *client) {
  while (wl_display_dispatch(client->display) != -1) {
  }
}

int
main() {
  struct client client = {0};
  struct window window = {0};

  init_client(&client);
  create_window(&client, &window);
  main_loop(&client);
}
