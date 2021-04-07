#define DR_WAV_IMPLEMENTATION
/* dr_wav uses stack buffers and we need to limit their size or risk dying
 * whenever the OS resumes our task and notices that we overflowed the stack
 * space allocated to us.
 */
#define DRWAV_STACK_BUFFER_SIZE (1024U)
#include "dr_wav.h"
