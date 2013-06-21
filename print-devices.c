#include "cl-helper.h"

int main(int argc, char **argv)
{
  print_platforms_devices();

  cl_context ctx;
  cl_command_queue queue;
  create_context_on(CHOOSE_INTERACTIVELY, CHOOSE_INTERACTIVELY, 0, &ctx, &queue, 0);
  print_device_info_from_queue(queue);

  return 0;
}
